//plc，2.4G 等数据链路层,支持分包，重发等机制
//byte 0-3:macaddr byte 0-3
//byte 4:  version bit7-6, pkg_type(普通和控制类) bit 5， len 4-0
//byte 5:  bit 7-4 trans sequnce , bit 3 acknck（pkg_type=1的时候，1:ack，0:nck）, bit 2-0 ??
//byte 6: checksum 
//byte 7-... data
/*
gateway      plc module   plcmodule    autodevice    autodevice    plcmodule  plcmodule     gateway
uartsend --> uartrecv --> plcsend ---> plcrecv--->op--->plcsend --->plcrecv--->uartsend---->uartrecv
*/

#include <hic.h>
#include "type.h"
#include "system.h"
#include "tool.h"
#include "plc_mac.h"
#include "timer16n.h"
#include "route.h"
#include "linklay_v2.h"
#ifdef CONFIG_HW2000
#include "hw2000_drv.h"
#endif
#define VERSION 0

typedef enum {
    send_idle = 0,
    send_txing,
    send_wait_ack,
    send_recved_ack, 
    send_error_no_router,
}send_stat_t;

typedef enum {
    recv_idle = 0,
    recv_finished,
}recv_stat_t;

#define TX_WAITACK_TIMEOUT 50// 500ms

typedef struct
{
    int8u mac_type:2;
    int8u send_stat :4;
    uint8_t ack_someone:1;
    uint8_t forward_pkg;
    int8u send_seq:1;
    int8u recv_stat:4;
    uint8_t res2:3;
    int8u send_bytes;
    int8u recv_bytes;
    uint8_t forward_bytes;
    int8u recv_overflow;
    uint8_t recv_rssi;
    uint8_t pkg_protocol;
    uint16_t send_timeout;
    //mac_addr send_next;
    link_frame_t send_frame;
    link_frame_t recv_frame;
    link_frame_t ack_frame;
    link_frame_t forward_frame;
}linklay_t;

#define linklay_ack_frame(mac, ack) \
do \
{ \
	linklay[mac].ack_someone = ack;\
}while(0);


#define linklay_forward_frame(mac, f) \
do \
{ \
	linklay[mac].forward_pkg = f;\
}while(0);

#define linklay_send_txing(mac) \
do \
{ \
	linklay[mac].send_stat = send_txing;\
}while(0);

#define linklay_send_sucess linklay_send_idle

#define linklay_send_idle(mac) \
do \
{ \
	linklay[mac].send_stat = send_idle;\
	linklay[mac].send_bytes = 0;\
}while(0);

#define linklay_send_waitack(mac) \
do \
{ \
	linklay[mac].send_stat = send_wait_ack;\
}while(0);

#define linklay_send_failt(mac, err) \
do \
{ \
	linklay[mac].send_stat = err;\
	linklay[mac].send_bytes = 0; \
}while(0);

#define linklay_is_txing(mac) (linklay[mac].send_stat != send_idle)
#define linklay_is_waitack(mac) (linklay[mac].send_stat == send_wait_ack)


#define linklay_data_recved(mac) do { linklay[mac].recv_stat = recv_finished;}while(0);
#define linklay_recv_idle(mac) do { linklay[mac].recv_stat = recv_idle;}while(0);
#define linklay_has_recved_data(mac) (linklay[mac].recv_stat == recv_finished)

section6 linklay_t linklay[MacTypeEnd]@0x300;

void linklay_send_process();
void linklay_recv_process();
uint8_t linklay_rx();
uint8_t mac_rx_bytes(uint8_t mac_type, link_frame_t *pFrame, uint8_t *prssi);
void linklay_build_package(linkaddr_t *plinkaddr);
uint8_t __linklay_recv_data(uint8_t *pdata, uint8_t mac, uint8_t protol);
uint8_t linklay_tx_bytes(uint8_t mac_type, mac_addr *paddr, uint8_t *pdata, uint8_t num);
void linklay_forward_process();
void linklay_ackframe_process();

