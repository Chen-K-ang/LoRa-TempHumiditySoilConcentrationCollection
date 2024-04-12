#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

typedef struct
{

	_Bool Led_Status;

} LED_INFO;

extern LED_INFO Led_Info;


//typedef struct
//{
//	//�ն�1
//	float temp1_soil;
//	float temp1_env;
//	float humi1_soil;
//	float humi1_env;
//	int lumi1_env;

//	//�ն�2
//	float temp2_soil;
//	float temp2_env;
//	float humi2_soil;
//	float humi2_env;
//	int lumi2_env;
//} terminal_INFO;

typedef struct
{
	//�ն�1
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

void LED_Init(void);//��ʼ��

		 				    
#endif
