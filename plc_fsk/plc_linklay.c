//plc，2.4G 等数据链路层,支持分包，重发等机制
//byte 0-3:macaddr byte 0-3
//byte 4:  version bit7-6, pkg_type(普通和控制类) bit 5， len 4-0
//byte 5:  bit 7-4 trans sequnce , bit 3 acknck（pkg_type=1的时候，1:ack，0:nck）, bit 2-0 ??
//byte 6: checksum 
//byte 7-... data

#include <hic.h>
#include "type.h"
#include "tool.h"

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



uchar mac_addr[MAC_ADDR_LEN];

uchar send_seq = 0;

typedef struct  
{
	int8u mac_addr[MAC_ADDR_LEN];			//MAC地址
	int8u Version : 2;	//协议版本号
	int8u Pkg_Type : 1;			//包类型，0：普通包，1：应答包
	int8u App_Len : 5;			//数据长度
	int8u seq : 4;		//发送序列号
	int8u anck : 1;				//应答
	int8u res : 3;		//保留
	int8u checksum;	//校验
	int8u App_data[MAX_DATA_LEN];
}linklay_Frame, *Plinklay_Frame;

linklay_Frame linklay_send_frame;
linklay_Frame linklay_recv_frame;
uchar linklay_recv_data_len;

/* 上层拿这个buf进行数据发送 */
uchar *linklay_send_buf()
{
    return (&linklay_send_frame.App_data[0]);    
}

/* 上层已经把数据填写到 linklay_send_buf 中了 */
int8u linklay_send_data(int8u len)
{
    if (len > 32)
    {
        return 0;//send null     
    }
    send_seq ++;
    linklay_send_frame.mac_addr[0] = mac_addr[0];
    linklay_send_frame.mac_addr[1] = mac_addr[1];
    linklay_send_frame.mac_addr[2] = mac_addr[2];
    linklay_send_frame.mac_addr[3] = mac_addr[3];
    
    linklay_send_frame.Version = VERSION
    linklay_send_frame.Pkt_Type = PKG_TYPE_NORMAL;
    linklay_send_frame.App_Len = len;
    linklay_send_frame.Seq = send_seq;
        
    linklay_send_frame.checksum = CalChecksum(pdata, len+LINK_LAY_HEAD_LEN);
    
    plc_tx_bytes(linklay_send_buf, len+LINK_LAY_HEAD_LEN);
    
    return len;
}


void linklay_recv_data()
{
    linklay_recv_data_len = plc_rx_bytes((uchar *)&linklay_recv_frame);
    if (linklay_recv_data_len == 0)//no data recved
        return;
    
    uchar checksum = CalChecksum((uchar *)&linklay_recv_frame, len);  
    if (checksum != linklay_recv_frame.checksum)
    {
        linklay_proc_error();
        return;    
    }

    if (linklay_recv_frame.macaddr[0] != mac_addr[0] || linklay_recv_frame.macaddr[1] != mac_addr[1] ||
        linklay_recv_frame.macaddr[2] != mac_addr[2] || linklay_recv_frame.macaddr[3] != mac_addr[3])
    {
        linklay_porc_not_me();//may be need to route
        return;
    }
    if (linklay_recv_frame.Pkg_Type == PKG_TYPE_NORMAL){
        linklay_proc_normal_pkg();
        return;    
    }
    if (linklay_recv_frame.Pkg_Type == PKG_TYPE_CTRL){
        linklay_proc_ctrl_pkg();
        return;    
    }
}


void linklay_process()
{
    linklay_recv_data();
}