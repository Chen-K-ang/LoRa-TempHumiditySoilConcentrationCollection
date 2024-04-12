#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//��������������	   
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

	_Bool Beep_Status;

} BEEP_INFO;

extern BEEP_INFO Beep_Info;

//������״̬
#define		Beep_ON		1
#define		Beep_OFF	0


#define 	BEEP_ON		GPIO_SetBits(GPIOB,GPIO_Pin_0);								//����PA0
#define 	BEEP_OFF	GPIO_ResetBits(GPIOB,GPIO_Pin_0);							//����PA0

//�������˿ڶ���
#define BEEP PBout(8)	// BEEP,�������ӿ�		   

void BEEP_Init(void);	//��ʼ��

void Beep_Set(_Bool Status);
		 				    
#endif

