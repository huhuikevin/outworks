//单片机系统架构 main入口
#include <hic.h>
#include "type.h"
#include "tool.h"


void sys_app()
{
    /* UART 、SPI 收发处理*/
    serial_txrx();
    
    /* plc mac层收发处理 */
    plcmac_txrx();
    
    /* 2.4G mac层收发处理 */
    w2_4mac_txrx();
    
    /* 链路层处理，包括了plc和2.4G或其他的协议 */
    linklay_process();
    
    
    /*UART,SPI 数据包处理*/
    serial_process();
    
    /* 上层处理，和具体的产品相关 */
    app_process();    
}