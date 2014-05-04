#ifndef __ROUTE__H_
#define __ROUTE__H_

#include "config.h"

#define ROUTETYPE_DFP 0
#define ROUTETYPE_DFAP 1
#define ROUTETYPE_BT   2
#define ROUTETYPE_BCAST_GW_ADDR 3
typedef struct {
	uchar mac_type:2;// 0: plc, 1:2.4G
	uchar route_type:2; // 0: gateway send:device found package, 1: device ack the found  package, 2:the beatheart between the next hop and the device
		                          // 3: broadcast the gateway addr
	uchar hop:4;//GFP:once through a device or gateway , hop++, and the linklayhead.Src = the deivce or gateway macaddr
			   //GFAP:once through a device ,hop--, if hop==0 and not arrived at the dst, drop it
	uchar rssiv;// 在link层被更新

	mac_addr pass_addr;// route data 经过的设备的地址

	mac_addr src_addr;// route data最初的发出地址
	mac_addr dst_addr;// route data 最后最终要接受的地址
	
}route_frame_t;

#define ROUTE_MAX_LIFTCYCLE MS_TO_TICK(20*1000)//20 second

typedef struct {
	mac_addr dst;
	mac_addr next;
	uchar rssiv;
	uchar hop:5;      //how many devices passed if we reached the dst
	uchar phy_type:2; //0: plc can reached to the dst, 1: 2.4G can reached the dst
	uchar seq:1;
	int16u ticks;  //ticks, 
	uchar valide:1;// used or not
	uchar dir:1;//0:means gateway to device, 1: means device to gateway
	uchar route_type:1;// 0:means not the direct connect to gateway, 1:means direct connect to the gateway 
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
}rt_stat;
#endif /*__ROUTE__H_*/

