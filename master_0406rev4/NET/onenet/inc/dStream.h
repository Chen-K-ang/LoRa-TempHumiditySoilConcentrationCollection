#ifndef _DSTREAM_H_
#define _DSTREAM_H_


typedef void (*task_fun)(char *msg);


typedef enum
{

	//NB-IOT IPSO文档规定
	TYPE_STRING = 1,
	TYPE_OPAQUE,
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_BOOL,
	TYPE_HEXSTR,
	
	//自定义
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
	
	task_fun write_fun;		//写操作
	task_fun execute_fun;	//执行操作

} DATA_STREAM;


_Bool DSTREAM_toString(DATA_STREAM *streamArray, char *buf, unsigned short pos, unsigned short bufLen);

_Bool DSTREAM_toString_ReadResp(DATA_STREAM *streamArray, char *buf, unsigned short pos, unsigned short bufLen, char *msg_id);


#endif
