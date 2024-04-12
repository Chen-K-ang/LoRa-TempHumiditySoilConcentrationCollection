/**************************************************************

	*	文件名称： 	    fault.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2019-1-5
	*
	*	说    明： 		错误处理
	*
	*	修改记录：		
	
****************************************************************/

//单片机相关组件
#include "mcu_gpio.h"

//协议
#include "onenet.h"
#include "fault.h"

//网络设备
#include "net_device.h"

//硬件驱动
#include "usart.h"
#include "delay.h"


NET_FAULT_INFO net_fault_info;


//==========================================================
//	函数名称：	Fault_Process
//
//	函数功能：	错误处理
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
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
		
		case NET_FAULT_LEVEL_1:											//错误等级1-负责重连IP
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_1\r\n");
		
			NET_DEVICE_ReConfig(10);
			
			net_device_info.net_work = 0;
			onenet_info.net_work = 0;
			
			if(++onenet_info.reinit_count >= 5)
			{
				onenet_info.reinit_count = 0;
				net_fault_info.net_fault_level = NET_FAULT_LEVEL_3;		//错误等级3
			}
			else
				net_fault_info.net_fault_level = NET_FAULT_LEVEL_0;		//将错误等级降至最低
		
		break;
		
		case NET_FAULT_LEVEL_2:											//错误等级2-负责重新注册网络
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_2\r\n");
			
			net_device_info.device_ok = 0;
			net_device_info.net_work = 0;
			NET_DEVICE_ReConfig(10);
			
			net_device_info.net_work = 0;
			onenet_info.net_work = 0;
			
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_0;			//将错误等级降至最低
		
		break;
		
		case NET_FAULT_LEVEL_3:											//错误等级3-负责复位网络设备
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_3\r\n");
			
			NET_DEVICE_Reset();
			//NET_DEVICE_ReConfig(4);
			
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_2;			//将错误等级降低
		
		break;
		
		case NET_FAULT_LEVEL_4:											//错误等级4-负责给网络设备重新上电
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_4\r\n");
		
			MCU_GPIO_Output_Ctl("nd_pwr", 1);
		
			//DelayXms(1000);
		  delay_ms(1000);
			
			MCU_GPIO_Output_Ctl("nd_pwr", 0);
		
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_3;			//将错误等级降低
		
		break;
		
		case NET_FAULT_LEVEL_5:
		
			UsartPrintf(USART_DEBUG, "WARN:	NET_FAULT_LEVEL_5\r\n");
		
			net_fault_info.net_fault_level = NET_FAULT_LEVEL_0;
		
		break;
		
		default:
		break;
	}

}
