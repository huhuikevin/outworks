//系统的一些函数
#include <hic.h>
#include "type.h"
#include "system.h"

void sys_app_process();

void RAM_Clr(void) //ram全清
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
    board_init();//device 相关的文件中定义
    watchdog();
   
    do {
        watchdog();
        enable_irq();
        sys_app_process();
    }while(1);
}