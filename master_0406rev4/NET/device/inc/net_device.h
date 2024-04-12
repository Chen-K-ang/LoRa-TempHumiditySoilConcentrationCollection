#ifndef _NET_DEVICE_H_
#define _NET_DEVICE_H_


//Э���װ�ļ�
#include "dStream.h"
#include "stdlib.h"

//=============================����==============================
//===========�����ṩRTOS���ڴ��������Ҳ����ʹ��C���=========

#define NET_MallocBuffer	malloc

#define NET_FreeBuffer		free
//==========================================================


struct NET_SEND_LIST
{

	unsigned short dataLen;			//���ݳ���
	unsigned char *buf;				//����ָ��
	
	struct NET_SEND_LIST *next;		//��һ��

};


typedef struct
{
	
	char *cmd_resp;					//����ظ�ָ�롣�����ȡ����ص����ݣ�������ȡ��ָ���ڵ�����
	char *cmd_ipd;					//�ڽ���onenetǰͨ�������ȡ�����ݣ���������ʱ�䡢��ȡ����ip��
	char *cmd_hdl;					//������ڷ�������󣬻��ڷ��������������û�ָ���ķ�������
	
	char *config_data;
	unsigned char config_len;
	
	unsigned int net_time;
	
	signed char signal;
	
	unsigned char socket_id;
	
	unsigned short err : 4; 		//��������
	unsigned short init_step : 4;	//��ʼ������
	unsigned short reboot : 1;		//����������־
	unsigned short net_work : 1;	//�������OK
	unsigned short device_ok : 1;
	unsigned short busy : 1;		//æ��־
	unsigned short send_count : 3;	//�����豸��ķ��ͳɹ�
	unsigned short reverse : 1;		//Ԥ��

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


#define NET_TIME_EN				0	//1-��ȡ����ʱ��		0-����ȡ


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
