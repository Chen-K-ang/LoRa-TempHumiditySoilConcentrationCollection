/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	miplconfig.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-12-26
	*
	*	版本： 		V1.0
	*
	*	说明： 		M5310-MiplConfig，配置数据的初始化和转换
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//头文件
#include "miplconfig.h"

//C库
#include <string.h>
#include <stdlib.h>


_Bool MIPLCONFIG_Init(MIPL_T *mipl, char *ep_name, char ep_name_len)
{
	
	unsigned char uri_len = strlen("coap://183.230.40.39:5683");

	mipl->boot = 1;							//Bootstrap 模式设置， 1 为打开， 0 为关闭； 需要设置为 1
	
	mipl->encrypt = 0;						//加密模式设置， 1 为打开， 0 为关闭；目前不支持加密模式
	
	mipl->debug = MIPL_DEBUG_LEVEL_NONE;	//关闭DeBug模式
	
	mipl->uri = (char *)malloc(uri_len + 1);//URI内存分配、数据复制
	if(mipl->uri == NULL)
		return 1;
	
	memset(mipl->uri, 0, uri_len + 1);
	memcpy(mipl->uri, "coap://183.230.40.39:5683", uri_len);
	
	mipl->uri_len = uri_len;
	
	mipl->ep = (char *)malloc(ep_name_len + 1);	//EP内存分配、数据复制
	if(mipl->ep == NULL)
	{
		free(mipl->uri);
		return 1;
	}
	
	memset(mipl->ep, 0, ep_name_len + 1);
	memcpy(mipl->ep, ep_name, ep_name_len);
	
	mipl->ep_len = ep_name_len;
	
	mipl->port = 0;							//本端端口号， 范围 0~65535； 缺省值 0， 当选择缺省时， 模组会自动从 32768~65535 中选择一个可用的端口号
	
	mipl->keep_alive = 256;					//设置范围为 10s~86400秒
	
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
