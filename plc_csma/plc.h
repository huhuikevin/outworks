/**************************************************************************
* 版权声明：Copyright@2014 上海海尔集成电路有限公司
* 文件名称：plc.h
* 功能描述：plc物理层传输控制函数声明	
* 函数列表：plc_write_reg
            plc_read_reg
            plc_init
            plc_tx_en
            plc_rx_en
            plc_idle_en
            plc_tx1_pn21
            plc_tx0_pn21
            plc_send_proc
            plc_tx_nextbit
            plc_data_send
            plc_rx_1bit
            plc_bit_sum
            plc_bit_sync
            plc_frame_sync
            plc_frame_recv
            plc_recv_proc
            plc_crc_rx
            plc_crc_tx
* 文件作者：sundy  	
* 文件版本：1.0			
* 完成日期：2014-4-24 			
* 修订历史：
* 修订日期：
**************************************************************************/
#ifndef _PLC_H_
#define _PLC_H_
#include "type.h"

//-------------------------------------------------------------------------
// 定义3040 PLC部分特殊功能寄存器                         
#define  PLC_00H_LNA1T0	     0x00    //测试预留
#define	PLC_01H_LNA1T1      0x01    //测试预留                                   
#define	PLC_02H_LNA1T2      0x02    //测试预留
#define	PLC_03H_LNA2T0      0x03    //测试预留
#define	PLC_04H_LNA2T1      0x04    //测试预留
#define	PLC_05H_LNA2T2	     0x05    //测试预留
#define	PLC_06H_LPFT        0x06    //测试预留
#define	PLC_07H_PGA1T0  	  0x07    //测试预留
#define	PLC_08H_PGA1T1	     0x08    //测试预留
#define	PLC_09H_PGA1T2	     0x09    //测试预留
#define	PLC_0AH_PGA2T0	     0x0A    //测试预留
#define	PLC_0BH_PGA2T1	     0x0B    //测试预留
#define	PLC_0CH_PGA2T2	     0x0C    //测试预留
#define	PLC_0DH_ADCT0	     0x0D    //测试预留
#define	PLC_0EH_ADCT1	     0x0E    //测试预留
#define	PLC_0FH_ADCT2	     0x0F    //测试预留


#define	PLC_10H_ADCT3	     0x10    //测试预留
#define	PLC_11H_DACT0	     0x11    //测试预留
#define	PLC_12H_PAMPT0	     0x12    //发送功率控制寄存器	
#define	PLC_13H_PDT0	     0x13    //测试预留
#define	PLC_14H_PLCT0	     0x14    //测试预留
#define	PLC_15H_ADC_D0	     0x15    //测试预留
#define	PLC_16H_ADC_D1	     0x16    //测试预留
#define	PLC_17H_ADC_DIN	  0x17    //测试预留
#define	PLC_18H_RESERVED	  0x18    //保留
#define	PLC_19H_RESERVED	  0x19    //保留
#define	PLC_1AH_RESERVED	  0x1A    //保留
#define	PLC_1BH_RESERVED	  0x1B    //保留
#define	PLC_1CH_RESERVED	  0x1C    //保留
#define	PLC_1DH_RESERVED	  0x1D    //保留
#define	PLC_1EH_RESERVED	  0x1E    //保留
#define	PLC_1FH_RESERVED	  0x1F    //保留


#define	PLC_20H_AGCP61LNA   0x20    //PLC发送频率字寄存器0
#define	PLC_21H_AGCP55LNA   0x21    //PLC发送频率字寄存器1
#define	PLC_22H_AGCP49LNA   0x22    //PLC发送频率字寄存器2
#define	PLC_23H_AGCP43LNA   0x23    //PLC发送频率字寄存器3
#define	PLC_24H_AGCP37LNA   0x24    //PLC接收频率字寄存器0
#define	PLC_25H_AGCP31LNA   0x25    //PLC接收频率字寄存器1
#define	PLC_26H_AGCP25LNA   0x26    //增益显示寄存器
#define	PLC_27H_AGCP19LNA   0x27    //PLC RSSI显示寄存器
#define	PLC_28H_AGCP13LNA   0x28    //PLC CSMA设置寄存器
#define	PLC_29H_AGCP07LNA   0x29    //PLC CSMA计算时间寄存器
#define	PLC_2AH_AGCP01LNA   0x2A    //PLC AFE控制寄存器
#define	PLC_2BH_AGCP05LNA   0x2B    //测试预留
#define	PLC_2CH_AGCP11LNA   0x2C    //测试预留
#define	PLC_2DH_AGCP17LNA   0x2D    //测试预留
#define	PLC_2EH_AGCP23LNA   0x2E    //测试预留
#define	PLC_2FH_RESERVED    0x2F    //保留


