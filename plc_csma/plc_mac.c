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
#include "plc_mac.h"
#include "plc.h"
#include "system.h"
//#include "io.h"
//#include "ram.h"
#include "tool.h"
#include "rand.h"
#include "timer16n.h"

#ifdef CONFIG_RLED
#define RLED_ON() CONFIG_RLED=1
#define RLED_OFF() CONFIG_RLED=0
#else
#define RLED_ON() NOP()
#define RLED_OFF() NOP()
#endif
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
    uint8_t plc_sent = 0;
    mac_frame_t *pframe;

    _mac_rx_buf.indication = 0; //ָʾ������1��TICK
    if (_plc_state == RECV) {
        if (_recv_buf.valid == 1) {
	     pframe = (mac_frame_t *)_recv_buf.data;
            if (pframe->dst.laddr == self_mac.laddr ||
				pframe->dst.laddr == BROADCAST_ADDR ||
				pframe->dst.laddr == MULTICAST_ADDR) {
                MMemcpy(&_mac_rx_buf.mac_frame, pframe, 
                        pframe->len); 
                _mac_rx_buf.indication = 1;
				_mac_rx_buf.length = pframe->len - (sizeof(mac_frame_t) - MSDU_MAX_LEN);
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
     
    if ((_mac_tx_buf.stat == tx_csma) && (_mac_csma.timeout == _sys_tick) ) {
        //_mac_csma.timeout = 0;
        _mac_tx_buf.stat = tx_request;
    }
       
    if (_mac_tx_buf.stat == tx_request) { 
        //_mac_tx_buf.request = 0;          
        //_mac_tx_buf.count++;
        
        if ((int8_t)_listen_buf.rssi < -56) {   //����ֱ�ӷ���    
            _mac_tx_buf.listen = 0;    
            
            RLED_ON();
            
            plc_tx_en();
            plc_sent = plc_data_send(&_mac_tx_buf.mac_frame, _mac_tx_buf.mac_frame.len);
            if (!plc_sent)
	         _mac_tx_buf.stat = tx_csma;
	     else
		  _mac_tx_buf.stat = tx_ok;
            
            return;            
        }
        else {
	     _mac_tx_buf.stat = tx_csma;
            RLED_OFF();   
        }  
    }

    if (_mac_tx_buf.stat == tx_csma){
            _mac_csma.back++;
            if (_mac_csma.back > 8) {
                _mac_csma.back = 8; 
            }    
            _mac_tx_buf.listen = 1; //��Ҫ����  
    }
	
    if (_mac_tx_buf.listen == 1) {  //ָ���˱�
        if ((int8_t)_listen_buf.rssi < -56) { 
            _mac_tx_buf.listen = 0;
        
            _mac_csma.timeout = (rand() / (32768 / (2^_mac_csma.back + 1)));
            _mac_csma.timeout *= _mac_csma.slot;
            _mac_csma.timeout += _sys_tick + 1;
            //_mac_csma.timeout = 1;
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
uint8_t plc_mac_tx(mac_addr *pdst, uint8_t *data, uint8_t length)
{
    if (_mac_tx_buf.stat != tx_idle &&  _mac_tx_buf.stat != tx_ok)
		return 0;
    _mac_csma.timeout = 0;
      
    _mac_tx_buf.stat = tx_request;
    _mac_tx_buf.confirm = 0x55; //��ʼ��
    _mac_tx_buf.listen = 0;  
    //    _mac_tx_buf.timeout = 0;
    //    _mac_tx_buf.count = 0;
    _mac_tx_buf.length = length;
    _mac_tx_buf.mac_frame.len = length + sizeof(mac_addr) + 1;
    _mac_tx_buf.mac_frame.dst.laddr = pdst->laddr;
    
    MMemcpy(&_mac_tx_buf.mac_frame.data[0], data, length);

    return length;
}

/**************************************************************************
* �������ƣ�plc_mac_tx_stat
* ������������ѯmac�㷢��״̬
* ���������dest     Ŀ�ĵ�ַ
            data     ����
            length   ���ݳ���
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
uint8_t plc_mac_tx_stat()
{
    return _mac_tx_buf.stat;
}


/**************************************************************************
* �������ƣ�plc_mac_rx
* ����������mac����պ������ϲ����
* ���������pdata   �ϴθ���dst buffer
            plen     :
           
* ���ز�����rssi ��ֵ
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
uint8_t plc_mac_rx_with_rssi(uint8_t *pdata, uint8_t *prssiv)
{
    if ( _mac_rx_buf.indication == 0)
        return 0;

     _mac_rx_buf.indication = 0;
	 
    MMemcmp(pdata, _mac_rx_buf.mac_frame.data, _mac_rx_buf.length);

    if (prssiv)
        *prssiv = _mac_rx_buf.rssiv;

    return _mac_rx_buf.length;
}

uint8_t plc_mac_rx(uint8_t *pdata)
{
    return plc_mac_rx_with_rssi(pdata, NULL);
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
void plc_mac_init(uint8_t back, uint8_t slot)
{
	init_t16g1();
    plc_init();

    _mac_csma.timeout = 0;
      
    _mac_tx_buf.stat = tx_idle;
    _mac_tx_buf.confirm = 0;    //��ʼ��
    _mac_tx_buf.listen = 0;  
    //    _mac_tx_buf.timeout = 0;
    //   _mac_tx_buf.count = 0;
    
    _mac_rx_buf.indication = 0;

    _mac_csma.back = back;
    _mac_csma.slot = slot;

    // _network_id = net;
    /* ÿ���豸��mac��ַ����һ�������Կ�����mac��ַ��Ϊ�������*/
    srand(self_mac.laddr);
}





