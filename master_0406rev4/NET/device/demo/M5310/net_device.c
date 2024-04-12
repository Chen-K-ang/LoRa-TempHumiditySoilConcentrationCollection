/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	net_device.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-08-05
	*
	*	�汾�� 		V1.2
	*
	*	˵���� 		�����豸Ӧ�ò�
	*
	*	�޸ļ�¼��	V1.1��1.ƽ̨IP��PORTͨ����������ķ�ʽȷ��������˲�ͬЭ�������豸������ͨ�õ����⡣
	*					  2.ȡ�����ֶ��������磬�ϵ�ȴ�wifiģ���Զ����ӣ������ɹ���ʹ��OneNET���ںŽ���������
	*					  3.NET_DEVICE_SendCmd����������mode���������Ƿ������������ķ���ֵ��
	*				V1.2��1.ȡ��V1.1�еĹ���3��
	*					  2.���������ݻ�ȡ�ӿڣ�����rb���ơ�
	*					  3.ȡ����͸��ģʽ��
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//��Ƭ��������
#include "mcu_gpio.h"

//�����豸
#include "net_device.h"
#include "net_io.h"

//�����ļ�
#include "miplconfig.h"

//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


NET_DEVICE_INFO net_device_info = {NULL, NULL, NULL,
									NULL, 0,
									0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0};


NET_DEVICE_CELL net_device_cell;


GPS_INFO gps;


MIPL_T mipl_config;


//Ϊ��ͨ���ԣ�gpio�豸�б����name�̶���ôд
const static GPIO_LIST net_device_gpio_list[] = {
													{GPIOA, GPIO_Pin_0, "nd_rst"},
													{GPIOC, GPIO_Pin_4, "nd_pwr"},
												};


