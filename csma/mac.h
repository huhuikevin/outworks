/**************************************************************************
* ��Ȩ������Copyright@2014 �Ϻ��������ɵ�·���޹�˾
* �ļ����ƣ�mac.h
* ����������MAC�㴫����Ƴ���		
* �����б�
* �ļ����ߣ�sundy  	
* �ļ��汾��1.0 			
* ������ڣ�2014-05-08			
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
#ifndef _MAC_H_
#define _MAC_H_

mac_addr _mac_addr;      //mac��ַ
//uint8_t _network_id;    //����ID

#define MSDU_MAX_LEN 59
typedef struct _mac_frame{
	uint8_t  len;// mac len, must be the first 
	mac_addr dst;
	uint8_t  data[MSDU_MAX_LEN];
}mac_frame_t;// 64 bytes

enum mac_tx_state
{
    tx_idle = 0,       //����
    tx_request = 1,       //Ҫ����
    tx_csma = 2,       // ��ͻ����
    tx_ok = 3
};

//MAC���仺����
struct mac_tx_buf
{
    mac_tx_state  stat;    //����㷢����������:1������,0������ 
    uint8_t  confirm;    //MACȷ�ϱ�־:0x55������1�������0ʧ��  
    uint8_t  listen;
//    uint8_t  sequence;   //֡���
    uint8_t  length;     //msdu����
//    uint8_t  count;      //�ش�����ͳ��
//    uint8_t  timeout;    //���ճ�ʱ��ѯ
//    uint16_t time;       //�����ʱ
//    uint8_t  acktimeout; //���ճ�ʱ��ѯ
//    uint16_t acktime;    //�����ʱ
//    uint8_t  ackflag;    //����ACK�ڼ����ݷ���ʹ��
    
    mac_frame_t mac_frame;
}section36 _mac_tx_buf;

//MAC CSMA����
struct mac_csma
{
    uint8_t  back;      //�˱�ָ��
    uint8_t  slot;      //ʱ϶����
    uint16_t  timeout;    //�ش�ʱ��
    //uint16_t time;      //�ش�ʱ��
}section36 _mac_csma;

//MAC���ջ�����
struct mac_rx_buf
{
    uint8_t  indication;    //MAC���������ָʾ1������0������
    uint8_t  length;      //msdu����
   // uint16_t droped;
    uint8_t  rssiv;      //mac_frame ��Ӧ��rssiv�Ĵ�С
    mac_frame_t mac_frame;     //MAC���������
}section37 _mac_rx_buf;

/**************************************************************************
* �������ƣ�plc_mac_proc
* ����������mac��������
* �����������
* ���ز�������
**************************************************************************/
void plc_mac_proc(void);

/**************************************************************************
* �������ƣ�plc_mac_tx
* ����������mac�㷢�����ݺ���
* ���������dest     Ŀ�ĵ�ַ
            data     ����
            length   ���ݳ���
* ���ز�������
**************************************************************************/
void plc_mac_tx(uint8_t dest, uint8_t *data, uint8_t length);

/**************************************************************************
* �������ƣ�plc_mac_init
* ����������mac���ʼ��
* ���������mac     ����MAC��ַ
            net     ����ID
            back    �˱ܳ�ʼֵ
            slot    �˱�ʱ϶
* ���ز�������
**************************************************************************/
void plc_mac_init(uint8_t mac, uint8_t net, uint8_t back, uint8_t slot);

uint8_t plc_mac_rx_with_rssi(uint8_t *pdata, uint8_t *prssiv);

uint8_t plc_mac_rx_with_rssi(uint8_t *pdata);

#endif

