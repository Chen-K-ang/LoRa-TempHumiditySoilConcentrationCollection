#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "usmart.h" 
#include "malloc.h"  
#include "ff.h"  
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"	 
#include "usart2.h"  
#include "soil.h"
#include "i2c_ee.h"	
#include "lora_app.h"

//C库
#include <string.h>
#include <time.h>
#include <stdio.h>
 
#include "hwtimer.h"


//网络设备
#include "net_device.h"

#include "mcu_gpio.h"
//协议
#include "onenet.h"
#include "fault.h"
#include "nb_cmd.h"

//写命令和执行命令的回调
#include "nb_callback.h"

terminal_INFO terminal_Info;

//数据流
DATA_STREAM data_stream[] = 
{
//	{"3200", "0", "5502",&Led_Info.Led_Status, TYPE_BOOL, 1, CALLBACK_Led, 	NULL},//RW-Led
//	{"3303", "0", "5700",&terminal_Info.temp1_env,TYPE_FLOAT,1,NULL, 		NULL},//终端1-环境温度
//	{"3303", "1", "5700",&terminal_Info.temp1_soil,TYPE_FLOAT,1,NULL, 		NULL},//终端1-土壤温度
	
	{"3303", "0", "5700",&terminal_Info.soil_temperature, TYPE_FLOAT,1,NULL, NULL},//终端1-环境温度
	{"3304", "0", "5700",&terminal_Info.soil_humidity,    TYPE_FLOAT,1,NULL, NULL},//终端1-土壤湿度
	{"3202", "0", "5600",&terminal_Info.soil_ph,          TYPE_FLOAT,1,NULL, NULL},//终端1-土壤PH
	{"3202", "1", "5600",&terminal_Info.soil_Nval,        TYPE_FLOAT,1,NULL, NULL},//终端1-土壤氮浓度
	{"3202", "2", "5600",&terminal_Info.soil_Pval,        TYPE_FLOAT,1,NULL, NULL},//终端1-土壤磷浓度
	{"3202", "3", "5600",&terminal_Info.soil_Kval,        TYPE_FLOAT,1,NULL, NULL},//终端1-土壤钾浓度
	
};
unsigned char data_stream_cnt = sizeof(data_stream) / sizeof(data_stream[0]);

u8 Run_state = 0;
u8 Task_state = 0;
u8 buffer[3];
float lm75a_temp=0;


void Hardwave_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断优先级分组2 
	delay_init();	    	 					   	//延时函数初始化	  
	uart_init(115200);								//串口1，打印信息用
	Lcd_Init();			                           	//初始化液晶
	LCD_Clear(BACK_COLOR);						   	//刷新屏幕背景色
	LED_Init();                                     //LED初始化	 
	KEY_Init();										//按键初始化
//	usmart_dev.init(72);							//usmart初始化	
	mem_init();										//初始化内存池	
	exfuns_init();									//为fatfs相关变量申请内存  
	f_mount(fs[0],"0:",1); 							//挂载SD卡 
	f_mount(fs[1],"1:",1); 							//挂载FLASH
	/*****************************传感器初始化程序**************************/
	usart2_init(9600);	//初始化USART2用来与土壤变送器进行通讯
	IIC_Init();
	
	while (LoRa_Init()) {
		Show_Str(0, 0, 240-1, 24, "未检测到LORA模块", 24, 0);
	}
	
	Show_Str(0, 0, 240-1, 24, "检测到LORA模块", 24, 0);
	LoRa_Set();//LoRa配置(进入配置需设置串口波特率为115200)
}

void Main_Menu(void)
{
	u8 show_x=0,show_y=0,num=0,size=0;
	LCD_Clear(BACK_COLOR);
	num=8;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"土壤环境检测系统",24,0);show_y+=size;
	show_y+=32;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"传感器校准程序",24,0);show_y+=size;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"传感器参数显示",24,0);show_y+=size;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"传感器阈值设置",24,0);show_y+=size;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"传感器数据存储",24,0);show_y+=size;
}

