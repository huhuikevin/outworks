#ifndef __ROUTE__H_
#define __ROUTE__H_

#include "config.h"

#define ROUTETYPE_MCAST_DFP 0
#define ROUTETYPE_DFAP 1
#define ROUTETYPE_BCAST_DEVADDR   2
#define ROUTETYPE_BCAST_GWADDR 3
#define ROUTETYPE_GW_INQ_DEV 4
typedef struct {
	uint8_t mac_type:4;// 0: plc, 1:2.4G
	uint8_t route_type:4; // 0: gateway send:device found package, 1: device ack the found  package, 2:the beatheart between the next hop and the device
		                          // 3: broadcast the MY addr
	uint8_t hop;//GFP:once through a device or gateway , hop++, and the linklayhead.Src = the deivce or gateway macaddr
			   //GFAP:once through a device ,hop--, if hop==0 and not arrived at the dst, drop it
	uint8_t rssiv;// 在link层被更新

	mac_addr pass_addr;// route data 经过的设备的地址

	mac_addr src_addr;// route data最初的发出地址
	mac_addr dst_addr;// route data 最后最终要接受的地址
	
}route_frame_t;

#define ROUTE_MAX_LIFTCYCLE 2000//20 second

typedef struct {
	mac_addr dst;
	mac_addr next;
	uint8_t rssiv;
	uint8_t hop:5;      //how many devices passed if we reached the dst
	uint8_t phy_type:2; //0: plc can reached to the dst, 1: 2.4G can reached the dst
	uint8_t seq:1;
	uint16_t ticks;  //ticks, 
	uint8_t valide:1;// used or not
	uint8_t dir:1;//0:means gateway to device, 1: means device to gateway
	uint8_t route_type:1;// 0:means not the direct connect to gateway, 1:means direct connect to the gateway 
}route_t;

#define ROUTE_DIR_TODEVICE 0
#define ROUTE_DIR_TOGATEWAY 1

#define ROUTE_TYPE_INDIRECT_GATEWAY 0
#define ROUTE_TYPE_DIRECT_GATEWAY 1

typedef enum {
    rt_idle = 0,
    rt_has_device,
    rt_waiting_fdap,
    rt_active,
    rt_inq,
}rt_stat;

extern mac_addr gateway_addr;
void route_update(route_frame_t *prt);
route_t *route_found_by_dst(mac_addr *pdst);
mac_addr *route_found_next_by_dst(mac_addr *pdst);
route_t *route_process_timeout();
uint8_t route_have_routes_to_device(void);
void route_init();
#endif /*__ROUTE__H_*/

