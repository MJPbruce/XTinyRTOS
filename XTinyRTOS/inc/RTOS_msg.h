#ifndef _RTOS_MSG_H__
#define _RTOS_MSG_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************ͷ�ļ�����************************************/
#include "RTOS_type.h"
/*********************************����******************************************/

/*********************************���ú�************************************/
#define MSG_MAX_LEN 32		/*��Ϣ����󳤶�*/
/*********************************����******************************************/

/*********************************�궨��************************************/
#define MSG_WAIT_ENDLESS 0xFFFFFFFF
/*********************************����******************************************/

/*********************************�Զ�������************************************/
typedef struct {
	/*��Ϣid*/
	uint32 		msgID;
	/*����ָ��*/
	int32 		front;
	int32 		rear;
	/*��Ϣ,��ŵ�ָ��*/
	void* 		msg[MSG_MAX_LEN];
}*PMSG_CB,MSG_CB;
/*********************************����******************************************/

/*********************************��������************************************/
PMSG_CB MsgCreate(void);
void 		MsgFree(PMSG_CB mTCB_MSG);

uint32 	MsgGet(PMSG_CB mTCB_MSG,void **msg,uint32 msgGetDelay);
uint32 	MsgPut(PMSG_CB mTCB_MSG,void* msg,uint32 msgGetDelay);

BOOL 		MsgIsEmpty(PMSG_CB pMsgCB);
BOOL 		MsgIsFull(PMSG_CB pMsgCB);
uint32 	MsgLen(PMSG_CB pMsgCB);
/*********************************����******************************************/
#ifdef __cplusplus
}
#endif
#endif
