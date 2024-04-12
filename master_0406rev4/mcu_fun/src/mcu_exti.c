/**************************************************************

	*	文件名称： 	    mcu_exti.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2018-11-23
	*
	*	说    明： 	    单片机exti驱动配置
	*					
	*	修改记录：		
	
****************************************************************/
//单片机相关组件
#include "mcu_rcc.h"
#include "mcu_gpio.h"
#include "mcu_exti.h"


/*
************************************************************
*	函数名称：	MCU_EXTI_Init
*
*	函数功能：	单片机EXTI初始化
*
*	入口参数：	GPIO_Group：GPIO组
*				GPIO_Pin：引脚号
*				EXTI_Mode：工作模式
*				EXTI_Trigger：边沿选择
*				EXTI_LineCmd：是否使能
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
_Bool MCU_EXTI_Init(GPIO_TypeDef *GPIO_Group, unsigned short GPIO_Pin,
						EXTIMode_TypeDef EXTI_Mode, EXTITrigger_TypeDef EXTI_Trigger, FunctionalState EXTI_LineCmd)
{
	
	EXTI_InitTypeDef exti_initstruct;

	unsigned char port_source = (((unsigned int)((void *)GPIO_Group) - APB2PERIPH_BASE) >> 10) - 2;
	unsigned char pin_source = 0xFF;
	unsigned int exti_line = GPIO_Pin;
	unsigned char i = 1;
	_Bool result = 1;
	
	if(GPIO_Pin == GPIO_Pin_0)
	{
		pin_source = GPIO_PinSource0;
		result = 0;
	}
	else
	{
		for(; i < GPIO_PinSource15 + 1; i++)
		{
			if(GPIO_Pin >> i == 1)
			{
				pin_source = i;
				result = 0;
				break;
			}
		}
	}
	
	if(result == 0)
	{
		MCU_RCC(AFIO, ENABLE);											//外部中断，需要使能AFIO时钟
		
		GPIO_EXTILineConfig(port_source, pin_source);
		
		exti_initstruct.EXTI_Line = exti_line;
		exti_initstruct.EXTI_LineCmd = EXTI_LineCmd;
		exti_initstruct.EXTI_Mode = EXTI_Mode;
		exti_initstruct.EXTI_Trigger = EXTI_Trigger;
		
		EXTI_Init(&exti_initstruct);
	}
	
	return result;

}
