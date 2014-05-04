#include "config.h"
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "plc.h"

typedef struct {
	mac_addr addr;
	uchar stat:2;
	uchar hop:4;
}device_t;

device_t device_tbl[CONFIG_GATEWAY_MNG_DEVICES];


// process when recved the route data package
void gateway_route_process(route_frame_t *prt)
{
	if (prt->route_type == ROUTETYPE_DFAP) {
		route_update(prt);
		gateway_set_device_active(&prt->src_addr);
	}
}
void gateway_set_device_active(mac_addr *paddr)
{
	uchar i;

	for (i = 0; i < CONFIG_GATEWAY_MNG_DEVICES; i++ ){
		if (device_tbl[i].addr.laddr && device_tbl[i].stat != rt_active){
			if (device_tbl[i].addr.laddr == paddr->laddr) {
				device_tbl[i].stat = rt_active;
				break;
			}
		}
	}	
}
/*
手机等智能设备扫描插座的条形码，得到插座的mac地址，
通过网络发送给智能网关,网关接收到后，调用本函数
*/
void gateway_add_device(mac_addr *paddr)
{
	uchar i;

	for (i = 0; i < CONFIG_GATEWAY_MNG_DEVICES; i++ ){
		if (!device_tbl[i].addr.laddr){
			device_tbl[i].addr.laddr = paddr->laddr;
			device_tbl[i].stat = rt_has_device;
			break;
		}
	}	
}


void gateway_broadcast_selfaddr()
{
	route_frame_t rt_frame;
	mac_addr dst_addr;

	rt_frame.dst_addr.laddr = 0x0;
	rt_frame.mac_type = MacPlc;
	rt_frame.pass_addr.laddr = self_mac.laddr;
	rt_frame.src_addr.laddr = self_mac.laddr;
	rt_frame.route_type = ROUTETYPE_BCAST_GW_ADDR;
	
	rt_frame.hop = 1;
	dst_addr.laddr = 0xffffffff;
	linklay_send_data(&dst_addr, &rt_frame, sizeof(rt_frame));
}

void gateway_found_device()
{
	uchar i;
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (!rt_table[i].valide&& rt_table[i].dst_addr.laddr){
			if (rt_table[i].rt_stat == rt_has_device) {
				gateway_send_fdp(&rt_table[i]);
				rt_table[i].rt_stat = rt_waiting_fdap;
				break;
			}
		}
	}
}

void gateway_send_fdp(route_t *proute)
{
	route_frame_t rt_frame;
	mac_addr dst_addr;

	rt_frame.dst_addr.laddr = proute->dst_addr.laddr;
	rt_frame.mac_type = MacPlc;
	rt_frame.pass_addr.laddr = self_mac.laddr;
	rt_frame.src_addr.laddr = self_mac.laddr;
	rt_frame.route_type = ROUTETYPE_DFP;
	
	if (!proute->valide){
		if (proute->hop == 0) {
			rt_frame.hop = 1;
			proute->hop = 1;
			dst_addr.laddr = proute->dst_addr.laddr;
		}else{

		}
	}
	proute->rt_ticks = Timetick();
	linklay_send_data(&dst_addr, &rt_frame, sizeof(rt_frame));
}

