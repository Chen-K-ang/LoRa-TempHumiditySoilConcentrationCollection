/**************************************************************

	*	文件名称： 	    net_device.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2019-1-10
	*
	*	说    明： 		网络设备配置
	*
	*	修改记录：		
	
****************************************************************/


//单片机头文件
#include "stm32f10x.h"

//单片机相关组件
#include "mcu_gpio.h"

//网络设备
#include "net_device.h"
#include "net_io.h"

//配置文件
#include "miplconfig.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


NET_DEVICE_INFO net_device_info = {NULL, NULL, NULL,
									NULL, 0,
									0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0};


NET_DEVICE_CELL net_device_cell;


GPS_INFO gps;


MIPL_T mipl_config;

extern NET_IO_INFO net_io_info;
//为了通用性，gpio设备列表里的name固定这么写
const static GPIO_LIST net_device_gpio_list[] = {
													{GPIOC, GPIO_Pin_2, "nd_rst"},
													{GPIOC, GPIO_Pin_3, "nd_pwr"},
												};

												

//反转字符串
char *reverse(char *s)
{
    char temp;
    char *p = s;    //p指向s的头部
    char *q = s;    //q指向s的尾部
    while(*q)
        ++q;
    q--;
    
    //交换移动指针，直到p和q交叉
    while(q > p)
    {
        temp = *p;
        *p++ = *q;
        *q-- = temp;
    }
    return s;
}

char *my_itoa(int n)
{
		int i = 0,isNegative = 0;
		static char s[100];      //必须为static变量，或者是全局变量
		if((isNegative = n) < 0) //如果是负数，先转为正数
		{
			n = -n;
		}
		do      //从各位开始变为字符，直到最高位，最后应该反转
		{
			s[i++] = n%10 + '0';
			n = n/10;
		}while(n > 0);
		
		if(isNegative < 0)   //如果是负数，补上负号
		{
			s[i++] = '-';
		}
		s[i] = '\0';    //最后加上字符串结束符
		return reverse(s);
}
			

//==========================================================
//	函数名称：	NET_DEVICE_IO_Init
//
//	函数功能：	初始化网络设备IO层
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		初始化网络设备的控制引脚、数据收发功能等
//==========================================================
void NET_DEVICE_IO_Init(void)
{

	MCU_GPIO_Init(net_device_gpio_list[0].gpio_group, net_device_gpio_list[0].gpio_pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, net_device_gpio_list[0].gpio_name);
	
	MCU_GPIO_Init(net_device_gpio_list[1].gpio_group, net_device_gpio_list[1].gpio_pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, net_device_gpio_list[1].gpio_name);
	
	NET_IO_Init();											//网络设备数据IO层初始化
	
//	MCU_GPIO_Output_Ctl("nd_pwr", 0);
//	delay_ms(200);
	MCU_GPIO_Output_Ctl("nd_pwr", 1);
	
	//NET_DEVICE_Reset();
	
	net_device_info.reboot = 0;

}

//==========================================================
//	函数名称：	NET_DEVICE_GetSignal
//
//	函数功能：	获取网络信号强弱
//
//	入口参数：	无
//
//	返回参数：	返回信号强弱值
//
//	说明：		
//==========================================================
signed char NET_DEVICE_GetSignal(void)
{
	
	char *dataPtr = NULL;
	char num[4] = {0, 0, 0, 0}, count = 0;
	
	if(net_device_info.busy)
		return 0;
	
	net_device_info.busy = 1;

	if(!NET_DEVICE_SendCmd("AT+CSQ\r\n", "OK"))
	{
		//RSSI:0~31，越大越强；99代表无网络
		dataPtr = strchr(net_device_info.cmd_resp, ':');
		if(dataPtr != NULL)
		{
			dataPtr++;
			if(*dataPtr == ' ')
				dataPtr++;
			
			while(*dataPtr >= '0' && *dataPtr <= '9')
				num[count++] = *dataPtr++;
			
			net_device_info.signal = atoi(num);
			
			UsartPrintf(USART_DEBUG, "CSQ: %d\r\n", net_device_info.signal);
		}
		else
			UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
	}
	
	net_device_info.busy = 0;
	
	return net_device_info.signal;

}

