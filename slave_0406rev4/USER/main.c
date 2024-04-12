#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"		
#include "usart3.h"
#include "string.h"
#include "touch.h"
#include "lora_app.h"

/************************************************
 ALIENTEK STM32F103������ ��չʵ��23
 ATK-LORA-01�޴�����ģ�����ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//������
int main(void)
{	 	 

	u8 key=0;
	u8 fontok=0; 
  	u8 t=0;

	delay_init();	    	                        //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	                        //���ڳ�ʼ��Ϊ115200
	LCD_Init();	 
	LED_Init();
	KEY_Init();
	key=KEY_Scan(0);  
	fontok=font_init();		                        //����ֿ��Ƿ�OK 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	
	while(LoRa_Init())//��ʼ��ATK-LORA-01ģ��
	{
		Show_Str(40+30,50+20,200,16,"δ��⵽ģ��!!!",16,0); 	
		delay_ms(300);
		Show_Str(40+30,50+20,200,16,"                ",16,0);
	}
	Show_Str(40+30,50+20,200,16,"��⵽ģ��!!!",16,0);
	delay_ms(500); 	
//	Menu_ui();//��������ʾ
	Show_Str(40+30,50+20,200,16,"                ",16,0);
	while(1)
	{
		LoRa_Process();//��ʼ���ݲ���
		t++;
		if(t==30)
		{
			t=0;
			LED1=~LED1;
		}
		delay_ms(10);
	}
	
}
