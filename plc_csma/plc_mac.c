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
    uint8_t plc_sent = 0;
    mac_frame_t *pframe;

    _mac_rx_buf.indication = 0; //指示仅持续1个TICK
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
        
        if ((int8_t)_listen_buf.rssi < -56) {   //空闲直接发射    
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
            _mac_tx_buf.listen = 1; //需要侦听  
    }
	
    if (_mac_tx_buf.listen == 1) {  //指数退避
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
uint8_t plc_mac_tx(mac_addr *pdst, uint8_t *data, uint8_t length)
{
    if (_mac_tx_buf.stat != tx_idle &&  _mac_tx_buf.stat != tx_ok)
		return 0;
    _mac_csma.timeout = 0;
      
    _mac_tx_buf.stat = tx_request;
    _mac_tx_buf.confirm = 0x55; //初始化
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
* 函数名称：plc_mac_tx_stat
* 功能描述：查询mac层发送状态
* 输入参数：dest     目的地址
            data     数据
            length   数据长度
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
uint8_t plc_mac_tx_stat()
{
    return _mac_tx_buf.stat;
}


/**************************************************************************
* 函数名称：plc_mac_rx
* 功能描述：mac层接收函数，上层调用
* 输入参数：pdata   上次给出dst buffer
            plen     :
           
* 返回参数：rssi 的值
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
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
void plc_mac_init(uint8_t back, uint8_t slot)
{
	init_t16g1();
    plc_init();

    _mac_csma.timeout = 0;
      
    _mac_tx_buf.stat = tx_idle;
    _mac_tx_buf.confirm = 0;    //初始化
    _mac_tx_buf.listen = 0;  
    //    _mac_tx_buf.timeout = 0;
    //   _mac_tx_buf.count = 0;
    
    _mac_rx_buf.indication = 0;

    _mac_csma.back = back;
    _mac_csma.slot = slot;

    // _network_id = net;
    /* 每个设备的mac地址都不一样，所以可以用mac地址作为随机种子*/
    srand(self_mac.laddr);
}





