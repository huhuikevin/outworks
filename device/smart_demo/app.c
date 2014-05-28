#include <hic.h>
#include "config.h"
#include "type.h"
#include "system.h"
#include "linklay_v2.h"
#include "tool.h"
#include "timer16n.h"
#include "uart.h"
#include "debug.h"


#define    OpenRLed CONFIG_RLED_IO=1
#define    CloseRLed CONFIG_RLED_IO=0



//#define SENDER 1

section4 uint8_t app_data[MAX_APP_DATA_LEN];

uint8_t app_recv_data();
uint8_t app_send_data();
void flush_led(uint8_t time);

void send_process()
{
    uint8_t len;
	mac_addr dst;
	uint8_t needack = 0;

    app_data[0] = 0xaa;
    app_data[1] = 0x55;

    app_data[2] = 0xaa;
    app_data[3] = 0x55;

    app_data[4] = 0xaa;
    app_data[5] = 0x55;

    app_data[6] = 0xaa;
    app_data[7] = 0x55;

    dst.laddr = 0xffffffff;
    len = linklay_send_app_data(&dst, app_data, 8, needack);
    if (len){
        delay_ms(2000);
    }
}

int8u recv_process()
{
    uint8_t len = linklay_recv_data(&app_data[0], MacPlc);

    if (len == 8){
        if (app_data[7] == 0x55 )
        {
			OpenRLed;
			delay_ms(50);
			CloseRLed;
        }
    }
}


void uart_test()
{
	uint8_t b=0x55;
#ifdef SENDER    
	uart_tx_bytes(&b, 1);
#else    
	b = uart_rx_byte();    
	uart_tx_bytes(&b, 1);
#endif
}

void plc_test()
{
#ifdef SENDER
	send_process();
#else
	recv_process();
#endif
}

void app_process(void)
{
#ifdef UART_TEST
	uart_test();
#endif

#ifdef PLC_TEST
	plc_test();
#endif
}


