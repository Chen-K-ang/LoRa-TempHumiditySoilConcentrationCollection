#include "usart2.h"  

#if EN_USART2_RX   //���ʹ���˽���
  	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
u16 USART2_RX_STA=0;

//����2�жϷ�����
void USART2_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�һ���ֽ�����
	{	 
		if((USART2_RX_STA&0X8000)==0)
		{
			USART2_RX_BUF[USART2_RX_STA&0X3FFF]=USART2->DR;
			USART2_RX_STA++;
		}
	}
	if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)//������һ֡����
	{
		USART2_RX_STA|=0X8000;
		res = USART2->SR;	//���IDLE�ж�
		res = USART2->DR;
	}
}  
#endif

//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void usart2_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	                       //GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);                          //����2ʱ��ʹ��

 	USART_DeInit(USART2);//��λ����2
	//USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                                     //PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	                               //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                         //��ʼ��PA2
   
    //USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                          //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                         //��ʼ��PA3
	
	USART_InitStructure.USART_BaudRate = bound;                                    //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure);       //��ʼ������2

	//ʹ�ܿ����ж�
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  //�����ж�   
	//ʹ�ܽ����ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	USART_Cmd(USART2, ENABLE);	//ʹ�ܴ��� 
}
