#include "config.h"
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "linklay.h"

int16u active_tick; 
#define DEVICE_TIMEOUT_R MS_TO_TICK(5*1000)
/*
dst  passaddr     src    final dst

*/
section32 route_t rt_table[CONFIG_ROUTE_TABLE_SIZE];
mac_addr gateway_addr;


// process when recved the route data package
void device_route_process()
{
	route_frame_t rt_frame;

	if (linklay_recv_data(&rt_frame, PROTOCOL_ROUTER) == 0)
		return;
	if (rt_frame.route_type == ROUTETYPE_MCAST_DFP || rt_frame.route_type == ROUTETYPE_BCAST_GWADDR)
	{
		gateway_addr.laddr = rt_frame.src_addr.laddr;
	}
	if (rt_frame.route_type == ROUTETYPE_MCAST_DFP){
		if (!_addr_is_nexthop_to_gateway(&rt_frame.pass_addr)) {//gateway --> device is down path, device ---> gateway is up path
			if (rt_frame.pass_addr.laddr == rt_frame.src_addr.laddr)//如果和网关直连，加入到gateway的route
				route_update(&rt_frame);

			if (self_mac.laddr == rt_frame.dst_addr.laddr){
				device_ack_gateway(&rt_frame);
			}			
			return;
		}
	}
	route_update(&rt_frame);

	// if me ,need to ack the gateway , so the gateway can know it can reached gateway
	if (self_mac.laddr == rt_frame.dst_addr.laddr){
		device_ack_gateway(&rt_frame);
	}else {// if not me and DFP pkg ,forward it if hop-- != 0
		device_sendto_next_hop(&rt_frame);// if hop==0, drop it
	}
	if (gateway_addr.laddr == 0){
		if (IsTimeOut(active_tick+DEVICE_TIMEOUT_R)){
			device_broadcast_selfaddr();
		}
	}

	route_process_timeout();
}

uchar _addr_is_nexthop_to_gateway(mac_addr *paddr)
{
	route_t *proute;
	if (!gateway_addr.laddr)
		return 0;
	proute = route_found_by_dst(&gateway_addr);
	if (proute == NULL)
		return 0;
	if (proute->next.laddr == paddr.laddr)
		return 1;
	return 0;
}


void device_ack_gateway(route_frame_t *prt)
{
	route_t *proute;
	if (prt->route_type != ROUTETYPE_DFAP) {
		if (prt->route_type == ROUTETYPE_MCAST_DFP || 
			prt->route_type == ROUTETYPE_BCAST_GWADDR ||
			prt->route_type == ROUTETYPE_GW_INQ_DEV)
			device_send_dfap();
	}
}

void device_send_dfap()
{
	route_frame_t rt_frame;
	route_t *proute;
	rt_frame.route_type = ROUTETYPE_DFAP;
	rt_frame.mac_type = MacPlc;
	rt_frame.src_addr.laddr = self_mac.laddr;
	rt_frame.dst_addr.laddr = gateway_addr.laddr;
	rt_frame.hop = 0;
	rt_frame.pass_addr.laddr = self_mac.laddr;
	proute = route_found_by_dst(&gateway_addr);
	if (proute == NULL){
		return;//never reached here
	}
	linklay_send_data(&proute->next_addr, &rt_frame, sizeof(route_frame_t));
}

void device_sendto_next_hop(route_frame_t *prt)
{
	route_t *proute;
	mac_addr dst;
	uchar i,len;

	prt->hop++;
	prt->pass_addr.laddr = self_mac.laddr;
	prt->mac_type = MacPlc;

	if (prt->route_type == ROUTETYPE_BCAST_DEVADDR)//to gateway
	{
		if (gateway_addr.laddr){
			proute = route_found_by_dst(&gateway_addr);
			if (proute){//如果有到网关的路由直接发给next
				dst.laddr = proute->next.laddr;
				prt->dst_addr.laddr = gateway_addr.laddr;
				linklay_send_data(&dst, prt, sizeof(route_frame_t));
			}
		}
		return;
	}

	proute = route_found_by_dst(&prt->dst_addr);
	if (proute){//如果有路由直接发给next
		dst.laddr = proute->next.laddr;
		linklay_send_data(&dst, prt, sizeof(route_frame_t));
		return;
	}else if (prt->route_type == ROUTETYPE_MCAST_DFP){
		if (route_have_routes_to_device()){// 有到device的路由
			dst.laddr = 0x00000000;// mcast the DFP
			linklay_send_data(&dst, prt, sizeof(route_frame_t));
		}else {//没有路由表直接发送给dst
			dst.laddr = prt->dst_addr.laddr;
			linklay_send_data(&dst, prt, sizeof(route_frame_t));
		}		
	}		
	
}


void device_broadcast_selfaddr()
{
	route_frame_t rt_frame;
	mac_addr dst_addr;

	rt_frame.dst_addr.laddr = 0xffffffff;
	rt_frame.mac_type = MacPlc;
	rt_frame.pass_addr.laddr = self_mac.laddr;
	rt_frame.src_addr.laddr = self_mac.laddr;
	rt_frame.route_type = ROUTETYPE_BCAST_DEVADDR;
	
	rt_frame.hop = 0;
	dst_addr.laddr = 0xffffffff;
	linklay_send_data(&dst_addr, &rt_frame, sizeof(rt_frame));
}

void device_route_init()
{
	active_tick = Timetick();
	route_init();
	device_broadcast_selfaddr();
}


