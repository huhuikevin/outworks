//单片机系统架构 main入口
#include <hic.h>
#include "type.h"
#include "tool.h"


void sys_app()
{
    /* UART 、SPI 收发处理*/
    //serial_process();
    
    /* plc mac层收发处理 */
    plcmac_process();
    
    /* 2.4G mac层收发处理 */
    //w2_4mac_process();
    /* 链路层处理，包括了plc和2.4G或其他的协议 */
    linklay_process();
    
    /* 上层处理，和具体的产品相关 */
    app_process();    
}