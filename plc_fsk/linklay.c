//plc��2.4G ��������·��,֧�ְַ����ط��Ȼ���
//byte 0-3:macaddr byte 0-3
//byte 4:  version bit7-6, pkg_type(��ͨ�Ϳ�����) bit 5�� len 4-0
//byte 5:  bit 7-4 trans sequnce , bit 3 acknck��pkg_type=1��ʱ��1:ack��0:nck��, bit 2-0 ??
//byte 6: checksum 
//byte 7-... data
/*
gateway      plc module   plcmodule    autodevice    autodevice    plcmodule  plcmodule     gateway
uartsend --> uartrecv --> plcsend ---> plcrecv--->op--->plcsend --->plcrecv--->uartsend---->uartrecv
*/

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

uchar G_macType;

typedef struct
{
 	int8u mac_addr[MAC_ADDR_LEN];			//MAC��ַ
	int8u Version : 2;	//Э��汾��
	int8u Pkg_Type : 1;			//�����ͣ�0����ͨ����1��Ӧ���
	int8u App_Len : 5;			//���ݳ���
	int8u seq : 4;		//�������к�
	int8u anck : 1;				//Ӧ��
	int8u res : 3;		//����
	int8u checksum;	//У��   
}sLinkLayHead, *PsLinkLayHead;


typedef struct  
{
    sLinkLayHead head;
	int8u App_data[MAX_DATA_LEN];
}linklay_Frame, *Plinklay_Frame;

typedef struct
{
    int8u mac_type;
    int8u send_bytes;
    int8u recv_bytes;
    int8u send_seq;
    int8u link_statmachine; 
    linklay_Frame send_frame;
    linklay_Frame recv_frame;
}sLinklayCtrl, *PsLinklayCtrl;

sLinklayCtrl linklay[MacTypeEnd];

/* �ϲ������buf�������ݷ��� */
uchar *linklay_send_buf()
{
    return (&linklay[G_macType].send_frame.App_data[0]);    
}

/* �ϲ��Ѿ���������д�� linklay_send_buf ���� */
int8u linklay_send_data(int8u len)
{
    PsLinkLayHead pHead = &linklay[G_macType].send_frame.head;
    if (len > 32)
    {
        return 0;//send null     
    }
    linklay[G_macType].send_seq ++;
    pHead->mac_addr[0] = mac_addr[0];
    pHead->mac_addr[1] = mac_addr[1];
    pHead->mac_addr[2] = mac_addr[2];
    pHead->mac_addr[3] = mac_addr[3];
    
    pHead->Version = VERSION
    pHead->Pkt_Type = PKG_TYPE_NORMAL;
    pHead->App_Len = len;
    pHead->Seq = send_seq;
        
    pHead->checksum = CalChecksum((uchar *)pHead, len+sizeof(sLinkLayHead));
    
    mac_tx_bytes((uchar *)pHead, len+LINK_LAY_HEAD_LEN);
    
    return len;
}


void linklay_recv_data()
{
    linklay_recv_data_len = mac_rx_bytes();
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

void linklay_setmac(uchar mactype)
{
    G_macType = mactype;    
}


void mac_rx_bytes()
{
    plc_rx_byte(&linklay[MacPlc].send_frame);
    wireless_2_4G_rx_byte(&linklay[MacWireless_2_4G].send_frame);
}

uchar mac_tx_bytes(uchar *pdata, uchar num)
{
    uchar realsend;
    
    if (G_macType == MAC_TYPE_PLC)
        realsend = plc_tx_bytes(pdata, num);
    if (G_macType == MAC_TYPE_2_4G)
        realsend = wireless_2_4G_tx_bytes(pdata, num);
    
    return realsend;    
}