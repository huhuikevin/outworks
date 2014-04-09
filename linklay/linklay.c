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
#include "tool.h"
#include "plc.h"

#define VERSION 0
#define PKG_TYPE_NORMAL 0
#define PKG_TYPE_CTRL    1

#define LINKLAY_BYTE4(ver, t, len) (ver<<6 | t << 5 | len)
#define LINKLAY_BYTE5(seq, f) (seq<<4 | f&0x3)

#define LINKLAY_BYTE5_ACK(seq, f) (seq<<4 | 1<<3 | f&0x3)

#define get_ver(byte4) ((bytes>>6) & 0x3)
#define get_type(byte4) ((byte4>>5) & 0x1)
#define get_len(byte4) (byte4 & 0x1f)
#define get_anck(byte4) (byte4>>3 & 0x1)

#define get_seq(byte5) ((byte5>>4) & 0xf)

typedef enum {
    LinkLayIdle = 0,
    LinkLayTxing,
    LinkLayTxPending,
    LinkLayTxFinish,
    LinkLayWaitAck,
    LinkLayRecvedAck,    
}uLinkLayStatMachine;



uchar mac_addr[MAC_ADDR_LEN];

uchar send_seq = 0;

uchar G_macType;

typedef struct
{
 	int8u mac_addr[MAC_ADDR_LEN];			//MAC地址
	int8u Version : 2;	//协议版本号
	int8u Pkg_Type : 1;			//包类型，0：普通包，1：应答包
	int8u App_Len : 5;			//数据长度
	int8u seq : 4;		//发送序列号
	int8u anck : 1;				//应答
	int8u res : 3;		//保留
}sLinkLayHead, *PsLinkLayHead;


typedef struct  
{
    sLinkLayHead head;
	int8u App_data[MAX_DATA_LEN];
	int8u checksum;	//校验  
}linklay_Frame, *Plinklay_Frame;

typedef struct
{
    int8u mac_type;
    int8u send_bytes;
    int8u recv_bytes;
    int8u send_seq;
    int8u link_statmachine; 
    int32u recv_overflov;
    linklay_Frame send_frame;
    linklay_Frame recv_frame;
}sLinklayCtrl, *PsLinklayCtrl;

sLinklayCtrl linklay[MacTypeEnd];



/* 上层拿这个buf进行数据发送 */
uchar *linklay_get_send_buf(int8u mac_type)
{
    if (linklay[i].link_statmachine == LinkLayIdle
        return (&linklay[mac_type].send_frame.App_data[0]);
    else
        return NULL;
}

int8u linklay_set_send_len(int8u mac_type, int8u need_send)
{
    if (linklay[i].link_statmachine == LinkLayIdle) {
        linklay[mac_type].send_bytes = need_send;
        return need_send;
    }else
        return 0;
}

void *linklay_get_recv_buf(int8u mac_type, int8u *pLen)
{
    if (linklay[mac_type].recv_bytes) {
        return (&linklay[mac_type].recv_frame.App_data[0]);
        *pLen = linklay[mac_type].recv_bytes;
    }
    else {
        *pLen = 0;
        return NULL;
    }
}


void *linklay_put_recv_buf(int8u mac_type)
{
    linklay[mac_type].recv_bytes = 0;
}



/* 上层已经把数据填写到 linklay_send_buf 中了 */
void linklay_send_process()
{
    int8u i;
    for (i = MacPlc; i < MacTypeEnd; i++) {
        PsLinkLayHead pHead = &linklay[i].send_frame.head;
        int8u pkglen = linklay[i].send_bytes + sizeof(sLinkLayHead);
        int8u sended = 0;
        
        if(linklay[i].send_bytes == 0)
            continue; 
        linklay[i].send_seq ++;
        pHead->mac_addr[0] = mac_addr[0];
        pHead->mac_addr[1] = mac_addr[1];
        pHead->mac_addr[2] = mac_addr[2];
        pHead->mac_addr[3] = mac_addr[3];
    
        pHead->Version = VERSION
        pHead->Pkt_Type = PKG_TYPE_NORMAL;
        pHead->App_Len = linklay[i].send_bytes;
        pHead->Seq = linklay[i].send_seq;
        
        linklay[i].checksum = CalChecksum((uchar *)pHead, pkglen);
    
        sended = mac_tx_bytes(linklay[i].mac_type, (uchar *)pHead, pkglen + sizoef(int8u));
        if (sended) {
            linklay[i].send_bytes = 0;//clear the send_bytes, means send ok
            linklay[i].link_statmachine = LinkLayIdle;
        }else
            linklay[i].link_statmachine = LinkLayTxPending;
    };
}


void linklay_recv_process()
{
    uchar i;
    uchar checksum;
    PsLinkLayHead pHead;
       
    for (i = MacPlc; i < MacTypeEnd; i++) {
        if (linklay[i].recv_bytes == 0)//no recv data
            continue;
        
        linklay[i].recv_bytes -= sizoef(int8u);//decc checksum len
        pHead = &linklay[i].recv_frame.head;
        checksum = CalChecksum((uchar *)&pHead, linklay[i].recv_bytes);
        linklay[i].recv_bytes -= sizeof(sLinkLayHead);//dec the head len
        if (checksum != linklay[i].checksum || linklay[i].recv_bytes != pHead->App_len)
        {
            linklay_proc_error(&linklay[i]);
            return;    
        }

        if (pHead->macaddr[0] != mac_addr[0] || pHead->macaddr[1] != mac_addr[1] ||
            pHead->macaddr[2] != mac_addr[2] || pHead->macaddr[3] != mac_addr[3])
        {
            linklay_porc_not_me(&linklay[i]);//may be need to route
            return;
        }
        if (pHead->Pkg_Type == PKG_TYPE_NORMAL){
            linklay_proc_normal_pkg(&linklay[i]);
            return;    
        }
        if (pHead->Pkg_Type == PKG_TYPE_CTRL){
            linklay_proc_ctrl_pkg(&linklay[i]);
            return;    
        }
    }
}


void linklay_process()
{
    if (mac_rx_bytes() != 0) {
        linklay_recv_data();
        linklay_recv_process();
    }
    linklay_send_process();
}

void linklay_setmac(uchar mactype)
{
    G_macType = mactype;    
}


intu8 linklay_rx(int8u mac_type, Plinklay_Frame pFrame)
{
    if (mac_type == MacPlc)
        return plc_rx_bytes((uchar *)pframe);
#if
    else if (mac_type == MacWireless_2_4G)
        return wireless_2_4G_rx_byte((uchar *)pframe);
#endif
    return 0;
}

intu8 mac_rx_bytes()
{
    intu8 len, i,tlen=0;
    for (i = MacPlc; i < MacTypeEnd; i++) {
        len = linklay_rx(i, &linklay[i].recv_frame);
        if (len) {
            if (linklay[i].recv_bytes != 0)
            linklay[i].recv_overflow ++;
            
            linklay[i].recv_bytes = len;
        }
        tlen += len;
    }
    return tlen;    
}

intu8 mac_tx_bytes(int8u mac_type, ucahr *pdata, int8u num)
{
    uchar realsend;
    
    if (mac_type == MAC_TYPE_PLC)
        realsend = plc_tx_bytes(pdata, num);
#ifdef W24G         
    else if (mac_type == MAC_TYPE_2_4G)
        realsend = wireless_2_4G_tx_bytes(pdata, num);
#endif    
    return realsend;    
}