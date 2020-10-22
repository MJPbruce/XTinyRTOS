/********************************************************************************
* @File name: RTOS_msg.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS ����ϵͳ����Ϣ�����ʵ�ֵĴ���
********************************************************************************/

/*********************************ͷ�ļ�����************************************/
#include "RTOS_msg.h"
#include "RTOS_mem.h"
#include "RTOS_task.h"
/*********************************����******************************************/

/*******************************************************
*
* Function name :MsgCreate
* Description        :����һ����Ϣ����,�ڴ�����ʧ���򷵻ش���. 
* Parameter         :��
* Return          : ����NULLʧ��
**********************************************************/
PMSG_CB MsgCreate(void){
	PMSG_CB pMsgCB=OSMalloc(sizeof(MSG_CB));
	if(pMsgCB==NULL){return NULL;}
	pMsgCB->msgID=(uint32)pMsgCB;
	pMsgCB->front=pMsgCB->rear=0;
	return pMsgCB;
}
/*******************************************************
*
* Function name :MsgFree
* Description        : �ͷ�һ����Ϣ����
* Parameter         :
*        @pMsgCB        ��Ϣ�������ָ�� 
* Return          : ��
**********************************************************/
void MsgFree(PMSG_CB pMsgCB){
	OSFree(pMsgCB);
}
/*******************************************************
*
* Function name :MsgEmpty
* Description        : ��Ϣ�Ƿ�Ϊ��
* Parameter         :
*        @pMsgCB        ��Ϣ�������ָ�� 
* Return          : ��
**********************************************************/
BOOL MsgIsEmpty(PMSG_CB pMsgCB){
	return (pMsgCB->front==pMsgCB->rear);
}
/*******************************************************
*
* Function name :MsgIsFull
* Description        : ��Ϣ�Ƿ���
* Parameter         :
*        @pMsgCB        ��Ϣ�������ָ�� 
* Return          : ��
**********************************************************/
BOOL MsgIsFull(PMSG_CB pMsgCB){
	return ((pMsgCB->rear + 1) % MSG_MAX_LEN == pMsgCB->front);
}
/*******************************************************
*
* Function name :MsgGet
* Description        :����Ϣ�����л�ȡ��Ϣ
* Parameter         :
*        @pMsgCB        Ҫ��ȡ����Ϣ����Ϣ���ƿ�    
*        @msg    Ҫ��ȡ����Ϣ��ŵ�ַ
*        @msgGetDelay    ��ȡ��Ϣ�ĳ�ʱʱ��
* Return          : TRUE success , FALSE fail
**********************************************************/
uint32 MsgGet(PMSG_CB pMsgCB,void **msg,uint32 msgGetDelay){
	if(pMsgCB==NULL){return FALSE;}
	
	if(msgGetDelay==MSG_WAIT_ENDLESS){
		while(MsgIsEmpty(pMsgCB)==TRUE);
		OSEnterExclusiveMode();
		pMsgCB->front=(pMsgCB->front+1) % MSG_MAX_LEN;
		*msg =pMsgCB->msg[pMsgCB->front];
		OSExitExclusiveMode();
		return TRUE;
	} else {
		if(msgGetDelay!=0){
			OSTaskDelay(msgGetDelay);
		}
		if(MsgIsEmpty(pMsgCB)!=TRUE){
			OSEnterExclusiveMode();
			pMsgCB->front=(pMsgCB->front+1) % MSG_MAX_LEN;
			*msg =pMsgCB->msg[pMsgCB->front];
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :MsgPut
* Description        :�����Ϣ
* Parameter         :
*        @pMsgCB        Ҫ���͵���Ϣ����Ϣ���ƿ�    
*        @msg    Ҫ���͵���Ϣ��ŵ�ַ
*        @msgGetDelay    ������Ϣ�ĳ�ʱʱ��
* Return          : TRUE success , FALSE fail
**********************************************************/
uint32 MsgPut(PMSG_CB pMsgCB,void* msg,uint32 msgGetDelay){
	if(pMsgCB==NULL){return FALSE;}
	
	if(msgGetDelay==MSG_WAIT_ENDLESS){
		while(MsgIsFull(pMsgCB)==TRUE);
		OSEnterExclusiveMode();
		pMsgCB->rear=(pMsgCB->rear+1)%MSG_MAX_LEN;
		pMsgCB->msg[pMsgCB->rear]=msg;
		OSExitExclusiveMode();	
		return TRUE;
	}else{
		if(msgGetDelay!=0){
			OSTaskDelay(msgGetDelay);
		}
		if(MsgIsFull(pMsgCB)==FALSE){
			OSEnterExclusiveMode();
			pMsgCB->rear=(pMsgCB->rear+1)%MSG_MAX_LEN;
			pMsgCB->msg[pMsgCB->rear]=msg;
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :MsgLen
* Description        :��ȡ��Ϣ�ĳ���
* Parameter         :
*        @pMsgCB        ��Ϣ���ƿ�  
* Return          : ��Ϣ���г���
**********************************************************/
uint32 MsgLen(PMSG_CB pMsgCB){
	return (pMsgCB->rear - pMsgCB->front + MSG_MAX_LEN)%MSG_MAX_LEN;
}
