/**************************************************************************
* ��Ȩ������Copyright@2014 �Ϻ��������ɵ�·���޹�˾
* �ļ����ƣ�timer.h
* ������������ʱ����������		
* �����б�init_t16g2
            init_t16g1
            delay_ms
            sys_tick_init
            sys_tick_run
* �ļ����ߣ�sundy  	
* �ļ��汾��1.0 			
* ������ڣ�2014-04-24			
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
#include "type.h"

#ifndef _TIMER_H_
#define _TIMER_H_
#define DELAY1NOP() NOP();
#define DELAY2NOP() DELAY1NOP();NOP();
#define DELAY3NOP() DELAY2NOP();NOP();
#define DELAY4NOP() DELAY3NOP();NOP();
#define DELAY5NOP() DELAY4NOP();NOP();
#define DELAY6NOP() DELAY5NOP();NOP();
#define DELAY7NOP() DELAY6NOP();NOP();
#define DELAY8NOP() DELAY7NOP();NOP();
#define DELAY9NOP() DELAY8NOP();NOP();

#define DELAY10NOP() DELAY9NOP();NOP();   
#define DELAY20NOP() DELAY10NOP();DELAY10NOP();
#define DELAY30NOP() DELAY20NOP();DELAY10NOP();
#define DELAY40NOP() DELAY30NOP();DELAY10NOP();
#define DELAY50NOP() DELAY40NOP();DELAY10NOP();
#define DELAY60NOP() DELAY50NOP();DELAY10NOP();
#define DELAY70NOP() DELAY60NOP();DELAY10NOP();
#define DELAY80NOP() DELAY70NOP();DELAY10NOP();
#define DELAY90NOP() DELAY80NOP();DELAY10NOP();

#define DELAY100NOP() DELAY90NOP();DELAY10NOP(); 

uint8_t     _t16g1_valid;   //�����ж��Ƚ�������ʱ��

uint8_t     _sys_flag;      //ϵͳʱ϶ָʾ0�ǹ���ʱ϶1����ʱ϶
uint16_t    _sys_tick;      //�Թ����Ϊ��׼��ϵͳʱ��
uint16_t    _sys_last;      //��һʱ϶��¼
uint16_t    _last_time;     //��¼��һ�ι���ʱ��
uint16_t    _now_time;      //��¼��ǰ����ʱ��
uint16_t    _half_time;     //������ʱ����¼�¼

/**************************************************************************
* �������ƣ�init_t16g2
* ����������T16G2��ʱ������ģʽ����Ϊ����ģʽ
* �����������
* ���ز�������
**************************************************************************/
void init_t16g1(void);

/**************************************************************************
* �������ƣ�init_t16g1
* ����������T16G1��ʱ������ģʽ����Ϊ�Ƚ���ģʽ
* ���������type: 0 ��ʱ7ms
                  1 ��ʱ10ms 
* ���ز�������
**************************************************************************/
void init_t16g2(uint8_t type);

/**************************************************************************
* �������ƣ�delay_ms
* �������������뼶��ʱ����
* ���������ms   ��ʱ�����������뷶Χ0-6553
* ���ز�������
**************************************************************************/
void delay_ms(uint16_t ms);

/**************************************************************************
* �������ƣ�sys_tick_init
* ����������ϵͳʱ�ӳ�ʼ��
* �����������
* ���ز�������
**************************************************************************/
void sys_tick_init(void);

/**************************************************************************
* �������ƣ�sys_tick
* ����������ϵͳʱ��Ƭʶ��
* �����������
* ���ز�������
**************************************************************************/
void sys_tick_run(void);

#endif

