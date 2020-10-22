/********************************************************************************
* @File name: RTOS_task.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS ����������ģ��
********************************************************************************/

/*********************************ͷ�ļ�����************************************/
#include "RTOS_task.h"
#include "RTOS_mem.h"
#include "RTOS_timer.h"
/*********************************����******************************************/

/*********************************ȫ�ֱ���************************************/
volatile 	uint32 sysClkCount = 0;								/*ϵͳ����ʱ�����*/
PTASK_TCB TASK_TCB_LIST[TASK_TCB_NUM] = {0};		/*������ƿ��б�*/
uint32 **	OSLastTaskMem = NULL;									/*��һ�����еĶ�ջ��ַ*/
PTASK_TCB TCBIng = NULL;												/*�������е�����*/
uint32 		exclusiveTemp=0;					
uint8 		exclusiveCount=0;											/*��ռģʽ����*/
BOOL			OSScheduleSW=TRUE;												/*OS�ĵ��ȿ���*/
/*********************************����******************************************/

/*********************************��������************************************/
void OSIdle(void *prg);
/*********************************����******************************************/

/******************************************************
*
* Function name :OSFindTasckCB
* Description        :�������tcb�����б����ҵ��ܹ�ʹ�õ������.
* Parameter         :
*        @CRC     �ϴμ����CRC16ֵ    
*        @byte    ��Ҫ��������һ�ֽ�
* Return          : ��ȡ�����õ��±꣬-1��ʾΪ��ȡ��
**********************************************************/
static int32 OSFindTasckCB(void){
	uint32 i;
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]==NULL){
			return i;
		}
	}
	return -1;
}

/*******************************************************
*
* Function name :	OSEnterExclusiveMode
* Description   : �����ռģʽ�����ᱻ�жϴ��
* Parameter     : ��
* Return        : ��
**********************************************************/
void OSEnterExclusiveMode(void){
	/*���û�н�����ٽ����������*/
	exclusiveTemp=DisableInterrupt();
	exclusiveCount++;
}

/*******************************************************
*
* Function name :	OSExitExclusiveMode
* Description   : �˳���ռģʽ
* Parameter     : ��
* Return        : ��
**********************************************************/
void OSExitExclusiveMode(void){
	if(exclusiveCount>0){
		exclusiveCount--;
		if(!exclusiveCount){
			EnableInterrupt(exclusiveTemp);
		}
	}
}

