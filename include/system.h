#ifndef __SYSTEM__H_
#define __SYSTEM__H_

#include "soc_25xx.h"

#define MAX_DATA_LEN 32

typedef enum {
    MacPlc = 0,
    MacWireless_2_4G,
    MacTypeEnd;
}uMacType;



#define NOP()		 __Asm  nop 
#define SLEEP()          __Asm  IDLE
#define watchdog()       __Asm  CWDT
#define enable_irq()     GIE=1
#define disable_irq()    GIE=0

#endif /* __SYSTEM__H_ */
