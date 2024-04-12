#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ��ӢSTM32������
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   
BEEP_INFO Beep_Info;



//��ʼ��PB8Ϊ�����.��ʹ������ڵ�ʱ��		    
//��������ʼ��
void BEEP_Init(void)
{
 
 	GPIO_InitTypeDef gpioInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//��GPIOA��ʱ��
	
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0;						//��ʼ��PA0����
	gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;				//����Ϊ���
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;				//�ɳ��ص����Ƶ��
	GPIO_Init(GPIOB, &gpioInitStruct);							//��ʼ��GPIO
		
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);							//����PA0

}

void Beep_Set(_Bool Status)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,Status == Beep_ON ? Bit_SET : Bit_RESET);
	Beep_Info.Beep_Status = !Status;
}
