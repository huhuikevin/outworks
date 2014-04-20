#ifndef __TIMER8N__H_
#define  __TIMER8N__H_
#include "type.h"

void timer8N(void);
void timer8n_init(void);
int16u Timetick(void);
sbit IsTimeOut(int16u timeout);
#endif /* __TIMER8N__H_ */

