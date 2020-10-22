/********************************************************************************
* @File name: RTOS_mutex.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS ������
********************************************************************************/

/*********************************ͷ�ļ�����************************************/
#include "RTOS_mutex.h"
#include "RTOS_mem.h"
#include "RTOS_task.h"
/*********************************����******************************************/

/*******************************************************
*
* Function name :MutexCreate
* Description        :�������Ĵ�������
* Parameter         :��
* Return          : ʧ�ܷ���NULL 
**********************************************************/
PMUTEX_CB MutexCreate(void){
	PMUTEX_CB pMutexCB;
	pMutexCB = (PMUTEX_CB)OSMalloc(sizeof(MUTEX_CB));
	if(pMutexCB == NULL){return NULL;}
	pMutexCB->mutexFlag=0;
	return pMutexCB;
}
/*******************************************************
*
* Function name :MutexFree
* Description        :���������ͷź���
* Parameter         :
*        @pMUTEX        �ź�������ָ�� 
* Return          : ��
**********************************************************/
void MutexFree(PMUTEX_CB pMUTEX){
	if(pMUTEX==NULL){return ;}
	OSFree(pMUTEX);
}
/*******************************************************
*
* Function name :MutexUnlock
* Description        :����
* Parameter         :
*        @pMUTEX        �ź�������ָ�� 
* Return          : ��
**********************************************************/
uint32 MutexUnlock(PMUTEX_CB pMUTEX){
	if(pMUTEX==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pMUTEX->mutexFlag=0;
	OSExitExclusiveMode();
	return TRUE;
}

/*******************************************************
*
* Function name :MutexUnlock
* Description        :����
* Parameter         :
*        @pMUTEX        �ź�������ָ�� 
*        @mutexWaitDelay        �ȴ�ʱ�� 
* Return          : TRUE or FALSE
**********************************************************/
uint32 MutexLock(PMUTEX_CB pMUTEX,uint32 mutexWaitDelay){
	if(pMUTEX==NULL){return FALSE;}
	if(mutexWaitDelay==MUTEX_WAIT_ENDLESS){
		while(pMUTEX->mutexFlag);
		OSEnterExclusiveMode();
		pMUTEX->mutexFlag=1;
		OSExitExclusiveMode();
		return TRUE;
	}else{
		if(mutexWaitDelay!=0){
			OSTaskDelay(mutexWaitDelay);
		}
		if(pMUTEX->mutexFlag == 0){
			OSEnterExclusiveMode();
			pMUTEX->mutexFlag=1;
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}
