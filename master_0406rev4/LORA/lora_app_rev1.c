#include "lora_app.h"
#include "lora_ui.h"
#include "uart4.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "lcd.h"
#include "stdio.h"
#include "text.h"
#include "key.h"

//设备参数初始化(具体设备参数见lora_cfg.h定义)
_LoRa_CFG LoRa_CFG =
{
	.addr     = LORA_ADDR,     //设备地址
	.power    = LORA_POWER,    //发射功率
	.chn      = LORA_CHN,      //信道
	.wlrate   = LORA_RATE,     //空中速率
	.wltime   = LORA_WLTIME,   //睡眠时间
	.mode     = LORA_MODE,     //工作模式
	.mode_sta = LORA_STA,      //发送状态
	.bps      = LORA_TTLBPS,   //波特率设置
	.parity   = LORA_TTLPAR    //校验位设置
};

//全局参数
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

//设备工作模式(用于记录设备状态)
u8 Lora_mode = 0;       //0:配置模式 1:接收模式 2:发送模式

//记录中断状态
static u8 Int_mode = 0; //0:关闭 1:上升沿 2:下降沿

//AUX中断设置
//mode:配置的模式 0:关闭 1:上升沿 2:下降沿
void Aux_Int(u8 mode)
{
	if (!mode) {
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;//关闭中断
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	} else {
		if (mode == 1)
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿
		else if (mode == 2)
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿

		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	}
	Int_mode = mode;//记录中断模式
	EXTI_Init(&EXTI_InitStructure);
	NVIC_Init(&NVIC_InitStructure);
}

//LORA_AUX中断服务函数
void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4)) {
		if (Int_mode == 1) {
			//上升沿(发送:开始发送数据 接收:数据开始输出)
			//接收模式
			if (Lora_mode == 1) {
				UART4_RX_STA = 0;//数据计数清0
			}

			Int_mode = 2; //设置下降沿触发
			LED0     = 0; //DS0亮
		} else if (Int_mode == 2) {
			//下降沿(发送:数据已发送完 接收:数据输出结束)
			if(Lora_mode == 1){
				//接收模式
				//数据计数标记完成
				UART4_RX_STA |= 1 << 15;
			} else if (Lora_mode == 2) {
				//发送模式(串口数据发送完毕)
				//进入接收模式
				Lora_mode = 1;
			}

			Int_mode = 1; //设置上升沿触发
			LED0     = 1; //DS0灭
		}
		
		Aux_Int(Int_mode);//重新设置中断边沿
		EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE4上的中断标志位
	}
}

//LoRa模块初始化
//返回值: 0,检测成功
//        1,检测失败
u8 LoRa_Init(void)
{
	u8 retry = 0;
	u8 temp  = 1;

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);	 //使能复用功能时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //禁止JTAG,从而PA15可以做普通IO使用,否则PA15不能做普通IO!!!	

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;                       //LORA_MD0
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;                  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);                             //推挽输出 ，IO口速度为50MHz

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;                        //LORA_AUX
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;                     //下拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);                             //根据设定参数初始化GPIOA.4

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

	EXTI_InitStructure.EXTI_Line    = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;             //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;                         //中断线关闭(先关闭后面再打开)
	EXTI_Init(&EXTI_InitStructure);                                    //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel                   = EXTI4_IRQn; //LORA_AUX
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;       //抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x03;	   //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd                = DISABLE;    //关闭外部中断通道（后面再打开）
	NVIC_Init(&NVIC_InitStructure); 

	LORA_MD0 = 0;
	LORA_AUX = 0;

	//确保LORA模块在空闲状态下(LORA_AUX = 0)
	while (LORA_AUX) {
		Show_Str(40 + 30, 50 + 20, 200, 16, "模块正忙,请稍等!!",    16, 0); 	
		delay_ms(500);
		Show_Str(40 + 30, 50 + 20, 200, 16, "                    ", 16, 0);
		delay_ms(100);		 
	}

	uart4_init(115200); //初始化串口4

	LORA_MD0 = 1;        //进入AT模式
	delay_ms(40);
	retry = 3;
	while (retry--) {
		if (!lora_send_cmd("AT", "OK", 70)) {
			temp = 0; //检测成功
			break;
		}	
	}
	
	if (retry == 0) 
		temp = 1;     //检测失败
	return temp;
}

