//ϵͳ��һЩ����
#include <hic.h>
#include "type.h"
#include "system.h"
#include "timer8n.h"
#include "linklay.h"

void sys_app_process();
void board_init();

void RAM_Clr(void) //ramȫ��
{
	__asm 
  { 
	CLR   IAAL;
	CLR   IAAH;
	CLR   IAD;
	INC   IAAL,1;
	JBS   PSW,C
	GOTO  $-3;
	INC   IAAH,1;
	JBS   IAAH,4
	GOTO  $-6;
  } 
}


void main(void)
{
    RAM_Clr();
    board_init();//device ��ص��ļ��ж���
    watchdog();
    timer8N();
	linklay_init();
    do {
        watchdog();
        enable_irq();
	 timer8N();
        sys_app_process();
    }while(1);
}