void Sensor_MenuInit(void)
{
	u8 show_x=0,show_y=0,num=0,size=0;
	LCD_Clear(BACK_COLOR);
	num=8;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"土壤环境检测系统",24,0);show_y+=size;
	show_x=0;show_y+=32;
	Show_Str(show_x,show_y,240-1,24,"土壤温度  :",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"土壤湿度  :",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"土壤酸碱度:",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"土壤氮浓度:",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"土壤磷浓度:",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"土壤钾浓度:",24,0);show_y+=size;
}

void Sensor_Adjust(void)
{
	u8 show_x=0,show_y=0,num=0,size=0;
	LCD_Clear(BACK_COLOR);
	num=8;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"土壤环境检测系统",24,0);show_y+=size;
	show_x=0;show_y+=32;
	Show_Str(show_x,show_y,240-1,24,"按键1退出校准界面",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"按键2校准传感器",24,0);show_y+=size;
	
}

void SensorData_Show(void)
{
	u8 show_x=0,show_y=0,size=0;
	u8 str[20];
	show_x=128;show_y=32;size=24;
	memset(str,0,sizeof(str));
	sprintf((char *)str,"%.2fC",lm75a_temp);
	LCD_ShowString(show_x,show_y,str,POINT_COLOR);show_y+=size;
	memset(str,0,sizeof(str));
	sprintf((char *)str,"%.2fC",SOIL_Sensor.temp);
	LCD_ShowString(show_x,show_y,str,POINT_COLOR);show_y+=size;
	memset(str,0,sizeof(str));
	sprintf((char *)str,"%.2f%%",SOIL_Sensor.humidity);
	LCD_ShowString(show_x,show_y,str,POINT_COLOR);show_y+=size;
	memset(str,0,sizeof(str));
	sprintf((char *)str,"%.2f",SOIL_Sensor.ph_val);
	LCD_ShowString(show_x,show_y,str,POINT_COLOR);show_y+=size;
	memset(str,0,sizeof(str));
	sprintf((char *)str,"%.2fmol/L",SOIL_Sensor.N_val);
	LCD_ShowString(show_x,show_y,str,POINT_COLOR);show_y+=size;
	memset(str,0,sizeof(str));
	sprintf((char *)str,"%.2fmol/L",SOIL_Sensor.P_val);
	LCD_ShowString(show_x,show_y,str,POINT_COLOR);show_y+=size;
	memset(str,0,sizeof(str));
	sprintf((char *)str,"%.2fmol/L",SOIL_Sensor.K_val);
	LCD_ShowString(show_x,show_y,str,POINT_COLOR);show_y+=size;
	
}

void Adjust_Sensor(void)
{
	;
}
 


