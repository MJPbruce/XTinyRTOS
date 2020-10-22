/********************************************************************************
* @File name: RTOS_sem.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS �ź���ģ��
********************************************************************************/

/*********************************ͷ�ļ�����************************************/
#include "RTOS_sem.h"
#include "RTOS_core.h"
#include "RTOS_task.h"
#include "RTOS_mem.h"
/*********************************����******************************************/

/*******************************************************
*
* Function name :SemCreate
* Description        :����һ���ź���
* Parameter         :
*        @initVal   �ź�����ʼֵ    
*        @maxVal    �ź������ֵ
* Return          : ʧ�ܷ���NULL�����򷵻��ź�������
**********************************************************/
PSEM_CB SemCreate(uint32 initVal,uint32 maxVal){
	PSEM_CB pSEM=(PSEM_CB)OSMalloc(sizeof(SEM_CB));
	if(pSEM==NULL){return NULL;}
	pSEM->semVal=initVal;
	pSEM->semMaxVal=maxVal;
	return pSEM;
}

/*******************************************************
*
* Function name :SemFree
* Description        :�ͷ�һ���ź���
* Parameter         :
*        @PSEM_CB   �ź�������ָ��
* Return          : ��
**********************************************************/
void SemFree(PSEM_CB pSEM){
	OSFree(pSEM);
}
/*******************************************************
*
* Function name :		SemPost
* Description        :����һ���ź���
* Parameter         :
*        @PSEM_CB   �ź�������ָ��
* Return          : TRUE or FALSE
**********************************************************/
uint32 SemPost(PSEM_CB pSEM){
	if(pSEM == NULL){ return FALSE; }
	OSEnterExclusiveMode();
	pSEM->semVal++;
	if(pSEM->semVal>pSEM->semMaxVal){
		/*���Ʋ��������ֵ*/
		pSEM->semVal=pSEM->semMaxVal;
	}
	OSExitExclusiveMode();
	return TRUE;
}
/*******************************************************
*
* Function name :		SemPend
* Description        :����һ���ź���
* Parameter         :
*        @PSEM_CB   �ź�������ָ��
*        @semWaitDelay   �ȴ�ʱ��
* Return          : TRUE or FALSE
**********************************************************/
uint32 SemPend(PSEM_CB pSEM,uint32 semWaitDelay){
	if(pSEM==NULL){return FALSE;}

	if(semWaitDelay==SEM_WAIT_ENDLESS){
		/*һֱ��*/
		while(pSEM->semVal==0);
		OSEnterExclusiveMode();
		pSEM->semVal--;
		OSExitExclusiveMode();
		return TRUE;
	}else{
		/*��ʱ���ڲ鿴*/
		if(semWaitDelay!=0){
			OSTaskDelay(semWaitDelay);
		}
		if(pSEM->semVal!=0){
			/*���ź���*/
			OSEnterExclusiveMode();
			pSEM->semVal--;
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}

