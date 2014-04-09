//��Ƭ��ϵͳ�ܹ� main���
#include <hic.h>
#include "type.h"
#include "tool.h"

#ifdef SERIAL_INTF
void serial_driver_txrx(void);
void serial_process(void);
#endif
#ifdef W24G    
void w2_4mac_driver_txrx(void);
#endif    
void plc_driver_txrx(void);
void linklay_process(void);

void sys_app_process()
{
#ifdef SERIAL_INTF    
    /* UART ��SPI �շ�����,������*/
    serial_driver_txrx();
    watchdog();
#endif    
    /* plc mac���շ����������� */
    plc_driver_txrx();
    watchdog();
#ifdef W24G    
    /* 2.4G mac���շ����������� */
    w2_4mac_driver_txrx();
    watchdog();
#endif    
    /* ��·�㴦��������plc��2.4G��������Э�� */
    linklay_process();
    watchdog();

#ifdef SERIAL_INTF    
    /* UART,SPI ���ݰ�����������·ͷ����Ϣ */
    serial_process();
    watchdog();
#endif    
    /* �ϲ㴦���;���Ĳ�Ʒ��� */
    app_process();
    watchdog();  
}