void linklay_init()
{
	MMemSet(&linklay[0], 0, sizeof(linklay_t));
	linklay[0].mac_type = MacPlc;
	MMemSet(&linklay[1], 0, sizeof(linklay_t));
	linklay[1].mac_type = MacHw2000;
    
   self_mac.laddr = 0xffffffff;
}

uint8_t linklay_send_data(int8u *pdata, int8u len, linkaddr_t *plinkaddr)
{
	linklay_send_process();
	if (!linklay_is_txing(plinkaddr->mac)){
		MMemcpy(&linklay[plinkaddr->mac].send_frame.link_data[0], pdata, len);
		linklay[plinkaddr->mac].send_bytes = len;
		//linklay[plinkaddr->mac].send_next.laddr = plinkaddr->next.laddr;
		linklay[plinkaddr->mac].send_timeout = 0;
		linklay_build_package(plinkaddr);
		linklay_send_txing(plinkaddr->mac);
		linklay_send_process();
		return len;
	}
	return 0;
}

uint8_t linklay_send_route_data(mac_addr *pdst, uint8_t *pdata, uint8_t len, uint8_t mac)
{
	linkaddr_t addr;
	addr.dest.laddr = pdst->laddr;
	addr.needack = 0;
	addr.protocol = PROTOCOL_ROUTER;
	addr.mac = mac;
	return linklay_send_data(pdata, len, &addr);
}


uint8_t linklay_send_app_data(mac_addr *pdst, uint8_t *pdata, uint8_t len, uint8_t needack)
{
	linkaddr_t addr;
	route_t *pdest;
	addr.dest.laddr = pdst->laddr;
	addr.needack = needack;
	addr.protocol = PROTOCOL_NORMAL;
	
	pdest = route_found_by_dst(pdst);
	if (!pdest) {
		linklay_send_failt(pdest->phy_type, send_error_no_router);
		return 0;
	}
	addr.mac = pdest->phy_type;
	return linklay_send_data(pdata, len, &addr);
}

send_stat_t linklay_get_tx_status(uint8_t mac)
{
	return linklay[mac].send_stat;
}

void linklay_build_package(linkaddr_t *plinkaddr)
{
	linklay_t *plink = &linklay[plinkaddr->mac];
	linkhead_t *pHead = &plink->send_frame.head;

    pHead->Version = 0;
	pHead->ack_pkg = 0;
	if (plinkaddr->protocol == PROTOCOL_ROUTER)
		pHead->need_ack = 0;
	else
	 	pHead->need_ack = plinkaddr->needack;
	plink->send_seq ^= 1;
	pHead->seq = plink->send_seq;
	pHead->protocol = plinkaddr->protocol;
	pHead->rtdst_addr.laddr = plinkaddr->dest.laddr;
    pHead->Version = VERSION;
    pHead->protocol = PROTOCOL_NORMAL;
    //pHead->App_Len = linklay[i].send_bytes;
    pHead->seq = plink->send_seq;
        
}

uint8_t linklay_recv_data(uint8_t *pdata, uint8_t mac)
{
	return __linklay_recv_data(pdata, mac, PROTOCOL_NORMAL);
}

uint8_t linklay_recv_data_with_rssi(uint8_t *pdata, uint8_t mac, uint8_t *prssi)
{
    if (prssi)
        *prssi = linklay[mac].recv_rssi;
	
    return __linklay_recv_data(pdata, mac, PROTOCOL_ROUTER);
}

uint8_t __linklay_recv_data(uint8_t *pdata, uint8_t mac, uint8_t protol)
{
    uint8_t len = linklay[mac].recv_bytes;
    if (linklay_has_recved_data(mac) && (linklay[mac].pkg_protocol == protol)) {
        MMemcpy (pdata, &linklay[mac].recv_frame.link_data[0], len);
        linklay_recv_idle(mac);
	 return len; 
    }
    return 0;
}


