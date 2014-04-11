#include "P111.h"
#include "timer8n.h"

section5 unsigned long global_timer;
/*************************************
** ����ԭ��: void timer0(void);     **
** ��    ��: ��RTC ����             **		
**           ��ȡ�����±�־λ��ʽ **
**           ��ȡRTC����            **
**           10ms
*************************************/
void timer8N(void)  	// 2.048MS
{
	if(T8NIF) {
 		T8N=96;							// ��ʱ�����¸� 				 
 		T8NIF = 0;
		global_timer++;
	}
}

void timer8n_init(void) // timer 8n run as 2ms 
{
	T8NC=0x8e;	/*bit 2-0 :101 128 ��Ƶ ,, 20M / 2 = 10M / 128 = 80K */
	T8N=96;
	T8NIE = 0; //disable irq, use poll mode   
 	global_timer=0;
}

int32u Timetick(void)
{
	timer8N();
	return global_timer;
}


