/**************************************************************

	*	文件名称： 	    dStream.c
	*
	*	作    者： 		苏锋
	*
	*	日    期： 		2019-1-6
	*
	*	说    明： 		封装数据流
	*
	*	修改记录：		
	
****************************************************************/

//协议封装文件
#include "dStream.h"

//C库
#include <string.h>
#include <stdio.h>


//==========================================================
//	函数名称：	DSTREAM_toString
//
//	函数功能：	将数据流信息转换为M5310-A命令的格式
//
//	入口参数：	streamArray：数据流结构体
//				buf：命令缓存
//				pos：第几个数据流
//				bufLen：缓存大小
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool DSTREAM_toString(DATA_STREAM *streamArray, char *buf, unsigned short pos, unsigned short bufLen)
{
	
	unsigned char dataType = 0;
	_Bool status = 0;
	
	if(streamArray[pos].dataType >= 7 && streamArray[pos].dataType <= 14)
		dataType = 3;
	else if(streamArray[pos].dataType == 15)
		dataType = 4;
	else if(streamArray[pos].dataType >= 1 && streamArray[pos].dataType <= 6)
		dataType = streamArray[pos].dataType;
	else
		return 1;
	
	if(streamArray[pos].flag)
	{
		switch((unsigned char)streamArray[pos].dataType)
		{
			case TYPE_STRING:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%s\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), (char *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_OPAQUE:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%s\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), (char *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_ULONG:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%ld\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(unsigned long *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_LONG:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%ld\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(signed long *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_UINT:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType, strlen(streamArray[pos].dataPoint),*(unsigned int *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_INT:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType, strlen(streamArray[pos].dataPoint),*(signed int *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_USHORT:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(unsigned short *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_SHORT:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(signed short *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_UCHAR:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint),*(unsigned char *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_CHAR:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(signed char *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_FLOAT:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%.2f\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(float *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_DOUBLE:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%.2f\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(double *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_BOOL:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), *(_Bool *)streamArray[pos].dataPoint);
			
			break;
			
			case TYPE_HEXSTR:
			
				snprintf(buf, bufLen, "AT+MIPLNOTIFY=0,0,%s,%s,%s,%d,%d,\"%s\",0,0\r\n",
										streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
										dataType,strlen(streamArray[pos].dataPoint), (char *)streamArray[pos].dataPoint);
			
			break;
			
			default:
				status = 1;
			break;
		}
	}
	else
		status = 1;
	
	return status;

}

//==========================================================
//	函数名称：	DSTREAM_toString_ReadResp
//
//	函数功能：	将数据流信息转换为M5310命令的格式
//
//	入口参数：	streamArray：数据流结构体
//				buf：命令缓存
//				pos：第几个数据流
//				bufLen：缓存大小
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool DSTREAM_toString_ReadResp(DATA_STREAM *streamArray, char *buf, unsigned short pos, unsigned short bufLen, char *msg_id)
{
	
	unsigned char dataType = 0;
	_Bool status = 0;
	
	if(streamArray[pos].dataType >= 7 && streamArray[pos].dataType <= 14)
		dataType = 3;
	else if(streamArray[pos].dataType == 15)
		dataType = 4;
	else if(streamArray[pos].dataType >= 1 && streamArray[pos].dataType <= 6)
		dataType = streamArray[pos].dataType;
	else
		return 1;

	switch((unsigned char)streamArray[pos].dataType)
	{
		case TYPE_STRING:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%s\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), (char *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_OPAQUE:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%s\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), (char *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_ULONG:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(unsigned int *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_LONG:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%ld\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(signed long *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_UINT:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%ld\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(unsigned long *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_INT:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(signed int *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_USHORT:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(unsigned short *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_SHORT:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(signed short *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_UCHAR:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(unsigned char *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_CHAR:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(signed char *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_FLOAT:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%f\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(float *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_DOUBLE:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%f\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), *(double *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_BOOL:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%d\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType, strlen(streamArray[pos].dataPoint),*(_Bool *)streamArray[pos].dataPoint);
		
		break;
		
		case TYPE_HEXSTR:
		
			snprintf(buf, bufLen, "AT+MIPLREADRSP=0,%s,1,%s,%s,%s,%d,%d,\"%s\",0,0\r\n",
									msg_id, streamArray[pos].obj_id, streamArray[pos].ins_id, streamArray[pos].res_id,
									dataType,strlen(streamArray[pos].dataPoint), (char *)streamArray[pos].dataPoint);
		
		break;
		
		default:
			status = 1;
		break;
	}
	
	if(streamArray[pos].flag == 0)
		status = 1;
	
	return status;

}
