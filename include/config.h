/***** DEFINED **************************/
#ifndef __CONFIG__H_
#define __CONFIG__H_

#define CONFIG_UART 1

#ifdef #define CONFIG_UART 1
#define CONFIG_LINKLAY_UART 0 // �����ĸ�uart������link��ͨ����

//#define CONFIG_CONSOLE 1 // ϵͳconsole��ʹ��

#ifdef CONFIG_CONSOLE
#define CONFIG_CONSOLE_UART 1 // ϵͳconsole print���ĸ�uart��
#endif
#endif

//#define CONFIG_W2_4G 1
#endif/* __CONFIG__H_ */