//Lora模块参数配置
void LoRa_Set(void)
{
	u8 sendbuf[20];
	u8 lora_addrh,lora_addrl = 0;

	uart4_set(LORA_TTLBPS_115200, LORA_TTLPAR_8N1); //进入配置模式前设置通信波特率和校验位(115200 8位数据 1位停止 无数据校验）
	uart4_rx(1);     //开启串口4接收

	while(LORA_AUX); //等待模块空闲
	LORA_MD0 = 1;    //进入配置模式
	delay_ms(40);
	Lora_mode = 0;   //标记"配置模式"

	lora_addrh = (LoRa_CFG.addr >> 8) & 0xff;
	lora_addrl = LoRa_CFG.addr & 0xff;
	sprintf((char*)sendbuf, "AT+ADDR=%02x,%02x", lora_addrh,   lora_addrl);      //设置设备地址
	lora_send_cmd (sendbuf, "OK", 50);
	sprintf((char*)sendbuf, "AT+WLRATE=%d,%d",   LoRa_CFG.chn, LoRa_CFG.wlrate); //设置信道和空中速率
	lora_send_cmd (sendbuf, "OK", 50);
	sprintf((char*)sendbuf, "AT+TPOWER=%d",      LoRa_CFG.power);                //设置发射功率
	lora_send_cmd (sendbuf, "OK", 50);
	sprintf((char*)sendbuf, "AT+CWMODE=%d",      LoRa_CFG.mode);                 //设置工作模式
	lora_send_cmd (sendbuf, "OK", 50);
	sprintf((char*)sendbuf, "AT+TMODE=%d",       LoRa_CFG.mode_sta);             //设置发送状态
	lora_send_cmd (sendbuf, "OK", 50);
	sprintf((char*)sendbuf, "AT+WLTIME=%d",      LoRa_CFG.wltime);               //设置睡眠时间
	lora_send_cmd (sendbuf, "OK", 50);
	sprintf((char*)sendbuf, "AT+UART=%d,%d",     LoRa_CFG.bps, LoRa_CFG.parity); //设置串口波特率、数据校验位
	lora_send_cmd (sendbuf, "OK", 50);

	LORA_MD0 = 0;                              //退出配置,进入通信
	delay_ms(40);
	while(LORA_AUX);                           //判断是否空闲(模块会重新配置参数)
	UART4_RX_STA = 0;
	Lora_mode = 1;                             //标记"接收模式"
	uart4_set(LoRa_CFG.bps, LoRa_CFG.parity);  //返回通信,更新通信串口配置(波特率、数据校验位)
	Aux_Int(1);                                //设置LORA_AUX上升沿中断

}

soil_data lora_tx_data;

static void tran_data_filling(u8* data)
{
	u8 i;
	*data = 0xaa;
	*(data + 17) = 0x55;
	data++;
	for (i = 0; i < 4; i++) {
		*(data + i)                 = lora_tx_data.humidity.byte[i];
		*(data + i + F_DATALEN)     = lora_tx_data.N_val.byte[i];
		*(data + i + F_DATALEN * 2) = lora_tx_data.P_val.byte[i];
		*(data + i + F_DATALEN * 3) = lora_tx_data.K_val.byte[i];
	}
}

u8 Dire_Date[]   = {0x11,0x22,0x33,0x44,0x55}; //定向传输数据
#define Dire_DateLen (sizeof(Dire_Date) / sizeof(Dire_Date[0]))
	
extern u32 obj_addr;    //记录用户输入目标地址
extern u8  obj_chn;     //记录用户输入目标信道

u8 wlcd_buff[10] = {0}; //LCD显示字符串缓冲区

u8 date[30]      = {0};                        //定向数组
u8 Tran_Data[18] = {0};                        //透传数组

//Lora模块发送数据
void LoRa_SendData(void)
{
	u8 chn;
	u16 addr;
	u16 i = 0; 
	tran_data_filling(Tran_Data);
	//透明传输
	if (LoRa_CFG.mode_sta == LORA_STA_Tran) {

		for (i = 0; i < 18; i++) {
			while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET); //循环发送,直到发送完毕
			USART_SendData(UART4, Tran_Data[i]);
		}

	} else if (LoRa_CFG.mode_sta == LORA_STA_Dire) {
		//定向传输
		addr = (u16)obj_addr; //目标地址
		chn = obj_chn;        //目标信道
		
		date[i++] = (addr >> 8) & 0xff; //高位地址
		date[i++] =  addr & 0xff;       //低位地址
		date[i] = chn;                  //无线信道
		
		//数据写到发送BUFF
		for (i = 0; i < Dire_DateLen; i++) {
			date[3 + i] = Dire_Date[i];
		}
		for(i = 0; i < (Dire_DateLen + 3); i++) {
			while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET); //循环发送,直到发送完毕
			USART_SendData(UART4, date[i]);
		}

		//将十六进制的数据转化为字符串打印在lcd_buff数组
		sprintf((char*)wlcd_buff, "%x %x %x %x %x %x %x %x",
			date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7]);

		LCD_Fill(0, 200, 240, 230,WHITE);          //清除显示
		Show_Str_Mid(10, 200, wlcd_buff, 16, 240); //显示发送的数据	
		Dire_Date[4]++;                            //Dire_Date[4]数据更新
	}		
}