//==========================================================
//	函数名称：	NET_DEVICE_GetCell
//
//	函数功能：	获取基站号
//
//	入口参数：	无
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool NET_DEVICE_GetCell(void)
{
	
	char *data_ptr = NULL;
	unsigned char i = 0;
	_Bool result = 1;

	//NUESTATS:CELL,3736,403,1,-666,-108,-560,216\r\nOK
	//NUESTATS:CELL,3736,404,1,-942,-120,-831,48\r\nOK
	
	if(net_device_info.busy)
		return 1;
	
	net_device_info.busy = 1;
	
	if(!NET_DEVICE_SendCmd("AT+NUESTATS=CELL\r\n", "NUESTATS"))
	{
		data_ptr = strchr(net_device_info.cmd_resp, ',');
		if(data_ptr != NULL)
		{
			//提取earfcn----------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.earfcn[i++] = *data_ptr++;
			
			//提取physical_cellid-------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.physical_cellid[i++] = *data_ptr++;
			
			//提取primary_cell----------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.primary_cell[i++] = *data_ptr++;
			
			//提取rsrp------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.rsrp[i++] = *data_ptr++;
			
			//提取rsrq------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.rsrq[i++] = *data_ptr++;
			
			//提取rssi------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != ',')
				net_device_cell.rssi[i++] = *data_ptr++;
			
			//提取snr-------------------------------------------------------
			i = 0;
			while(*data_ptr != ',')
				data_ptr++;
			data_ptr++;
			while(*data_ptr != '\r')
				net_device_cell.snr[i++] = *data_ptr++;
			
			result = 0;
			
			UsartPrintf(USART_DEBUG, "earfcn:%s, physical_cellid:%s, primary_cell:%s, rsrp:%s, rsrq:%s, rssi:%s, snr:%s\r\n",
										net_device_cell.earfcn, net_device_cell.physical_cellid, net_device_cell.primary_cell,
										net_device_cell.rsrp, net_device_cell.rsrq, net_device_cell.rssi, net_device_cell.snr);
		}
	}
	
	net_device_info.busy = 0;
	
	return result;

}

//==========================================================
//	函数名称：	NET_DEVICE_Exist
//
//	函数功能：	网络设备存在检查
//
//	入口参数：	无
//
//	返回参数：	返回结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool NET_DEVICE_Exist(void)
{
	
	_Bool result = 1;

	if(!NET_DEVICE_SendCmd("AT\r\n", "OK"))
		result = 0;
	else
		result = 1;
	
	return result;

}

