#ifndef __SYSTEM__H_
#define __SYSTEM__H_

#include "soc_25xx.h"

#define NOP()		 __Asm  nop 
#define SLEEP()          __Asm  IDLE
#define watchdog()       __Asm  CWDT
#define enable_irq()     GIE=1
#define disable_irq()    GIE=0

#define MSG_MAX_LEN 32

#define SYSTEM_TICK 1 //MS
#define SYSTEM_HZ (1000/SYSTEM_TICK) // 500hz

#define TICK_2_S (t) (t / SYSTEM_HZ)
#define TICK_2_MS(t) (t)

#define MS_TO_TICK(m) (m)

#define SYNC_WORD 0x7e
#define FRAME_START 0xaa

#endif /* __SYSTEM__H_ */
