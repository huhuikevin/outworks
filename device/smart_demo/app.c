#include <hic.h>
#include "config.h"
#include "type.h"
#include "system.h"
#include "linklay_v2.h"
#include "tool.h"
#include "timer16n.h"
#include "uart.h"
#include "debug.h"


#define    OpenRLed CONFIG_LED2=1
#define    CloseRLed CONFIG_LED2=0

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

	for (len = 0; len < 16; len++)
		app_data[len] = len;

    dst.laddr = 0xffffffff;
    len = linklay_send_app_data(&dst, app_data, 16, needack);
    if (len){
        delay_ms(1000);
    }
}

int8u recv_process()
{
	uint8_t i;
    uint8_t len = linklay_recv_data(&app_data[0], MacHw2000);

    if (len == 16){
        for (i = 0; i< 16; i++)
        {
        	if (app_data[i] != i)
				break;
        }
		if (i == 16)
        {
			OpenRLed;
			delay_ms(200);
			CloseRLed;
        }
    }
}

void uart_test()
{
	uint8_t b=0x55;

#ifdef SENDER    
	console_tx_one_byte(b);
#else    
	b = console_rx_one_byte();
   print_char(b);
   //print_char('\n');
   print_num(200);
   //delay_ms(1000);
   print_char('\r');
   print_char('\n');
	//console_tx_one_byte(b);
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


