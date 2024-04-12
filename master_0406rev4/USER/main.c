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

//C��
#include <string.h>
#include <time.h>
#include <stdio.h>
 
#include "hwtimer.h"


//�����豸
#include "net_device.h"

#include "mcu_gpio.h"
//Э��
#include "onenet.h"
#include "fault.h"
#include "nb_cmd.h"

//д�����ִ������Ļص�
#include "nb_callback.h"

terminal_INFO terminal_Info;

//������
DATA_STREAM data_stream[] = 
{
//	{"3200", "0", "5502",&Led_Info.Led_Status, TYPE_BOOL, 1, CALLBACK_Led, 	NULL},//RW-Led
//	{"3303", "0", "5700",&terminal_Info.temp1_env,TYPE_FLOAT,1,NULL, 		NULL},//�ն�1-�����¶�
//	{"3303", "1", "5700",&terminal_Info.temp1_soil,TYPE_FLOAT,1,NULL, 		NULL},//�ն�1-�����¶�
	
	{"3303", "0", "5700",&terminal_Info.soil_temperature, TYPE_FLOAT,1,NULL, NULL},//�ն�1-�����¶�
	{"3304", "0", "5700",&terminal_Info.soil_humidity,    TYPE_FLOAT,1,NULL, NULL},//�ն�1-����ʪ��
	{"3202", "0", "5600",&terminal_Info.soil_ph,          TYPE_FLOAT,1,NULL, NULL},//�ն�1-����PH
	{"3202", "1", "5600",&terminal_Info.soil_Nval,        TYPE_FLOAT,1,NULL, NULL},//�ն�1-������Ũ��
	{"3202", "2", "5600",&terminal_Info.soil_Pval,        TYPE_FLOAT,1,NULL, NULL},//�ն�1-������Ũ��
	{"3202", "3", "5600",&terminal_Info.soil_Kval,        TYPE_FLOAT,1,NULL, NULL},//�ն�1-������Ũ��
	
};
unsigned char data_stream_cnt = sizeof(data_stream) / sizeof(data_stream[0]);

u8 Run_state = 0;
u8 Task_state = 0;
u8 buffer[3];
float lm75a_temp=0;


void Hardwave_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж����ȼ�����2 
	delay_init();	    	 					   	//��ʱ������ʼ��	  
	uart_init(115200);								//����1����ӡ��Ϣ��
	Lcd_Init();			                           	//��ʼ��Һ��
	LCD_Clear(BACK_COLOR);						   	//ˢ����Ļ����ɫ
	LED_Init();                                     //LED��ʼ��	 
	KEY_Init();										//������ʼ��
//	usmart_dev.init(72);							//usmart��ʼ��	
	mem_init();										//��ʼ���ڴ��	
	exfuns_init();									//Ϊfatfs��ر��������ڴ�  
	f_mount(fs[0],"0:",1); 							//����SD�� 
	f_mount(fs[1],"1:",1); 							//����FLASH
	/*****************************��������ʼ������**************************/
	usart2_init(9600);	//��ʼ��USART2��������������������ͨѶ
	IIC_Init();
	
	while (LoRa_Init()) {
		Show_Str(0, 0, 240-1, 24, "δ��⵽LORAģ��", 24, 0);
	}
	
	Show_Str(0, 0, 240-1, 24, "��⵽LORAģ��", 24, 0);
	LoRa_Set();//LoRa����(�������������ô��ڲ�����Ϊ115200)
}

void Main_Menu(void)
{
	u8 show_x=0,show_y=0,num=0,size=0;
	LCD_Clear(BACK_COLOR);
	num=8;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"�����������ϵͳ",24,0);show_y+=size;
	show_y+=32;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"������У׼����",24,0);show_y+=size;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"������������ʾ",24,0);show_y+=size;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"��������ֵ����",24,0);show_y+=size;
	num=7;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"���������ݴ洢",24,0);show_y+=size;
}

void Sensor_MenuInit(void)
{
	u8 show_x=0,show_y=0,num=0,size=0;
	LCD_Clear(BACK_COLOR);
	num=8;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"�����������ϵͳ",24,0);show_y+=size;
	show_x=0;show_y+=32;
	Show_Str(show_x,show_y,240-1,24,"�����¶�  :",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"����ʪ��  :",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"��������:",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"������Ũ��:",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"������Ũ��:",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"������Ũ��:",24,0);show_y+=size;
}

