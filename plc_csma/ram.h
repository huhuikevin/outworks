/**************************************************************************
* 版权声明：Copyright@2014 上海海尔集成电路有限公司
* 文件名称：ram.h
* 功能描述：内存操作函数声明		
* 函数列表：ram_clr
            mmemset
            mmemcpy
* 文件作者：sundy  	
* 文件版本：1.0 			
* 完成日期：2014-04-24			
* 修订历史：
* 修订日期：
**************************************************************************/
#ifndef _RAM_H_
#define _RAM_H_

/**************************************************************************
* 函数名称：ram_clr
* 功能描述：清除全部ram数据
* 输入参数：无 
* 返回参数：无
**************************************************************************/
void ram_clr(void);

/**************************************************************************
* 函数名称：mmemset
* 功能描述：内存设置
* 输入参数：dest  目的内存地址
            value 设置值
            count 设置内存长度
* 返回参数：无
**************************************************************************/
void mmemset(void *dest, uint8_t value, uint8_t count);

/**************************************************************************
* 函数名称：mmemcpy
* 功能描述：内存拷贝
* 输入参数：dest  目的地址
            src   源地址
            count 数据长度
* 返回参数：无
**************************************************************************/
void mmemcpy(void *dest, void *src, uint8_t count);

#endif


