//��Ƭ��ϵͳ�ܹ� main���
#include <hic.h>
#include "type.h"
#include "tool.h"


void sys_app()
{
    /* UART ��SPI �շ�����,������*/
    serial_txrx();
    
    /* plc mac���շ����������� */
    plcmac_txrx();
    
    /* 2.4G mac���շ����������� */
    w2_4mac_txrx();
    
    /* ��·�㴦��������plc��2.4G��������Э�� */
    linklay_process();
    
    
    /* UART,SPI ���ݰ�����������·ͷ����Ϣ */
    serial_process();
    
    /* �ϲ㴦���;���Ĳ�Ʒ��� */
    app_process();    
}