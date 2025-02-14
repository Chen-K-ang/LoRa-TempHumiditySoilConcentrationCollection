/**************************************************************

	*	文件名称： 	    mcu_nvic.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2018-11-23
	*
	*	说    明： 	    单片机NVIC驱动的配置
	*					
	*	修改记录：		
	
****************************************************************/
//单片机相关组件
#include "mcu_nvic.h"


/*
************************************************************
*	函数名称：	MCU_NVIC_Init
*
*	函数功能：	中断配置
*
*	入口参数：	NVIC_IRQChannel：通道
*				NVIC_IRQChannelCmd：ENABLE or DISABLE
*				NVIC_IRQChannelPreemptionPriority：抢占中断优先级
*				NVIC_IRQChannelSubPriority：中断子优先级
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void MCU_NVIC_Init(unsigned char NVIC_IRQChannel, FunctionalState NVIC_IRQChannelCmd,
					unsigned char NVIC_IRQChannelPreemptionPriority, unsigned char NVIC_IRQChannelSubPriority)
{

	NVIC_InitTypeDef nvic_initstruct;
	
	nvic_initstruct.NVIC_IRQChannel = NVIC_IRQChannel;
	nvic_initstruct.NVIC_IRQChannelCmd = NVIC_IRQChannelCmd;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = NVIC_IRQChannelPreemptionPriority;
	nvic_initstruct.NVIC_IRQChannelSubPriority = NVIC_IRQChannelSubPriority;
	
	NVIC_Init(&nvic_initstruct);

}
