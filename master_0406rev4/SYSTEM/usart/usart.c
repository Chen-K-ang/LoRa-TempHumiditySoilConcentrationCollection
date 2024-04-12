#include "sys.h"
#include "usart.h"	
#include "delay.h"

//��Ƭ��������
#include "mcu_usart.h"
#include "mcu_nvic.h"
#include "mcu_dma.h"
//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


ALTER_INFO alter_info;

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	


void Usart3_Init(unsigned int baud)
{
	
	MCU_USART_Init(USART3, baud, USART_HardwareFlowControl_None, USART_Mode_Rx | USART_Mode_Tx,
					USART_Parity_No, USART_StopBits_1, USART_WordLength_8b);
	
	USART_Cmd(USART3, ENABLE);														//ʹ�ܴ���

#if(USART_DMA_RX_EN == 0)
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
#endif

	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);									//ʹ��IDLE�ж�
	
	MCU_NVIC_Init(USART3_IRQn, ENABLE, 0, 0);
	
#if(USART_DMA_TX_EN == 1)
	MCU_DMA_Init(DMA1, DMA1_Channel2, (unsigned int)&USART3->DR, NULL, DMA_DIR_PeripheralDST, 0,
					DMA_PeripheralInc_Disable, DMA_MemoryInc_Enable, DMA_PeripheralDataSize_Byte,
					DMA_MemoryDataSize_Byte, DMA_Mode_Normal, DMA_Priority_Medium, DMA_M2M_Disable);
	
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);									//ʹ��USART2��DMA���͹���
	
	USARTx_ResetMemoryBaseAddr(USART3, (unsigned int)UsartPrintfBuf, 1, USART_TX_TYPE);//����һ������
#endif
	
#if(USART_DMA_RX_EN == 1)
	MCU_DMA_Init(DMA1, DMA1_Channel3, (unsigned int)&USART3->DR, NULL, DMA_DIR_PeripheralSRC, 0,
					DMA_PeripheralInc_Disable, DMA_MemoryInc_Enable, DMA_PeripheralDataSize_Byte,
					DMA_MemoryDataSize_Byte, DMA_Mode_Normal, DMA_Priority_Medium, DMA_M2M_Disable);
	
	MCU_NVIC_Init(DMA1_Channel3_IRQn, ENABLE, 1, 2);
	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);									//������������ж�

	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);									//ʹ��USART2��DMA���չ���
#endif

}


/*
************************************************************
*	�������ƣ�	USARTx_ResetMemoryBaseAddr
*
*	�������ܣ�	����DMA�ڴ��ַ��ʹ�ܷ���
*
*	��ڲ�����	USARTx��������
*				mAddr���ڴ��ֵַ
*				num�����η��͵����ݳ���(�ֽ�)
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USARTx_ResetMemoryBaseAddr(USART_TypeDef *USARTx, unsigned int mAddr, unsigned short num, _Bool type)
{

#if(USART_DMA_TX_EN == 1)
	if(type == USART_TX_TYPE)
	{
		if(USARTx == USART1)
			MCU_DMA_ResetMemoryBaseAddr(DMA1_Channel4, mAddr, num);
		else if(USARTx == USART2)
			MCU_DMA_ResetMemoryBaseAddr(DMA1_Channel7, mAddr, num);
		else if(USARTx == USART3)
			MCU_DMA_ResetMemoryBaseAddr(DMA1_Channel2, mAddr, num);
	}
#endif
	
#if(USART_DMA_RX_EN == 1)
	if(type == USART_RX_TYPE)
	{
		if(USARTx == USART1)
			MCU_DMA_ResetMemoryBaseAddr(DMA1_Channel5, mAddr, num);
		else if(USARTx == USART2)
			MCU_DMA_ResetMemoryBaseAddr(DMA1_Channel6, mAddr, num);
		else if(USARTx == USART3)
			MCU_DMA_ResetMemoryBaseAddr(DMA1_Channel3, mAddr, num);
	}
#endif

}

/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

#if(USART_DMA_TX_EN == 0)
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ�������ɾ�
		USART_SendData(USARTx, *str++);									//������
		
	}
#else
	unsigned int mAddr = (unsigned int)str;
	
	if(USARTx == USART1)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);					//�ȴ�ͨ��4�������
		DMA_ClearFlag(DMA1_FLAG_TC4);										//���ͨ��4������ɱ�־
	}
	else if(USARTx == USART2)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);					//�ȴ�ͨ��7�������
		DMA_ClearFlag(DMA1_FLAG_TC7);										//���ͨ��7������ɱ�־
	}
	else if(USARTx == USART3)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);					//�ȴ�ͨ��7�������
		DMA_ClearFlag(DMA1_FLAG_TC2);										//���ͨ��7������ɱ�־
	}
	
	USARTx_ResetMemoryBaseAddr(USARTx, mAddr, len, USART_TX_TYPE);
#endif

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	va_list ap;
	
#if(USART_DMA_TX_EN == 0)
	unsigned char UsartPrintfBuf[128];
#endif
	
	unsigned char *pStr = UsartPrintfBuf;
	
#if(USART_DMA_TX_EN == 1)
	if(USARTx == USART1)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);					//�ȴ�ͨ��4�������
		DMA_ClearFlag(DMA1_FLAG_TC4);										//���ͨ��4������ɱ�־
	}
	else if(USARTx == USART2)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC7) == RESET);					//�ȴ�ͨ��7�������
		DMA_ClearFlag(DMA1_FLAG_TC7);										//���ͨ��7������ɱ�־
	}
	else if(USARTx == USART3)
	{
		while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);					//�ȴ�ͨ��7�������
		DMA_ClearFlag(DMA1_FLAG_TC2);										//���ͨ��7������ɱ�־
	}
	memset(UsartPrintfBuf, 0, sizeof(UsartPrintfBuf));					//���buffer
#endif
	
	va_start(ap, fmt);
	vsprintf((char *)UsartPrintfBuf, fmt, ap);							//��ʽ��
	va_end(ap);
	
#if(USART_DMA_TX_EN == 1)
	USARTx_ResetMemoryBaseAddr(USARTx, (unsigned int)UsartPrintfBuf,
							strlen((const char *)pStr), USART_TX_TYPE);
#else
	while(*pStr != 0)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		USART_SendData(USARTx, *pStr++);
		
	}
#endif

}
