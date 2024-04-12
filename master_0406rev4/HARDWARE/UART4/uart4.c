#include "delay.h"
#include "uart4.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "lora_cfg.h"
#include "lora_app.h"

//���ڽ��ջ����� 	
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 			//���ջ���,���UART4_MAX_RECV_LEN���ֽ�.
u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 			//���ͻ���,���UART4_MAX_SEND_LEN�ֽ�

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 UART4_RX_STA = 0;   	

void UART4_IRQHandler(void)
{
	u8 res;
	//���յ�����	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) {
		res = USART_ReceiveData(UART4);

		//�������һ������,��û�б�����,���ٽ�����������
		if ((UART4_RX_STA & (1 << 15)) == 0) {
			//�����Խ�������
			if (UART4_RX_STA < UART4_MAX_RECV_LEN) {
				//���ù���(������ʱ����ʱ)
				if (!Lora_mode) {
					TIM_SetCounter(TIM7, 0);        //���������
					//ʹ�ܶ�ʱ��7���ж�
					if (UART4_RX_STA == 0) {
						TIM_Cmd(TIM7, ENABLE);  //ʹ�ܶ�ʱ��7
					}
				}
				UART4_RX_BUF[UART4_RX_STA++] = res;	//��¼���յ���ֵ
			} else {
				UART4_RX_STA |= 1 << 15;		//ǿ�Ʊ�ǽ������
			} 
		}
	}
}   

USART_InitTypeDef USART_InitStructure;
//��ʼ��IO ����4
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void uart4_init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); //����4ʱ��ʹ��

	USART_DeInit(UART4);                            //��λ����4
	//USART4_TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;      //PC10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//USART4_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;      //PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;                     //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;             //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(UART4, &USART_InitStructure); //��ʼ������4

	USART_Cmd(UART4, ENABLE);                //ʹ�ܴ��� 

	//ʹ�ܽ����ж�
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); //�����ж�

	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;  //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;          //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                             //����ָ���Ĳ�����ʼ��VIC�Ĵ���

	TIM7_Int_Init(99, 7199); //10ms�ж�
	UART4_RX_STA = 0;	 //����
	TIM_Cmd(TIM7,DISABLE);	 //�رն�ʱ��7
}

//����4,printf ����
//ȷ��һ�η������ݲ�����UART4_MAX_SEND_LEN�ֽ�
void u4_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)UART4_TX_BUF, fmt,ap);
	va_end(ap);
	i = strlen((const char*)UART4_TX_BUF); //�˴η������ݵĳ���
	
	//ѭ����������
	for (j = 0; j < i; j++) {
		//ѭ������,ֱ��������� 
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);

		USART_SendData(UART4, UART4_TX_BUF[j]); 
	} 
}

//����4�����ʺ�У��λ����
//bps:�����ʣ�1200~115200��
//parity:У��λ���ޡ�ż���棩
void uart4_set(u8 bps, u8 parity)
{
	static u32 bound = 0;
	
	switch (bps) {
		case LORA_TTLBPS_1200:   bound=1200;     break;
		case LORA_TTLBPS_2400:   bound=2400;     break;
		case LORA_TTLBPS_4800:   bound=4800;     break;
		case LORA_TTLBPS_9600:   bound=9600;     break;
		case LORA_TTLBPS_19200:  bound=19200;    break;
		case LORA_TTLBPS_38400:  bound=38400;    break;
		case LORA_TTLBPS_57600:  bound=57600;    break;
		case LORA_TTLBPS_115200: bound=115200;   break;
	}

	USART_Cmd(UART4, DISABLE); //�رմ��� 
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	
	if (parity == LORA_TTLPAR_8N1) {
		//��У��
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
		USART_InitStructure.USART_Parity = USART_Parity_No;
	} else if (parity == LORA_TTLPAR_8E1) {
		//żУ��
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;    
		USART_InitStructure.USART_Parity = USART_Parity_Even;
	} else if (parity == LORA_TTLPAR_8O1) {
		//��У��
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;    
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
	}
	USART_Init(UART4, &USART_InitStructure); //��ʼ������4
	USART_Cmd(UART4, ENABLE);                //ʹ�ܴ��� 
	
}
 
//���ڽ���ʹ�ܿ���
//enable:0,�ر� 1,��
void uart4_rx(u8 enable)
{
	USART_Cmd(UART4, DISABLE); //ʧ�ܴ��� 

	if (enable) {
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
	} else {
		USART_InitStructure.USART_Mode = USART_Mode_Tx;                 //ֻ���� 
	}

	USART_Init(UART4, &USART_InitStructure); //��ʼ������4
	USART_Cmd(UART4, ENABLE);                //ʹ�ܴ��� 

}