void Sensor_Adjust(void)
{
	u8 show_x=0,show_y=0,num=0,size=0;
	LCD_Clear(BACK_COLOR);
	num=8;size=24;show_x = ((240-size*num)/2)-1;
	Show_Str(show_x,show_y,240-1,24,"�����������ϵͳ",24,0);show_y+=size;
	show_x=0;show_y+=32;
	Show_Str(show_x,show_y,240-1,24,"����1�˳�У׼����",24,0);show_y+=size;
	Show_Str(show_x,show_y,240-1,24,"����2У׼������",24,0);show_y+=size;
	
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
	Hardwave_Init();	//Ӳ�������ʼ��
	/*****************���W25Q64�ֿ��ļ�*****************/
	while(font_init()) 	//����ֿ�
	{
		LCD_Clear(BACK_COLOR);	//����
		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		LCD_ShowString(0,0,"mini STM32",16);
		while(SD_Initialize())	//���SD��
		{
			LCD_ShowString(0,16,"SD Card Failed!",16);
			delay_ms(200);
			LCD_Fill(0,16,240-1,16+16,WHITE);
			delay_ms(200);		    
		}		
		LCD_ShowString(0,16,"SD Card OK",16);
		LCD_ShowString(0,32,"Font Updating...",16);
		key=update_font(20,110,16);//�����ֿ�
		while(key)//����ʧ��		
		{		
			LCD_ShowString(0,32+16,"Font Update Failed!",16);
			delay_ms(200);
			LCD_Fill(0,32+16,240-1,32+16+16,WHITE);
			delay_ms(200);		       
		} 
		LCD_ShowString(0,32+16,"Font Update Success!",16);
		delay_ms(1000);	
		LCD_Clear(BACK_COLOR);//����	       
	}  
	Show_Str(0,0,240-1,24,"Ӳ����ʼ�����!!!",16,0);
	
	/******************������*******************/
	Timer3_4_Init(TIM3, 9999, 71);		//64��Ƶ��ÿ��625�Σ�����1250�Σ�2s	
	Timer3_4_Init(TIM4, 1999, 35999);	//1S	
	UsartPrintf(USART_DEBUG, "��ʼ�����\r\n");							//��ʾ��ʼ�����	 
	Main_Menu();
	while(1)
	{
		/****************����ɨ��***************/
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
		/****************���г���***************/
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
						/*����У׼����*/
						if(key == KEY0_PRES)
						{
							Show_Str(23,104,240-1,24,"��ʼУ׼",24,0);
							Adjust_Sensor();
							delay_ms(500);
							Show_Str(23,128,240-1,24,"У׼���",24,0);
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
						//��ȡ�ź�����
						if(timInfo.timer6Out - signal_time >= 2500)	//5sһ��(25ms�ж�)
						{
							signal_time = timInfo.timer6Out;
							if(net_device_info.net_work)
							{
								NET_DEVICE_GetSignal();
							}
						}
						*/
						/*****************���������ݲɼ�******************/
						if(i%20==0)
						{
							lm75a_temp = 0.125*(I2C_LM75_Read());
							while(1)
							{
								Soil_SendCommand(SOIL_SensorADDR,0X03,HUMIDITY_REG,7);//��ȡ��������������
								delay_ms(100);
								//UsartPrintf(USART_DEBUG, "Get SoilData\r\n");
								if(USART2_RX_STA&0X8000)break;
							}
							Soil_ValAnalysis(&SOIL_Sensor);//�ɼ���������������
							USART2_RX_STA=0;
//							SOIL_Sensor.temp = lm75a_temp;
							SensorData_Show();		
						}
						/************************END***************************/
						
						/******************************************************
											   ���ݷ���
						******************************************************/
						if(onenet_info.net_work == 1)
						{
							if(timInfo.timer6Out - send_time >= 1200)//3sһ��(25ms�ж�)
							{
								send_time = timInfo.timer6Out;
								
								onenet_info.send_data = SEND_TYPE_DATA;//�ϴ����ݵ�ƽ̨
								
								//�������ӻ�
								LoRa_SendData();
							}
							
							switch(onenet_info.send_data)
							{
								case SEND_TYPE_DATA:
									 onenet_info.send_data = OneNet_SendData(data_stream, data_stream_cnt);//�ϴ����ݵ�ƽ̨
								break;
							}
							
							/*******************************************************
													������
							*******************************************************/
							if(timInfo.timer6Out - sensor_time >= 400)//1sһ��(25ms�ж�)
							{
								sensor_time = timInfo.timer6Out;
								terminal_Info.soil_temperature=SOIL_Sensor.temp;
								terminal_Info.soil_humidity=SOIL_Sensor.humidity;
								terminal_Info.soil_ph = SOIL_Sensor.ph_val;
								terminal_Info.soil_Nval = SOIL_Sensor.N_val;
								terminal_Info.soil_Pval = SOIL_Sensor.P_val;
								terminal_Info.soil_Kval = SOIL_Sensor.K_val;
								
								//lora���ݸ�ֵ
								lora_tx_data.humidity.f_data = SOIL_Sensor.humidity;
								lora_tx_data.N_val.f_data    = SOIL_Sensor.N_val;
								lora_tx_data.P_val.f_data    = SOIL_Sensor.P_val;
								lora_tx_data.K_val.f_data    = SOIL_Sensor.K_val;
							}
								
							/******************************************************
												   ������
							******************************************************/
							if(net_fault_info.net_fault_level != NET_FAULT_LEVEL_0)	//��������־������
							{
								UsartPrintf(USART_DEBUG, "WARN:	NET Fault Process---%d\r\n", net_fault_info.net_fault_level);
								
								NET_Fault_Process();	//�����������
							}
							
							/*****************************************************
												   ��������
							******************************************************/
							if(timInfo.timer6Out - update_time >= 2000)	//5sһ��(25ms�ж�)
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
												��������
						*******************************************************/
						if(onenet_info.net_work == 0)
						{
							NET_DEVICE_IO_Init();	
							if(!onenet_info.net_work && (net_device_info.device_ok == 1))//��û������ �� ����ģ���⵽ʱ
							{
								if(!NET_DEVICE_Init(data_stream, data_stream_cnt))//��ʼ�������豸������������
								{
									if(net_device_info.net_work)
									{
										onenet_info.net_work = 1;
										net_fault_info.net_fault_flag = 1;
										onenet_info.reinit_count = 0;
										send_time 	= timInfo.timer6Out;	//����ʱ��
										update_time = timInfo.timer6Out;	//����ʱ��
										sensor_time = timInfo.timer6Out;	//����ʱ��
										//signal_time = timInfo.timer6Out;	//����ʱ��
									}
								}
							}
							//�������豸δ�����
							if(net_device_info.device_ok == 0)	
							{
								if(!NET_DEVICE_Exist())	//�����豸���
								{
									UsartPrintf(USART_DEBUG, "NET Device :Ok\r\n");
									net_device_info.device_ok = 1;	//��⵽�����豸�����
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
					//ע���豸
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
