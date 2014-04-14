#include <hic.h>
#include "soc_25xx.h"
#include "type.h"
#include "linklay.h"
#include "tool.h"
#include "timer8n.h"
#include "debug.h"

#define    OpenRLed PB5=1
#define    CloseRLed PB5=0

#define    OpenSLed PC1=1
#define    CloseSLed PC1=0

//#define SENDER 1

section4 int8u app_data[MAX_APP_DATA_LEN];

int8u app_recv_data();
int8u app_send_data();
void flush_led(int8u time);

void send_process()
{
    int8u len;

    app_data[0] = 0xaa;
    app_data[1] = 0x55;
    	
    len = linklay_send_data(app_data, 2, MacPlc);
    if (len){
        DelayMs(2000);
    }
}

int8u recv_process()
{
    int8u len = linklay_recv_data(&app_data[0], MacPlc);

    if (len == 2){
        if (app_data[0] == 0xaa && app_data[1] == 0x55)
        {
			OpenRLed;
			DelayMs(200);
			CloseRLed;
        }
    }
}

void app_process(void)
{
#ifdef SENDER
    send_process();
#else
    recv_process();
#endif
}


