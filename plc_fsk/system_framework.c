//��Ƭ��ϵͳ�ܹ� main���
#include <hic.h>
#include "type.h"
#include "tool.h"


void sys_app()
{
    /* UART ��SPI �շ�����*/
    //serial_process();
    
    /* plc mac���շ����� */
    plcmac_process();
    
    /* 2.4G mac���շ����� */
    //w2_4mac_process();
    /* ��·�㴦��������plc��2.4G��������Э�� */
    linklay_process();
    
    /* �ϲ㴦���;���Ĳ�Ʒ��� */
    app_process();    
}