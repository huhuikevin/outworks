#include <hic.h>
#include "config.h"
#include "type.h"
#include "system.h"
#include "linklay_v2.h"
#include "tool.h"
#include "timer16n.h"
#include "uart.h"
#include "route.h"
#include "debug.h"
#include "hw2000_drv.h"

#define    OpenRLed CONFIG_LED2=1
#define    CloseRLed CONFIG_LED2=0

//#define SENDER 1

section4 uint8_t app_data[MAX_APP_DATA_LEN];

static uint8_t curMac = MacPlc;
static uint8_t curMacRecv;

uint8_t app_recv_data(uint8_t *pdata);
uint8_t app_send_data();
void flush_led(uint8_t time);

uint8_t app_recv_data(uint8_t *pdata)
{
#if 1
	uint8_t i = MacTypeEnd;
	uint8_t len = 0;
	while(i--){
		curMacRecv = curMac;
		len = linklay_recv_data(pdata, curMac);
		curMac = (curMac + 1) % MacTypeEnd;
		if (len) {
			break;
		}
	}
	return len;	
#else
	curMacRecv = MacHw2000;
	return linklay_recv_data(pdata, MacHw2000);
#endif
}

void send_process()
{
    uint8_t len;
	mac_addr dst;
	uint8_t needack = 0;

	for (len = 0; len < 16; len++)
		app_data[len] = len;
#ifdef PLC_TEST	
	route_test_set_mac(0);
    dst.laddr = 0xffffffff;
    len = linklay_send_app_data(&dst, app_data, 16, needack);
    delay_ms(500);
#endif
#ifdef HW2000_TEST
	route_test_set_mac(1);
    dst.laddr = 0xffffffff;
    len = linklay_send_app_data(&dst, app_data, 16, needack);
    delay_ms(500);
#endif
	
}

int8u recv_process()
{
	uint8_t i;
    uint8_t len = app_recv_data(&app_data[0]);

    if (len == 16){
        for (i = 0; i< 16; i++)
        {
        	if (app_data[i] != i)
				break;
        }
		if (i == 16)
        {
        	if (curMacRecv == 0)
				CONFIG_LED1 = 1;
			else if(curMacRecv == 1)
				CONFIG_LED2 = 1;
			delay_ms(200);
			if (curMacRecv == 0)
				CONFIG_LED1 = 0;
			else if(curMacRecv == 1)
				CONFIG_LED2 = 0;
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

void mac_test()
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

	mac_test();
	//hw2000_read_rssi();
}


