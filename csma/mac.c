/**************************************************************************
* ��Ȩ������Copyright@2014 �Ϻ��������ɵ�·���޹�˾
* �ļ����ƣ�mac.c
* ����������MAC�㴫����Ƴ���		
* �����б�plc_mac_proc
            
            
* �ļ����ߣ�sundy  	
* �ļ��汾��1.0 			
* ������ڣ�2014-05-08			
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
#include <hic.h>
#include "type.h"
#include "mac.h"
#include "plc.h"
#include "io.h"
#include "ram.h"
#include "rand.h"
#include "timer.h"

/**************************************************************************
* �������ƣ�plc_mac_proc
* ����������mac��������,�����ز�������ͻ�ط��ͽ���
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_mac_proc(void)
{
    uint8_t ack[5] = {0x05, 0x40, 0xFF, 0x00, 0x00};
    
    _mac_rx_buf.indication = 0; //ָʾ������1��TICK
    if (_plc_state == RECV) {
        if (_recv_buf.valid == 1) {
			mmemcpy(_mac_rx_buf.mac_frame.data, _recv_buf.data, 
                        _recv_buf.data[0]); 
            if (_mac_rx_buf.mac_frame.dst.laddr == self_mac.laddr ||
				_mac_rx_buf.mac_frame.dst.laddr == BROADCAST_ADDR ||
				_mac_rx_buf.mac_frame.dst.laddr == MULTICAST_ADDR) {
                                     
                _mac_rx_buf.indication = 1;
				_mac_rx_buf.length = _recv_buf.data[0] - (sizeof(mac_frame_t) - MSDU_MAX_LEN);
				_mac_rx_buf.rssiv = _recv_buf.rssiv;
            }
            else {
                // not send to me
                //return;
            }
        }
        else {
            //SLED_OFF();
        }    
    }
     
    if ((_mac_csma.time == _sys_tick) && (_mac_csma.timeout == 1)) {
        _mac_csma.timeout = 0;
        
        _mac_tx_buf.request = 1;
    }
       
    if (_mac_tx_buf.request == 1) { 
        _mac_tx_buf.request = 0;          
        _mac_tx_buf.count++;
        
        if ((int8_t)_listen_buf.rssi < -56) {   //����ֱ�ӷ���    
            _mac_tx_buf.listen = 0;    
            
            RLED_ON();
            
            plc_tx_en();
            plc_data_send(_mac_tx_buf.data, _mac_tx_buf.data[0]);
             
            _mac_tx_buf.timeout = 1; 
            _mac_tx_buf.time = _sys_tick + _mac_tx_buf.data[0] + 4 + 9;
                
            return;            
        }
        else {
            RLED_OFF();
            
            _mac_csma.back++;
            if (_mac_csma.back > 8) {
                _mac_csma.back = 8; 
            }    
            _mac_tx_buf.listen = 1; //��Ҫ����     
        }  
    }
        
    if (_mac_tx_buf.listen == 1) {  //ָ���˱�
        if ((int8_t)_listen_buf.rssi < -56) { 
            _mac_tx_buf.listen = 0;
        
            _mac_csma.time = (rand() / (32768 / (2^_mac_csma.back)));
            _mac_csma.time *= _mac_csma.slot;
            _mac_csma.time += _sys_tick + 1;
            _mac_csma.timeout = 1;
        }
    }
}

/**************************************************************************
* �������ƣ�plc_mac_tx
* ����������mac�㷢�����ݺ���
* ���������dest     Ŀ�ĵ�ַ
            data     ����
            length   ���ݳ���
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_mac_tx(mac_addr *pdst, uint8_t *data, uint8_t length)
{
    _mac_csma.timeout = 0;
      
    _mac_tx_buf.request = 1;
    _mac_tx_buf.confirm = 0x55; //��ʼ��
    _mac_tx_buf.listen = 0;  
    _mac_tx_buf.timeout = 0;
    _mac_tx_buf.count = 0;
    _mac_tx_buf.length = length;
    _mac_tx_buf.mac_frame.len = length + 5;
    _mac_tx_buf.mac_frame.dst.laddr = pdst->laddr;
    
    mmemcpy(&_mac_tx_buf.mac_frame.data[0], data, length);
}

/**************************************************************************
* �������ƣ�plc_mac_init
* ����������mac���ʼ��
* ���������mac     ����MAC��ַ
            net     ����ID
            back    �˱ܳ�ʼֵ
            slot    �˱�ʱ϶
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_mac_init(mac_addr mac, uint8_t back, uint8_t slot)
{
    _mac_csma.timeout = 0;
      
    _mac_tx_buf.request = 0;
    _mac_tx_buf.confirm = 0;    //��ʼ��
    _mac_tx_buf.listen = 0;  
    _mac_tx_buf.timeout = 0;
    _mac_tx_buf.count = 0;
    
    _mac_rx_buf.indication = 0;

    _mac_csma.back = back;
    _mac_csma.slot = slot;
    
    _mac_addr.laddr = mac.laddr;
    // _network_id = net;
    /* ÿ���豸��mac��ַ����һ�������Կ�����mac��ַ��Ϊ�������*/
    srand(_mac_addr.laddr);
}





