#include <hic.h>
#include "soc_25xx.h"
#include "type.h"
#include "linklay.h"
#include "tool.h"

#define  R_LED PB5
#define OpenLed R_LED=1
#define CloseLed R_LED=0

section4 int8u app_data[MAX_APP_DATA_LEN];

int8u app_recv_data();
int8u app_send_data();
void flush_led(int8u time);

void app_process(void)
{
	int8u len =app_recv_data();
	if (len != 0) {
		if (app_data[0] != 1)
		{
			OpenLed;
			DelayMs(2000);
			CloseLed;
		}else
			flush_led(app_data[1]);
		app_send_data();
	}
   app_send_data();
}


void flush_led(int8u time)
{
	while(time--) {
		OpenLed;
		DelayMs(200);
		CloseLed;
		DelayMs(200);
	}
}

int8u app_recv_data()
{
	return linklay_recv_data(app_data, MacPlc);
}

int8u app_send_data()
{
	app_data[0] = 1;
	app_data[1]++;

	while(linklay_send_data(app_data, 2, MacPlc) != 2);
}
