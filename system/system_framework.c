//��Ƭ��ϵͳ�ܹ� main���
#include <hic.h>
#include "config.h"
#include "type.h"
#include "system.h"
#include "tool.h"
#include "timer16n.h"
#include "plc_mac.h"
#ifdef CONFIG_LINKLAY_UART
void uart_driver_process(void);
#endif
#ifdef CONFIG_HW2000    
void hw2000_driver_process(void);
#endif
#ifdef CONFIG_UART
void uart_driver_process(void);
#endif

void plc_driver_process(void);
void linklay_process(void);
void app_process(void);

void sys_app_process()
{
#ifdef CONFIG_UART    
    /* UART ��SPI �շ�����,������*/
    uart_driver_process();
    watchdog();
    //timer8N();
#endif    
    /* plc mac���շ����������� */
    plc_mac_proc();
    watchdog();
    //timer8N();
#ifdef CONFIG_HW2000    
    /* 2.4G mac���շ����������� */
    hw2000_driver_process();
    watchdog();
#endif  
    /* ��·�㴦��������plc��2.4G��������Э�� */
    linklay_process();
    watchdog();
    //timer8N();
    /* �ϲ㴦���;���Ĳ�Ʒ��� */
    app_process();
    watchdog(); 
    //timer8N();
}
