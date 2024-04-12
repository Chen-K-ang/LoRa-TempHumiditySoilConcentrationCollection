#ifndef _NET_DEVICE_H_
#define _NET_DEVICE_H_


//协议封装文件
#include "dStream.h"
#include "stdlib.h"

//=============================配置==============================
//===========可以提供RTOS的内存管理方案，也可以使用C库的=========

#define NET_MallocBuffer	malloc

#define NET_FreeBuffer		free
//==========================================================


struct NET_SEND_LIST
{

	unsigned short dataLen;			//数据长度
	unsigned char *buf;				//数据指针
	
	struct NET_SEND_LIST *next;		//下一个

};


typedef struct
{
	
	char *cmd_resp;					//命令回复指针。比如获取命令返回的数据，可以提取此指针内的数据
	char *cmd_ipd;					//在接入onenet前通过网络获取的数据，比如网络时间、获取接入ip等
	char *cmd_hdl;					//命令处理。在发送命令后，会在返回内容里搜索用户指定的返回数据
	
	char *config_data;
	unsigned char config_len;
	
	unsigned int net_time;
	
	signed char signal;
	
	unsigned char socket_id;
	
	unsigned short err : 4; 		//错误类型
	unsigned short init_step : 4;	//初始化步骤
	unsigned short reboot : 1;		//死机重启标志
	unsigned short net_work : 1;	//网络访问OK
	unsigned short device_ok : 1;
	unsigned short busy : 1;		//忙标志
	unsigned short send_count : 3;	//网络设备层的发送成功
	unsigned short reverse : 1;		//预留

} NET_DEVICE_INFO;

extern NET_DEVICE_INFO net_device_info;


typedef struct
{

	char earfcn[8];
	char physical_cellid[8];
	char primary_cell[8];
	char rsrp[8];
	char rsrq[8];
	char rssi[8];
	char snr[8];

} NET_DEVICE_CELL;

extern NET_DEVICE_CELL net_device_cell;


typedef struct
{

	char lon[16];
	char lat[16];
	
	_Bool flag;

} GPS_INFO;

extern GPS_INFO gps;


#define NET_TIME_EN				0	//1-获取网络时间		0-不获取


void NET_DEVICE_IO_Init(void);

_Bool NET_DEVICE_Exist(void);

signed char NET_DEVICE_GetSignal(void);

_Bool NET_DEVICE_GetCell(void);

_Bool NET_DEVICE_Init(DATA_STREAM *streamArray, unsigned short streamArrayCnt);

void NET_DEVICE_Reset(void);

void NET_DEVICE_CmdHandle(char *cmd);

_Bool NET_DEVICE_SendCmd(char *cmd, char *res);

unsigned char *NET_DEVICE_Read(void);

_Bool NET_DEVICE_Update(unsigned int time);

unsigned char NET_DEVICE_Check(void);

void NET_DEVICE_ReConfig(unsigned char step);


int HexString_2_Num(char *hs);

int Num_2_HexString(unsigned int num, char *hs, _Bool flag);

#endif