//==========================================================
//	函数名称：	NET_DEVICE_Init
//
//	函数功能：	网络设备初始化
//
//	入口参数：	protocol：协议号(参考NET_DEVICE_GetLinkIP的说明)
//				ip：IP地址缓存指针
//				port：端口缓存指针
//
//	返回参数：	返回初始化结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool NET_DEVICE_Init(DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	_Bool status = 1;
	
	net_device_info.net_work = 0;

	switch(net_device_info.init_step)
	{
		case 0:
		
			if(!NET_DEVICE_SendCmd("AT+CIMI\r\n", "OK"))   
			{
				//UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
				net_device_info.init_step++;
			}
		//net_device_info.init_step++;
		break;
		
		case 1:

		if(!NET_DEVICE_SendCmd("AT+CEREG?\r\n", "0,1"))  //网络附着检测  
		{
			UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
			
			net_device_info.init_step++;
		}
		break;
		
		case 2:
	
		if(!NET_DEVICE_SendCmd("AT+CGATT=?\r\n", "0,1"))    //连接MT状态检查 
		{
			UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
			
			net_device_info.init_step++;
		}
		
		break;
		
		
		case 3:
		
			if(!NET_DEVICE_SendCmd("AT+CGSN=1\r\n", "OK"))
			{
				UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
				
				net_device_info.init_step++;
			}
		
		break;
		
		case 4:
		
			if(!NET_DEVICE_SendCmd("AT+CIMI\r\n", "OK"))
			{
				UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
				net_device_info.err = 0;
				net_device_info.init_step++;
			}
		
			else if(++net_device_info.err >= 5)
			{
				net_device_info.err = 0;
				net_device_info.init_step++;
			}
		
		
		break;
	
		case 5:																		//初始化config参数
		{
			char *dataPtr = (char *)NET_MallocBuffer(net_device_info.config_len);	//分配内存
			
			UsartPrintf(USART_DEBUG, "Tips:	MIPLCREATE_Init\r\n");
			
			if(dataPtr != NULL)
			{
				memset((void *)dataPtr,0,sizeof(net_device_info.config_len));			//清空内存
				
				
				 //strcpy(dataPtr,"AT+MIPLCREATE=49,130031F10003F2002304001100000000000010123138332E3233302E34302E33393A35363833000131F300080000000000,0,49,0\r\n");     //上海生成 2020/1/11   
				
				 strcpy(dataPtr,"AT+MIPLCREATE=49,130031F10003F2002304001100000000000010123138332E3233302E34302E33393A35363833000131F300087100000000,0,49,0\r\n");   //汉中
//				 strcpy(dataPtr,"AT+MIPLCREATE=49,130031F10003F2002304001100000000000010123138332E3233302E34302E33393A35363833000131F300080000000000,0,49,0\r\n");   //汉中
				
				if(!NET_DEVICE_SendCmd(dataPtr, "+MIPLCREATE:0"))
				{
					net_device_info.err = 0;
					net_device_info.init_step++;
				
				}
				else if(++net_device_info.err >= 5)
				{
					net_device_info.err = 0;
					net_device_info.device_ok = 0;
					NET_DEVICE_ReConfig(0);
					
					NET_DEVICE_Reset();
				}
				NET_FreeBuffer(dataPtr);
			}
			else
//				UsartPrintf(USART_DEBUG, "mem failed\r\n");
				delay_ms(500);	
				
		}
		break;
		
		
		case 6:
		{
			
			char obj_buf[56];
			//char bitmap[16];
			//char *bm_temp = NULL;

			UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ And MIPLNOTIFY\r\n");					//添加osb和notify

			
//			//添加Object对象
//			snprintf(obj_buf, sizeof(obj_buf), "AT+MIPLADDOBJ=0,%s,0,\"1\",0,0\r\n", streamArray[0].obj_id);			//添加实例√

//			if(!NET_DEVICE_SendCmd(obj_buf, "OK"))
//				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Ok\r\n");
//			else
//			{
//				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Err\r\n");
//			}
		
			//添加Object对象 温度
			snprintf(obj_buf, sizeof(obj_buf), "AT+MIPLADDOBJ=0,%s,1,\"1\",0,0\r\n", streamArray[0].obj_id);			//添加实例

			if(!NET_DEVICE_SendCmd(obj_buf, "OK"))
				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Ok\r\n");
			else
			{
				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Err\r\n");
			}
			
			//添加Object对象 湿度
			snprintf(obj_buf, sizeof(obj_buf), "AT+MIPLADDOBJ=0,%s,1,\"1\",0,0\r\n", streamArray[1].obj_id);			//添加实例

			if(!NET_DEVICE_SendCmd(obj_buf, "OK"))
				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Ok\r\n");
			else
			{
				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Err\r\n");
			}
			
			//添加Object对象 模拟
			snprintf(obj_buf, sizeof(obj_buf), "AT+MIPLADDOBJ=0,%s,4,\"1111\",0,0\r\n", streamArray[2].obj_id);			//添加实例

			if(!NET_DEVICE_SendCmd(obj_buf, "OK"))
				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Ok\r\n");
			else
			{
				UsartPrintf(USART_DEBUG, "Tips:	MIPLADDOBJ Err\r\n");
			}
				
			net_device_info.init_step++;
			

		}
		break;
		
		
		case 7:
		{
			u8 buf[30];
			u8 bufLen;
			memset(buf,0,sizeof(bufLen));
			UsartPrintf(USART_DEBUG, "Tips:	MIPLDISCOVERRSP\r\n");
			if(!NET_DEVICE_SendCmd("AT+MIPLDISCOVERRSP=0,3303,1,4,\"5700\"\r\n", "OK")&&
				 !NET_DEVICE_SendCmd("AT+MIPLDISCOVERRSP=0,3304,1,4,\"5700\"\r\n", "OK")&&
					!NET_DEVICE_SendCmd("AT+MIPLDISCOVERRSP=0,3202,1,4,\"5600\"\r\n", "OK"))
			
				net_device_info.init_step++;
			else
				delay_ms(500);
			
			
				
		
		}
		break;

		case 8:
			
			net_device_info.signal = NET_DEVICE_GetSignal();
			if(net_device_info.signal > 5 && net_device_info.signal < 32)
				net_device_info.init_step++;
			else
				delay_ms(500);
				
		
		break;
			
		case 9:
		
			UsartPrintf(USART_DEBUG, "Tips:	NUESTATS=CELL\r\n");
			if(!NET_DEVICE_GetCell())
				net_device_info.init_step++;
			else
				delay_ms(500);
				
		
		break;
			
		case 10:
		
			UsartPrintf(USART_DEBUG, "Tips:	MIPLOPEN\r\n");
			if(!NET_DEVICE_SendCmd("AT+MIPLOPEN=0,300,30\r\n", "OK"))
				net_device_info.init_step++;
			else
				delay_ms(500);
				
		
		break;
			
		case 11:
			
			UsartPrintf(USART_DEBUG, "Tips:	Wait MIPLOPEN\r\n");
			if(!NET_DEVICE_SendCmd("AT+MIPLOPEN?\r\n", "+MIPLEVENT:0,6"))
				net_device_info.init_step++;
			else
				delay_ms(500);
				

		break;
		
		default:
			
			status = 0;
			net_device_info.init_step = 0;
			net_device_info.net_work = 1;
			UsartPrintf(USART_DEBUG, "Tips:	M5310-A OK\r\n");
		
		break;	
	}
	
	return status;

}

