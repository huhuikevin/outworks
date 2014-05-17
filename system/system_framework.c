//��Ƭ��ϵͳ�ܹ� main���
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "timer16n.h"

#ifdef CONFIG_LINKLAY_UART
void uart_driver_process(void);
#endif
#ifdef CONFIG_W2_4G    
void w2_4mac_driver_process(void);
#endif    
void plc_driver_process(void);
void linklay_process(void);
void app_process(void);


void isr(void) interrupt 
{	
    uint8_t j;
    
	 if (T16G2IF && T16G2IE) {
	     init_t16g1(0);
        _sys_tick++;
        _t16g1_valid = 1;
	     T16G2IF = 0;
     }

    if (T16G1IF && T16G1IE) {       //1   
        T16G1IE = 0; 
	     T16G1IF = 0;        
        T16G2IF = 0;
        
        switch (_plc_state) {        
        case SEND: {    
           DELAY3NOP();  //��REC����
           if (_t16g1_valid) {          
                init_t16g1(1);
                _t16g1_valid = 0;
            }
            plc_send_proc();    
            break;            
            }            
        case RECV: {
            if (_t16g1_valid) {          
                init_t16g1(1);
                _t16g1_valid = 0;
            }
            plc_recv_proc();
            DELAY1NOP();
            break;
            }
        case IDLE: {
            NOP();
            break;
            }
        default:
            NOP();
            break;        
        }
    }

}

void sys_app_process()
{
#ifdef CONFIG_UART    
    /* UART ��SPI �շ�����,������*/
    uart_driver_process();
    watchdog();
    //timer8N();
#endif    
    /* plc mac���շ����������� */
    plc_driver_process();
    watchdog();
    //timer8N();
#ifdef CONFIG_W2_4G    
    /* 2.4G mac���շ����������� */
    w2_4mac_driver_process();
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
