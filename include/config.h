/***** DEFINED **************************/
#ifndef __CONFIG__H_
#define __CONFIG__H_

#define CONFIG_UART 1

#ifdef #define CONFIG_UART 1
#define CONFIG_LINKLAY_UART 0 // 定义哪个uart口用作link层通信用

//#define CONFIG_CONSOLE 1 // 系统console口使能

#ifdef CONFIG_CONSOLE
#define CONFIG_CONSOLE_UART 1 // 系统console print用哪个uart口
#endif
#endif

//#define CONFIG_W2_4G 1
#endif/* __CONFIG__H_ */

