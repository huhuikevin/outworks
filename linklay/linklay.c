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
#include "linklay.h"

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
    LinkSendIdle = 0,
    LinkSendTxing,
    LinkSendWaitAck,
    LinkSendRecvedAck,    
}uLinkLayStatMachine;


section4 uchar mac_addr[MAC_ADDR_LEN];

section4 uchar send_seq ;

section4 uchar G_macType;

typedef struct
{
 	int8u mac_addr[MAC_ADDR_LEN];			//MAC地址
	int8u Version : 2;	//协议版本号
	int8u Pkg_Type : 1;			//包类型，0：普通包，1：应答包
	int8u App_Len : 5;			//数据长度
	int8u Seq : 4;		//发送序列号
	int8u Anck : 1;				//应答
	int8u Res : 3;		//保留
	int8u checksum;	//校验  
}sLinkLayHead, *PsLinkLayHead;


typedef struct  
{
    sLinkLayHead head;
	int8u App_data[MAX_APP_DATA_LEN];
}linklay_Frame, *Plinklay_Frame;

typedef struct
{
    int8u mac_type:2;
    int8u send_bytes:6;
    int8u recv_bytes:6;
    int8u send_statmachine :2;
    int8u send_seq:4;
    int8u frame_state:4;
    int32u recv_overflow;
    linklay_Frame send_frame;
    linklay_Frame recv_frame;
}sLinklayCtrl, *PsLinklayCtrl;

#define FRMAE_SEND_DATA 0x1
#define FRMAE_RECV_DATA 0x2

#define linklay_set_sending(mac) \
do \
{ \
	linklay[mac].frame_state=linklay[mac].frame_state|FRMAE_SEND_DATA;\
	linklay[mac].send_statmachine = LinkSendTxing;\
}while(0);

#define linklay_set_sendready(mac) \
do \
{ \
	linklay[mac].frame_state=linklay[mac].frame_state&(~FRMAE_SEND_DATA);\
	linklay[mac].send_statmachine = LinkSendIdle;\
}while(0);

#define linklay_tx_is_sending(mac) (linklay[mac].frame_state & FRMAE_SEND_DATA)



#define linklay_set_datarecved(mac) do { linklay[mac].frame_state=linklay[mac].frame_state|FRMAE_RECV_DATA;}while(0);
#define linklay_set_recv_empty(mac) do { linklay[mac].frame_state=linklay[mac].frame_state&(~FRMAE_RECV_DATA);}while(0);
#define linklay_has_recved_data(mac) (linklay[mac].frame_state & FRMAE_RECV_DATA)

section6 sLinklayCtrl linklay[MacTypeEnd]@300;

void linklay_send_process();
void linklay_recv_process();
int8u mac_rx_bytes();
int8u mac_tx_bytes(int8u mac_type, uchar *pdata, int8u num);


void linklay_init()
{
	MMemSet(&linklay[0], 0, sizeof(sLinklayCtrl)*MacTypeEnd);
	linklay[0].mac_type = MacPlc;
	linklay[1].mac_type = MacWireless_2_4G;
}

int8u linklay_send_data(int8u *pdata, int8u len, int8u mac)
{
	linklay_send_process();
	if (!linklay_tx_is_sending(mac)){
		MMemcpy(&linklay[mac].send_frame.App_data[0], pdata, len);
		linklay[mac].send_bytes = len;
		linklay_set_sending(mac);
		linklay_send_process();
		return len;
	}
	return 0;
}

int8u linklay_recv_data(int8u pdata, int8u mac)
{
    int8u len = linklay[mac].recv_bytes;
    if (linklay_has_recved_data(mac)) {
        MMemcpy (pdata, &linklay[mac].recv_frame.App_data[0], len);
        linklay_set_recv_empty(mac);
	 return len; 
    }
    return 0;
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
    
        pHead->Version = VERSION;
        pHead->Pkg_Type = PKG_TYPE_NORMAL;
        pHead->App_Len = linklay[i].send_bytes;
        pHead->Seq = linklay[i].send_seq;
        
        pHead->checksum = CalChecksum((uchar *)pHead, sizeof(sLinkLayHead) - sizeof(int8u));
    	pHead->checksum += CalChecksum((uchar *)(pHead + 1), pHead->App_Len);
        sended = mac_tx_bytes(linklay[i].mac_type, (uchar *)pHead, pkglen);
        if (sended) {
            //linklay[i].send_bytes = 0;//clear the send_bytes, means send ok
            //linklay[i].send_statmachine = LinkLayIdle;
	     linklay_set_sendready(i);
        }else
            linklay_set_sending(i);
    };
}


void linklay_proc_error(PsLinklayCtrl *pCtrl)
{
	
}

void linklay_porc_not_me(PsLinklayCtrl *pCtrl)
{
	
}

void linklay_proc_normal_pkg(PsLinklayCtrl pCtrl)
{
	linklay_set_datarecved(pCtrl->mac_type);
}


void linklay_proc_ctrl_pkg(PsLinklayCtrl *pCtrl)
{
	
}

void linklay_recv_process()
{
    uchar i;
    uchar checksum;
    PsLinkLayHead pHead;
       
    for (i = MacPlc; i < MacTypeEnd; i++) {
        if (linklay[i].recv_bytes == 0)//no recv data
            continue;
        
        pHead = &linklay[i].recv_frame.head;
        checksum = CalChecksum((uchar *)pHead, sizeof(sLinkLayHead) - sizeof(int8u));
		checksum += CalChecksum((uchar *)(pHead + 1), pHead->App_Len);
        linklay[i].recv_bytes -= sizeof(sLinkLayHead);//dec the head len
        if (checksum != pHead->checksum || linklay[i].recv_bytes != pHead->App_Len)
        {
            linklay_proc_error(&linklay[i]);
            return;    
        }

        if (pHead->mac_addr[0] != mac_addr[0] || pHead->mac_addr[1] != mac_addr[1] ||
            pHead->mac_addr[2] != mac_addr[2] || pHead->mac_addr[3] != mac_addr[3])
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
        //linklay_recv_data();
        linklay_recv_process();
    }
    linklay_send_process();
}

void linklay_setmac(uchar mactype)
{
    G_macType = mactype;    
}


int8u linklay_rx(int8u mac_type, Plinklay_Frame pFrame)
{
    if (mac_type == MacPlc)
        return plc_rx_bytes((uchar *)pFrame);
#ifdef W24G
    else if (mac_type == MacWireless_2_4G)
        return wireless_2_4G_rx_byte((uchar *)pFrame);
#endif
    return 0;
}

int8u mac_rx_bytes()
{
    int8u len, i,tlen=0;
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

int8u mac_tx_bytes(int8u mac_type, uchar *pdata, int8u num)
{
    uchar realsend;
    
    if (mac_type == MacPlc)
        realsend = plc_tx_bytes(pdata, num);
#ifdef W24G         
    else if (mac_type == MacWireless_2_4G)
        realsend = wireless_2_4G_tx_bytes(pdata, num);
#endif    
    return realsend;    
}