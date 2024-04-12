#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"	 

//#define KEY0 PCin(5)   	
//#define KEY1 PAin(15)	 
//#define WK_UP  PAin(0)	 
 

#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//读取按键1
#define KEY2   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//读取按键2 
 

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数					    
#endif
