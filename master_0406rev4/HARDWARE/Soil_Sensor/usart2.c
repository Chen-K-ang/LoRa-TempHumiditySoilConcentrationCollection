#include "usart2.h"  

#if EN_USART2_RX   //如果使能了接收
  	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
u16 USART2_RX_STA=0;

//串口2中断服务函数
void USART2_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到一个字节数据
	{	 
		if((USART2_RX_STA&0X8000)==0)
		{
			USART2_RX_BUF[USART2_RX_STA&0X3FFF]=USART2->DR;
			USART2_RX_STA++;
		}
	}
	if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)//接受完一帧数据
	{
		USART2_RX_STA|=0X8000;
		res = USART2->SR;	//清除IDLE中断
		res = USART2->DR;
	}
}  
#endif

//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void usart2_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	                       //GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);                          //串口2时钟使能

 	USART_DeInit(USART2);//复位串口2
	//USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                                     //PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	                               //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                         //初始化PA2
   
    //USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                          //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                         //初始化PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //收发模式
	USART_Init(USART2, &USART_InitStructure);       //初始化串口2

	//使能空闲中断
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //开启中断   
	//使能接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	USART_Cmd(USART2, ENABLE);	//使能串口 
}
