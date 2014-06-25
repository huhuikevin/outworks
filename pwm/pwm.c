#include <hic.h>
#include "type.h"
#include "config.h"
#include "system.h"
#include "uart.h"
#include "pwm.h"
#include "tool.h"
typedef struct{
	uint8_t count;
	uint8_t preriod;
	uint8_t width;
	uint8_t div:2;
	uint8_t mod:2;
	uint8_t pos:4;
}pwm_t;

#define TSOC 10000000L

section64 volatile uart_register pwm[2]@0xFFC0;

/*
最大频率 312K    Hz
最小频率 1220    Hz
*/
void pwm_init(uint8_t idx, uint32_t freq)
{
	uint8_t preriod;

	if (freq >= 312000)
		preriod = 0;
	else if (freq <= 1220)
		preriod = 0xff;
	else{
		preriod = (TSOC)/(freq*32) - 1;
	}
	if (preriod == 0)
		preriod = 1;
	if (preriod == 255)
		preriod = 254;
	pwm[idx].mod = 0x0;//stop
	pwm[idx].preriod = preriod;
	pwm[idx].count = preriod-1;
	pwm[idx].div = 0x3;
	pwm[idx].width = 0x0;
#if CONFIG_PWM_IO=PB7
	T8POC = 0x20;
#endif
#if CONFIG_PWM_IO=PB6
	T8POC = 0x10;
#endif
#if CONFIG_PWM_IO=PB5
	T8POC = 0x02;
#endif
#if CONFIG_PWM_IO=PB4
	T8POC = 0x01;
#endif
	pwm[idx].mod = 0x2;// start
}
/*3.125k 占空比 0-100 */
void pwm_duty_cycle(uint8 duty_cycle)
{
	pwm[idx].width = duty_cycle;
}