int main(void)
{ 
	u16 send_time= 0,update_time =0,sensor_time = 0;		  
	u8 i;
	u8 key;
	u8 str[10];
	Hardwave_Init();	//硬件外设初始化
	/*****************检查W25Q64字库文件*****************/
	while(font_init()) 	//检查字库
	{
		LCD_Clear(BACK_COLOR);	//清屏
		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(0,0,"mini STM32",16);
		while(SD_Initialize())	//检测SD卡
		{
			LCD_ShowString(0,16,"SD Card Failed!",16);
			delay_ms(200);
			LCD_Fill(0,16,240-1,16+16,WHITE);
			delay_ms(200);		    
		}		
		LCD_ShowString(0,16,"SD Card OK",16);
		LCD_ShowString(0,32,"Font Updating...",16);
		key=update_font(20,110,16);//更新字库
		while(key)//更新失败		
		{		
			LCD_ShowString(0,32+16,"Font Update Failed!",16);
			delay_ms(200);
			LCD_Fill(0,32+16,240-1,32+16+16,WHITE);
			delay_ms(200);		       
		} 
		LCD_ShowString(0,32+16,"Font Update Success!",16);
		delay_ms(1000);	
		LCD_Clear(BACK_COLOR);//清屏	       
	}  
	Show_Str(0,0,240-1,24,"硬件初始化完成!!!",16,0);
	
	/******************检查完毕*******************/
	Timer3_4_Init(TIM3, 9999, 71);		//64分频，每秒625次，重载1250次，2s	
	Timer3_4_Init(TIM4, 1999, 35999);	//1S	
	UsartPrintf(USART_DEBUG, "初始化完成\r\n");							//提示初始化完成	 
	Main_Menu();
	while(1)
	{
		/****************按键扫描***************/
		if(Run_state==0)
		{
			key = KEY_Scan(0);
			if(key == KEY0_PRES)
			{
				Task_state++;
				if(Task_state>3)
					Task_state=0;
			}
			if(key == KEY1_PRES)
			{
				Task_state--;
				if(Task_state==0XFF)
					Task_state=3;
			}
			if(key == WKUP_PRES)
			{
				Run_state=1;
			}
			memset(str,0,sizeof(str));
			sprintf((char *)str,"Task:%d",Task_state);
			LCD_ShowString(0,223,str,16);
		}
		/****************运行程序***************/
		else if(Run_state==1)
		{
			switch(Task_state)
			{
				case(0):
				{
					Sensor_Adjust();
					while(1)
					{
						key = KEY_Scan(0);
						if(key == WKUP_PRES)
						{
							Run_state=0;
							break;
						}
						/*数据校准操作*/
						if(key == KEY0_PRES)
						{
							Show_Str(23,104,240-1,24,"开始校准",24,0);
							Adjust_Sensor();
							delay_ms(500);
							Show_Str(23,128,240-1,24,"校准完成",24,0);
						}
					}
					Main_Menu();
					break;
				}
				case(1):
				{
					Sensor_MenuInit();
					MCU_GPIO_Output_Ctl("nd_pwr", 0);
					delay_ms(200);
					while(1)
					{
						key = KEY_Scan(0);
						if(key == WKUP_PRES)
						{
							Run_state=0;
							break;
						}
						/*
						//获取信号质量
						if(timInfo.timer6Out - signal_time >= 2500)	//5s一次(25ms中断)
						{
							signal_time = timInfo.timer6Out;
							if(net_device_info.net_work)
							{
								NET_DEVICE_GetSignal();
							}
						}
						*/
						/*****************传感器数据采集******************/
						if(i%20==0)
						{
							lm75a_temp = 0.125*(I2C_LM75_Read());
							while(1)
							{
								Soil_SendCommand(SOIL_SensorADDR,0X03,HUMIDITY_REG,7);//获取土壤传感器数据
								delay_ms(100);
								//UsartPrintf(USART_DEBUG, "Get SoilData\r\n");
								if(USART2_RX_STA&0X8000)break;
							}
							Soil_ValAnalysis(&SOIL_Sensor);//采集土壤传感器数据
							USART2_RX_STA=0;
//							SOIL_Sensor.temp = lm75a_temp;
							SensorData_Show();		
						}
						/************************END***************************/
						
						/******************************************************
											   数据发送
						******************************************************/
						if(onenet_info.net_work == 1)
						{
							if(timInfo.timer6Out - send_time >= 1200)//3s一次(25ms中断)
							{
								send_time = timInfo.timer6Out;
								
								onenet_info.send_data = SEND_TYPE_DATA;//上传数据到平台
								
								//发送至从机
								LoRa_SendData();
							}
							
							switch(onenet_info.send_data)
							{
								case SEND_TYPE_DATA:
									 onenet_info.send_data = OneNet_SendData(data_stream, data_stream_cnt);//上传数据到平台
								break;
							}
							
							/*******************************************************
													传感器
							*******************************************************/
							if(timInfo.timer6Out - sensor_time >= 400)//1s一次(25ms中断)
							{
								sensor_time = timInfo.timer6Out;
								terminal_Info.soil_temperature=SOIL_Sensor.temp;
								terminal_Info.soil_humidity=SOIL_Sensor.humidity;
								terminal_Info.soil_ph = SOIL_Sensor.ph_val;
								terminal_Info.soil_Nval = SOIL_Sensor.N_val;
								terminal_Info.soil_Pval = SOIL_Sensor.P_val;
								terminal_Info.soil_Kval = SOIL_Sensor.K_val;
								
								//lora数据赋值
								lora_tx_data.humidity.f_data = SOIL_Sensor.humidity;
								lora_tx_data.N_val.f_data    = SOIL_Sensor.N_val;
								lora_tx_data.P_val.f_data    = SOIL_Sensor.P_val;
								lora_tx_data.K_val.f_data    = SOIL_Sensor.K_val;
							}
								
							/******************************************************
												   错误处理
							******************************************************/
							if(net_fault_info.net_fault_level != NET_FAULT_LEVEL_0)	//如果错误标志被设置
							{
								UsartPrintf(USART_DEBUG, "WARN:	NET Fault Process---%d\r\n", net_fault_info.net_fault_level);
								
								NET_Fault_Process();	//进入错误处理函数
							}
							
							/*****************************************************
												   发送心跳
							******************************************************/
							if(timInfo.timer6Out - update_time >= 2000)	//5s一次(25ms中断)
							{
								update_time = timInfo.timer6Out;
								if(OneNet_Update(30) == 0)
								{
									UsartPrintf(USART_DEBUG, "Update Time Ok\r\n");
								}
								else
									UsartPrintf(USART_DEBUG, "Update Time Err\r\n");
							}
						}
						
						/******************************************************
												网络连接
						*******************************************************/
						if(onenet_info.net_work == 0)
						{
							NET_DEVICE_IO_Init();	
							if(!onenet_info.net_work && (net_device_info.device_ok == 1))//当没有网络 且 网络模块检测到时
							{
								if(!NET_DEVICE_Init(data_stream, data_stream_cnt))//初始化网络设备，能连入网络
								{
									if(net_device_info.net_work)
									{
										onenet_info.net_work = 1;
										net_fault_info.net_fault_flag = 1;
										onenet_info.reinit_count = 0;
										send_time 	= timInfo.timer6Out;	//更新时间
										update_time = timInfo.timer6Out;	//更新时间
										sensor_time = timInfo.timer6Out;	//更新时间
										//signal_time = timInfo.timer6Out;	//更新时间
									}
								}
							}
							//当网络设备未做检测
							if(net_device_info.device_ok == 0)	
							{
								if(!NET_DEVICE_Exist())	//网络设备检测
								{
									UsartPrintf(USART_DEBUG, "NET Device :Ok\r\n");
									net_device_info.device_ok = 1;	//检测到网络设备，标记
								}
								else
								{
									UsartPrintf(USART_DEBUG, "NET Device :Error\r\n");
								}
							}
						}
						memset(str,0,sizeof(str));
						sprintf((char *)str,"Task:%d",Run_state);
						LCD_ShowString(0,223,str,16);
						
						i++;
						if(i%20==0)
						{
							LED0=!LED0;
						}
						delay_ms(5);
					}
					//注销设备
					{
						NET_DEVICE_SendCmd("AT+MIPLCLOSE=0\r\n", "OK");
						NET_DEVICE_SendCmd("AT+MIPLDELOBJ=0,3200\r\n", "OK");
						NET_DEVICE_SendCmd("AT+MIPLDEL=0\r\n", "OK");
						net_device_info.device_ok=0;
						onenet_info.net_work = 0;
					}
					Main_Menu();
					break;
				}
				case(2):
				{
					while(1)
					{
						key = KEY_Scan(0);
						if(key == WKUP_PRES)
						{
							Run_state=0;
							break;
						}
					}
					break;
				}
			}
		}
		/************************END***************************/
		i++;
		if(i%20==0)
		{
			LED0=!LED0;
		}
		delay_ms(5);
	} 
}
