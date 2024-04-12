/**************************************************************

	*	�ļ����ƣ� 	    fault.c
	*
	*	��    �ߣ� 		�շ�
	*
	*	��    �ڣ� 		2019-1-5
	*
	*	˵    ���� 		������
	*
	*	�޸ļ�¼��		
	
****************************************************************/

//��Ƭ��������
#include "mcu_gpio.h"

//Э��
#include "onenet.h"
#include "fault.h"

//�����豸
#include "net_device.h"

//Ӳ������
#include "usart.h"
#include "delay.h"


NET_FAULT_INFO net_fault_info;


//==========================================================
//	�������ƣ�	Fault_Process
//
//	�������ܣ�	������
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_Fault_Process(void)
{
	
	if(net_fault_info.net_fault_flag)
	{
		net_fault_info.net_fault_flag = 0;
		net_fault_info.net_fault_count++;
	}

	switch(net_fault_info.net_fault_level)
	{
		case NET_FAULT_LEVEL_0:
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_0\r\n");
		
		break;
		
		case NET_FAULT_LEVEL_1:											//����ȼ�1-��������IP
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_1\r\n");
		
			NET_DEVICE_ReConfig(10);
			
			net_device_info.net_work = 0;
			onenet_info.net_work = 0;
			
			if(++onenet_info.reinit_count >= 5)
			{
				onenet_info.reinit_count = 0;
				net_fault_info.net_fault_level = NET_FAULT_LEVEL_3;		//����ȼ�3
			}
			else
				net_fault_info.net_fault_level = NET_FAULT_LEVEL_0;		//������ȼ��������
		
		break;
		
		case NET_FAULT_LEVEL_2:											//����ȼ�2-��������ע������
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_2\r\n");
			
			net_device_info.device_ok = 0;
			net_device_info.net_work = 0;
			NET_DEVICE_ReConfig(10);
			
			net_device_info.net_work = 0;
			onenet_info.net_work = 0;
			
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_0;			//������ȼ��������
		
		break;
		
		case NET_FAULT_LEVEL_3:											//����ȼ�3-����λ�����豸
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_3\r\n");
			
			NET_DEVICE_Reset();
			//NET_DEVICE_ReConfig(4);
			
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_2;			//������ȼ�����
		
		break;
		
		case NET_FAULT_LEVEL_4:											//����ȼ�4-����������豸�����ϵ�
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_4\r\n");
		
			MCU_GPIO_Output_Ctl("nd_pwr", 1);
		
			//DelayXms(1000);
		  delay_ms(1000);
			
			MCU_GPIO_Output_Ctl("nd_pwr", 0);
		
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_3;			//������ȼ�����
		
		break;
		
		case NET_FAULT_LEVEL_5:
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_5\r\n");
		
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_0;
		
		break;
		
		default:
		break;
	}

}