/*******************************************************
*
* Function name :	OSTaskEnd
* Description   : ��һ���������ʱ����ִ�д˺����������������ɾ����ǰ����
* Parameter     : ��
* Return        : ��
**********************************************************/
void OSTaskEnd(void){
	if(TCBIng->taskMode==TASK_MODE_THREAD){
		OSTaskDel();
	}else{
		if(((PTIMER_CB)TCBIng)->mode==TIMER_MODE_ONCE){
			OSTaskDel();
		}else{
			int32 dTimer;
			dTimer=((PTIMER_CB)TCBIng)->backCallTimer - TCBIng->runCount;
			//�������е�ʱ��Ȼ�������ʱ
			if(dTimer>0){
				OSTaskDelay(dTimer);
			}
			TimerReset(((PTIMER_CB)TCBIng));
		}
	}
}
/*******************************************************
*
* Function name :OSCreateBaseManual
* Description        :�ֶ�����tcb���ƿ�,����һ�����񣬲��Ҵ��ھ���״̬,�ú�����ѡ�񴴽�Ϊ��ʱ�������ǣ��߳�����
* Parameter         :
* @mTASK_TCB   ָ�������ָ��   
* @fun_poi   �����񴫵ݵĲ���
* @prg	  �������Ķ����ֳ��Ķ�ջ
* @level   ���ȼ�
* @mem   		ջ��ָ��
* @mem_num   ջ��С
* @mem_low   ջ��ָ��
* @mode   	������ģʽ��������task��timerģʽ
* Return          : TRUE success , FALSE fail
**********************************************************/
uint32 OSCreateBaseManual(
	TASK_TCB *mTASK_TCB,TaskRunFunction taskFun,void *prg,uint8 level,uint32 *mem,uint32 mem_num,TaskMode mode
){
	/*ջ��ָ��*/
	uint32 *memLow;
	/*ջ��ָ��*/
	uint32 *memBottom;
	int32 usableTask;
	if(mTASK_TCB == NULL){return FALSE;}
	if(mem == NULL){return FALSE;}
	/*��ȡ���õ���Դ*/
	usableTask=OSFindTasckCB();
	if(usableTask<0){return FALSE;}
	/* �����λ�ĵ�ַ */
	memLow=mem-mem_num;
	/*����ջ����ָ��*/
	memBottom=mem;

	/* Registers stacked as if auto-saved on exception */
	*(mem) = (uint32)0x01000000L; /* xPSR */
	*(--mem) = ((uint32)taskFun); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--mem) = (uint32)OSTaskEnd;
	*(--mem) = (uint32)0x12121212L; /* R12*/
	*(--mem) = (uint32)0x03030303L; /* R3 */
	*(--mem) = (uint32)0x02020202L; /* R2 */
	*(--mem) = (uint32)0x01010101L; /* R1 */
	*(--mem) = (uint32)prg; 				/* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--mem) = (uint32)0x11111111L; /* R11 */
	*(--mem) = (uint32)0x10101010L; /* R10 */
	*(--mem) = (uint32)0x09090909L; /* R9 */
	*(--mem) = (uint32)0x08080808L; /* R8 */
	*(--mem) = (uint32)0x07070707L; /* R7 */
	*(--mem) = (uint32)0x06060606L; /* R6 */
	*(--mem) = (uint32)0x05050505L; /* R5 */
	*(--mem) = (uint32)0x04040404L; /* R4 */	
	/*���ö�ջ�ĵ�ַ*/
	mTASK_TCB->memTask=mem;
	/*��ʼ��������ƿ�*/
	mTASK_TCB->runCount=0;
	mTASK_TCB->delayCount=0;
	/*������ƿ�ĵ�ַ��Ϊ����id*/
	mTASK_TCB->taskID=(uint32)mTASK_TCB;
	mTASK_TCB->status=TRUE;
	/*�����������ȼ�*/
	if(level>TASK_MIN_LEVEL){level=TASK_MIN_LEVEL;};
	/*������������ȼ�*/
	mTASK_TCB->level=level;
	/*����ջ��ָ��*/
	mTASK_TCB->memLowTask=memLow;
	/*����ջ����ָ��*/
	mTASK_TCB->memBottomTask=memBottom;	
	/*��ǰ�����ģʽ*/
	mTASK_TCB->taskMode=mode;
	/*����������ƿ�*/
	TASK_TCB_LIST[usableTask]=mTASK_TCB;
	return TRUE;
}

/*******************************************************
*
* Function name :OSCreateBase
* Description        :����һ�����񣬲��Ҵ��ھ���״̬,�ú�����ѡ�񴴽�Ϊ��ʱ�������ǣ��߳�����
* Parameter         :
*        @fun_poi        ָ�������ָ��    
*        @prg    �����񴫵ݵĲ���
*        @level    ���ȼ�
*        @task_num    �������Ķ����ֳ��Ķ�ջ
*        @mode    ������ģʽ��������task timer
* Return          : ʧ�ܷ���NULL�����򷵻�������ƿ�
**********************************************************/
TASK_TCB *OSCreateBase(
	TaskRunFunction taskFun,void *	prg,uint8 level,uint32 	task_num,TaskMode mode
){
	uint32 	*mem;
	TASK_TCB *mTASK_TCB;	
	/*������ƿ���ڴ�*/
	mTASK_TCB=(TASK_TCB *)OSMalloc(sizeof(TASK_TCB));
	if(mTASK_TCB==NULL){return NULL;}
	/*�����ջ���ڴ�*/
	mem=(uint32 *)OSMalloc(sizeof(uint32)*task_num);
	if(mem==NULL){OSFree(mTASK_TCB);return NULL;}		
	/*��������*/
	if(
		OSCreateBaseManual(mTASK_TCB,taskFun,prg,level,&mem[task_num-1],task_num,TASK_MODE_THREAD)
		==FALSE
	){
		/*��������ˣ�����ʧ����*/
		OSFree(mem);
		OSFree(mTASK_TCB);
		return NULL;
	}
	return mTASK_TCB;
}

