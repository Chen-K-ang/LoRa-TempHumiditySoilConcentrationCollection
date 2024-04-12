/**************************************************************

	*	文件名称： 	    nb_cmd.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2019-1-8
	*
	*	说    明： 		解析平台下发的命令
	*
	*	修改记录：		
	
****************************************************************/

//头文件
#include "nb_cmd.h"

//硬件驱动
#include "net_device.h"

//C库
#include <stdio.h>
#include <string.h>
#include "usart.h"

//==========================================================
//	函数名称：	NB_CMD_UnPacketGeneralID
//
//	函数功能：	命令下发的通用信息提取
//
//	入口参数：	cmd：命令指针
//				msg_id：消息id
//				obj_id：obj id
//				ins_id：ins id
//				res_id：res id
//
//	返回参数：	无
//
//	说明：		读、写、执行都可以调用此函数解析出各id
//==========================================================
void NB_CMD_UnPacketGeneralID(unsigned char *cmd, char *msg_id, char *obj_id, char *ins_id, char *res_id)
{

	unsigned char *data_ptr = cmd;
	unsigned short count = 0;
	
	while(*data_ptr != ',' && *data_ptr != '\0')
		data_ptr++;
	
	//复制msg_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		msg_id[count++] = *data_ptr++;
	msg_id[count++] = 0;
	
	//复制obj_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		obj_id[count++] = *data_ptr++;
	obj_id[count++] = 0;
	
	//复制ins_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		ins_id[count++] = *data_ptr++;
	ins_id[count++] = 0;
	
	//复制res_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		res_id[count++] = *data_ptr++;
	res_id[count++] = 0;

}

//==========================================================
//	函数名称：	NB_CMD_Unpacket_W_E_Msg
//
//	函数功能：	写或执行命令的消息内容提取
//
//	入口参数：	cmd：命令指针
//				msg：消息内容缓存指针
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool NB_CMD_Unpacket_W_E_Msg(unsigned char *cmd, char **msg)
{

	unsigned char *data_ptr = cmd, *data_ptr_t = NULL;
	unsigned short count = 0;
	
	while(*data_ptr != '\0')
	{
		if(*data_ptr == ',')
		{
			if(++count == 7)
			{
				data_ptr++;
				break;
			}
		}
		
		data_ptr++;
	}
	
	if(count == 7)
	{
		count = 0;
		
		data_ptr_t = data_ptr;
		while(*data_ptr_t != ',')
		{
			count++;
			data_ptr_t++;
		}
		
		*msg = (char *)NET_MallocBuffer(count + 1);
		if(*msg == NULL)
			return 1;
		
		memset(*msg, 0, count + 1);
		memcpy(*msg, data_ptr, count);
		
		return 0;
	}
	else
		return 1;

}

//==========================================================
//	函数名称：	NB_CMD_W_E_Resp
//
//	函数功能：	写和执行命令的回复
//
//	入口参数：	msg_id：消息id
//				result：0-写失败		1-写成功
//				w_e：1-回复write命令		0-回复execute命令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool NB_CMD_W_E_Resp(char *msg_id, _Bool result, _Bool w_e)
{

	char resp_buf[32];
	
	if(w_e)
		snprintf(resp_buf, sizeof(resp_buf), "AT+MIPLWRITERSP=0,%s,%d\r\n", msg_id, result);
	else
		snprintf(resp_buf, sizeof(resp_buf), "AT+MIPLEXECUTERSP=0,%s,%d\r\n", msg_id, result);
	
	return NET_DEVICE_SendCmd(resp_buf, "OK");

}

//==========================================================
//	函数名称：	NB_CMD_ReadResp
//
//	函数功能：	读命令的回复
//
//	入口参数：	msg_id：消息id
//				obj_id：obj id
//				ins_id：ins id
//				res_id：res id
//				streamArray：数据流结构
//				streamArrayCnt：结构乘以个数
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool NB_CMD_ReadResp(char *msg_id, char *obj_id, char *ins_id, char *res_id, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{

	char resp_buf[64];
	unsigned short pos = 0;
	
	for(; pos < streamArrayCnt; pos++)
	{
		if(strcmp(streamArray[pos].obj_id, obj_id) == 0)			//obj id匹配
		{
			if(strcmp(streamArray[pos].res_id, res_id) == 0)		//res id匹配
			{
				if(strcmp(streamArray[pos].ins_id, ins_id) == 0)	//ins id匹配
				{
					if(DSTREAM_toString_ReadResp(streamArray, resp_buf, pos, sizeof(resp_buf), msg_id) == 0)
					{
						NET_DEVICE_SendCmd(resp_buf, "OK");
						return 0;
					}
				}
			}
		}
	}
	
	return 1;

}

//==========================================================
//	函数名称：	NB_CMD_Write
//
//	函数功能：	写操作
//
//	入口参数：	obj_id：obj id
//				ins_id：ins id
//				res_id：res id
//				streamArray：数据流结构
//				streamArrayCnt：结构乘以个数
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool NB_CMD_Write(char *obj_id, char *ins_id, char *res_id, char *msg, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{

	unsigned short pos = 0;
	
	for(; pos < streamArrayCnt; pos++)
	{
		if(strcmp(streamArray[pos].obj_id, obj_id) == 0)			//obj id匹配
		{
			if(strcmp(streamArray[pos].res_id, res_id) == 0)		//res id匹配
			{
				if(strcmp(streamArray[pos].ins_id, ins_id) == 0)	//ins id匹配
				{
					if(streamArray[pos].write_fun != NULL)
					{
						streamArray[pos].write_fun(msg);
						return 0;
					}
					else
						return 1;
				}
			}
		}
	}
	
	return 1;

}

//==========================================================
//	函数名称：	NB_CMD_Execute
//
//	函数功能：	执行操作
//
//	入口参数：	obj_id：obj id
//				ins_id：ins id
//				res_id：res id
//				streamArray：数据流结构
//				streamArrayCnt：结构乘以个数
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool NB_CMD_Execute(char *obj_id, char *ins_id, char *res_id, char *msg, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{

	unsigned short pos = 0;
	
	for(; pos < streamArrayCnt; pos++)
	{
		if(strcmp(streamArray[pos].obj_id, obj_id) == 0)			//obj id匹配
		{
			if(strcmp(streamArray[pos].res_id, res_id) == 0)		//res id匹配
			{
				if(strcmp(streamArray[pos].ins_id, ins_id) == 0)	//ins id匹配
				{
					if(streamArray[pos].execute_fun != NULL)
					{
						streamArray[pos].execute_fun(msg);
						return 0;
					}
					else
						return 1;
				}
			}
		}
	}
	
	return 1;

}
