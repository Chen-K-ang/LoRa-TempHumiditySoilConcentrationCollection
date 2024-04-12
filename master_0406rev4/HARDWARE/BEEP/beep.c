#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 精英STM32开发板
//蜂鸣器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   
BEEP_INFO Beep_Info;



//初始化PB8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
 
 	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//打开GPIOA的时钟
	
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0;						//初始化PA0引脚
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//设置为输出
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;				//可承载的最大频率
	GPIO_Init(GPIOB, &gpioInitStruct);							//初始化GPIO
		
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);							//拉低PA0

}

void Beep_Set(_Bool Status)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,Status == Beep_ON ? Bit_SET : Bit_RESET);
	Beep_Info.Beep_Status = !Status;
}
