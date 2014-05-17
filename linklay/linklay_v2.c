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
#include "plc.h"
#include "linklay_v2.h"

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


#define linklay_data_recved(mac) do { linklay[mac].recv_state = recv_finished;}while(0);
#define linklay_recv_idle(mac) do { linklay[mac].recv_state = recv_idle;}while(0);
#define linklay_has_recved_data(mac) (linklay[mac].recv_state == recv_finished)

section6 sLinklayCtrl linklay[MacTypeEnd]@0x300;

void linklay_send_process();
void linklay_recv_process();

int8u mac_tx_bytes(int8u mac_type, uchar *pdata, int8u num);

int8u linklay_rx();
int8u mac_rx_bytes(int8u mac_type, Plinklay_Frame pFrame);

void linklay_init()
{
	MMemSet(&linklay[0], 0, sizeof(sLinklayCtrl));
	linklay[0].mac_type = MacPlc;
	MMemSet(&linklay[1], 0, sizeof(sLinklayCtrl));
	linklay[1].mac_type = MacHw2000;
}

int8u linklay_send_data(int8u *pdata, int8u len, linkaddr_t *plinkaddr)
{
	linklay_send_process();
	if (!linklay_is_txing(plinkaddr->mac)){
		MMemcpy(&linklay[plinkaddr->mac].send_frame.App_data[0], pdata, len);
		linklay[plinkaddr->mac].send_bytes = len;
		//linklay[plinkaddr->mac].send_next.laddr = plinkaddr->next.laddr;
		linklay[i].send_timeout = 0;
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
	route_t pdest;
	addr.dest.laddr = pdst->laddr;
	addr.needack = needack;
	addr.protocol = PROTOCOL_NORMAL;
	
	pdest = route_found_by_dst(pdst);
	if (pdest) {
		linklay_send_failt(send_error_no_router);
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
	linkhead_t pHead = &plink->send_frame.head;
	linklay_t *plink = &linklay[plinkaddr->mac];
	 
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
    pHead->protocol = PKG_TYPE_NORMAL;
    //pHead->App_Len = linklay[i].send_bytes;
    pHead->Seq = plink->send_seq;
        
}

int8u linklay_recv_data(int8u *pdata, int8u mac)
{
	return __linklay_recv_data(pdata, mac, PROTOCOL_NORMAL);
}

int8u linklay_recv_data_with_rssi(int8u *pdata, int8u mac, uint8_t *prssi)
{
    if (prssi)
        *prssi = linklay[mac].recv_rssi;
	
    return __linklay_recv_data(pdata, mac, PROTOCOL_ROUTER);
}

int8u __linklay_recv_data(int8u *pdata, int8u mac, uint8_t protol)
{
    int8u len = linklay[mac].recv_bytes;
    if (linklay_has_recved_data(mac) && (linklay[mac].pkg_protocol = protol)) {
        MMemcpy (pdata, &linklay[mac].recv_frame.App_data[0], len);
        linklay_recv_idle(mac);
	 return len; 
    }
    return 0;
}


/* 上层已经把数据填写到 linklay_send_buf 中了 */
void linklay_send_process()
{
	int8u i, mac;
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
	    		linklay_send_failt(send_error_no_router);
        		continue;
			}
			mac = pdest->phy_type;
			paddr = &pdest->next;
		}else if (pHead->protocol == PROTOCOL_ROUTER) {
			paddr = &pHead->rtdst_addr;
			mac = linklay[i].mac;
		}
			
 		sended = linklay_tx_bytes(mac, paddr, (uchar *)pHead, pkglen);
    	if (sended) {
			if (!pHead->need_ack)
	         	linklay_send_idle(i);
	    	else {
	         	linklay_send_waitack(i);
	         	linklay[i].send_timeout = _sys_tick + TX_WAITACK_TIMEOUT;
	    	}
    	}else
    		linklay_send_txing(i);
    }
#ifdef CONFIG_TYPE_AUTODEVICE
    linklay_forward_process();
    linklay_ackframe_process();
#endif	
}


void linklay_forward_process()
{
    int8u i;
    route_t *pdest;

    for (i = MacPlc; i < MacTypeEnd; i++) {
        linkhead_t *pHead = &linklay[i].forward_frame.head;
        int8u pkglen = linklay[i].forward_bytes;
        int8u sended = 0;
        
        if(linklay[i].forward_pkg == 0)
            continue; 

	 pdest = route_found_by_dst(&pHead->rtdst_addr);
	 if (!pdest){
	     linklay_forward_frame(linklay[i].mac, 0);
            continue;
	 }
        sended = linklay_tx_bytes(pdest->phy_type, &pdest->next, (uchar *)pHead, pkglen);
        if (sended) {
            linklay_forward_frame(linklay[i].mac, 0);
        }
    }
}


