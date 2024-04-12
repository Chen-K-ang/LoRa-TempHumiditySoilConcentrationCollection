/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	miplconfig.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-12-26
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		M5310-MiplConfig���������ݵĳ�ʼ����ת��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//ͷ�ļ�
#include "miplconfig.h"

//C��
#include <string.h>
#include <stdlib.h>


_Bool MIPLCONFIG_Init(MIPL_T *mipl, char *ep_name, char ep_name_len)
{
	
	unsigned char uri_len = strlen("coap://183.230.40.39:5683");

	mipl->boot = 1;							//Bootstrap ģʽ���ã� 1 Ϊ�򿪣� 0 Ϊ�رգ� ��Ҫ����Ϊ 1
	
	mipl->encrypt = 0;						//����ģʽ���ã� 1 Ϊ�򿪣� 0 Ϊ�رգ�Ŀǰ��֧�ּ���ģʽ
	
	mipl->debug = MIPL_DEBUG_LEVEL_NONE;	//�ر�DeBugģʽ
	
	mipl->uri = (char *)malloc(uri_len + 1);//URI�ڴ���䡢���ݸ���
	if(mipl->uri == NULL)
		return 1;
	
	memset(mipl->uri, 0, uri_len + 1);
	memcpy(mipl->uri, "coap://183.230.40.39:5683", uri_len);
	
	mipl->uri_len = uri_len;
	
	mipl->ep = (char *)malloc(ep_name_len + 1);	//EP�ڴ���䡢���ݸ���
	if(mipl->ep == NULL)
	{
		free(mipl->uri);
		return 1;
	}
	
	memset(mipl->ep, 0, ep_name_len + 1);
	memcpy(mipl->ep, ep_name, ep_name_len);
	
	mipl->ep_len = ep_name_len;
	
	mipl->port = 0;							//���˶˿ںţ� ��Χ 0~65535�� ȱʡֵ 0�� ��ѡ��ȱʡʱ�� ģ����Զ��� 32768~65535 ��ѡ��һ�����õĶ˿ں�
	
	mipl->keep_alive = 256;					//���÷�ΧΪ 10s~86400��
	
	mipl->block1 = 5;
	mipl->block2 = 5;
	mipl->block2th = 2;
	
	return 0;

}

/*****************************************************************
       0               1               2               3 
0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| boot | encrypt|     debug     |          local_port         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          life_time                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           host_len            |             host            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          ep_name_len          |      ep_name(imei;imsi)     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  coap_block1  |  coap_block2  |    block2th   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*****************************************************************/
unsigned char MIPLCONFIG_SetConf(MIPL_T *mipl, char *buf)
{
	
	unsigned char offset = 0;
	char mode = ((mipl->boot & 0x1) << 4) | (mipl->encrypt & 0x1);

	memcpy(buf + offset, &mode, 1);
	offset++;
	memcpy(buf + offset, &mipl->debug, 1);
	offset++;
	memcpy(buf + offset, &mipl->port, 2);
	offset += 2;
	memcpy(buf + offset, &mipl->keep_alive, 4);
	offset += 4;

	memcpy(buf + offset, &mipl->uri_len, 2);
	offset += 2;
	memcpy(buf + offset, mipl->uri, mipl->uri_len);
	offset += mipl->uri_len;
	memcpy(buf + offset, &mipl->ep_len, 2);
	offset += 2;
	memcpy(buf + offset, mipl->ep, mipl->ep_len);
	offset += mipl->ep_len;

	*(buf + offset) = mipl->block1;
	offset++;
	*(buf + offset) = mipl->block2;
	offset++;
	*(buf + offset) = mipl->block2th;
	offset++;
	
	free(mipl->uri);
	mipl->uri = NULL;
	
	free(mipl->ep);
	mipl->ep = NULL;
	
	return offset;
	
}
