/**************************************************************************
* 版权声明：Copyright@2014 上海海尔集成电路有限公司
* 文件名称：mac.h
* 功能描述：MAC层传输控制程序		
* 函数列表：
* 文件作者：sundy  	
* 文件版本：1.0 			
* 完成日期：2014-05-08			
* 修订历史：
* 修订日期：
**************************************************************************/
#ifndef _MAC_H_
#define _MAC_H_

mac_addr _mac_addr;      //mac地址
//uint8_t _network_id;    //网络ID

#define MSDU_MAX_LEN 59
typedef struct _mac_frame{
	uint8_t  len;// mac len, must be the first 
	mac_addr dst;
	uint8_t  data[MSDU_MAX_LEN];
}mac_frame_t;// 64 bytes

//MAC发射缓冲区
struct mac_tx_buf
{
    uint8_t  request;    //网络层发送数据请求:1有请求,0无请求 
    uint8_t  confirm;    //MAC确认标志:0x55发射中1发射完成0失败  
    uint8_t  listen;
    uint8_t  sequence;   //帧序号
    uint8_t  length;     //msdu长度
    uint8_t  count;      //重传次数统计
    uint8_t  timeout;    //接收超时查询
    uint16_t time;       //发射计时
    uint8_t  acktimeout; //接收超时查询
    uint16_t acktime;    //发射计时
    uint8_t  ackflag;    //缓存ACK期间数据发射使能
    uint8_t  rssiv;      //mac_frame 对应的rssiv的大小
    mac_frame_t mac_frame;
}section36 _mac_tx_buf;

//MAC CSMA参数
struct mac_csma
{
    uint8_t  back;      //退避指数
    uint8_t  slot;      //时隙长度
    uint8_t  timeout;   //退避时隙判定
    uint16_t time;      //重传时间
}section36 _mac_csma;

//MAC接收缓冲区
struct mac_rx_buf
{
    uint8_t  indication;    //MAC层接收数据指示1有数据0无数据
    uint8_t  length;      //msdu长度
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
void plc_mac_tx(uint8_t dest, uint8_t *data, uint8_t length);

/**************************************************************************
* 函数名称：plc_mac_init
* 功能描述：mac层初始化
* 输入参数：mac     本机MAC地址
            net     网络ID
            back    退避初始值
            slot    退避时隙
* 返回参数：无
**************************************************************************/
void plc_mac_init(uint8_t mac, uint8_t net, uint8_t back, uint8_t slot);

#endif

