/***** DEFINED **************************/
#ifndef __CONFIG__H_
#define __CONFIG__H_

#define CONFIG_NO_ROUTE 
#define CONFIG_DEFAULT_MAC 0

#define CONFIG_TYPE_AUTODEVICE
//#define CONFIG_TYPE_AUTOGATEWAY
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
#define CONFIG_IO_PA_ENABLE PC1
#define CONFIG_UART 1

#ifdef CONFIG_UART
//#define CONFIG_LINKLAY_UART 2 // �����ĸ�uart������link��ͨ����

#define CONFIG_CONSOLE 1 // ϵͳconsole��ʹ��

#ifdef CONFIG_CONSOLE
#define CONFIG_CONSOLE_UART 1 // ϵͳconsole print���ĸ�uart��
#endif
#endif //CONFIG_UART

#ifdef SPI_GPIO
#define CONFIG_SPI_CE   PA2
#define CONFIG_SPI_CSN  PA3
#define CONFIG_SPI_SCK  PA4
#define CONFIG_SPI_MOSI PA5
#define CONFIG_SPI_MISO PA6
#endif
#define CONFIG_SHUTDOWN12V_IO PA7

#define CONFIG_IO_RLED PB5

#define CONFIG_MANAL_SWITCH PB4
#define CONFIG_SWITCH_CTRL  PB5

#define CONFIG_LED1 PC2
#define CONFIG_LED3 PC3

#define CONFIG_HG7220_MODE 1 //0 SPI ,1 UART 
#define CONFIG_HG72xx_UART 0

//#define CONFIG_HW2000 1
#endif/* __CONFIG__H_ */