#define	PLC_30H_AGCN61PGA   0x30    //PLC发送频率字寄存器0
#define	PLC_31H_AGCN55PGA   0x31    //PLC发送频率字寄存器1
#define	PLC_32H_AGCN49PGA   0x32    //PLC发送频率字寄存器2
#define	PLC_33H_AGCN43PGA   0x33    //PLC发送频率字寄存器3
#define	PLC_34H_AGCN37PGA   0x34    //PLC接收频率字寄存器0
#define	PLC_35H_AGCN31PGA   0x35    //PLC接收频率字寄存器1
#define	PLC_36H_AGCN25PGA   0x36    //增益现实寄存器
#define	PLC_37H_AGCN19PGA   0x37    //PLC RSSI显示寄存器
#define	PLC_38H_AGCN13PGA   0x38    //PLC CSMA设置寄存器
#define	PLC_39H_AGCN07PGA   0x39    //PLC CSMA计算时间寄存器
#define	PLC_3AH_AGCN01PGA   0x3A    //PLC AFE控制寄存器
#define	PLC_3BH_AGCN05PGA   0x3B    //测试预留
#define	PLC_3CH_AGCN11PGA   0x3C    //测试预留
#define	PLC_3DH_AGCN17PGA   0x3D    //测试预留
#define	PLC_3EH_AGCN23PGA   0x3E    //测试预留
#define	PLC_3FH_RESERVED    0x3F    //保留


#define	PLC_40H_FREQ_TX0	  0x40    //发送FSK下变的频率字低8位
#define	PLC_41H_FREQ_TX1	  0x41    //发送FSK下变的频率字高6位
#define	PLC_42H_FREQ_TX2	  0x42    //发送FSK上变的频率字低8位
#define  PLC_43H_FREQ_TX3	  0x43    //发送FSK上变的频率字高6位
#define	PLC_44H_FREQ_RX0	  0x44    //接收频率字高8位寄存器
#define  PLC_45H_FREQ_RX1    0x45    //接收频率字低8位寄存器
#define 	PLC_46H_CSMAS	     0x46    //载波侦听能量值设置寄存器
#define 	PLC_47H_CSMAT	     0x47    //载波侦听时间控制和刷新模式选择寄存器  
#define 	PLC_48H_AFEC	     0x48    //模拟前端控制寄存器  
#define 	PLC_49H_IIRT	     0x49    //PSK环路控制设置  
#define 	PLC_4AH_DKG		     0x4a    //PSK内部环路增益控制寄存器  
#define 	PLC_4BH_DAG		     0x4b    //DA数字码输出增益设置寄存器  
#define 	PLC_4CH_MIXFWL	     0x4C    //混频器频率字低8位  
#define 	PLC_4DH_MIXFWH	     0x4D    //混频器频率字高8位 
#define 	PLC_4EH_DIGTEST	  0x4E    //测试预留 
#define 	PLC_4FH_DST	        0x4F    //测试预留 


#define 	PLC_50H_AGCGAIN	  0x50    //增益显示寄存器    
#define 	PLC_51H_RSSIV	     0x51    //PLC RSSI显示寄存器  
#define 	PLC_52H_RSSIT	     0x52    //PLC RSSI测试寄存器  
#define 	PLC_53H_RESERVED	  0x53    //保留     
#define 	PLC_54H_RESERVED	  0x54    //保留     
#define 	PLC_55H_RESERVED	  0x55    //保留     
#define 	PLC_56H_RESERVED	  0x56    //保留     
#define 	PLC_57H_RESERVED	  0x57    //保留     
#define 	PLC_58H_RESERVED	  0x58    //保留     
#define 	PLC_59H_RESERVED	  0x59    //保留     
#define 	PLC_5AH_RESERVED	  0x5A    //保留     
#define 	PLC_5BH_RESERVED	  0x5B    //保留     
#define 	PLC_5CH_RESERVED	  0x5C    //保留     
#define 	PLC_5DH_RESERVED	  0x5D    //保留     
#define 	PLC_5EH_RESERVED	  0x5E    //保留     
#define 	PLC_5FH_RESERVED	  0x5F    //保留  

#define  TX_BIT1()  PLC_MOD |= 0x01;
#define  TX_BIT0()  PLC_MOD &= 0xFE;

static volatile section64 sbit PLC_FSK_RXD @ (unsigned) &PLC_MOD* 8 + 3;

//状态设置
enum plc_state
{
    SEND = 0,       //发射态
    RECV = 1,       //接收态
    IDLE = 2,       //空闲态
}section32 _plc_state;

//侦听缓冲区
struct listen_buf
{
    uint8_t valid;      //发射状态判断:1有效,0无效    
    uint8_t  rssi;       //侦听信道rssi值
}section32 _listen_buf;

//发射缓冲区
struct send_buf
{
    uint8_t valid;      //发射状态判断:1有效,0无效
    uint8_t bitdata;    //当前发射比特位数据0/1
    uint8_t bitoff;     //bit偏移
    uint8_t byteoff;    //byte偏移
    uint8_t length;     //数据长度，最长64
    uint8_t data[68];   //数据缓冲区,用户数据最长64
}section32 _send_buf;

