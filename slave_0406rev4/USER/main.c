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
 ALIENTEK STM32F103开发板 扩展实验23
 ATK-LORA-01无串口线模块测试实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//主函数
int main(void)
{	 	 

	u8 key=0;
	u8 fontok=0; 
  	u8 t=0;

	delay_init();	    	                        //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	                        //串口初始化为115200
	LCD_Init();	 
	LED_Init();
	KEY_Init();
	key=KEY_Scan(0);  
	fontok=font_init();		                        //检查字库是否OK 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	
	while(LoRa_Init())//初始化ATK-LORA-01模块
	{
		Show_Str(40+30,50+20,200,16,"未检测到模块!!!",16,0); 	
		delay_ms(300);
		Show_Str(40+30,50+20,200,16,"                ",16,0);
	}
	Show_Str(40+30,50+20,200,16,"检测到模块!!!",16,0);
	delay_ms(500); 	
//	Menu_ui();//主界面显示
	Show_Str(40+30,50+20,200,16,"                ",16,0);
	while(1)
	{
		LoRa_Process();//开始数据测试
		t++;
		if(t==30)
		{
			t=0;
			LED1=~LED1;
		}
		delay_ms(10);
	}
	
}
