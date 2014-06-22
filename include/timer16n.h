/**************************************************************************
* 版权声明：Copyright@2014 上海海尔集成电路有限公司
* 文件名称：timer.h
* 功能描述：定时器函数声明		
* 函数列表：init_t16g2
            init_t16g1
            delay_ms
            sys_tick_init
            sys_tick_run
* 文件作者：sundy  	
* 文件版本：1.0 			
* 完成日期：2014-04-24			
* 修订历史：
* 修订日期：
**************************************************************************/
#include "type.h"

#ifndef _TIMER_H_
#define _TIMER_H_
#define DELAY1NOP() NOP();
#define DELAY2NOP() DELAY1NOP();NOP();
#define DELAY3NOP() DELAY2NOP();NOP();
#define DELAY4NOP() DELAY3NOP();NOP();
#define DELAY5NOP() DELAY4NOP();NOP();
#define DELAY6NOP() DELAY5NOP();NOP();
#define DELAY7NOP() DELAY6NOP();NOP();
#define DELAY8NOP() DELAY7NOP();NOP();
#define DELAY9NOP() DELAY8NOP();NOP();

#define DELAY10NOP() DELAY9NOP();NOP();   
#define DELAY20NOP() DELAY10NOP();DELAY10NOP();
#define DELAY30NOP() DELAY20NOP();DELAY10NOP();
#define DELAY40NOP() DELAY30NOP();DELAY10NOP();
#define DELAY50NOP() DELAY40NOP();DELAY10NOP();
#define DELAY60NOP() DELAY50NOP();DELAY10NOP();
#define DELAY70NOP() DELAY60NOP();DELAY10NOP();
#define DELAY80NOP() DELAY70NOP();DELAY10NOP();
#define DELAY90NOP() DELAY80NOP();DELAY10NOP();

#define DELAY100NOP() DELAY90NOP();DELAY10NOP(); 

uint8_t     _t16g1_valid;   //用于判定比较器设置时刻

uint8_t     _sys_flag;      //系统时隙指示0非工作时隙1工作时隙
uint16_t    _sys_tick;      //以过零点为基准的系统时钟
uint16_t    _sys_last;      //上一时隙记录
uint16_t    _last_time;     //记录上一次过零时刻
uint16_t    _now_time;      //记录当前过零时刻
uint16_t    _half_time;     //半周期时间更新记录

/**************************************************************************
* 函数名称：init_t16g2
* 功能描述：T16G2定时器工作模式设置为捕获模式
* 输入参数：无
* 返回参数：无
**************************************************************************/
void init_t16g1(void);

/**************************************************************************
* 函数名称：init_t16g1
* 功能描述：T16G1定时器工作模式设置为比较器模式
* 输入参数：type: 0 延时7ms
                  1 延时10ms 
* 返回参数：无
**************************************************************************/
void init_t16g2(uint8_t type);

/**************************************************************************
* 函数名称：delay_ms
* 功能描述：毫秒级延时函数
* 输入参数：ms   延时毫秒数，输入范围0-6553
* 返回参数：无
**************************************************************************/
void delay_ms(uint16_t ms);

/**************************************************************************
* 函数名称：sys_tick_init
* 功能描述：系统时钟初始化
* 输入参数：无
* 返回参数：无
**************************************************************************/
void sys_tick_init(void);

/**************************************************************************
* 函数名称：sys_tick
* 功能描述：系统时间片识别
* 输入参数：无
* 返回参数：无
**************************************************************************/
void sys_tick_run(void);

#endif

