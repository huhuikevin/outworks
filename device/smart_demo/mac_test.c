#include <hic.h>
#include "type.h"
#include "tool.h"
#include "debug.h"
#include "system.h"
#include "timer16n.h"
extern uint8_t plc_mac_tx(mac_addr *pdst, uint8_t *data, uint8_t length);
extern uint8_t plc_mac_rx_with_rssi(uint8_t *pdata, uint8_t *prssiv);
extern void plc_mac_proc();
#define    OpenRLed PB5=1
#define    CloseRLed PB5=0

#define    OpenSLed PC1=1
#define    CloseSLed PC1=0

//#define SENDER 1

section4 int8u app_data[MSG_MAX_LEN];

int8u app_recv_data();
int8u app_send_data();
void flush_led(int8u time);

void send_process()
{
    int8u len;
	mac_addr addr;

    app_data[0] = 0xaa;
    app_data[1] = 0x55;

    app_data[2] = 0xaa;
    app_data[3] = 0x55;

    app_data[4] = 0xaa;
    app_data[5] = 0x55;

    app_data[6] = 0xaa;
    app_data[7] = 0x55;

	addr.laddr = 0xffffffff;
    	
    len = plc_mac_tx(&addr, app_data, 8);
    if (len == 0){
        delay_ms(2000);
    }
}

int8u recv_process()
{
	uint8_t rssi=0xaa;
    int8u len = plc_mac_rx_with_rssi(&app_data[0], &rssi);

    if (len == 8){
        if (app_data[7] == 0x55 )
        {
			OpenRLed;
			delay_ms(500);
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

void sys_app_process()
{
	plc_mac_proc();
	watchdog();
	app_process();
	watchdog();
}

