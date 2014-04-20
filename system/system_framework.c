//单片机系统架构 main入口
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "timer8n.h"

#ifdef CONFIG_LINKLAY_UART
void uart_driver_process(void);
#endif
#ifdef CONFIG_W2_4G    
void w2_4mac_driver_process(void);
#endif    
void plc_driver_process(void);
void linklay_process(void);
void app_process(void);

void sys_app_process()
{
#ifdef CONFIG_UART    
    /* UART 、SPI 收发处理,驱动层*/
    uart_driver_process();
    watchdog();
    //timer8N();
#endif    
    /* plc mac层收发处理，驱动层 */
    plc_driver_process();
    watchdog();
    //timer8N();
#ifdef CONFIG_W2_4G    
    /* 2.4G mac层收发处理，驱动层 */
    w2_4mac_driver_process();
    watchdog();
#endif    
    /* 链路层处理，包括了plc和2.4G或其他的协议 */
    linklay_process();
    watchdog();
    //timer8N();
    /* 上层处理，和具体的产品相关 */
    app_process();
    watchdog(); 
    //timer8N();
}