u8 rlcd_buff[10] = {0}; //LCD显示字符串缓冲区

//Lora模块接收数据
void LoRa_ReceData(void)
{
	u16 i = 0;
	u16 len = 0;
   
	//有数据来了
	if(UART4_RX_STA & 0x8000) {
		len = UART4_RX_STA&0X7FFF;
		UART4_RX_BUF[len] = 0;//添加结束符
		UART4_RX_STA = 0;

		for (i = 0; i < len ; i++) {
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); //循环发送,直到发送完毕   
			USART_SendData(USART1, UART4_RX_BUF[i]); 
		}

		LCD_Fill(10, 260, 240, 320, WHITE);

		//透明传输
		if (LoRa_CFG.mode_sta==LORA_STA_Tran) {
			Show_Str_Mid(10,270,UART4_RX_BUF,16,240);//显示接收到的数据

		} else if (LoRa_CFG.mode_sta==LORA_STA_Dire) {
			//定向传输
			//将十六进制的数据转化为字符串打印在lcd_buff数组
			sprintf((char*)rlcd_buff, "%x %x %x %x %x",
				UART4_RX_BUF[0], UART4_RX_BUF[1], UART4_RX_BUF[2], UART4_RX_BUF[3], UART4_RX_BUF[4]);

			Show_Str_Mid(10, 270, rlcd_buff, 16, 240);//显示接收到的数据
		}
		memset((char*)UART4_RX_BUF, 0x00, len);//串口接收缓冲区清0
	}
}

//发送和接收处理
void LoRa_Process(void)
{
//	static u8 num = 0;
	u8 key = 0;
	u8 t = 0;

//	DATA:
//	Process_ui();//界面显示
	LoRa_Set();//LoRa配置(进入配置需设置串口波特率为115200)
	
	while (1) {

		key = KEY_Scan(0);
		
		//发送数据
		if (key == KEY1_PRES) {
			//空闲且非省电模式
			if(!LORA_AUX&&(LoRa_CFG.mode != LORA_MODE_SLEEP)) {
				Lora_mode = 2;   //标记"发送状态"
				LoRa_SendData(); //发送数据    
			}
		}
		
//		//透明传输
//		if (LoRa_CFG.mode_sta == LORA_STA_Tran) {

//			sprintf((char*)Tran_Data, "%d", num);
//			u3_printf("%s\r\n", Tran_Data);
//			LCD_Fill(0, 195, 240, 220, WHITE);         //清除显示
//			Show_Str_Mid(10, 195, Tran_Data, 16, 240); //显示发送的数据

//			num++;
//			if(num == 255)
//				num = 0;
//		}
//		//数据接收
//		LoRa_ReceData();

		t++;
		if (t == 20) {
			t = 0;
			LED1 = ~LED1;
		}			
		delay_ms(10);		
	}
}

//主测试函数
void Lora_Test(void)
{	
	u8 t = 0;

	LCD_Clear(WHITE);
	BACK_COLOR = RED;

	//初始化ATK-LORA-01模块
	while (LoRa_Init()) {
		Show_Str(40 + 30, 50 + 20, 200, 16, "未检测到模块!!!",  16, 0); 	
		delay_ms(300);
		Show_Str(40 + 30, 50 + 20, 200, 16, "                ", 16, 0);
	}
	
	Show_Str(40 + 30, 50 + 20, 200, 16, "检测到模块!!!",  16, 0); 	
	delay_ms(300);
	Show_Str(40 + 30, 50 + 20, 200, 16, "                ", 16, 0);

	while (1) {
		LoRa_Process();//开始数据测试
		t++;
		if (t == 30) {
			t = 0;
			LED1 = ~LED1;
		}
		delay_ms(10);
	}
}