//==========================================================
//	函数名称：	NET_DEVICE_Reset
//
//	函数功能：	网络设备复位
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void NET_DEVICE_Reset(void)
{
	
	UsartPrintf(USART_DEBUG, "Tips:	NET_DEVICE_Reset\r\n");
	
	MCU_GPIO_Output_Ctl("nd_rst", 1);		//复位
	delay_ms(500);
	
	
	MCU_GPIO_Output_Ctl("nd_rst", 0);		//结束复位
	delay_ms(1000);
	

}

//==========================================================
//	函数名称：	NET_DEVICE_CmdHandle
//
//	函数功能：	检查命令返回是否正确
//
//	入口参数：	cmd：需要发送的命令
//
//	返回参数：	无
//
//	说明：		命令处理成功则将指针置NULL
//==========================================================
void NET_DEVICE_CmdHandle(char *cmd)
{
	
	if(strstr(cmd, net_device_info.cmd_hdl) != NULL)
		net_device_info.cmd_hdl = NULL;
	
	net_device_info.cmd_resp = cmd;

}

//==========================================================
//	函数名称：	NET_DEVICE_SendCmd
//
//	函数功能：	向网络设备发送一条命令，并等待正确的响应
//
//	入口参数：	cmd：需要发送的命令
//				res：需要检索的响应
//
//	返回参数：	返回连接结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool NET_DEVICE_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;
	
	NET_IO_Send((unsigned char *)cmd, strlen(cmd));			//写命令到网络设备
	
	if(res == NULL)											//如果为空，则只是发送
		return 0;
	
	net_device_info.cmd_hdl = res;							//需要所搜的关键词
	
	//UsartPrintf(USART_DEBUG, "%s\r\n", cmd);
	UsartPrintf(USART_DEBUG, "%s\r\n", net_device_info.cmd_resp);
	
	
	while((net_device_info.cmd_hdl != NULL) && --timeOut)		//等待
		delay_ms(10);
		//DelayXms(10);

	if(timeOut > 0)
		return 0;
	else
		return 1;

}

