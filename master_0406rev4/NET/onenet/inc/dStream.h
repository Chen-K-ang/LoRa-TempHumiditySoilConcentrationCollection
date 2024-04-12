#ifndef _DSTREAM_H_
#define _DSTREAM_H_


typedef void (*task_fun)(char *msg);


typedef enum
{

	//NB-IOT IPSO�ĵ��涨
	TYPE_STRING = 1,
	TYPE_OPAQUE,
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_BOOL,
	TYPE_HEXSTR,
	
	//�Զ���
	TYPE_ULONG = 7,
	TYPE_LONG,
	TYPE_UINT,
	TYPE_INT,
	TYPE_USHORT,
	TYPE_SHORT,
	TYPE_UCHAR,
	TYPE_CHAR,
	
	TYPE_DOUBLE = 15,

} DATA_TYPE;

typedef struct
{

	char obj_id[8];
	char ins_id[8];
	char res_id[8];
	
	void *dataPoint;
	DATA_TYPE dataType;
	_Bool flag;
	
	task_fun write_fun;		//д����
	task_fun execute_fun;	//ִ�в���

} DATA_STREAM;


_Bool DSTREAM_toString(DATA_STREAM *streamArray, char *buf, unsigned short pos, unsigned short bufLen);

_Bool DSTREAM_toString_ReadResp(DATA_STREAM *streamArray, char *buf, unsigned short pos, unsigned short bufLen, char *msg_id);


#endif
