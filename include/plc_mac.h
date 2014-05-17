#ifndef __PLC_MAC_H_
#define __PLC_MAC_H_

#include "type.h"

#define MSDU_MAX_LEN 59

typedef struct {
    uint8_t  len;// mac len, must be the first 
    mac_addr dst;
    uint8_t  data[MSDU_MAX_LEN];
}mac_frame_t;// 64 bytes


enum mac_tx_state
{
    tx_idle = 0,       //空闲
    tx_request = 1,       //要发送
    tx_csma = 2,       // 冲突避让
    tx_ok = 3,
};


//MAC发射缓冲区
struct mac_tx_buf
{
    uint8_t  stat;    //网络层发送数据请求:1有请求,0无请求 
    uint8_t  confirm;    //MAC确认标志:0x55发射中1发射完成0失败  
    uint8_t  listen;
//    uint8_t  sequence;   //帧序号
    uint8_t  length;     //msdu长度
//    uint8_t  count;      //重传次数统计
//    uint8_t  timeout;    //接收超时查询
//    uint16_t time;       //发射计时
//    uint8_t  acktimeout; //接收超时查询
//    uint16_t acktime;    //发射计时
//    uint8_t  ackflag;    //缓存ACK期间数据发射使能
    
    mac_frame_t mac_frame;
}section36 _mac_tx_buf;

//MAC CSMA参数
struct mac_csma
{
    uint8_t  back;      //退避指数
    uint8_t  slot;      //时隙长度
    uint16_t  timeout;    //重传时间
    //uint16_t time;      //重传时间
}section36 _mac_csma;

//MAC接收缓冲区
struct mac_rx_buf
{
    uint8_t  indication;    //MAC层接收数据指示1有数据0无数据
    uint8_t  length;      //msdu长度
   // uint16_t droped;
    uint8_t  rssiv;      //mac_frame 对应的rssiv的大小
    mac_frame_t mac_frame;     //MAC层接收数据
}section37 _mac_rx_buf;

/**************************************************************************
* 函数名称：plc_mac_proc
* 功能描述：mac处理流程
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_mac_proc(void);

/**************************************************************************
* 函数名称：plc_mac_tx
* 功能描述：mac层发射数据函数
* 输入参数：dest     目的地址
            data     数据
            length   数据长度
* 返回参数：无
**************************************************************************/
uint8_t plc_mac_tx(mac_addr *pdst, uint8_t *data, uint8_t length);

/**************************************************************************
* 函数名称：plc_mac_init
* 功能描述：mac层初始化
* 输入参数：mac     本机MAC地址
            net     网络ID
            back    退避初始值
            slot    退避时隙
* 返回参数：无
**************************************************************************/
void plc_mac_init(uint8_t back, uint8_t slot);

uint8_t plc_mac_rx_with_rssi(uint8_t *pdata, uint8_t *prssiv);

uint8_t plc_mac_rx(uint8_t *pdata);

#endif

