#ifndef _LORA_APP_H_
#define _LORA_APP_H_

#include "sys.h"
#include "lora_cfg.h"

//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板 
//ATK-LORA-01模块功能驱动	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/4/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//******************************************************************************** 
//无

#define LORA_AUX  PAin(4)    //LORA模块状态引脚
#define LORA_MD0  PAout(15)  //LORA模块控制引脚

#define F_DATALEN 4 //整数+小数点+小数的长度  eg: 32.1和1.24长度都为4
union float_data
{
	float f_data;
	u8 byte[F_DATALEN];
};

typedef struct
{
	union float_data temp;
	union float_data humidity;
	union float_data ph_val;
	union float_data N_val;
	union float_data P_val;
	union float_data K_val;
}soil_data;

extern _LoRa_CFG LoRa_CFG;
extern u8 Lora_mode;
extern soil_data lora_tx_data;

u8 LoRa_Init(void);
void Aux_Int(u8 mode);
void LoRa_Set(void);
void LoRa_SendData(void);
void LoRa_ReceData(void);
void LoRa_Process(void);
void Lora_Test(void);


#endif