/* 上层已经把数据填写到 linklay_send_buf 中了 */
void linklay_send_process()
{
	uint8_t i, mac;
    route_t *pdest;
	mac_addr *paddr;
    for (i = MacPlc; i < MacTypeEnd; i++) {
        linkhead_t *pHead = &linklay[i].send_frame.head;
        int8u pkglen = linklay[i].send_bytes + sizeof(linkhead_t);
        int8u sended = 0;
        
        if(linklay[i].send_bytes == 0)
            continue; 

        if (linklay_is_waitack(i)){
            if (linklay[i].send_timeout > _sys_tick)//not timeouted
	         continue;
		}
		if (pHead->protocol == PROTOCOL_NORMAL) {
			pdest = route_found_by_dst(&pHead->rtdst_addr);
			if (!pdest){
	    		linklay_send_failt(i, send_error_no_router);
        		continue;
			}
			mac = pdest->phy_type;
			paddr = &pdest->next;
		}else if (pHead->protocol == PROTOCOL_ROUTER) {
			paddr = &pHead->rtdst_addr;
			mac = linklay[i].mac_type;
		}
			
 		sended = linklay_tx_bytes(mac, paddr, (uint8_t *)pHead, pkglen);
    	if (sended) {
			if (!pHead->need_ack) {
	         	linklay_send_idle(i);
			}
	    	else {
	         	linklay_send_waitack(i);
	         	linklay[i].send_timeout = _sys_tick + TX_WAITACK_TIMEOUT;
	    	}
    	}else
    		linklay_send_txing(i);
    }
#ifndef CONFIG_NO_ROUTE
#ifdef CONFIG_TYPE_AUTODEVICE
    linklay_forward_process();
    linklay_ackframe_process();
#endif
#endif
}


void linklay_forward_process()
{
    uint8_t i;
    route_t *pdest;

    for (i = MacPlc; i < MacTypeEnd; i++) {
        linkhead_t *pHead = &linklay[i].forward_frame.head;
        uint8_t pkglen = linklay[i].forward_bytes;
        uint8_t sended = 0;
        
        if(linklay[i].forward_pkg == 0)
            continue; 

	 pdest = route_found_by_dst(&pHead->rtdst_addr);
	 if (!pdest){
	     linklay_forward_frame(linklay[i].mac_type, 0);
            continue;
	 }
        sended = linklay_tx_bytes(pdest->phy_type, &pdest->next, (uint8_t *)pHead, pkglen);
        if (sended) {
            linklay_forward_frame(linklay[i].mac_type, 0);
        }
    }
}


void linklay_ackframe_process()
{
    uint8_t i;
    route_t *pdest;

    for (i = MacPlc; i < MacTypeEnd; i++) {
        linkhead_t *pHead = &linklay[i].ack_frame.head;
        int8u sended = 0;
        
        if(linklay[i].ack_someone == 0)
            continue; 

	 pdest = route_found_by_dst(&pHead->rtdst_addr);
	 if (!pdest){
	     linklay_ack_frame(linklay[i].mac_type, 0);
            continue;
	 }
        sended = linklay_tx_bytes(pdest->phy_type, &pdest->next, (uint8_t *)pHead, sizeof(linkhead_t));
        if (sended) {
            linklay_ack_frame(linklay[i].mac_type, 0);
        }
    }
}

void linklay_process_error(linklay_t *plink)
{
	
}

void linklay_process_ack(linklay_t *plink)
{
	linkhead_t *pHead = &plink->recv_frame.head;
	linkhead_t *pHeads =  &plink->send_frame.head;
	if (!linklay_is_waitack(plink->mac_type))
	    return;
	if (pHead->seq == pHeads->seq){
           linklay_send_sucess(plink->mac_type);
	}
}

uint8_t linklay_forward_data(linklay_t *plink)
{
	linkhead_t *pHead = &plink->recv_frame.head;
	linklay_t *outlink;
	route_t *proute;
	if (pHead->rtdst_addr.laddr == self_mac.laddr){
		return 0;
	}
	linklay_recv_idle(plink->mac_type);
	proute = route_found_by_dst(&pHead->rtdst_addr);
	if (proute) {
		outlink = &linklay[proute->phy_type];
		linklay_forward_frame(outlink->mac_type, 1);
		MMemcpy(&outlink->forward_frame, &plink->recv_frame, plink->recv_bytes+sizeof(linkhead_t));
		outlink->forward_bytes = plink->recv_bytes+sizeof(linkhead_t);
		if (!linklay_tx_bytes(outlink->mac_type, &proute->next, &outlink->forward_frame, outlink->forward_bytes))
			linklay_forward_frame(outlink->mac_type, 1);
		
	}
	return 1;
}

