#include "config.h"
#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "plc.h"

/*
dst  passaddr     src    final dst

*/
section32 route_t rt_table[CONFIG_ROUTE_TABLE_SIZE];
mac_addr gateway_addr;

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



void route_put_rt_item(route_t *proute)
{
	proute->valide = 0;
}


// process when recved the route data package
void route_device_process(route_frame_t *prt)
{
	route_update(prt);

	// if me ,need to ack the gateway , so the gateway can know it can reached gateway
	if (self_mac.laddr == prt->dst_addr.laddr){
		route_ack_gateway(prt);
	}else{// if not me ,forward it if hop-- != 0
		route_sendto_next_hop(prt);// if hop==0, drop it
	}
}

void route_ack_gateway(route_frame_t *prt)
{
	route_t *proute;
	if (prt->route_type == ROUTETYPE_DFP) {
		gateway_addr.laddr = prt->src_addr.laddr;
	
		prt->route_type = ROUTETYPE_DFAP;
		prt->mac_type = MacPlc;
		prt->src_addr.laddr = self_mac.laddr;
		prt->dst_addr.laddr = gateway_addr.laddr;
		prt->hop = CONFIG_MAX_HOP;
		prt->pass_addr.laddr = self_mac.laddr;
		proute = route_found_by_dst(&gateway_addr);
		if (proute == NULL){
			return;//never reached here
		}
		linklay_send_data(&proute->next_addr, prt, sizeof(route_data_t));
	}
}
void route_sendto_next_hop(route_frame_t *prt)
{
	prt->hop--;

	if (prt->hop == 0)// hop==0 drop it
		return ;
	prt->pass_addr.laddr = self_mac.laddr;
	prt->mac_type = MacPlc;
	prt->hop = CONFIG_MAX_HOP;
	linklay_send_data(&prt->dst_addr, prt, sizeof(route_data_t));
}
void route_add(route_frame_t *prt)
{
	route_t *proute;

	proute = route_get_rt_item();
	if (proute != NULL){// 按照最近最少使用的原则替换掉一个
		proute = route_found_useless();
	}
	if (proute)
	{//new one
		if (prt->route_type == ROUTETYPE_DFP)
			proute->dir = ROUTE_DIR_TODEVICE;
		else if (prt->route_type == ROUTETYPE_DFAP)
			proute->dir = ROUTE_DIR_TOGATEWAY;
		else if (prt->route_type == ROUTETYPE_BT)
			proute->dir = ROUTE_DIR_TOGATEWAY;

		proute->dst.laddr = prt->src_addr.laddr;
		proute->next.laddr = prt->pass_addr.laddr;
		proute->hop = prt->hop;
		proute->phy_type = prt->mac_type;
		proute->rssiv = prt->rssiv;
		proute->seq = 0;
		proute->ticks = Timetick();
		proute->valide = 1;
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


