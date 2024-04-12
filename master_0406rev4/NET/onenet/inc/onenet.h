#ifndef _ONENET_H_
#define _ONENET_H_


#include "dstream.h"


typedef struct
{
	
	unsigned short net_work : 1;	//1-OneNET����ɹ�		0-OneNET����ʧ��
	unsigned short send_data : 2;
	unsigned short err_count : 3;	//�������
	unsigned short heart_beat : 1;	//����
	unsigned short reinit_count : 3;//
	unsigned short reverse : 6;		//����
	unsigned char *handle_flag ;		//����

} ONETNET_INFO;

extern ONETNET_INFO onenet_info;


#define SEND_TYPE_OK			0	//

#define SEND_TYPE_DATA			1	//

#define	SEND_FLAG_OFF			0

#define	SEND_FLAG_ON			1


unsigned char OneNet_SendData(DATA_STREAM *streamArray, unsigned short streamArrayCnt);

_Bool OneNet_Update(unsigned int time);

void OneNET_CmdHandle(DATA_STREAM *streamArray, unsigned short streamArrayCnt);

#endif
