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
�ֻ��������豸ɨ������������룬�õ�������mac��ַ��
ͨ�����緢�͸���������,���ؽ��յ��󣬵��ñ�����
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


