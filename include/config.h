/***** DEFINED **************************/
#ifndef __CONFIG__H_
#define __CONFIG__H_

//#define CONFIG_TYPE_AUTODEVICE
#define CONFIG_TYPE_AUTOGATEWAY
#define CONFIG_MAX_HOP 8

#ifdef CONFIG_TYPE_AUTODEVICE
#define CONFIG_ROUTE_TABLE_SIZE 64
#endif

#ifdef CONFIG_TYPE_AUTOGATEWAY
#define CONFIG_GATEWAY_MNG_DEVICES 512
#define CONFIG_ROUTE_TABLE_SIZE CONFIG_GATEWAY_MNG_DEVICES
#endif

//#define CONFIG_RW_FLASH
#ifdef CONFIG_RW_FLASH
#define CONFIG_MACADDR_FLASHADDR 0x7000
#endif

//#define CONFIG_200BPS_PLC 1
//#define CONFIG_400BPS_PLC 1
#define CONFIG_PLC_ENABLE_IO PC1
#define CONFIG_UART 1

#ifdef CONFIG_UART
//#define CONFIG_LINKLAY_UART 0 // 定义哪个uart口用作link层通信用

//#define CONFIG_CONSOLE 1 // 系统console口使能

#ifdef CONFIG_CONSOLE
#define CONFIG_CONSOLE_UART 1 // 系统console print用哪个uart口
#endif
#endif

#ifdef SPI_GPIO
#define CONFIG_SPI_CSN  PC1
#define CONFIG_SPI_SCK  PC2
#define CONFIG_SPI_MOSI PC3
#define CONFIG_SPI_MISO PC4
#endif

#define CONFIG_RLED_IO PB5

//#define CONFIG_HW2000 1
#endif/* __CONFIG__H_ */


