/**************************************************************

	*	�ļ����ƣ� 	    nb_cmd.c
	*
	*	��    �ߣ� 		�շ�
	*
	*	��    �ڣ� 		2019-1-8
	*
	*	˵    ���� 		����ƽ̨�·�������
	*
	*	�޸ļ�¼��		
	
****************************************************************/

//ͷ�ļ�
#include "nb_cmd.h"

//Ӳ������
#include "net_device.h"

//C��
#include <stdio.h>
#include <string.h>
#include "usart.h"

//==========================================================
//	�������ƣ�	NB_CMD_UnPacketGeneralID
//
//	�������ܣ�	�����·���ͨ����Ϣ��ȡ
//
//	��ڲ�����	cmd������ָ��
//				msg_id����Ϣid
//				obj_id��obj id
//				ins_id��ins id
//				res_id��res id
//
//	���ز�����	��
//
//	˵����		����д��ִ�ж����Ե��ô˺�����������id
//==========================================================
void NB_CMD_UnPacketGeneralID(unsigned char *cmd, char *msg_id, char *obj_id, char *ins_id, char *res_id)
{

	unsigned char *data_ptr = cmd;
	unsigned short count = 0;
	
	while(*data_ptr != ',' && *data_ptr != '\0')
		data_ptr++;
	
	//����msg_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		msg_id[count++] = *data_ptr++;
	msg_id[count++] = 0;
	
	//����obj_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		obj_id[count++] = *data_ptr++;
	obj_id[count++] = 0;
	
	//����ins_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		ins_id[count++] = *data_ptr++;
	ins_id[count++] = 0;
	
	//����res_id
	count = 0;
	data_ptr++;
	while(*data_ptr >= '0' && *data_ptr <= '9' && *data_ptr != '\0')
		res_id[count++] = *data_ptr++;
	res_id[count++] = 0;

}

//==========================================================
//	�������ƣ�	NB_CMD_Unpacket_W_E_Msg
//
//	�������ܣ�	д��ִ���������Ϣ������ȡ
//
//	��ڲ�����	cmd������ָ��
//				msg����Ϣ���ݻ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
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
//	�������ƣ�	NB_CMD_W_E_Resp
//
//	�������ܣ�	д��ִ������Ļظ�
//
//	��ڲ�����	msg_id����Ϣid
//				result��0-дʧ��		1-д�ɹ�
//				w_e��1-�ظ�write����		0-�ظ�execute����
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
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
//	�������ƣ�	NB_CMD_ReadResp
//
//	�������ܣ�	������Ļظ�
//
//	��ڲ�����	msg_id����Ϣid
//				obj_id��obj id
//				ins_id��ins id
//				res_id��res id
//				streamArray���������ṹ
//				streamArrayCnt���ṹ���Ը���
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool NB_CMD_ReadResp(char *msg_id, char *obj_id, char *ins_id, char *res_id, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{

	char resp_buf[64];
	unsigned short pos = 0;
	
	for(; pos < streamArrayCnt; pos++)
	{
		if(strcmp(streamArray[pos].obj_id, obj_id) == 0)			//obj idƥ��
		{
			if(strcmp(streamArray[pos].res_id, res_id) == 0)		//res idƥ��
			{
				if(strcmp(streamArray[pos].ins_id, ins_id) == 0)	//ins idƥ��
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
//	�������ƣ�	NB_CMD_Write
//
//	�������ܣ�	д����
//
//	��ڲ�����	obj_id��obj id
//				ins_id��ins id
//				res_id��res id
//				streamArray���������ṹ
//				streamArrayCnt���ṹ���Ը���
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool NB_CMD_Write(char *obj_id, char *ins_id, char *res_id, char *msg, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{

	unsigned short pos = 0;
	
	for(; pos < streamArrayCnt; pos++)
	{
		if(strcmp(streamArray[pos].obj_id, obj_id) == 0)			//obj idƥ��
		{
			if(strcmp(streamArray[pos].res_id, res_id) == 0)		//res idƥ��
			{
				if(strcmp(streamArray[pos].ins_id, ins_id) == 0)	//ins idƥ��
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
//	�������ƣ�	NB_CMD_Execute
//
//	�������ܣ�	ִ�в���
//
//	��ڲ�����	obj_id��obj id
//				ins_id��ins id
//				res_id��res id
//				streamArray���������ṹ
//				streamArrayCnt���ṹ���Ը���
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool NB_CMD_Execute(char *obj_id, char *ins_id, char *res_id, char *msg, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{

	unsigned short pos = 0;
	
	for(; pos < streamArrayCnt; pos++)
	{
		if(strcmp(streamArray[pos].obj_id, obj_id) == 0)			//obj idƥ��
		{
			if(strcmp(streamArray[pos].res_id, res_id) == 0)		//res idƥ��
			{
				if(strcmp(streamArray[pos].ins_id, ins_id) == 0)	//ins idƥ��
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
