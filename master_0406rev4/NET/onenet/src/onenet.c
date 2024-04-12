/**************************************************************

	*	文件名称： 	    onenet.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2019-1-5
	*
	*	说    明： 		连接onenet平台
	*
	*	修改记录：		
	
****************************************************************/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "net_device.h"

//协议文件
#include "onenet.h"
#include "fault.h"
#include "nb_cmd.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "beep.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


ONETNET_INFO onenet_info = {0, 0, 0, 1, 0, 0};


//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	streamArray：数据流结构
//				streamArrayCnt：成员个数
//
//	返回参数：	SEND_TYPE_OK-发送成功	SEND_TYPE_DATA-需要重送
//
//	说明：		
//==========================================================
unsigned char OneNet_SendData(DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	unsigned char i = 0, j = 0, err_count = 0;
	char data_buf[256];
	
	if(!onenet_info.net_work)
		return SEND_TYPE_DATA;
	
	if(net_device_info.busy)
		return SEND_TYPE_DATA;
	
	net_device_info.busy = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_SendData-Total-%d\r\n", streamArrayCnt);
	for(; i < streamArrayCnt;)
	{
		if(!DSTREAM_toString(streamArray, data_buf, i, sizeof(data_buf)))
		{
			j++;
			
			if(!NET_DEVICE_SendCmd(data_buf, "OK"))
				i++;
			else
			{
				if(++err_count >= 3)
				{
					err_count = 0;
					i++;
					net_device_info.send_count++;
				}
				delay_ms(30);
				//DelayXms(30);
			}
		}
		else
			i++;
	}
	
	if(j >= streamArrayCnt)
		net_device_info.send_count = 0;

	UsartPrintf(USART_DEBUG, "OneNet_SendData-Real-%d\r\n", j);
	
	net_device_info.busy = 0;
	
	return SEND_TYPE_OK;
	
}

_Bool OneNet_Update(unsigned int time)
{

	return NET_DEVICE_Update(time);

}

//==========================================================
//	函数名称：	OneNET_CmdHandle
//
//	函数功能：	读取平台rb中的数据
//
//	入口参数：	streamArray：数据流结构
//				streamArrayCnt：成员个数
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_CmdHandle(DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	unsigned char *dataPtr = NULL;						//数据指针

	dataPtr = NET_DEVICE_Read();						//等待数据
	
	//写命令回复
	if(dataPtr != NULL)									//数据有效
	{
		//UsartPrintf(USART_DEBUG, "-----\r\nOneNET_CmdHandle:%s\r\n*****\r\n", dataPtr);
		//UsartPrintf(USART_DEBUG, "OneNET_CmdHandle:%s\r\n\r\n", dataPtr);
		
		if(strstr((char *)dataPtr, "+MIPLWRITE"))
		{
			char msg_id[8], obj_id[8], ins_id[8], res_id[8];
			char *msg = NULL;
			//+MIPLWRITE=0,18173,3200,0,5750,1,4,test,0,0
			
			
			NB_CMD_UnPacketGeneralID(dataPtr, msg_id, obj_id, ins_id, res_id);
			
			if(NB_CMD_Unpacket_W_E_Msg(dataPtr, &msg) == 0)
			{
				UsartPrintf(USART_DEBUG, "Write->	msg_id:%s, obj_id:%s, ins_id:%s, res_id:%s, msg:%s\r\n", msg_id, obj_id, ins_id, res_id, msg);
				
				if(NB_CMD_Write(obj_id, ins_id, res_id, msg, streamArray, streamArrayCnt) == 0)
					NB_CMD_W_E_Resp(msg_id, 1, 1);
				else
					NB_CMD_W_E_Resp(msg_id, 0, 1);
				
				NET_FreeBuffer(msg);
			}
		}
		
		//读命令回复
		else if(strstr((char *)dataPtr, "+MIPLREAD"))
		{
			char msg_id[8], obj_id[8], ins_id[8], res_id[8];
			char resp_buf[32];
			memset(resp_buf,0,sizeof(resp_buf));

			//+MIPLREAD=0,18172,3200,0,5750
			
			NB_CMD_UnPacketGeneralID(dataPtr, msg_id, obj_id, ins_id, res_id);
			
			UsartPrintf(USART_DEBUG, "Read->	msg_id:%s, obj_id:%s, ins_id:%s, res_id:%s\r\n", msg_id, obj_id, ins_id, res_id);
			
			if(!NB_CMD_ReadResp(msg_id, obj_id, ins_id, res_id, streamArray, streamArrayCnt))
				UsartPrintf(USART_DEBUG, "Read Ok!\r\n");
			else
			{
				sprintf(resp_buf, "AT+MIPLREADRSP=0,%s,11\r\n", msg_id);
				if(!NET_DEVICE_SendCmd(resp_buf, "OK"))
					UsartPrintf(USART_DEBUG, "Read Failed!\r\n");
			}

		}
		
		//执行命令回复
		else if(strstr((char *)dataPtr, "+MIPLEXECUTE"))
		{
			char msg_id[8], obj_id[8], ins_id[8], res_id[8];
			char *msg = NULL;
			//+MIPLEXECUTE=0,18174,3200,0,5750,4,”test”
			
			NB_CMD_UnPacketGeneralID(dataPtr, msg_id, obj_id, ins_id, res_id);
			
			if(NB_CMD_Unpacket_W_E_Msg(dataPtr, &msg) == 0)
			{
				UsartPrintf(USART_DEBUG, "Execute->		msg_id:%s, obj_id:%s, ins_id:%s, res_id:%s, msg:%s\r\n", msg_id, obj_id, ins_id, res_id, msg);
				
				if(NB_CMD_Execute(obj_id, ins_id, res_id, msg, streamArray, streamArrayCnt) == 0)
					NB_CMD_W_E_Resp(msg_id, 1, 0);
				else
					NB_CMD_W_E_Resp(msg_id, 0, 0);
				
				NET_FreeBuffer(msg);
			}
		}
		else if(strstr((char *)dataPtr, "+MIPLEVENT:0,15"))
		{
			UsartPrintf(USART_DEBUG, "WARN:	连接断开，准备重连\r\n");
			
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_3;			//错误等级1
			
			onenet_info.net_work = 0;
			
			NVIC_SystemReset();
		}
		else
			NET_DEVICE_CmdHandle((char *)dataPtr);
	}

}
