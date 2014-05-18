#include "config.h"
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "plc.h"

typedef struct {
	mac_addr addr;
	uchar stat;
	int16u ticks;
}device_t;

#define WAITING_FDAP_TIMEOUT MS_TO_TICK(2*1000)

device_t device_tbl[CONFIG_GATEWAY_MNG_DEVICES];
int16u find_device_tick;
#define FIND_DEVICE_INTVAL MS_TO_TICK(500)

#define INQ_DEVICE_INTVAL MS_TO_TICK(500)

void gateway_route_process()
{
	route_frame_t rt_frame;
	uchar len;
	uint8_t rssiv;

	len = linklay_recv_data_with_rssi(&rt_frame, MacPlc, &rssiv);
	if (len)
		gateway_process_device(&rt_frame);

	gateway_found_device();

	gateway_route_timeout();

	gateway_inq_devices();
}

void gateway_inq_devices()
{
	uchar i;
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){//if is waiting_fdap timeout(in the gaven time, don't recv fdap pkg)
		if (device_tbl[i].addr.laddr) {
			if (device_tbl[i].stat == rt_active) {
				if (IsTimeOut(device_tbl[i].ticks+INQ_DEVICE_INTVAL)) {
					gateway_inq_addr(&device_tbl[i].addr);
					device_tbl[i].ticks = Timetick();
					device_tbl[i].stat = rt_inq;
				}
				//device_tbl[i].stat = rt_waiting_fdap;
				return;
			}
		}
	}	
}

void gateway_inq_addr(mac_addr *paddr)
{
	route_frame_t rt_frame;
	route_t *proute;

	proute = route_found_by_dst(paddr);

	if (!proute) return;
	
	rt_frame.dst_addr.laddr = paddr->laddr;
	rt_frame.mac_type = MacPlc;
	rt_frame.pass_addr.laddr = self_mac.laddr;
	rt_frame.src_addr.laddr = self_mac.laddr;
	rt_frame.route_type = ROUTETYPE_GW_INQ_DEV;
	
	rt_frame.hop = 0;
	
	linklay_send_route_data(&proute->dst, &rt_frame, sizeof(rt_frame), MacPlc);	
}
void gateway_route_timeout()
{
	route_t *proute = route_process_timeout();
	if (proute){
		gateway_deactive_device(&proute->dst);
	}
}

void gateway_deactive_device(mac_addr *paddr)
{
	uchar i;

	for (i = 0; i < CONFIG_GATEWAY_MNG_DEVICES; i++ ){
		if (device_tbl[i].addr.laddr && device_tbl[i].stat == rt_active){
			if (device_tbl[i].addr.laddr == paddr->laddr) {
				device_tbl[i].stat = rt_idle;
				break;
			}
		}
	}
}
void gateway_active_device(mac_addr *paddr)
{
	uchar i;

	for (i = 0; i < CONFIG_GATEWAY_MNG_DEVICES; i++ ){
		if (device_tbl[i].addr.laddr && device_tbl[i].stat != rt_active){
			if (device_tbl[i].addr.laddr == paddr->laddr) {
				device_tbl[i].stat = rt_active;
				device_tbl[i].ticks = Timetick();
				break;
			}
		}
	}	
}


// process when recved the route data package
void gateway_process_device(route_frame_t *prt)
{
	if (prt->route_type == ROUTETYPE_BCAST_DEVADDR){
		if (prt->hop == 0)//can direct connect
			gateway_broadcast_selfaddr();
		else{
			gateway_mcast_fdp( &prt->src_addr);
		}
	}
	if (prt->route_type == ROUTETYPE_DFAP) {
		route_update(prt);
		gateway_active_device(&prt->src_addr);
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
		if (device_tbl[i].stat == rt_idle){
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
	rt_frame.route_type = ROUTETYPE_BCAST_GWADDR;
	
	rt_frame.hop = 0;
	dst_addr.laddr = 0xffffffff;
	linklay_send_route_data(&dst_addr, &rt_frame, sizeof(rt_frame), MacPlc);
}

uchar _gateway_found_device1()
{
	uchar i;
	uchar has_device = 0;
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (device_tbl[i].addr.laddr) {
			if (device_tbl[i].stat == rt_has_device) {
				has_device = 1;
				if (IsTimeOut(FIND_DEVICE_INTVAL+find_device_tick)) {
					gateway_mcast_fdp(&rt_table[i]);
					device_tbl[i].stat = rt_waiting_fdap;
					device_tbl[i].ticks = Timetick();
				}
				break;
			}
		}
	}
	return has_device;
}

void _gateway_found_device2()
{
	uchar i;
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){//if is waiting_fdap timeout(in the gaven time, don't recv fdap pkg)
		if (device_tbl[i].addr.laddr) {
			if (device_tbl[i].stat == rt_waiting_fdap) {
				if (IsTimeOut(device_tbl[i].ticks+WAITING_FDAP_TIMEOUT)) {
					gateway_mcast_fdp(&rt_table[i]);
					device_tbl[i].ticks = Timetick();
				}
				//device_tbl[i].stat = rt_waiting_fdap;
				return;
			}
		}
	}

}
/*
先对每一个没有发送过FDP的mac addr发送fdp，如果都发送过了，经过2s后，
还有没有收到fdap的mac addr，继续发送fdp
*/
void gateway_found_device()
{
	if (_gateway_found_device1() == 0)
		_gateway_found_device2();
	
}

void gateway_send_fdp(mac_addr *ppassaddr, mac_addr *paddr)
{
	route_frame_t rt_frame;
	uchar i,len;

	rt_frame.dst_addr.laddr = paddr->laddr;
	rt_frame.mac_type = MacPlc;
	rt_frame.pass_addr.laddr = self_mac.laddr;
	rt_frame.src_addr.laddr = self_mac.laddr;
	rt_frame.route_type = ROUTETYPE_MCAST_DFP;
	rt_frame.hop = 0;

	linklay_send_route_data(ppassaddr, &rt_frame, sizeof(rt_frame), MacPlc);
}

void gateway_mcast_fdp(mac_addr *paddr)
{
	mac_addr baddr;
	baddr.laddr = 0x00000000;

	gateway_send_fdp(&baddr, paddr);
}

void gateway_route_init()
{
	route_init();
	MMemSet(&device_tbl[0],0,sizeof(device_t)*CONFIG_GATEWAY_MNG_DEVICES)
	find_device_tick=Timetick();
	gateway_broadcast_selfaddr();
}
