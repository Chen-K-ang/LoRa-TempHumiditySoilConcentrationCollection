#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "led.h"
#include "dht11.h"
#include "delay.h"
#include "key.h"

u8 temp_min = 10,temp_max = 40,humi_min = 12,humi_max = 60;//��ʪ�������޳�ʼֵ

int main(void)
{
	unsigned char i=0;
	char keynum = 0,setN = 0;
	unsigned char temperature,humidity;
	unsigned int  count = 0,mode_cnt = 0;
	unsigned char temp_alarm = 0,humi_alarm = 0;
	
	DelayInit(); //��ʱ��ʼ��
	BEEP_GPIO_Config();//��������ʼ��
	BEEP(OFF);//�رշ�����
	I2C_Configuration();//IIC��ʼ��
	OLED_Init();//OLED��Ļ��ʼ��
	KEY_Init();//������ʼ��
	while(DHT11_Init());//DHT11��ʼ��
	DelayMs(200);
  OLED_CLS();//����
	for(i=0;i<6;i++)OLED_ShowCN(i*16+8,0,i+1);//��ʾ���֣����������ϵͳ������鿴codetab.h�ֿ�
	OLED_ShowStr(0,2,"Temp:   C",2);
	OLED_ShowStr(0,4,"Humi:   %",2);
	
	while(1)
	{
		  keynum = KEY_Scan(0);//��ȡ����ֵ
		  if(keynum == 1)//���ü�����
			{
				  BEEP(ON);//��������
					DelayMs(100);//��ʱ
					BEEP(OFF);//�رշ�����
				
					setN++;//��¼�������´���
				  if(setN > 4)//����4�Σ��˳�����
						setN = 0;
					if(setN == 3 || setN == 0 || setN == 1)
						OLED_CLS();//����
					if(setN != 0)
					{
							if(setN < 3)
							{
									if(setN == 1)
									{
											OLED_ShowCN(96,2,0);//��ʾ��ͷ����鿴codetab.h�ֿ�
									}
									if(setN == 2)
									{
											OLED_ShowStr(96,2,"  ",2);
											OLED_ShowCN(96,6,0);//��ʾ��->��
									}
									for(i=0;i<4;i++)OLED_ShowCN(i*16+38,0,i+7);//��ʾ���֣��������¶ȡ�����鿴codetab.h�ֿ�
									for(i=0;i<2;i++)OLED_ShowCN(i*16,2,i+15);//��ʾ���֣������ޡ�����鿴codetab.h�ֿ�
									OLED_ShowStr(35,2,":   C",2);
									for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+17);//��ʾ���֣������ޡ�����鿴codetab.h�ֿ�
									OLED_ShowStr(35,6,":   C",2);
									
									OLED_ShowTH(48,2,temp_max);//��ʾֵ
									OLED_ShowTH(48,6,temp_min);
							}
							else
							{
									if(setN == 3)
									{
											OLED_ShowCN(96,2,0);//��ʾ��->��
									}
									if(setN == 4)
									{
											OLED_ShowStr(96,2,"  ",2);
											OLED_ShowCN(96,6,0);//��ʾ��->��
									}
									for(i=0;i<4;i++)OLED_ShowCN(i*16+38,0,i+11);//��ʾ���֣�������ʪ�ȡ�����鿴codetab.h�ֿ�
									for(i=0;i<2;i++)OLED_ShowCN(i*16,2,i+15);//��ʾ���֣������ޡ�����鿴codetab.h�ֿ�
									OLED_ShowStr(35,2,":   %",2);
									for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+17);//��ʾ���֣������ޡ�����鿴codetab.h�ֿ�
									OLED_ShowStr(35,6,":   %",2);
									
									OLED_ShowTH(48,2,humi_max);//��ʾֵ
									OLED_ShowTH(48,6,humi_min);
							}
				}
				else
				{
						for(i=0;i<6;i++)OLED_ShowCN(i*16+8,0,i+1);//��ʾ���֣����������ϵͳ������鿴codetab.h�ֿ�
						OLED_ShowStr(0,2,"Temp:   C",2);
						OLED_ShowStr(0,4,"Humi:   %",2);
				}
			}
			if(setN != 0)
			{
					if(keynum == 2)//�����Ӱ���
					{
							BEEP(ON);
							DelayMs(100);
							BEEP(OFF);
						
							if(setN == 1)//�����¶�����
							{
									if(temp_max < 99)//����ֵ���99
										temp_max++;
									OLED_ShowTH(48,2,temp_max);//��ʾ
							}
							if(setN == 2)//�����¶�����
							{
									if(temp_min < temp_max)//����ֵ�����������ֵ������ֵ���ܼ�
										temp_min++;
									OLED_ShowTH(48,6,temp_min);//��ʾ
							}
							if(setN == 3)//����ʪ������
							{
									if(humi_max < 99)
										humi_max++;
									OLED_ShowTH(48,2,humi_max);
							}
							if(setN == 4)//����ʪ������
							{
									if(humi_min < humi_max)
										humi_min++;
									OLED_ShowTH(48,6,humi_min);
							}
					}
					if(keynum == 3)//����������
					{
							BEEP(ON);
							DelayMs(100);
							BEEP(OFF);
						
							if(setN == 1)//�����¶�����
							{
									if(temp_max > temp_min)//����ֵ�����������ֵ������ֵ���ܼ�
										temp_max--;
									OLED_ShowTH(48,2,temp_max);//��ʾ
							}
							if(setN == 2)//�����¶�����
							{
									if(temp_min > 0)//����ֵ��С��0
										temp_min--;
									OLED_ShowTH(48,6,temp_min);//��ʾ
							}
							if(setN == 3)//����ʪ������
							{
									if(humi_max > humi_min)
										humi_max--;
									OLED_ShowTH(48,2,humi_max);//��ʾ
							}
							if(setN == 4)//����ʪ������
							{
									if(humi_min > 0)
										humi_min--;
									OLED_ShowTH(48,6,humi_min);
							}
					}
		}
		  if(count++ > 500)//��ʱһ��ʱ���ȡ��ʪ�ȣ�����ʾ
			{
				  count = 0;
					DHT11_Read_Data(&temperature,&humidity);//��ȡ��ʪ��
					if(setN == 0)//�����ǲ�������ģʽ�£�����������ʾ��ʪ��ֵ
					{
							OLED_ShowTH(47,2,temperature);//��ʾ�¶�
							OLED_ShowTH(47,4,humidity);//��ʾʪ��
							
							if(temperature <= temp_min || temperature >= temp_max)
							{
									temp_alarm = 1;//�¶ȳ��ޱ�־��1
							}
							else
							{
									temp_alarm = 0;
							}
							
							if(humidity <= humi_min || humidity >= humi_max)
							{
									humi_alarm = 1;//ʪ�ȳ��ޱ�־��1
							}
							else
							{
									humi_alarm = 0;
							}
							
							if(temp_alarm == 1 && humi_alarm == 1)//�������ͬʱ���꣬��Ļ������ʾ����
							{
									mode_cnt++;
								  if(mode_cnt > 6)
										mode_cnt = 0;
									
									if(mode_cnt <= 3)
									{
										if(mode_cnt > 2)
											OLED_ShowStr(0,6,"                ",2);
										else
											OLED_ShowStr(0,6," Temp Overrun!  ",2);//��ʾ�¶ȱ���
									}
									else
									{
										if(mode_cnt > 5)
											OLED_ShowStr(0,6,"                ",2);
										else
											OLED_ShowStr(0,6," Humi Overrun!  ",2);//��ʾʪ�ȱ���
									}
							}
							else
							{
								  mode_cnt = 0;
								
									if(temp_alarm == 1)//�¶ȳ��ޣ���ʾ�¶ȱ���
									{
											OLED_ShowStr(0,6," Temp Overrun!  ",2);
									}
									if(humi_alarm == 1)//ʪ�ȳ��ޣ���ʾʪ�ȱ���
									{
											OLED_ShowStr(0,6," Humi Overrun!  ",2);
									}
							}
							
							if(temp_alarm != 0 || humi_alarm != 0)//��ʪ�ȣ�����һ�����꣬��������
							{
								  BEEP(ON);
							}
							else
							{
									OLED_ShowStr(0,6,"State: normal  ",2);//��ʪ����������Ļ��ʾ����
								  BEEP(OFF);
							}
					}
			}
			DelayMs(1);
	}
}