void linklay_process_normal(linklay_t *plink)
{
	linkhead_t *pHead = &plink->recv_frame.head;
	mac_addr *pdst;

#ifdef CONFIG_TYPE_AUTODEVICE	
	if (linklay_forward_data(plink))//router forward
	    return;
#endif

	if (pHead->ack_pkg){
	    linklay_process_ack(plink);
           return;
	}
	
	linklay_data_recved(plink->mac_type);

	plink->pkg_protocol = PROTOCOL_NORMAL;
	if (!pHead->need_ack)
		return;
#ifdef CONFIG_TYPE_AUTODEVICE	
	MMemcpy(&plink->ack_frame.head, pHead, sizeof(linkhead_t));

	plink->ack_frame.head.ack_pkg = 1;
	
	plink->ack_frame.head.rtdst_addr.laddr = gateway_addr.laddr;
	pdst = route_found_next_by_dst(&gateway_addr);
	if (pdst)
	{
		if (!linklay_tx_bytes(plink->mac_type, pdst, &plink->ack_frame, sizeof(linkhead_t)))
			linklay_ack_frame(plink->mac_type, 1);
		
	}
#endif

}


void linklay_process_route(linklay_t *plink)
{
	linkhead_t *pHead = &plink->recv_frame.head;
	linklay_data_recved(plink->mac_type);
	plink->pkg_protocol = PROTOCOL_ROUTER;
}

void linklay_recv_process()
{
    uint8_t i;
    linkhead_t *pHead;
       
    for (i = MacPlc; i < MacTypeEnd; i++) {
        if (linklay[i].recv_bytes == 0)//no recv data
            continue;
        
        pHead = &linklay[i].recv_frame.head;
        linklay[i].recv_bytes -= sizeof(linkhead_t);//dec the head len

        if (pHead->protocol == PROTOCOL_ROUTER){
            linklay_process_route(&linklay[i]);
            continue;    
        }

        if (pHead->protocol == PROTOCOL_NORMAL){
            linklay_process_normal(&linklay[i]);
            continue;    
        }

    }
}

void linklay_process()
{
    if (linklay_rx() != 0) {
        //linklay_recv_data();
        linklay_recv_process();
    }
    linklay_send_process();
}


int8u linklay_rx()
{
    uint8_t len, i,tlen=0;
    for (i = MacPlc; i < MacTypeEnd; i++) {
        len = mac_rx_bytes(i, &linklay[i].recv_frame, &linklay[i].recv_rssi);
        if (len) {
            if (linklay[i].recv_bytes != 0)
            linklay[i].recv_overflow ++;
            
            linklay[i].recv_bytes = len;
        }
        tlen += len;
    }
    return tlen;    
}

uint8_t mac_rx_bytes(uint8_t mac_type, link_frame_t *pFrame, uint8_t *prssi)
{
    if (mac_type == MacPlc)
        return plc_mac_rx_with_rssi((uint8_t *)pFrame, prssi);
#ifdef CONFIG_HW2000
    else if (mac_type == MacHw2000)
        return hw2000_rx_bytes((uint8_t *)pFrame);
#endif
    return 0;
}

uint8_t linklay_tx_bytes(uint8_t mac_type, mac_addr *paddr, uint8_t *pdata, uint8_t num)
{
    uint8_t realsend;
    
    if (mac_type == MacPlc)
        realsend = plc_mac_tx(paddr, pdata, num);
#ifdef CONFIG_HW2000         
    else if (mac_type == MacHw2000)
        realsend = hw2000_tx_bytes(pdata, num);
#endif    
    return realsend;    
}
