#ifndef _LORA_APP_H_
#define _LORA_APP_H_

#include "sys.h"
#include "lora_cfg.h"

//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������ 
//ATK-LORA-01ģ�鹦������	  
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/4/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//******************************************************************************** 
//��

#define LORA_AUX  PAin(4)    //LORAģ��״̬����
#define LORA_MD0  PAout(15)  //LORAģ���������

#define F_DATALEN 4 //����+С����+С���ĳ���  eg: 32.1��1.24���ȶ�Ϊ4
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
extern soil_data lora_rx_data;

u8 LoRa_Init(void);
void Aux_Int(u8 mode);
void LoRa_Set(void);
void LoRa_SendData(void);
void LoRa_ReceData(void);
void LoRa_Process(void);
void Lora_Test(void);


#endif

