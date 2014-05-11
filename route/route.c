#include "config.h"
#include <hic.h>
#include "type.h"
#include "tool.h"


/*
dst  passaddr     src    final dst

*/

void route_init()
{
	MMemSet((void *)&rt_table[0],0,sizeof(route_t)*CONFIG_ROUTE_TABLE_SIZE)
}

route_t *route_get_rt_item()
{
	uchar i;

	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (!rt_table[i].valide){
			return &rt_table[i];
		}
	}
	return NULL;
}
// if found , update the time tick, and return the item
route_t *route_found_and_update_ticks(mac_addr *pdst, mac_addr *pnext)
{
	uchar i;
	
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (rt_table[i].valide){
			if ((pdst->laddr == rt_table[i].dst.laddr) && 
				(pnext->laddr == rt_table[i].next.laddr)){
				rt_table[i].ticks = Timetick();
				return &rt_table[i];
			}
		}
	}
	return NULL;
}

// if found , update the time tick, and return the item
route_t *route_found_by_dst(mac_addr *pdst)
{
	uchar i;
	
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (rt_table[i].valide){
			if (pdst->laddr == rt_table[i].dst.laddr){
				return &rt_table[i];
			}
		}
	}
	return NULL;
}

route_t *route_found_useless()
{
	uchar i, founded;
	int16u min=0xffff;
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (rt_table[i].valide){
			if (rt_table[i].ticks < min) {
				min = rt_table[i].ticks;
				founded = i;
			}
		}
	}
	if (i  < CONFIG_ROUTE_TABLE_SIZE)
		return &rt_table[founded];
	else
		return NULL;
}

uchar route_have_routes_to_device(void)
{
	uchar i;
	
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (rt_table[i].valide){
			if (gateway_addr.laddr != rt_table[i].dst.laddr){
				return 1;
			}
		}
	}
	return 0;

}
void route_put_rt_item(route_t *proute)
{
	proute->valide = 0;
}


#if 0
void route_process()
{
	route_frame_t rt_frame;
	uchar len;
	len = linklay_recv_data(&rt_frame, PROTOCOL_ROUTER);
	if (len)
		route_device_process(&rt_frame);
}
#endif



void route_add(route_frame_t *prt)
{
	route_t *proute;

	proute = route_get_rt_item();
	if (proute != NULL){// 按照最近最少使用的原则替换掉一个
		proute = route_found_useless();
	}
	if (proute)
	{//new one
		if (prt->route_type == ROUTETYPE_MCAST_DFP)
			proute->dir = ROUTE_DIR_TODEVICE;
		else if (prt->route_type == ROUTETYPE_DFAP)
			proute->dir = ROUTE_DIR_TOGATEWAY;
		else if (prt->route_type == ROUTETYPE_BCAST_DEVADDR)
			proute->dir = ROUTE_DIR_TOGATEWAY;
		else if (prt->route_type == ROUTETYPE_BCAST_GWADDR)
			proute->dir = ROUTE_DIR_TODEVICE;
		proute->dst.laddr = prt->src_addr.laddr;
		proute->next.laddr = prt->pass_addr.laddr;
		proute->hop = prt->hop;
		proute->phy_type = prt->mac_type;
		proute->rssiv = prt->rssiv;
		proute->seq = 0;
		proute->ticks = Timetick();
		proute->valide = 1;
		if (prt->route_type == ROUTETYPE_BCAST_GWADDR)
			proute->route_type = ROUTE_TYPE_DIRECT_GATEWAY;
		else
			proute->route_type = ROUTE_TYPE_INDIRECT_GATEWAY;
		return;
	}
}
void route_update(route_frame_t *prt)
{
	route_t *proute;

	proute = route_found_and_update_ticks(&prt->src_addr, &prt->pass_addr);
	if (proute != NULL){// if found do nothing
		return;
	}
	proute = route_found_by_dst(&prt->src_addr);
	if (!proute){//new one
		route_add(prt);
		return;
	}
	if (prt->mac_type == proute->phy_type){// update the exsit router item
		if ((prt->hop < proute->hop) || (prt->rssiv > proute->rssiv)){
			proute->next.laddr = prt->pass_addr.laddr;
			proute->hop = prt->hop;
			proute->rssiv = prt->rssiv;
			proute->ticks = Timetick();
		}
	}else{
		//if (prt->mac_type == )
		// to be done
	}
}


route_t *route_process_timeout()
{
	uchar i;
	
	for (i = 0; i < CONFIG_ROUTE_TABLE_SIZE; i++ ){
		if (rt_table[i].valide){
			if (IsTimeOut(rt_table[i].ticks + ROUTE_MAX_LIFTCYCLE))
			{
				rt_table[i].valide = 0;
				return &rt_table[i];
			}
		}
	}
	return NULL;
}