//==========================================================
//	函数名称：	HexString_2_Num
//
//	函数功能：	将十六进制字符串转为数值
//
//	入口参数：	hs：十六进制字符串
//
//	返回参数：	转换结果
//
//	说明：		不要带0x前缀
//==========================================================
int HexString_2_Num(char *hs)
{

	int num = 0;
	unsigned char len = 0, i = 0;
	
	if(hs == NULL)
		return -1;
	
	while(hs[i++] != '\0')
		len++;
	
	for(i = 0; i < len; i++)
	{
		if(hs[i] >= 'A' && hs[i] <= 'F')
			num |= (hs[i] - '7') << ((len - i - 1) << 2);
		else if(hs[i] >= 'a' && hs[i] <= 'f')
			num |= (hs[i] - 'W') << ((len - i - 1) << 2);
		else if(hs[i] >= '0' && hs[i] <= '9')
			num |= (hs[i] - '0') << ((len - i - 1) << 2);
		else
			return -2;
	}
	
	return num;

}

//==========================================================
//	函数名称：	Num_2_HexString
//
//	函数功能：	吧数值转为十六进制字符串
//
//	入口参数：	num：数值
//				hs：字符串缓存
//				flag：1-转为大写		0-转为小写
//
//	返回参数：	返回字符串个数	小于0失败
//
//	说明：		
//==========================================================
int Num_2_HexString(unsigned int num, char *hs, _Bool flag)
{

	unsigned char len = 0, i = 0;
	short n_temp = 0;
	
	if(num == 0)
	{
		hs[0] = '0';
		return 1;
	}
	
	while(1)					//转换后有多少个字符
	{
		if(num >> (len << 2))
			len++;
		else
			break;
	}
	
	for(; i < len; i++)
	{
		n_temp = (short)((num >> (i << 2)) & 0xf);
		
		if(n_temp >= 0 && n_temp <= 9)
			hs[len - i - 1] = n_temp + '0';
		else if(n_temp >= 10 && n_temp <= 16)
			hs[len - i - 1] = n_temp + (flag ? '7' : 'W');
		else
		{
			for(i = 0; i < len; i++)
				hs[i] = 0;
			
			return -1;
		}
	}
	
	return len;

}

//==========================================================
//	函数名称：	NET_DEVICE_Read
//
//	函数功能：	读取一帧数据
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
unsigned char *NET_DEVICE_Read(void)
{

	return NET_IO_Read();

}

//==========================================================
//	函数名称：	NET_DEVICE_Update
//
//	函数功能：	更新生命周期
//
//	入口参数：	time：时间，单位秒
//
//	返回参数：	0-成功	1-失败
//
//	说明：		在10s~86400s之间
//==========================================================
_Bool NET_DEVICE_Update(unsigned int time)
{

	char update_buf[24];
	unsigned char time_out = 200;
	
	if(!net_device_info.net_work)
		return 1;
	
	while(net_device_info.busy && --time_out)
		delay_ms(10);
		
	
	if(!time_out)
		return 1;
	
	net_device_info.busy = 1;
	
	if(time < 10 || time > 86400)
		return 1;
	
	snprintf(update_buf, sizeof(update_buf), "AT+MIPLUPDATE=0,%d,1\r\n", time);
	
	net_device_info.busy = 0;
	
	return NET_DEVICE_SendCmd(update_buf, "+MIPLEVENT:0,11");

}

//==========================================================
//	函数名称：	NET_DEVICE_Check
//
//	函数功能：	检查网络设备连接状态
//
//	入口参数：	无
//
//	返回参数：	返回状态
//
//	说明：		
//==========================================================
unsigned char NET_DEVICE_Check(void)
{

	return 0;

}

//==========================================================
//	函数名称：	NET_DEVICE_ReConfig
//
//	函数功能：	设备网络设备初始化的步骤
//
//	入口参数：	步骤值
//
//	返回参数：	无
//
//	说明：		该函数设置的参数在网络设备初始化里边用到
//==========================================================
void NET_DEVICE_ReConfig(unsigned char step)
{

	net_device_info.init_step = step;

}
