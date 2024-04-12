#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "led.h"
#include "dht11.h"
#include "delay.h"
#include "key.h"

u8 temp_min = 10,temp_max = 40,humi_min = 12,humi_max = 60;//温湿度上下限初始值

int main(void)
{
	unsigned char i=0;
	char keynum = 0,setN = 0;
	unsigned char temperature,humidity;
	unsigned int  count = 0,mode_cnt = 0;
	unsigned char temp_alarm = 0,humi_alarm = 0;
	
	DelayInit(); //延时初始化
	BEEP_GPIO_Config();//蜂鸣器初始化
	BEEP(OFF);//关闭蜂鸣器
	I2C_Configuration();//IIC初始化
	OLED_Init();//OLED屏幕初始化
	KEY_Init();//按键初始化
	while(DHT11_Init());//DHT11初始化
	DelayMs(200);
  OLED_CLS();//清屏
	for(i=0;i<6;i++)OLED_ShowCN(i*16+8,0,i+1);//显示汉字，“环境监测系统”，请查看codetab.h字库
	OLED_ShowStr(0,2,"Temp:   C",2);
	OLED_ShowStr(0,4,"Humi:   %",2);
	
	while(1)
	{
		  keynum = KEY_Scan(0);//获取按键值
		  if(keynum == 1)//设置键按下
			{
				  BEEP(ON);//蜂鸣器响
					DelayMs(100);//延时
					BEEP(OFF);//关闭蜂鸣器
				
					setN++;//记录按键按下次数
				  if(setN > 4)//大于4次，退出设置
						setN = 0;
					if(setN == 3 || setN == 0 || setN == 1)
						OLED_CLS();//清屏
					if(setN != 0)
					{
							if(setN < 3)
							{
									if(setN == 1)
									{
											OLED_ShowCN(96,2,0);//显示箭头，请查看codetab.h字库
									}
									if(setN == 2)
									{
											OLED_ShowStr(96,2,"  ",2);
											OLED_ShowCN(96,6,0);//显示“->”
									}
									for(i=0;i<4;i++)OLED_ShowCN(i*16+38,0,i+7);//显示汉字，“设置温度”，请查看codetab.h字库
									for(i=0;i<2;i++)OLED_ShowCN(i*16,2,i+15);//显示汉字，“上限”，请查看codetab.h字库
									OLED_ShowStr(35,2,":   C",2);
									for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+17);//显示汉字，“下限”，请查看codetab.h字库
									OLED_ShowStr(35,6,":   C",2);
									
									OLED_ShowTH(48,2,temp_max);//显示值
									OLED_ShowTH(48,6,temp_min);
							}
							else
							{
									if(setN == 3)
									{
											OLED_ShowCN(96,2,0);//显示“->”
									}
									if(setN == 4)
									{
											OLED_ShowStr(96,2,"  ",2);
											OLED_ShowCN(96,6,0);//显示“->”
									}
									for(i=0;i<4;i++)OLED_ShowCN(i*16+38,0,i+11);//显示汉字，“设置湿度”，请查看codetab.h字库
									for(i=0;i<2;i++)OLED_ShowCN(i*16,2,i+15);//显示汉字，“上限”，请查看codetab.h字库
									OLED_ShowStr(35,2,":   %",2);
									for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+17);//显示汉字，“下限”，请查看codetab.h字库
									OLED_ShowStr(35,6,":   %",2);
									
									OLED_ShowTH(48,2,humi_max);//显示值
									OLED_ShowTH(48,6,humi_min);
							}
				}
				else
				{
						for(i=0;i<6;i++)OLED_ShowCN(i*16+8,0,i+1);//显示汉字，“环境监测系统”，请查看codetab.h字库
						OLED_ShowStr(0,2,"Temp:   C",2);
						OLED_ShowStr(0,4,"Humi:   %",2);
				}
			}
			if(setN != 0)
			{
					if(keynum == 2)//按键加按下
					{
							BEEP(ON);
							DelayMs(100);
							BEEP(OFF);
						
							if(setN == 1)//设置温度上限
							{
									if(temp_max < 99)//上限值最大到99
										temp_max++;
									OLED_ShowTH(48,2,temp_max);//显示
							}
							if(setN == 2)//设置温度下限
							{
									if(temp_min < temp_max)//上限值必须大于上限值，下限值才能加
										temp_min++;
									OLED_ShowTH(48,6,temp_min);//显示
							}
							if(setN == 3)//设置湿度上限
							{
									if(humi_max < 99)
										humi_max++;
									OLED_ShowTH(48,2,humi_max);
							}
							if(setN == 4)//设置湿度下限
							{
									if(humi_min < humi_max)
										humi_min++;
									OLED_ShowTH(48,6,humi_min);
							}
					}
					if(keynum == 3)//按键减按下
					{
							BEEP(ON);
							DelayMs(100);
							BEEP(OFF);
						
							if(setN == 1)//设置温度上限
							{
									if(temp_max > temp_min)//上限值必须大于上限值，上限值才能减
										temp_max--;
									OLED_ShowTH(48,2,temp_max);//显示
							}
							if(setN == 2)//设置温度下限
							{
									if(temp_min > 0)//下限值最小到0
										temp_min--;
									OLED_ShowTH(48,6,temp_min);//显示
							}
							if(setN == 3)//设置湿度上限
							{
									if(humi_max > humi_min)
										humi_max--;
									OLED_ShowTH(48,2,humi_max);//显示
							}
							if(setN == 4)//设置湿度下限
							{
									if(humi_min > 0)
										humi_min--;
									OLED_ShowTH(48,6,humi_min);
							}
					}
		}
		  if(count++ > 500)//延时一段时间读取温湿度，并显示
			{
				  count = 0;
					DHT11_Read_Data(&temperature,&humidity);//读取温湿度
					if(setN == 0)//必须是不在设置模式下，才能正常显示温湿度值
					{
							OLED_ShowTH(47,2,temperature);//显示温度
							OLED_ShowTH(47,4,humidity);//显示湿度
							
							if(temperature <= temp_min || temperature >= temp_max)
							{
									temp_alarm = 1;//温度超限标志置1
							}
							else
							{
									temp_alarm = 0;
							}
							
							if(humidity <= humi_min || humidity >= humi_max)
							{
									humi_alarm = 1;//湿度超限标志置1
							}
							else
							{
									humi_alarm = 0;
							}
							
							if(temp_alarm == 1 && humi_alarm == 1)//如果两个同时超标，屏幕交替显示报警
							{
									mode_cnt++;
								  if(mode_cnt > 6)
										mode_cnt = 0;
									
									if(mode_cnt <= 3)
									{
										if(mode_cnt > 2)
											OLED_ShowStr(0,6,"                ",2);
										else
											OLED_ShowStr(0,6," Temp Overrun!  ",2);//显示温度报警
									}
									else
									{
										if(mode_cnt > 5)
											OLED_ShowStr(0,6,"                ",2);
										else
											OLED_ShowStr(0,6," Humi Overrun!  ",2);//显示湿度报警
									}
							}
							else
							{
								  mode_cnt = 0;
								
									if(temp_alarm == 1)//温度超限，显示温度报警
									{
											OLED_ShowStr(0,6," Temp Overrun!  ",2);
									}
									if(humi_alarm == 1)//湿度超限，显示湿度报警
									{
											OLED_ShowStr(0,6," Humi Overrun!  ",2);
									}
							}
							
							if(temp_alarm != 0 || humi_alarm != 0)//温湿度，任意一个超标，蜂鸣器响
							{
								  BEEP(ON);
							}
							else
							{
									OLED_ShowStr(0,6,"State: normal  ",2);//温湿度正常，屏幕显示正常
								  BEEP(OFF);
							}
					}
			}
			DelayMs(1);
	}
}
