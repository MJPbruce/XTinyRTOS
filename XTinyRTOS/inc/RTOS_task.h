#ifndef _RTOS_TASK_H__
#define _RTOS_TASK_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************ͷ�ļ�����************************************/
#include "RTOS_type.h"
#include "RTOS_core.h"
/*********************************����******************************************/

/*********************************���ú�************************************/
#define OS_TICKS_PER_SEC 	1000		/*RTOS����Ƶ��*/
#define TASK_MIN_LEVEL 		10			/*������С���ȼ� */
#define TASK_TCB_NUM 			16			/*�������� */
/*********************************����******************************************/

/*********************************�Զ�������************************************/
typedef enum {
	TASK_MODE_THREAD=0,	/** ��ǰ�����߳�ģʽ */
	TASK_MODE_TIMER=1		/** ��ǰ����ʱ��ģʽ */
}TaskMode;

typedef void(*TaskRunFunction)(void* prg);
typedef struct {
	/** ��ջ��ַ��ָ�����������з����仯 */
	uint32 *memTask;
	/** ��ջ��ջ��ָ�룬�������ս�ʱ�����ڴ��ͷ� */
	uint32 *memLowTask;
	/** ��ջ��ջ��ָ�� */
	uint32 *memBottomTask;
	/** ����ʱ����� */
	uint32 runCount;
	/** ��ʱ���� */
	uint32 delayCount;
	/** ����id */
	uint32 taskID;
	/** ��ǰ״̬ */
	uint8 status;
	/** �������ȼ� */
	uint8 level;
	/** ��ǰ�����ģʽ */
	TaskMode taskMode;
}*PTASK_TCB,TASK_TCB;
/*********************************����******************************************/

/*********************************��������************************************/
uint32 		OSCreateBaseManual(TASK_TCB *mTASK_TCB,TaskRunFunction taskFun,void *prg,uint8 level,uint32 *mem,uint32 mem_num,	TaskMode mode);
TASK_TCB *OSCreateBase(TaskRunFunction taskFun,void *prg,uint8 level,uint32 task_num,	TaskMode mode);
void 			OSTaskEnd(void);
	
/*�����û�ʹ��*/
void 			OSInit(void);
void 			OSStart(void);
void 			OSTaskDelay(uint32 count);
void	 		OSTaskDel(void);
uint32 		OSSystickCount(void);
TASK_TCB *OSCreateTask(TaskRunFunction taskFun,void *prg,uint8 level,uint32 task_num);
void 			OSSchedule(void);
void 			OSScheduleCTRL(BOOL sw);
BOOL 			OSIsRun(void);

void 			OSEnterExclusiveMode(void);
void 			OSExitExclusiveMode(void);
/*********************************����******************************************/	
#ifdef __cplusplus
}
#endif
#endif
