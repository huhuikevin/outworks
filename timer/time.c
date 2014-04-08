#include "P111.h"
#include "timer8n.h"
#include "uart.h"
section5 unsigned short gblClock;
section5 unsigned short gblCLock1;
unsigned long global_timer=0;
/*************************************
** 函数原型: void timer0(void);     **
** 功    能: 读RTC 程序             **		
**           采取检查更新标志位方式 **
**           读取RTC数据            **
**           10ms
*************************************/
void timer8N(void)  		//2.048MS
{
 
 T8N=96;								// 定时器重新赋 				 
 global_timer++; 
 

  
}
void Timetick(void)
{
if(gblCLock1>gblClock)
global_timer+=(0xFFFF-gblCLock1+gblClock);
else if(gblCLock1<gblClock);
global_timer+=(gblClock-gblCLock1);
gblCLock1=gblClock;

}


void timer8n_init(void)
{
T8NC=0X8F;	
T8N=96;
//T8NIE=1;
gblClock=0;
gblCLock1=0;

}


int32u halCommonGetInt32uMillisecondTick(void)//ms counter;
{
int32u time;
time=global_timer;
time/=10;
return time;
}
int16u halCommonGetInt16uQuarterSecondTick(void)
{
  return (int16u)(halCommonGetInt32uMillisecondTick()>>8);
}
#ifdef MSEVENT
int16u halCommonGetInt16uMillisecondTick(void) 
{
  return (int16u)(halCommonGetInt32uMillisecondTick() & 0xFFFF);
}
 #endif