/*******************************************************
*
* Function name :OSCreateTask
* Description        :����һ�����񣬲��Ҵ��ھ���״̬
* Parameter         :
*        @fun_poi     ָ�������ָ��       
*        @prg    			�����񴫵ݵĲ���
*        @level    		�������ȼ�
*        @task_num    �������Ķ����ֳ��Ķ�ջ
* Return          : TRUE success , FALSE fail
**********************************************************/
TASK_TCB* OSCreateTask(TaskRunFunction taskFun,void *prg,uint8 level,uint32 task_num){
	return OSCreateBase(taskFun,prg,level,task_num,TASK_MODE_THREAD);
}


/*******************************************************
*
* Function name :OSTaskDel
* Description        :ɾ���������е�����
* Parameter         :none
* Return          :none
**********************************************************/
void OSTaskDel(void ){
	uint32 i;
	uint32 lev;
	lev=DisableInterrupt();
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]==TCBIng){
			OSFree(TCBIng->memLowTask);
			OSFree(TCBIng);
			TASK_TCB_LIST[i] = NULL;
			break;
		}
	}	
	EnableInterrupt(lev);
	OpenSchedule();
}

/*******************************************************
*
* Function name :OSTaskDelay
* Description        :��ǰ������ʱ
* Parameter         :
*        @count     ��ʱ���ٸ�ϵͳʱ��   
* Return          :none
**********************************************************/
void OSTaskDelay(uint32 count){
	uint32 lev;
	lev=DisableInterrupt();
	TCBIng->delayCount=count;
	EnableInterrupt(lev);
	OpenSchedule();
}
/*******************************************************
*
* Function name :TaskSW
* Description        :�����л����ú�����core.c�е���
* Parameter         :none 
* Return          :none
**********************************************************/
void *OSTaskSW(void){
	uint32 i=0;
	TASK_TCB *max_TASK_TCB=NULL;
	static TASK_TCB *back_task_tcb=NULL;
	/*��ʱ��ģʽ*/
	if(TCBIng->taskMode==TASK_MODE_TIMER){
		/*����Ƕ�ʱ��ģʽ*/
		if(((PTIMER_CB)TCBIng)->resetFlag==1){
			/*�˵�ѹջʱ���ֵ*/
			*OSLastTaskMem=TCBIng->memBottomTask-15;
			((PTIMER_CB)TCBIng)->resetFlag=0;
		}
	}
	/*����û����ͬ���ȼ���*/
	if(back_task_tcb!=NULL){
		uint32 spotted=0;
		for(i=0;i<TASK_TCB_NUM;i++){
			if(back_task_tcb==TASK_TCB_LIST[i]){
				spotted=1;
				continue; 
			}
			/*ȷ����û�б����ȹ�������*/
			if(spotted==1){
				if(TASK_TCB_LIST[i]!=NULL&&
					TASK_TCB_LIST[i]->status==TRUE&&
				/*����û�б���ʱ*/
					TASK_TCB_LIST[i]->delayCount==0&&
					back_task_tcb->level==TASK_TCB_LIST[i]->level){
					max_TASK_TCB=TASK_TCB_LIST[i];
					goto _exit;
				}
			}
		}
	}
	back_task_tcb=NULL;
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]!=NULL){
			if(TASK_TCB_LIST[i]->status==TRUE&&
				/*����û�б���ʱ*/
					TASK_TCB_LIST[i]->delayCount==0){
				if(max_TASK_TCB==NULL||(
					max_TASK_TCB->status==FALSE||
				/*����û�б���ʱ*/
					max_TASK_TCB->delayCount>0)){
					max_TASK_TCB=TASK_TCB_LIST[i];
					continue;
				}
				/*��ȡ���ȼ���ߵ�*/
				if(max_TASK_TCB->level > TASK_TCB_LIST[i]->level){
					max_TASK_TCB=TASK_TCB_LIST[i];
				}
			}
		}
	}
//	for(i=0;i<TASK_TCB_NUM;i++){
//		if(TASK_TCB_LIST[i]!=null){
//			if(TASK_TCB_LIST[i]->status==true&&
//				//����û�б���ʱ
//					TASK_TCB_LIST[i]->delay_count==0){
//				//��ȡ���ȼ���ߵ�
//				if(max_TASK_TCB==NULL){
//					max_TASK_TCB=TASK_TCB_LIST[i];
//				}else if(max_TASK_TCB->level > TASK_TCB_LIST[i]->level){
//					max_TASK_TCB=TASK_TCB_LIST[i];
//				}
//			}
//		}
//	}

