//单片机系统架构 main入口
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "timer8n.h"

#ifdef SERIAL_INTF
void serial_driver_txrx(void);
void serial_process(void);
#endif
#ifdef W24G    
void w2_4mac_driver_txrx(void);
#endif    
void plc_driver_txrx(void);
void linklay_process(void);
void app_process(void);

void sys_app_process()
{
#ifdef SERIAL_INTF    
    /* UART 、SPI 收发处理,驱动层*/
    serial_driver_txrx();
    watchdog();
    //timer8N();
#endif    
    /* plc mac层收发处理，驱动层 */
    plc_driver_txrx();
    watchdog();
    //timer8N();
#ifdef W24G    
    /* 2.4G mac层收发处理，驱动层 */
    w2_4mac_driver_txrx();
    watchdog();
#endif    
    /* 链路层处理，包括了plc和2.4G或其他的协议 */
    linklay_process();
    watchdog();
    //timer8N();
#ifdef SERIAL_INTF    
    /* UART,SPI 数据包处理，处理链路头等信息 */
    serial_process();
    watchdog();
    //timer8N();
#endif    
    /* 上层处理，和具体的产品相关 */
    app_process();
    watchdog(); 
    //timer8N();
}