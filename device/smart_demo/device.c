#include <hic.h>
#include "config.h"
#include "type.h"
#include "tool.h"
//#include "soc_25xx.h"
#include "plc_mac.h"
//#include "timer8n.h"
//#include "uart.h"

void iniSCI(void)
{
    TX1M=1; 
    RX1M=1;
}

void Ini2571(void)
{    
    PAT=0X80;    
    PBT=0X05;     //0x85      
    PCT=0X10;  
}       


void IO_Init(void)
{
    //GIE=1;
    PBT=0x05;
    PAT=0x80;
    PCT=0x10;
    PB5 = 1;
	PB6 = 0;
	PB7 = 0;    
    DelayMs(1000);
    PB5 = 0;
}

void board_init()
{
    IO_Init();
 
	plc_mac_init(50,2);
    //timer8n_init();
	
#ifdef CONFIG_LINKLAY_UART	
	UartInit(CONFIG_LINKLAY_UART, 9600);
#endif

#ifdef CONFIG_CONSOLE_UART
	UartInit(CONFIG_CONSOLE_UART, 9600);
#endif
}


