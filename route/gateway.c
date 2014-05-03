#include "config.h"
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "plc.h"

// process when recved the route data package
void route_gateway_process(route_frame_t *prt)
{
	if (prt->route_type == ROUTETYPE_DFAP) {
		route_update(prt);
	}
}

/*
手机等智能设备扫描插座的条形码，得到插座的mac地址，
通过网络发送给智能网关,网关接收到后，调用本函数
*/
void gateway_add_device(mac_addr *paddr)
{
	uchar i;

	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (!rt_table[i].dst_addr.laddr){
			rt_table[i].dst_addr.laddr = paddr->laddr;
			rt_table[i].rt_stat = rt_has_device;
			break;
		}
	}	
}


void gateway_found_device()
{
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (!rt_table[i].valide&& rt_table[i].dst_addr.laddr){
			if (rt_table[i].rt_stat == rt_has_device) {
				gateway_send_fdp(&rt_table[i].dst_addr);
				rt_table[i].rt_stat = rt_waiting_fdap;
				break;
			}
		}
	}
}


