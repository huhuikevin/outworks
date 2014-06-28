#include <hic.h>
#include "config.h"
#include "type.h"
#include "tool.h"
//#include "soc_25xx.h"
#include "plc_mac.h"
//#include "timer8n.h"
#include "uart.h"
#include "hw2000_drv.h"

void iniSCI(void)
{
    TX1M=1; 
    RX1M=1;
}

void Ini2571(void)
{    
    PAT=0X40;    
    PBT=0X05;     //0x85      
    PCT=0X10;  
}       


void IO_Init(void)
{
    //GIE=1;
    /* PB 5out 4out 3out 2in 1out 0in*/
    PBT=0x05;
    PAT=0x40;
    PCT=0x10;
    PB5 = 0;
	PB6 = 0;
	PB7 = 0;
	CONFIG_3_3V_CTRL = 0;
	CONFIG_LED1 = 1;
	CONFIG_LED2 = 1;
    DelayMs(1000);
	CONFIG_LED1 = 0;
	CONFIG_LED2 = 0;

}

void board_init()
{
    IO_Init();
 
	plc_mac_init(50,2);
    //timer8n_init();
#ifdef CONFIG_HW2000    
	hw2000_init();
#endif
#ifdef CONFIG_LINKLAY_UART	
	uart_init(CONFIG_LINKLAY_UART, 9600, 0, 0);
#endif

#ifdef CONFIG_CONSOLE_UART
	uart_init(CONFIG_CONSOLE_UART, 9600, 0, 0);
#endif
#ifdef CONFIG_HG72xx_UART
	uart_init(CONFIG_HG72xx_UART, 2400, 1, 1);
#endif
}