//==========================================================
//	�������ƣ�	NET_DEVICE_IO_Init
//
//	�������ܣ�	��ʼ�������豸IO��
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		��ʼ�������豸�Ŀ������š������շ����ܵ�
//==========================================================
void NET_DEVICE_IO_Init(void)
{

	MCU_GPIO_Init(net_device_gpio_list[0].gpio_group, net_device_gpio_list[0].gpio_pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, net_device_gpio_list[0].gpio_name);
	
	MCU_GPIO_Init(net_device_gpio_list[1].gpio_group, net_device_gpio_list[1].gpio_pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, net_device_gpio_list[1].gpio_name);
	
	NET_IO_Init();											//�����豸����IO���ʼ��
	
	MCU_GPIO_Output_Ctl("nd_pwr", 0);
	
	net_device_info.reboot = 0;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_GetSignal
//
//	�������ܣ�	��ȡ�����ź�ǿ��
//
//	��ڲ�����	��
//
//	���ز�����	�����ź�ǿ��ֵ
//
//	˵����		
//==========================================================
signed char NET_DEVICE_GetSignal(void)
{
	
	char *dataPtr = NULL;
	char num[4] = {0, 0, 0, 0}, count = 0;
	
	if(net_device_info.busy)
		return 0;
	
	net_device_info.busy = 1;

	if(!NET_DEVICE_SendCmd("AT+CSQ\r\n", "OK"))
	{
		//RSSI:0~31��Խ��Խǿ��99����������
		dataPtr = strchr(net_device_info.cmd_resp, ':');
		if(dataPtr != NULL)
		{
			dataPtr++;
			if(*dataPtr == ' ')
				dataPtr++;
			
			while(*dataPtr >= '0' && *dataPtr <= '9')
				num[count++] = *dataPtr++;
			
			net_device_info.signal = atoi(num);
			
			UsartPrintf(USART_DEBUG, "CSQ: %d\r\n", net_device_info.signal);
		}
		else
			UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
	}
	
	net_device_info.busy = 0;
	
	return net_device_info.signal;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_GetCell
//
//	�������ܣ�	��ȡ��վ��
//
//	��ڲ�����	��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool NET_DEVICE_GetCell(void)
{
	
	char *data_ptr = NULL;
	unsigned char i = 0;
	_Bool result = 1;

	//NUESTATS:CELL,3736,403,1,-666,-108,-560,216\r\nOK
	//NUESTATS:CELL,3736,404,1,-942,-120,-831,48\r\nOK
	
	if(net_device_info.busy)
		return 1;
	
	net_device_info.busy = 1;
	
	if(!NET_DEVICE_SendCmd("AT+NUESTATS=CELL\r\n", "NUESTATS"))
	{
		data_ptr = strchr(net_device_info.cmd_resp, ',');
		if(data_ptr != NULL)
		{
			//��ȡearfcn----------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.earfcn[i++] = *data_ptr++;
			
			//��ȡphysical_cellid-------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.physical_cellid[i++] = *data_ptr++;
			
			//��ȡprimary_cell----------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.primary_cell[i++] = *data_ptr++;
			
			//��ȡrsrp------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.rsrp[i++] = *data_ptr++;
			
			//��ȡrsrq------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.rsrq[i++] = *data_ptr++;
			
			//��ȡrssi------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.rssi[i++] = *data_ptr++;
			
			//��ȡsnr-------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != '\r')
				net_device_cell.snr[i++] = *data_ptr++;
			
			result = 0;
			
			UsartPrintf(USART_DEBUG, "earfcn:%s, physical_cellid:%s, primary_cell:%s, rsrp:%s, rsrq:%s, rssi:%s, snr:%s\r\n",
										net_device_cell.earfcn, net_device_cell.physical_cellid, net_device_cell.primary_cell,
										net_device_cell.rsrp, net_device_cell.rsrq, net_device_cell.rssi, net_device_cell.snr);
		}
	}
	
	net_device_info.busy = 0;
	
	return result;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Exist
//
//	�������ܣ�	�����豸���ڼ��
//
//	��ڲ�����	��
//
//	���ز�����	���ؽ��
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_Exist(void)
{
	
	_Bool result = 1;

	if(!NET_DEVICE_SendCmd("AT\r\n", "OK"))
		result = 0;
	else
		result = 1;
	
	return result;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Init
//
//	�������ܣ�	�����豸��ʼ��
//
//	��ڲ�����	protocol��Э���(�ο�NET_DEVICE_GetLinkIP��˵��)
//				ip��IP��ַ����ָ��
//				port���˿ڻ���ָ��
//
//	���ز�����	���س�ʼ�����
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_Init(DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	_Bool status = 1;
	
	net_device_info.net_work = 0;

	switch(net_device_info.init_step)
	{
		case 0:
		
			if(!NET_DEVICE_SendCmd("AT+CGMR\r\n", "OK"))
			{
				UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
				net_device_info.init_step++;
			}
		
		break;
		
		case 1:																		//��ʼ��config����
		
			UsartPrintf(USART_DEBUG, "Tips:	MIPLCONFIG_Init\r\n");
			if(!MIPLCONFIG_Init(&mipl_config, EP_NAME, strlen(EP_NAME)))
				net_device_info.init_step++;
			else
				RTOS_TimeDly(200);
		
		break;
			
		case 2:																			//��config�ṹת��Ϊģ��涨��config��������
		
			UsartPrintf(USART_DEBUG, "Tips:	MIPLCONFIG_SetConf\r\n");
		
			if(net_device_info.config_data == NULL)
			{
				net_device_info.config_data = NET_MallocBuffer(128);
				memset(net_device_info.config_data, 0, 128);
			}
			
			if(net_device_info.config_data != NULL)
			{
				net_device_info.config_len = MIPLCONFIG_SetConf(&mipl_config, net_device_info.config_data);
				
				net_device_info.init_step++;
			}
			else
				RTOS_TimeDly(200);
		
		break;
			
		case 3:																			//����������ת��Ϊģ��ɽ��ܵ����ݸ�ʽ
		{
			unsigned char offset = 0, i = 0;
			
			char *dataPtr = (char *)NET_MallocBuffer(net_device_info.config_len << 1);	//�����ڴ�
			
			if(dataPtr != NULL)
			{
				memset(dataPtr, 0, net_device_info.config_len << 1);					//��ջ���
				
				UsartPrintf(USART_DEBUG, "Tips:	MIPLCONFIG_Set\r\n");
				
				if(net_device_info.config_len >= 100)									//����λ��ƫ��
					offset = 4 + 12;
				else
					offset = 3 + 12;
				
				snprintf(dataPtr, offset + 1, "AT+MIPLCONF=%d,", net_device_info.config_len);
				
				for(; i < net_device_info.config_len; i++)								//һ���ֽ�һ���ֽڵİ���ֵתΪ�ַ���ʽ
				{
					if(net_device_info.config_data[i] > 0x0f)
					{
						//sprintf((char *)dataPtr + (i << 1), "%X", data[i]);
						
						Num_2_HexString(net_device_info.config_data[i], dataPtr + (i << 1) + offset, 1);
					}
					else
					{
						//sprintf((char *)dataPtr + (i << 1), "0%X", data[i]);
						
						*(dataPtr + (i << 1) + offset)  = '0';
						Num_2_HexString(net_device_info.config_data[i], dataPtr + (i << 1) + offset + 1, 1);
					}
				}
				
				strcat(dataPtr, ",1,1\r\n");
				
				if(!NET_DEVICE_SendCmd(dataPtr, "OK"))
				{
					NET_FreeBuffer(net_device_info.config_data);
					net_device_info.config_data = NULL;
					
					net_device_info.err = 0;
					net_device_info.init_step++;
				}
				else if(++net_device_info.err >= 3)
				{
					NET_FreeBuffer(net_device_info.config_data);
					net_device_info.config_data = NULL;
					
					net_device_info.err = 0;
					net_device_info.device_ok = 0;
					NET_DEVICE_ReConfig(0);
					
					NET_DEVICE_Reset();
				}
				
				NET_FreeBuffer(dataPtr);
			}
			else
				RTOS_TimeDly(200);
		}
		break;
		
		case 4:
		{
			unsigned short i = 0, err_count = 0;
			char obj_buf[56];
			
			UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ And MIPLNOTIFY\r\n");		//���osb��notify
			
			for(; i <streamArrayCnt;)												//������������ѭ�����
			{
				snprintf(obj_buf, sizeof(obj_buf), "AT+MIPLADDOBJ=0,%s,%s\r\n", streamArray[i].obj_id, streamArray[i].ins_id);
				
				if(!NET_DEVICE_SendCmd(obj_buf, "OK"))
					i++;
				else
				{
					if(++err_count >= 5)
						break;
					
					UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Err\r\n");
					RTOS_TimeDly(80);
				}
			}
			
			UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Ok\r\n");
			
			if(i == streamArrayCnt)
			{
				err_count = 0;
				
				for(i = 0; i <streamArrayCnt;)
				{
					DSTREAM_toString(streamArray, obj_buf, i, sizeof(obj_buf));
		
					if(!NET_DEVICE_SendCmd(obj_buf, "OK"))
						i++;
					else
					{
						if(++err_count >= 5)
							break;
						
						UsartPrintf(USART_DEBUG, "Tips:	MIPLNOTIFY Err\r\n");
						RTOS_TimeDly(50);
					}
				}
			}
			
			UsartPrintf(USART_DEBUG, "Tips:	MIPLNOTIFY Ok\r\n");
			
			if(i == streamArrayCnt)
				net_device_info.init_step++;
		}
		break;
		
		case 5:
			
			net_device_info.signal = NET_DEVICE_GetSignal();
			if(net_device_info.signal > 5 && net_device_info.signal < 32)
				net_device_info.init_step++;
			else
				RTOS_TimeDly(200);
		
		break;
			
		case 6:
		
			UsartPrintf(USART_DEBUG, "Tips:	NUESTATS=CELL\r\n");
			if(!NET_DEVICE_GetCell())
				net_device_info.init_step++;
			else
				RTOS_TimeDly(400);
		
		break;
			
		case 7:
		
			UsartPrintf(USART_DEBUG, "Tips:	MIPLOPEN\r\n");
			if(!NET_DEVICE_SendCmd("AT+MIPLOPEN=0,30\r\n", "OK"))
				net_device_info.init_step++;
			else
				RTOS_TimeDly(400);
		
		break;
			
		case 8:
			
			if(strstr(net_device_info.cmd_resp, "+MIPLOPEN:0,1") != NULL)
				net_device_info.init_step++;

		break;
		
		default:
			
			status = 0;
			net_device_info.net_work = 1;
			UsartPrintf(USART_DEBUG, "Tips:	M5310 OK\r\n");
		
		break;
	}
	
	return status;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Reset
//
//	�������ܣ�	�����豸��λ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_Reset(void)
{
	
	UsartPrintf(USART_DEBUG, "Tips:	NET_DEVICE_Reset\r\n");
	
	MCU_GPIO_Output_Ctl("nd_rst", 1);		//��λ
	RTOS_TimeDly(100);
	
	MCU_GPIO_Output_Ctl("nd_rst", 0);		//������λ
	RTOS_TimeDly(200);

}

//==========================================================
//	�������ƣ�	NET_DEVICE_CmdHandle
//
//	�������ܣ�	���������Ƿ���ȷ
//
//	��ڲ�����	cmd����Ҫ���͵�����
//
//	���ز�����	��
//
//	˵����		�����ɹ���ָ����NULL
//==========================================================
void NET_DEVICE_CmdHandle(char *cmd)
{
	
	if(strstr(cmd, net_device_info.cmd_hdl) != NULL)
		net_device_info.cmd_hdl = NULL;
	
	net_device_info.cmd_resp = cmd;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_SendCmd
//
//	�������ܣ�	�������豸����һ��������ȴ���ȷ����Ӧ
//
//	��ڲ�����	cmd����Ҫ���͵�����
//				res����Ҫ��������Ӧ
//
//	���ز�����	�������ӽ��
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;
	
	NET_IO_Send((unsigned char *)cmd, strlen(cmd));			//д��������豸
	
	if(res == NULL)											//���Ϊ�գ���ֻ�Ƿ���
		return 0;
	
	net_device_info.cmd_hdl = res;							//��Ҫ���ѵĹؼ���
	
	while((net_device_info.cmd_hdl != NULL) && --timeOut)		//�ȴ�
		RTOS_TimeDly(2);
	
	if(timeOut > 0)
		return 0;
	else
		return 1;

}

//==========================================================
//	�������ƣ�	HexString_2_Num
//
//	�������ܣ�	��ʮ�������ַ���תΪ��ֵ
//
//	��ڲ�����	hs��ʮ�������ַ���
//
//	���ز�����	ת�����
//
//	˵����		��Ҫ��0xǰ׺
//==========================================================
int HexString_2_Num(char *hs)
{

	int num = 0;
	unsigned char len = 0, i = 0;
	
	if(hs == NULL)
		return -1;
	
	while(hs[i++] != '\0')
		len++;
	
	for(i = 0; i < len; i++)
	{
		if(hs[i] >= 'A' && hs[i] <= 'F')
			num |= (hs[i] - '7') << ((len - i - 1) << 2);
		else if(hs[i] >= 'a' && hs[i] <= 'f')
			num |= (hs[i] - 'W') << ((len - i - 1) << 2);
		else if(hs[i] >= '0' && hs[i] <= '9')
			num |= (hs[i] - '0') << ((len - i - 1) << 2);
		else
			return -2;
	}
	
	return num;

}

//==========================================================
//	�������ƣ�	Num_2_HexString
//
//	�������ܣ�	����ֵתΪʮ�������ַ���
//
//	��ڲ�����	num����ֵ
//				hs���ַ�������
//				flag��1-תΪ��д		0-תΪСд
//
//	���ز�����	�����ַ�������	С��0ʧ��
//
//	˵����		
//==========================================================
int Num_2_HexString(unsigned int num, char *hs, _Bool flag)
{

	unsigned char len = 0, i = 0;
	short n_temp = 0;
	
	if(num == 0)
	{
		hs[0] = '0';
		return 1;
	}
	
	while(1)					//ת�����ж��ٸ��ַ�
	{
		if(num >> (len << 2))
			len++;
		else
			break;
	}
	
	for(; i < len; i++)
	{
		n_temp = (short)((num >> (i << 2)) & 0xf);
		
		if(n_temp >= 0 && n_temp <= 9)
			hs[len - i - 1] = n_temp + '0';
		else if(n_temp >= 10 && n_temp <= 16)
			hs[len - i - 1] = n_temp + (flag ? '7' : 'W');
		else
		{
			for(i = 0; i < len; i++)
				hs[i] = 0;
			
			return -1;
		}
	}
	
	return len;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Read
//
//	�������ܣ�	��ȡһ֡����
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
unsigned char *NET_DEVICE_Read(void)
{

	return NET_IO_Read();

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Update
//
//	�������ܣ�	������������
//
//	��ڲ�����	time��ʱ�䣬��λ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		��10s~86400s֮��
//==========================================================
_Bool NET_DEVICE_Update(unsigned int time)
{

	char update_buf[24];
	unsigned char time_out = 200;
	
	if(!net_device_info.net_work)
		return 1;
	
	while(net_device_info.busy && --time_out)
		RTOS_TimeDly(2);
	
	if(!time_out)
		return 1;
	
	net_device_info.busy = 1;
	
	if(time < 10 || time > 86400)
		return 1;
	
	snprintf(update_buf, sizeof(update_buf), "AT+MIPLUPDATE=0,%d,0\r\n", time);
	
	net_device_info.busy = 0;
	
	return NET_DEVICE_SendCmd(update_buf, "0,1");

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Check
//
//	�������ܣ�	��������豸����״̬
//
//	��ڲ�����	��
//
//	���ز�����	����״̬
//
//	˵����		
//==========================================================
unsigned char NET_DEVICE_Check(void)
{

	return 0;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_ReConfig
//
//	�������ܣ�	�豸�����豸��ʼ���Ĳ���
//
//	��ڲ�����	����ֵ
//
//	���ز�����	��
//
//	˵����		�ú������õĲ����������豸��ʼ������õ�
//==========================================================
void NET_DEVICE_ReConfig(unsigned char step)
{

	net_device_info.init_step = step;

}
