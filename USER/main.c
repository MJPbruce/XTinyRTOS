
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "tim3.h"
#include "net_init.h"
#include "sdio_sdcard.h"
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
	RTOSUserInit();
	OSStart();
	while(1);
}





































/************************************************************************/

//���败��������
 
//DevEBox  ��Խ����
//�Ա����̣�mcudev.taobao.com
//�Ա����̣�shop389957290.taobao.com	

/************************************************************************/








