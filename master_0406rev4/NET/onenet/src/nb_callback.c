/**************************************************************

	*	�ļ����ƣ� 	    nb_callback.c
	*
	*	��    �ߣ� 		�շ�
	*
	*	��    �ڣ� 		2019-1-7
	*
	*	˵    ���� 		ƽ̨�ص������
	*
	*	�޸ļ�¼��		
	
****************************************************************/

//ͷ�ļ�
#include "nb_callback.h"

#include "onenet.h"
#include "net_device.h"

//Ӳ������
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "delay.h"


//C��
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void CALLBACK_Led(char *msg)
{
	//Led_Set((*msg - '0')?LED_ON:LED_OFF);
	
	if((*msg - '0')==1)   //��ƽ̨����true�򰴰���ONʱ��������ֵ1�����ɿ���
	{
		LED0=0;  //����
		Led_Info.Led_Status=!LED0; //��״̬��ֵ
	}
	else
	{
		LED0=1;  //�ص�
		Led_Info.Led_Status=!LED0; //��״̬��ֵ
	}
	delay_ms(100);
	

}

