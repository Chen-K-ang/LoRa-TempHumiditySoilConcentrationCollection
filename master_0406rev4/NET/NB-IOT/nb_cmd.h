#ifndef _NB_CMD_H_
#define _NB_CMD_H_


//协议封装文件
#include "dStream.h"


void NB_CMD_UnPacketGeneralID(unsigned char *cmd, char *msg_id, char *obj_id, char *ins_id, char *res_id);

_Bool NB_CMD_Unpacket_W_E_Msg(unsigned char *cmd, char **msg);

_Bool NB_CMD_W_E_Resp(char *msg_id, _Bool result, _Bool w_e);

_Bool NB_CMD_ReadResp(char *msg_id, char *obj_id, char *ins_id, char *res_id, DATA_STREAM *streamArray, unsigned short streamArrayCnt);

_Bool NB_CMD_Write(char *obj_id, char *ins_id, char *res_id, char *msg, DATA_STREAM *streamArray, unsigned short streamArrayCnt);

_Bool NB_CMD_Execute(char *obj_id, char *ins_id, char *res_id, char *msg, DATA_STREAM *streamArray, unsigned short streamArrayCnt);


#endif
