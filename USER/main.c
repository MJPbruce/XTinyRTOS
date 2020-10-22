
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "tim3.h"
#include "net_init.h"
#include <stdlib.h>

#include "RTOS_task.h"
#include "RTOS_APP.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/lwiperf.h"

 int main(void)
 {	 		     
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	 
	OSInit();
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
	 
	if(lwip_comm_init()==0){
		//printf("�����ʼ���ɹ�..\r\n");
	}
	httpd_init();
	//RTOSUserInit();
	OSStart();
	while(1);
}
////�ŵ��������ĳ�ʼ���г�ʼ��
//void Timer1CountInitial(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_DeInit(TIM1);	/*��λTIM1��ʱ��*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	/*��ʱ��*/
//	TIM_TimeBaseStructure.TIM_Period = 100;	/*ʱ�ӵδ�Ĵ����������ж�������1ms�ж�һ��*/     
//	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;	/* ��Ƶ720*/       
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  	 
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	/*�����������ϼ���*/
//	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
//	/* Clear TIM1 update pending flag  ���TIM1����жϱ�־]  */
//	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
//	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;	/*����ж�*/
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);	/* Enable TIM1 Update interrupt TIM1����ж�����*/
//	TIM1->CR1 |= TIM_CR1_CEN;	/*����Timer1����*/
//}


//void TIM1_UP_IRQHandler(void)
//{        
//	//TIM_TimeBaseStructure.TIM_Period = 100-1;//�Զ���װֵ����ʱ���жϵ�����Ϊ100ms��
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
//	{  
//		sys_tick++;
//		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
//	}
//}








































/************************************************************************/

//���败��������
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/








