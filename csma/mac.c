/**************************************************************************
* 版权声明：Copyright@2014 上海海尔集成电路有限公司
* 文件名称：mac.c
* 功能描述：MAC层传输控制程序		
* 函数列表：plc_mac_proc
            
            
* 文件作者：sundy  	
* 文件版本：1.0 			
* 完成日期：2014-05-08			
* 修订历史：
* 修订日期：
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
* 函数名称：plc_mac_proc
* 功能描述：mac处理流程,处理载波侦听冲突重发和接收
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_mac_proc(void)
{
    uint8_t ack[5] = {0x05, 0x40, 0xFF, 0x00, 0x00};
    
    _mac_rx_buf.indication = 0; //指示仅持续1个TICK
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
        
        if ((int8_t)_listen_buf.rssi < -56) {   //空闲直接发射    
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
            _mac_tx_buf.listen = 1; //需要侦听     
        }  
    }
        
    if (_mac_tx_buf.listen == 1) {  //指数退避
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
* 函数名称：plc_mac_tx
* 功能描述：mac层发射数据函数
* 输入参数：dest     目的地址
            data     数据
            length   数据长度
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_mac_tx(mac_addr *pdst, uint8_t *data, uint8_t length)
{
    _mac_csma.timeout = 0;
      
    _mac_tx_buf.request = 1;
    _mac_tx_buf.confirm = 0x55; //初始化
    _mac_tx_buf.listen = 0;  
    _mac_tx_buf.timeout = 0;
    _mac_tx_buf.count = 0;
    _mac_tx_buf.length = length;
    _mac_tx_buf.mac_frame.len = length + 5;
    _mac_tx_buf.mac_frame.dst.laddr = pdst->laddr;
    
    mmemcpy(&_mac_tx_buf.mac_frame.data[0], data, length);
}

/**************************************************************************
* 函数名称：plc_mac_init
* 功能描述：mac层初始化
* 输入参数：mac     本机MAC地址
            net     网络ID
            back    退避初始值
            slot    退避时隙
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_mac_init(mac_addr mac, uint8_t back, uint8_t slot)
{
    _mac_csma.timeout = 0;
      
    _mac_tx_buf.request = 0;
    _mac_tx_buf.confirm = 0;    //初始化
    _mac_tx_buf.listen = 0;  
    _mac_tx_buf.timeout = 0;
    _mac_tx_buf.count = 0;
    
    _mac_rx_buf.indication = 0;

    _mac_csma.back = back;
    _mac_csma.slot = slot;
    
    _mac_addr.laddr = mac.laddr;
    // _network_id = net;
    /* 每个设备的mac地址都不一样，所以可以用mac地址作为随机种子*/
    srand(_mac_addr.laddr);
}





