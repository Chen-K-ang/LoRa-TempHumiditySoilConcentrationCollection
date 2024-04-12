#ifndef _MIPLCONFIG_H_
#define _MIPLCONFIG_H_





typedef enum
{
	
	MIPL_DEBUG_LEVEL_NONE = 0,
	MIPL_DEBUG_LEVEL_RXL,
	MIPL_DEBUG_LEVEL_RXL_RXD,
	MIPL_DEBUG_LEVEL_TXL_TXD,
	
} MIPL_DEBUG_LEVEL_E;

typedef struct
{
	
	unsigned char boot;
	unsigned char encrypt;
	MIPL_DEBUG_LEVEL_E debug;
	unsigned short port;
	unsigned int keep_alive;
	unsigned int uri_len;
	char *uri;
	unsigned int ep_len;
	char *ep;
	unsigned char block1;	//COAP option BLOCK1(PUT or POST),0-6. 2^(4+n)  bytes
	unsigned char block2;	//COAP option BLOCK2(GET),0-6. 2^(4+n)  bytes
	unsigned char block2th;	//max size to trigger block-wise operation,0-2. 2^(8+n) bytes
	
} MIPL_T;


_Bool MIPLCONFIG_Init(MIPL_T *mipl, char *ep_name, char ep_name_len);

unsigned char MIPLCONFIG_SetConf(MIPL_T *mipl, char *buf);


#endif
