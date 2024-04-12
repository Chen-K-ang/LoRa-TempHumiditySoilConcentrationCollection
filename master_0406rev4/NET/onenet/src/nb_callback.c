/**************************************************************

	*	文件名称： 	    nb_callback.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2019-1-7
	*
	*	说    明： 		平台回调命令处理
	*
	*	修改记录：		
	
****************************************************************/

//头文件
#include "nb_callback.h"

#include "onenet.h"
#include "net_device.h"

//硬件驱动
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "delay.h"


//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void CALLBACK_Led(char *msg)
{
	//Led_Set((*msg - '0')?LED_ON:LED_OFF);
	
	if((*msg - '0')==1)   //当平台发送true或按按键ON时，返回数值1，即可开灯
	{
		LED0=0;  //开灯
		Led_Info.Led_Status=!LED0; //灯状态赋值
	}
	else
	{
		LED0=1;  //关灯
		Led_Info.Led_Status=!LED0; //灯状态赋值
	}
	delay_ms(100);
	

}