void linklay_ackframe_process()
{
    int8u i;
    route_t *pdest;

    for (i = MacPlc; i < MacTypeEnd; i++) {
        linkhead_t *pHead = &linklay[i].ack_frame.head;
        int8u sended = 0;
        
        if(linklay[i].ack_someone == 0)
            continue; 

	 pdest = route_found_by_dst(&pHead->rtdst_addr);
	 if (!pdest){
	     linklay_ack_frame(linklay[i].mac, 0);
            continue;
	 }
        sended = linklay_tx_bytes(pdest->phy_type, &pdest->next, (uchar *)pHead, sizeof(linkhead_t));
        if (sended) {
            linklay_ack_frame(linklay[i].mac, 0);
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
	if (!linklay_send_waitack(plink->mac))
	    return;
	if (pHead->seq == pHeads->seq){
           linklay_send_sucess(plink->mac);
	}
}

uint8_t linklay_forward_data(linklay_t *plink)
{
	linkhead_t *pHead = &plink->recv_frame.head;
	mac_addr *pdst;
	if (pHead->rtdst_addr.laddr == self_mac.laddr){
		return 0;
	}
	linklay_recv_idle(plink->mac);
	pdst = route_found_next_by_dst(&pHead->rtdst_addr);
	if (pdst) {
		linklay_forward_frame(plink->mac, 1);
		MMemcpy(&plink->forward_frame, &plink->recv_frame, plink->recv_bytes+sizeof(linkhead_t));
		plink->forward_bytes = plink->recv_bytes+sizeof(linkhead_t);
		if (!linklay_tx_bytes(plink->mac, pdst, &plink->forward_frame, plink->forward_bytes))
			linklay_forward_frame(plink->mac, 1);
		
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
	
	linklay_data_recved(plink->mac);

#ifdef CONFIG_TYPE_AUTODEVICE	
	plink->pkg_protocol = PROTOCOL_NORMAL;

	MMemcpy(&plink->ack_frame.head, pHead, sizeof(linkhead_t));

	plink->ack_frame.head.ack_pkg = 1;
	
	plink->ack_frame.head.rtdst_addr.laddr = gateway_addr.laddr;
	pdst = route_found_next_by_dst(&gateway_addr);
	if (pdst)
	{
		if (!linklay_tx_bytes(plink->mac, pdst, &plink->ack_frame, sizeof(linkhead_t)))
			linklay_ack_frame(plink->mac, 1);
		
	}
#endif

}


void linklay_process_route(linklay_t *plink)
{
	linkhead_t *pHead = &plink->recv_frame.head;
	linklay_data_recved(plink->mac);
	plink->pkg_protocol = PROTOCOL_ROUTER;
}

void linklay_recv_process()
{
    uchar i;
    linkhead_t *pHead;
       
    for (i = MacPlc; i < MacTypeEnd; i++) {
        if (linklay[i].recv_bytes == 0)//no recv data
            continue;
        
        pHead = &linklay[i].recv_frame.head;
        linklay[i].recv_bytes -= sizeof(linkhead_t);//dec the head len
        if ( linklay[i].recv_bytes != pHead->App_Len)
        {
            linklay_process_error(&linklay[i]);
            return;    
        }
        if (pHead->protocol == PROTOCOL_ROUTER){
            linklay_process_route(&linklay[i]);
            return;    
        }

        if (pHead->protocol == PROTOCOL_NORMAL){
            linklay_process_normal(&linklay[i]);
            return;    
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
    int8u len, i,tlen=0;
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

int8u mac_rx_bytes(int8u mac_type, link_frame_t *pFrame, uint8_t *prssi)
{
    if (mac_type == MacPlc)
        return plc_mac_rx_with_rssi((uchar *)pFrame, prssi);
#ifdef CONFIG_HW2000
    else if (mac_type == MacHw2000)
        return hw2000_rx_bytes((uchar *)pFrame);
#endif
    return 0;
}

int8u linklay_tx_bytes(int8u mac_type, mac_addr *paddr, uchar *pdata, int8u num)
{
    uchar realsend;
    
    if (mac_type == MacPlc)
        realsend = plc_mac_tx(paddr, pdata, num);
#ifdef CONFIG_HW2000         
    else if (mac_type == MacHw2000)
        realsend = hw2000_tx_bytes(pdata, num);
#endif    
    return realsend;    
}
