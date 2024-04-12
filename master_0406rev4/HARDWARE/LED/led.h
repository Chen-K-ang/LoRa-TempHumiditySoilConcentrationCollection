#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

typedef struct
{

	_Bool Led_Status;

} LED_INFO;

extern LED_INFO Led_Info;


//typedef struct
//{
//	//终端1
//	float temp1_soil;
//	float temp1_env;
//	float humi1_soil;
//	float humi1_env;
//	int lumi1_env;

//	//终端2
//	float temp2_soil;
//	float temp2_env;
//	float humi2_soil;
//	float humi2_env;
//	int lumi2_env;
//} terminal_INFO;

typedef struct
{
	//终端1
	float soil_temperature;
	float soil_humidity;
	float soil_ph;
	float soil_Nval;
	float soil_Pval;
	float soil_Kval;
} terminal_INFO;

extern terminal_INFO terminal_Info;

#define LED0 PDout(2)	// PA8
#define LED1 PAout(8)	// PD2	

void LED_Init(void);//初始化

		 				    
#endif