_exit:
	OSLastTaskMem=&max_TASK_TCB->memTask;	/*��ǰ�Ķ�ջ*/
	TCBIng=max_TASK_TCB;									/*���浱ǰ���е�tcb*/
	back_task_tcb=max_TASK_TCB;						/*�����ϴλ�õ�������ȼ�*/
	return max_TASK_TCB->memTask;					/*���ض�ջ�ĵ�ַ*/
}


/*******************************************************
*
* Function name :OSInit
* Description        :����ϵͳ��ʼ��
* Parameter         :��
* Return          :��
**********************************************************/
void OSInit(void){
	InitMem();
	
	OSScheduleSW=FALSE;/*���ȿ�*/
}

/*******************************************************
*
* Function name :OSStart
* Description        :����ϵͳ����
* Parameter         :��
* Return          :��
**********************************************************/
void OSStart(void){
	/*����һ����������,������������ȼ�Ϊ������ȼ�*/
	if(OSCreateTask(OSIdle,NULL,TASK_MIN_LEVEL,64)==FALSE){return ;}
	OSScheduleSW=TRUE;
	/*��ת�������������ϵͳ*/
	StartRTOS();
}

/*******************************************************
*
* Function name :OSStart
* Description        :��ȡ����ϵͳ������ʱ�Ӽ���
* Parameter         :��
* Return          :����ϵͳ������ʱ��
**********************************************************/
uint32 OSSystickCount(void){
	return sysClkCount;
}

/*******************************************************
*
* Function name : OSIdle
* Description        :����ϵͳ���к�����������������ȼ�
* Parameter         :��
* Return          :��
**********************************************************/
void OSIdle(void *prg){
	prg=prg;
	for(;;){
	}
}
/*******************************************************
*
* Function name :OSScheduleCTRL
* Description        :���ȿ�������
* Parameter         :��
* Return          :��
**********************************************************/
void OSScheduleCTRL(BOOL sw){
	OSEnterExclusiveMode();
	OSScheduleSW = sw;
	OSExitExclusiveMode();
}
/*******************************************************
*
* Function name :OSScheduleCTRL
* Description        :OS�Ƿ����\��
* Parameter         :��
* Return          :����״̬
**********************************************************/
BOOL OSIsRun(void){
	return OSScheduleSW;
}
/*******************************************************
*
* Function name :OSSchedule
* Description        :���ý�����һ�β���ϵͳ����
* Parameter         :��
* Return          :��
**********************************************************/
void OSSchedule(void){
	if(OSScheduleSW){
		/*������˵��ȣ����������*/
		OpenSchedule();
	}
}
/*******************************************************
*
* Function name :TaskLoop
* Description        :ʹ�������ߵ������ʱ���һ
* Parameter         :none
* Return          :none
**********************************************************/
void OSProcess(void){
	uint32 i;
	uint32 lev;
	/*ϵͳ������ʱ��+1*/
	sysClkCount++;
	for(i=0;i<TASK_TCB_NUM;i++){
		/*�̴߳��ڻ�Ծ״̬*/
		if(TASK_TCB_LIST[i] != NULL && TASK_TCB_LIST[i]->status == TRUE){
			/*�Ƿ�����ʱ*/
			if(TASK_TCB_LIST[i]->delayCount>0){
				lev=DisableInterrupt();
				TASK_TCB_LIST[i]->delayCount--;
				EnableInterrupt(lev);
			}
			lev=DisableInterrupt();
			/*����ʱ��+1*/
			TASK_TCB_LIST[i]->runCount++;	
			EnableInterrupt(lev);
		}
	}
}
/*******************************************************
*
* Function name :OSScheduleProcess
* Description        :�ú��������жϵ��ȵĴ���
* Parameter         :none
* Return          :none
**********************************************************/
void OSScheduleProcess(void){
	if(OSScheduleSW){
		OSProcess();			/*�߳�����ʱ�䴦��*/
		OSSchedule();			/*����pendsv�ж�*/
	}
}
