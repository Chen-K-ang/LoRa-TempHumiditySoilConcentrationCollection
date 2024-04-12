/**************************************************************

	*	�ļ����ƣ� 	    hwtimer.c
	*
	*	��    �ߣ� 		�շ�
	*
	*	��    �ڣ� 		2019-1-16
	*
	*	˵    ���� 		��ʱ������
	*
	*	�޸ļ�¼��		
	
****************************************************************/

//Э���
#include "onenet.h"
#include "fault.h"

//�����豸
#include "net_device.h"

//Ӳ������
#include "hwtimer.h"


unsigned short timerCount = 0;	//ʱ�����--��λ��


TIM_INFO timInfo = {0};



/*
************************************************************
*	�������ƣ�	Timer1_8_Init
*
*	�������ܣ�	Timer1��8��PWM����
*
*	��ڲ�����	TIMx��TIM1 ���� TIM8
*				arr������ֵ
*				psc��Ƶֵ
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
	
	GPIO_InitTypeDef gpioInitStruct;
	TIM_TimeBaseInitTypeDef timerInitStruct;
	TIM_OCInitTypeDef timerOCInitStruct;

	if(TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	}
	
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInitStruct);	
	
	timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	timerOCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
 	timerOCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	timerOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//�������:TIM����Ƚϼ��Ե�
	timerOCInitStruct.TIM_Pulse = 0;
	
	TIM_OC4Init(TIMx, &timerOCInitStruct);
	
	TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE �����ʹ��	
	
	TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);			//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
 
	TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPEʹ��
	
	TIM_Cmd(TIMx, ENABLE);										//ʹ��TIMx

}


/*
************************************************************
*	�������ƣ�	Timer3_4_Init
*
*	�������ܣ�	Timer3��4�Ķ�ʱ����
*
*	��ڲ�����	TIMx��TIM3 ���� TIM4
*				arr������ֵ
*				psc��Ƶֵ
*
*	���ز�����	��
*
*	˵����		timer6��timer7ֻ���и����жϹ���
************************************************************
*/
void Timer3_4_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{

	TIM_TimeBaseInitTypeDef timerInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	if(TIMx == TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM3_IRQn;
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM4_IRQn;
	}
	
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);					//ʹ�ܸ����ж�
	
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvicInitStruct);
	
	TIM_Cmd(TIMx, ENABLE); //ʹ�ܶ�ʱ��
}

/*
************************************************************
*	�������ƣ�	TIM3_IRQHandler
*
*	�������ܣ�	RTOS��������ʱ�ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
extern DATA_STREAM data_stream[];
extern u8 data_stream_cnt;
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		//do something...
		if(++timInfo.timer6Out >= 4294967290UL)
			timInfo.timer6Out = 0;
		//Iwdg_Feed(); 		//ι��
		TIM_Cmd(TIM3, DISABLE); //���ö�ʱ��
		//
		OneNET_CmdHandle(data_stream, data_stream_cnt); //�������ݴ�����
		TIM_Cmd(TIM3, ENABLE);  //ʹ�ܶ�ʱ��
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

/*
************************************************************
*	�������ƣ�	TIM4_IRQHandler
*
*	�������ܣ�	Timer7�����жϷ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void TIM4_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		if(onenet_info.net_work == 0)											//�������Ͽ�
		{
			if(++timerCount >= NET_TIME) 									//�������Ͽ���ʱ
			{
				timerCount = 0;
				net_fault_info.net_fault_level = NET_FAULT_LEVEL_3;			//����ȼ�3
			}
		}
		else
		{
			timerCount = 0;													//�������
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
