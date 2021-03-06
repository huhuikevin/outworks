/**************************************************************************
* 版权声明：Copyright@2014 上海海尔集成电路有限公司
* 文件名称：timer.c
* 功能描述：定时器函数		
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
#include <hic.h>
#include "type.h"
#include "system.h"
#include "timer16n.h"

/**************************************************************************
* 函数名称：init_t16g1
* 功能描述：T16G1定时器工作模式设置为捕获模式
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void init_t16g1(void)
{
    T16G1CL=0x21;   //(Fosc/2) 4:1预分频,每周期0.4ms 
    T16G1CH=0x04;   //下降沿捕获
    
    T16G1IF=0;      
    T16G1IE=1;     
    
    _t16g1_valid = 0;
	_half_time = 50000;    //初始化为10ms
}

/**************************************************************************
* 函数名称：init_t16g1
* 功能描述：T16G1定时器工作模式设置为比较器模式
* 输入参数：type: 0 延时7ms
                  1 延时10ms 
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void init_t16g2(uint8_t type)
{
    uint16_t t16g1, delta;
        
    T16G1CL = 0x20;   //关闭timer
    t16g1 = T16G1H*256 | T16G1L;
	t16g1 += 27;
    T16G1H = t16g1 / 256;
    T16G1L = t16g1;

   	T16G1CL = 0x21;   //(Fosc/2) 4:1预分频,每周期0.4us 
    t16g1 -= 27;

    if (t16g1 > _now_time) {        
        delta = t16g1 - _now_time;
        DELAY10NOP();
        DELAY1NOP();    //if/else对齐			
    }
    else
    {
        delta = 0xFFFF - _now_time;
        delta += t16g1 + 1;
    }
    delta = 50000 - 7750 - delta*2 - 168; //过零点前1.55ms产生中断   
    
    T16G2CH=0x0b;   //匹配时复位T16GxH/T16GxL
    T16G2CL=0x10;	  //2:1预分频,每周期0.2ms,关闭定时器
    
    T16G2RH=delta / 256;   //过零后7ms产生中断 
    T16G2RL=delta;
    
    T16G2L=0x00;
    T16G2H=0x00; 
    
    T16G2CL=0x11;	    //使能比较器,(Fosc/2) 2:1预分频,每周期0.2ms 
    
    T16G2IF=0;
    T16G2IE=1;
}

/**************************************************************************
* 函数名称：delay_ms
* 功能描述：毫秒级延时函数
* 输入参数：ms   延时毫秒数，输入范围0-6553
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void delay_ms(uint16_t ms)
{
    uint16_t i;

    T8N = 0x00;
    T8NIF = 0;
    T8NC = 0x99;    //预分频1:4
    
    for (i = 0; i < 10*ms; i++) {
        while (T8NIF == 0);     //0.1024ms delay
        T8NIF = 0;
        watchdog();
    }
}

/**************************************************************************
* 函数名称：sys_tick_init
* 功能描述：系统时钟初始化
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void sys_tick_init(void)
{
    _sys_flag = 0;      
    _sys_tick = 0xABCD;      
    _sys_last = 0xABCE;     
}

/**************************************************************************
* 函数名称：sys_tick_run
* 功能描述：系统时间片识别
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void sys_tick_run(void)
{
    if (_sys_tick == (_sys_last + 1)) {
        _sys_last = _sys_tick;   
        _sys_flag = 1;
    }
    else
    {
        _sys_flag = 0; 
    }
}