//帧同步缓冲区
struct frame_sync
{
    uint8_t  valid;       //同步状态判断:1同步完成,0未完成
    uint8_t  bitcnt;      //同步计数     
    uint16_t syncword;    //同步字缓冲
}section32 _frame_sync;

//接收缓冲区
struct recv_buf
{
    uint8_t valid;      //接收数据是否有效:1有效,0无效
    uint8_t bitcnt;     //接收比特计数 
    uint8_t length;     //数据长度，最长64
    uint8_t data[65];   //数据缓冲区，用户最长64
    uint8_t rssiv;
}section33 _recv_buf;

//码片采样缓冲区标志位
struct chip_flag
{         
    uint8_t bitdata;        //接收比特数据，最长8个
    uint8_t chipnum[62];   //统计chipdata每byte中1的个数
}section34 _chip_flag;


/**************************************************************************
* 函数名称：plc_write_reg
* 功能描述：plc间接寻址写操作
* 输入参数：addr 间接寻址地址
            data 数据
* 返回参数：无
**************************************************************************/
void plc_write_reg(uint8_t addr, uint8_t data);

/**************************************************************************
* 函数名称：plc_read_reg
* 功能描述：plc间接寻址读操作
* 输入参数：addr 间接寻址地址
* 返回参数：寄存器数据
**************************************************************************/
uint8_t plc_read_reg(uint8_t addr);

/**************************************************************************
* 函数名称：plc_init
* 功能描述：plc初始化
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_init(void);

/**************************************************************************
* 函数名称：plc_tx_en
* 功能描述：发射使能
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_tx_en(void);

/**************************************************************************
* 函数名称：plc_rx_en
* 功能描述：接收使能
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_rx_en(void);

/**************************************************************************
* 函数名称：plc_listen_en
* 功能描述：信道侦听
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_listen_en(void);

/**************************************************************************
* 函数名称：plc_idle_en
* 功能描述：接收空闲
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_idle_en(void);

/**************************************************************************
* 函数名称：plc_tx_preamble
* 功能描述：发射2位导频信号,每位50us
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_tx_preamble(void);

/**************************************************************************
* 函数名称：plc_tx1_pn15
* 功能描述：发射1比特数据1(010 1100 1000 1111)
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_tx1_pn15(void);

/**************************************************************************
* 函数名称：plc_tx0_pn15
* 功能描述：发射1比特数据0(101 0011 0111 0000)
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_tx0_pn15(void);

/**************************************************************************
* 函数名称：plc_send_proc
* 功能描述：发射流程
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_send_proc(void);

/**************************************************************************
* 函数名称：plc_tx_nextbit
* 功能描述：发射比特数据读取
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_tx_nextbit(void);

/**************************************************************************
* 函数名称：plc_tx_nextbit
* 功能描述：发射比特数据读取
* 输入参数：data   数据
            length 数据长度
* 返回参数：FAILED   数据长度错误
            SUCCEED  符合发射要求
**************************************************************************/
uint8_t plc_data_send(uint8_t *data, uint8_t length);

/**************************************************************************
* 函数名称：plc_rx_1bit
* 功能描述：接收1比特数据
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_rx_1bit(void);

/**************************************************************************
* 函数名称：plc_bit_sum
* 功能描述：扩频解扩
* 输入参数：无
* 返回参数：sum 接扩结果
**************************************************************************/
uint8_t plc_bit_sum(uint8_t *chipnum);

/**************************************************************************
* 函数名称：plc_bit_sync
* 功能描述：接收比特同步
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_bit_recv(void);

/**************************************************************************
* 函数名称：plc_frame_sync
* 功能描述：接收帧同步
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_frame_sync(void);

/**************************************************************************
* 函数名称：plc_frame_recv
* 功能描述：帧接收
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_frame_recv(void);

/**************************************************************************
* 函数名称：plc_recv_proc
* 功能描述：接收流程
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_recv_proc(void);

/**************************************************************************
* 函数名称：plc_listen_proc
* 功能描述：侦听流程
* 输入参数：无
* 返回参数：无
**************************************************************************/
void plc_listen_proc(void);

/**************************************************************************
* 函数名称：plc_crc_rx
* 功能描述：crc校验
* 输入参数：data     输入数据
            regval   前一字节校验结果,初始值取0xFFFF
* 返回参数：regval   校验结果
**************************************************************************/
uint16_t plc_crc_rx(uint8_t data, uint16_t regval);

/**************************************************************************
* 函数名称：plc_crc_tx
* 功能描述：crc校验
* 输入参数：data     输入数据
            regval   前一字节校验结果,初始值取0xFFFF
* 返回参数：regval   校验结果
**************************************************************************/
uint16_t plc_crc_tx(uint8_t data, uint16_t regval);

#endif
