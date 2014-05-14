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
}send_stat_t;

typedef enum {
    recv_idle = 0,
    recv_finished,
}recv_stat_t;


section4 uchar mac_addr[MAC_ADDR_LEN];

section4 uchar send_seq ;

section4 uchar G_macType;



typedef struct
{
    int8u mac_type:2;
    int8u send_bytes:6;
    int8u recv_bytes:6;
    int8u send_stat :2;
    int8u send_seq:4;
    int8u recv_stat:4;
    int8u recv_overflow;
    uint8_t recv_rssi;
    link_frame_t send_frame;
    link_frame_t recv_frame;
}linklay_t;

#define FRMAE_SEND_DATA 0x1
#define FRMAE_RECV_DATA 0x2

#define linklay_send_txing(mac) \
do \
{ \
	linklay[mac].send_stat = send_txing;\
}while(0);

#define linklay_send_idle(mac) \
do \
{ \
	linklay[mac].send_stat = send_idle;\
}while(0);

#define linklay_send_waitack(mac) \
do \
{ \
	linklay[mac].send_stat = send_wait_ack;\
}while(0);


#define linklay_is_txing(mac) (linklay[mac].send_stat != send_idle)



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

int8u linklay_send_data(int8u *pdata, int8u len, int8u mac, uint8_t protocol, uint8_t ack)
{
	linklay_send_process();
	if (!linklay_tx_is_sending(mac)){
		MMemcpy(&linklay[mac].send_frame.App_data[0], pdata, len);
		linklay[mac].send_bytes = len;
		linklay_build_package(&linklay[mac], protocol, ack);
		linklay_send_txing(mac);
		linklay_send_process();
		return len;
	}
	return 0;
}


void linklay_build_package(linklay_t *plink, uint8_t protocol, uint8_t ack)
{
        linkhead_t pHead = &plink->send_frame.head;
        pHead->Version = 0;
	 pHead->ack_pkg = ack;
	 if (protocol == PROTOCOL_ROUTER)
	 	pHead->need_ack = 0;
	 else
	 	pHead->need_ack = 1;
	 plink->send_seq ^= 1;
	 pHead->seq = plink->send_seq;
	 pHead->protocol = protocol;
	 pHead->rtdst_addr.laddr
        pHead->Version = VERSION;
        pHead->protocol = PKG_TYPE_NORMAL;
        //pHead->App_Len = linklay[i].send_bytes;
        pHead->Seq = linklay[i].send_seq;
        
}

int8u linklay_recv_data(int8u *pdata, int8u mac)
{
    int8u len = linklay[mac].recv_bytes;
    if (linklay_has_recved_data(mac)) {
        MMemcpy (pdata, &linklay[mac].recv_frame.App_data[0], len);
        linklay_recv_idle(mac);
	 return len; 
    }
    return 0;
}


/* 上层已经把数据填写到 linklay_send_buf 中了 */
void linklay_send_process()
{
    int8u i;
    for (i = MacPlc; i < MacTypeEnd; i++) {
        linkhead_t pHead = &linklay[i].send_frame.head;
        int8u pkglen = linklay[i].send_bytes + sizeof(linkhead_t);
        int8u sended = 0;
        
        if(linklay[i].send_bytes == 0)
            continue; 
        linklay[i].send_seq ++;
        pHead->mac_addr[0] = mac_addr[0];
        pHead->mac_addr[1] = mac_addr[1];
        pHead->mac_addr[2] = mac_addr[2];
        pHead->mac_addr[3] = mac_addr[3];
    
        pHead->Version = VERSION;
        pHead->protocol = PKG_TYPE_NORMAL;
        //pHead->App_Len = linklay[i].send_bytes;
        pHead->Seq = linklay[i].send_seq;
        
        sended = mac_tx_bytes(linklay[i].mac_type, (uchar *)pHead, pkglen);
        if (sended) {
            //linklay[i].send_bytes = 0;//clear the send_bytes, means send ok
            //linklay[i].send_statmachine = LinkLayIdle;
	     linklay_set_sendready(i);
        }else
            linklay_send_txing(i);
    };
}


void linklay_proc_error(PsLinklayCtrl *pCtrl)
{
	
}

void linklay_porc_not_me(PsLinklayCtrl *pCtrl)
{
	
}

void linklay_proc_normal(PsLinklayCtrl pCtrl)
{
	linklay_set_datarecved(pCtrl->mac_type);
}


void linklay_proc_route(linklay_t *plink)
{
	linkhead_t *pHead = &plink->recv_frame.head;
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
            linklay_proc_error(&linklay[i]);
            return;    
        }
        if (pHead->protocol == PROTOCOL_ROUTER){
            linklay_proc_route(&linklay[i]);
            return;    
        }

        if (pHead->protocol == PROTOCOL_NORMAL){
            linklay_proc_normal(&linklay[i]);
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

void linklay_setmac(uchar mactype)
{
    G_macType = mactype;    
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

int8u mac_rx_bytes(int8u mac_type, Plinklay_Frame pFrame, uint8_t *prssi)
{
    if (mac_type == MacPlc)
        return plc_mac_rx_with_rssi((uchar *)pFrame, prssi);
#ifdef CONFIG_HW2000
    else if (mac_type == MacHw2000)
        return hw2000_rx_bytes((uchar *)pFrame);
#endif
    return 0;
}

int8u mac_tx_bytes(int8u mac_type, uchar *pdata, int8u num)
{
    uchar realsend;
    
    if (mac_type == MacPlc)
        realsend = plc_tx_bytes(pdata, num);
#ifdef CONFIG_HW2000         
    else if (mac_type == MacHw2000)
        realsend = hw2000_tx_bytes(pdata, num);
#endif    
    return realsend;    
}