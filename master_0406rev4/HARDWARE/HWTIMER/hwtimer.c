/**************************************************************

	*	文件名称： 	    hwtimer.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2019-1-16
	*
	*	说    明： 		定时器配置
	*
	*	修改记录：		
	
****************************************************************/

//协议层
#include "onenet.h"
#include "fault.h"

//网络设备
#include "net_device.h"

//硬件驱动
#include "hwtimer.h"


unsigned short timerCount = 0;	//时间计数--单位秒


TIM_INFO timInfo = {0};



/*
************************************************************
*	函数名称：	Timer1_8_Init
*
*	函数功能：	Timer1或8的PWM配置
*
*	入口参数：	TIMx：TIM1 或者 TIM8
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		
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
	
	timerOCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;				//选择定时器模式:TIM脉冲宽度调制模式1
 	timerOCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	timerOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
	timerOCInitStruct.TIM_Pulse = 0;
	
	TIM_OC4Init(TIMx, &timerOCInitStruct);
	
	TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE 主输出使能	
	
	TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPE使能
	
	TIM_Cmd(TIMx, ENABLE);										//使能TIMx

}


/*
************************************************************
*	函数名称：	Timer3_4_Init
*
*	函数功能：	Timer3或4的定时配置
*
*	入口参数：	TIMx：TIM3 或者 TIM4
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		timer6和timer7只具有更新中断功能
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
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);					//使能更新中断
	
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvicInitStruct);
	
	TIM_Cmd(TIMx, ENABLE); //使能定时器
}

/*
************************************************************
*	函数名称：	TIM3_IRQHandler
*
*	函数功能：	RTOS的心跳定时中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
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
		//Iwdg_Feed(); 		//喂狗
		TIM_Cmd(TIM3, DISABLE); //禁用定时器
		//
		OneNET_CmdHandle(data_stream, data_stream_cnt); //下行数据处理函数
		TIM_Cmd(TIM3, ENABLE);  //使能定时器
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

/*
************************************************************
*	函数名称：	TIM4_IRQHandler
*
*	函数功能：	Timer7更新中断服务函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM4_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		if(onenet_info.net_work == 0)											//如果网络断开
		{
			if(++timerCount >= NET_TIME) 									//如果网络断开超时
			{
				timerCount = 0;
				net_fault_info.net_fault_level = NET_FAULT_LEVEL_3;			//错误等级3
			}
		}
		else
		{
			timerCount = 0;													//清除计数
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
