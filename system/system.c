//系统的一些函数
#include <hic.h>
#include "config.h"
#include "type.h"
#include "system.h"
#include "timer16n.h"
#include "linklay_v2.h"

mac_addr self_mac;

void sys_app_process();
void board_init();
void plc_recv_proc();
void plc_send_proc();
#ifdef CONFIG_TYPE_AUTODEVICE
void device_route_init();
#endif
#ifdef CONFIG_TYPE_AUTOGATEWAY
void gateway_route_init();
#endif

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
#ifdef CONFIG_MACADDR_FLASHADDR
    flash_read(CONFIG_MACADDR_FLASHADDR, &self_mac, sizeof(self_mac));
#endif
    //timer8N();
    linklay_init();

#ifndef CONFIG_NO_ROUTE
#ifdef CONFIG_TYPE_AUTODEVICE
	device_route_init();
#endif

#ifdef CONFIG_TYPE_AUTOGATEWAY
	gateway_route_init();
#endif
#endif

    do {
        watchdog();
        enable_irq();
	 //timer8N();
        sys_app_process();
    }while(1);
}

