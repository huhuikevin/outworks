#include <hic.h>
#include "type.h"
#include "soc_25xx.h"
#include "plc.h"

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
}

void board_init()
{
    IO_Init();
    plc_init(); 
}
