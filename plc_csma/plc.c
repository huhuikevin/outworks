/**************************************************************************
* 版权声明：Copyright@2014 上海海尔集成电路有限公司
* 文件名称：plc.c
* 功能描述：plc物理层传输控制函数		
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
#include <hic.h>
#include "type.h"
#include "config.h"
#include "system.h"
#include "plc.h"
#include "timer16n.h"
#include "tool.h"

#define tx_pa_enable(en) \
do { \
	if (en) \
		CONFIG_IO_PA_ENABLE = 1; \
	else \
		CONFIG_IO_PA_ENABLE = 0; \
}while(0);
/**************************************************************************
* 函数名称：plc_write_reg
* 功能描述：plc间接寻址写操作
* 输入参数：addr 间接寻址地址
            data 数据
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_write_reg(uint8_t addr, uint8_t data)
{
    PLC_RW = 0x02;      //写使能
	
    PLC_ADDR = addr;    //间接寻址
    PLC_DATA = data;	   //赋值地址
      
    PLC_RW = 0x00;      //读写禁止
}

/**************************************************************************
* 函数名称：plc_read_reg
* 功能描述：plc间接寻址读操作
* 输入参数：addr 间接寻址地址
* 返回参数：寄存器数据
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
uint8_t plc_read_reg(uint8_t addr)
{
    uint8_t data;
	
    PLC_RW = 0x01;      //读使能
      
    PLC_ADDR = addr;    //赋值地址
    data = PLC_DATA;    //读数据
      
    PLC_RW = 0x00;      //读写禁止
      
    return data;                                                        
}

/**************************************************************************
* 函数名称：plc_init
* 功能描述：plc初始化
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_init(void)
{
    plc_write_reg(PLC_00H_LNA1T0,0x43);    
    plc_write_reg(PLC_01H_LNA1T1,0x44);

    
    plc_write_reg(PLC_02H_LNA1T2,0x41);    
    plc_write_reg(PLC_03H_LNA2T0,0x38);   
    plc_write_reg(PLC_04H_LNA2T1,0x88);    
    plc_write_reg(PLC_05H_LNA2T2,0x04);     

    plc_write_reg(PLC_06H_LPFT,0xB1);   // deadbeef
    
    plc_write_reg(PLC_07H_PGA1T0,0x45);     
    plc_write_reg(PLC_08H_PGA1T1,0x18);    
    plc_write_reg(PLC_09H_PGA1T2,0x88);     
    plc_write_reg(PLC_0AH_PGA2T0,0x1F);    
    plc_write_reg(PLC_0BH_PGA2T1,0x88);    
    plc_write_reg(PLC_0CH_PGA2T2,0x01);    
    plc_write_reg(PLC_0DH_ADCT0,0x80);     
    plc_write_reg(PLC_0EH_ADCT1,0x88);     
    plc_write_reg(PLC_0FH_ADCT2,0x88);      
    plc_write_reg(PLC_10H_ADCT3,0x88);     
    
    plc_write_reg(PLC_11H_DACT0,0x13);  //DA输出功率控制寄存器
    plc_write_reg(PLC_12H_PAMPT0,0x03); //发射功率设置9.8dB
    
    plc_write_reg(PLC_13H_PDT0,0x00);       
    plc_write_reg(PLC_14H_PLCT0,0x02);     
    plc_write_reg(PLC_15H_ADC_D0,0x00);    
    plc_write_reg(PLC_16H_ADC_D1,0x00);     
    plc_write_reg(PLC_17H_ADC_DIN,0x00);    
    
    plc_write_reg(PLC_20H_AGCP61LNA,0x33);  // deadbeef 
    plc_write_reg(PLC_21H_AGCP55LNA,0x33); 
    plc_write_reg(PLC_22H_AGCP49LNA,0x33);  
    plc_write_reg(PLC_23H_AGCP43LNA,0x33);  
    plc_write_reg(PLC_24H_AGCP37LNA,0x33);  
    plc_write_reg(PLC_25H_AGCP31LNA,0x33);                               
    plc_write_reg(PLC_26H_AGCP25LNA,0x33);                               
    plc_write_reg(PLC_27H_AGCP19LNA,0x33);                                
    plc_write_reg(PLC_28H_AGCP13LNA,0x33);                                 
    plc_write_reg(PLC_29H_AGCP07LNA,0x33);                                
    plc_write_reg(PLC_2AH_AGCP01LNA,0x33);                                 
    plc_write_reg(PLC_2BH_AGCP05LNA,0x33);                                
    plc_write_reg(PLC_2CH_AGCP11LNA,0x33);                                
    plc_write_reg(PLC_2DH_AGCP17LNA,0x33);                                 
    plc_write_reg(PLC_2EH_AGCP23LNA,0x33);    
    
    plc_write_reg(PLC_30H_AGCN61PGA,0x55);  // deadbeef                               
    plc_write_reg(PLC_31H_AGCN55PGA,0x55);                                 
    plc_write_reg(PLC_32H_AGCN49PGA,0x55);                                 
    plc_write_reg(PLC_33H_AGCN43PGA,0x55);                                 
    plc_write_reg(PLC_34H_AGCN37PGA,0x55);                                 
    plc_write_reg(PLC_35H_AGCN31PGA,0x55);                                 
    plc_write_reg(PLC_36H_AGCN25PGA,0x55);                                 
    plc_write_reg(PLC_37H_AGCN19PGA,0x55);                                 
    plc_write_reg(PLC_38H_AGCN13PGA,0x55);                                 
    plc_write_reg(PLC_39H_AGCN07PGA,0x55);                                 
    plc_write_reg(PLC_3AH_AGCN01PGA,0x55);                                 
    plc_write_reg(PLC_3BH_AGCN05PGA,0x55);                                 
    plc_write_reg(PLC_3CH_AGCN11PGA,0x55);                                 
    plc_write_reg(PLC_3DH_AGCN17PGA,0x55);                                 
    plc_write_reg(PLC_3EH_AGCN23PGA,0x55);   
    
    plc_write_reg(PLC_40H_FREQ_TX0,0x06);   //发送PSK的频率F0 = 131.5KHZ
    plc_write_reg(PLC_41H_FREQ_TX1,0x02);                                        
    plc_write_reg(PLC_42H_FREQ_TX2,0x30);                                      
    plc_write_reg(PLC_43H_FREQ_TX3,0x02);  
    
    plc_write_reg(PLC_44H_FREQ_RX0,0x1B);   //接收PSK频率F0 = 131.5KHZ
    plc_write_reg(PLC_45H_FREQ_RX1,0x02);      
    
    plc_write_reg(PLC_46H_CSMAS,0xC8); //载波侦听能量值设置寄存器（-56dB）
    plc_write_reg(PLC_47H_CSMAT,0xC9); //载波侦听时间控制和刷新模式    
    
    plc_write_reg(PLC_48H_AFEC,0x30); //模拟前端增益控制寄存器（增益6dB）  
	 plc_write_reg(PLC_49H_IIRT,0x06); //环路滤波器 
    plc_write_reg(PLC_4AH_DKG,0x0B);  //环路增益控制寄存器(5-0):11dB
    plc_write_reg(PLC_4BH_DAG,0x1F);  //D/A数字码输出增益控制 
	 plc_write_reg(PLC_4CH_MIXFWL,0x00);         
	 plc_write_reg(PLC_4DH_MIXFWH,0x80); 
    plc_write_reg(PLC_4EH_DIGTEST,0x04);                                   
    plc_write_reg(PLC_4FH_DST,0x00);                                        
    plc_write_reg(PLC_50H_AGCGAIN,0x00); //AGC增益   
        
    _send_buf.valid = 0;  
    _send_buf.bitdata = 0; 
    _send_buf.bitoff = 0; 
    _send_buf.byteoff = 0; 
    _send_buf.length = 0;  
    
    _frame_sync.valid = 0;
    _frame_sync.bitcnt = 0;
    _frame_sync.syncword = 0;
    
    _recv_buf.valid = 0;
    _recv_buf.bitcnt = 0;
    _recv_buf.length = 0;
	       
}

/**************************************************************************
* 函数名称：plc_tx_en
* 功能描述：发射使能
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_tx_en(void)
{
#if 0
    _send_buf.valid = 0;  
    _send_buf.bitdata = 0; 
    _send_buf.bitoff = 0; 
    _send_buf.byteoff = 0; 
    _send_buf.length = 0;  
#endif         
    _listen_buf.valid = 0;
    _listen_buf.rssi = 0x81;
    
    plc_write_reg(PLC_06H_LPFT,0x31);   //deadbeef
    
    plc_write_reg(PLC_4BH_DAG,0x36);  //D/A数字码输出增益控制 
    plc_write_reg(PLC_11H_DACT0,0x32);  //DA输出功率控制寄存器
    plc_write_reg(PLC_12H_PAMPT0,0x23); //发射功率设置
    
    tx_pa_enable(1);                            //PA使能
}

/**************************************************************************
* 函数名称：plc_rx_en
* 功能描述：接收使能
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_rx_en(void)
{
    uint8_t i;
    
    tx_pa_enable(0);   //PA禁能
    
    plc_write_reg(PLC_06H_LPFT,0xB1);   // deadbeef
    plc_write_reg(PLC_11H_DACT0,0x13);  //DA输出功率控制寄存器
    plc_write_reg(PLC_12H_PAMPT0,0x03); //发射功率设置
    
    for (i = 0; i < 62; i++) {     //清空缓冲区
        _chip_flag.chipnum[i] = 0;
     } 
     
    _frame_sync.valid = 0;
    _frame_sync.bitcnt = 0;
    _frame_sync.syncword = 0;
    
    _recv_buf.valid = 0;
    _recv_buf.bitcnt = 0;
    _recv_buf.length = 0;
    _recv_buf.length = 0;

    _listen_buf.valid = 0;
    _listen_buf.rssi = 0x81;
        
    _plc_state = RECV;
}

/**************************************************************************
* 函数名称：plc_idle_en
* 功能描述：接收空闲
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_idle_en(void)
{    
    PLC_MOD = 0x00;     //PLC禁能
               
  
    _plc_state = IDLE;
}

/**************************************************************************
* 函数名称：plc_tx_preamble
* 功能描述：发射2位导频信号，每位50us
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_tx_preamble(void)
{
                  
    TX_BIT0();
    DELAY100NOP(); 
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
    DELAY90NOP();
    DELAY3NOP();
    TX_BIT0();
    DELAY100NOP(); 
    DELAY100NOP();
    DELAY100NOP();
    DELAY70NOP();
    DELAY1NOP();

}

/**************************************************************************
* 函数名称：plc_tx1_pn15
* 功能描述：发射1比特数据1(010 1100 1000 1111)
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_tx1_pn15(void)
{
    uint8_t i, j;
    uint16_t pn15 = 0b0101100100011110; //低位补零，但不发射
   
    //扩频发送时间间隔50us，即500个NOP
    for (i = 0; i < 14; i++) {
        if ((pn15 & 0x8000) != 0) {
            DELAY1NOP();  //if/esle时间相等
            
            DELAY4NOP(); //增加循环NOP确保最后一位码片时间充足
            TX_BIT1();   
        }
        else {       
            DELAY4NOP(); //增加循环NOP确保最后一位码片时间充足
            TX_BIT0();  
            DELAY2NOP();  //if/esle时间相等
        }
        pn15 <<= 1; // 5nop
 
        //码片保证50us延时
        DELAY100NOP(); 
        DELAY100NOP();
        DELAY100NOP();
        DELAY100NOP();
        DELAY70NOP();
        DELAY8NOP();
        
    }  
    
    //调整承接码片时间长度为50us
    DELAY3NOP();
    //后续码片需要上级程序调研补齐延时
    if ((pn15 & 0x8000) != 0) {
        DELAY1NOP();  //if/esle时间相等
        TX_BIT1(); 
    }
    else {
        TX_BIT0();
        DELAY2NOP();  //if/esle时间相等
    }
    
    //函数返回3个NOP  
}

/**************************************************************************
* 函数名称：plc_tx0_pn15
* 功能描述：发射1比特数据0(101 0011 0111 0000)
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_tx0_pn15(void)
{ 
    uint8_t i, j;
    uint16_t pn15 = 0b1010011011100000; //低位补零，但不发射
   
    //扩频发送时间间隔50us，即500个NOP
    for (i = 0; i < 14; i++) {
        if ((pn15 & 0x8000) != 0) {
            DELAY1NOP();  //if/esle时间相等
            
            DELAY4NOP(); //增加循环NOP确保最后一位码片时间充足
            TX_BIT1();   
        }
        else {       
            DELAY4NOP(); //增加循环NOP确保最后一位码片时间充足
            TX_BIT0();  
            DELAY2NOP();  //if/esle时间相等
        }
        pn15 <<= 1;
 
        //码片保证50us延时
        DELAY100NOP(); 
        DELAY100NOP();
        DELAY100NOP();
        DELAY100NOP();
        DELAY70NOP();
        DELAY8NOP();
        
    }  
    
    //调整承接码片时间长度为50us
    DELAY3NOP();
    //后续码片需要上级程序调研补齐延时
    if ((pn15 & 0x8000) != 0) {
        DELAY1NOP();  //if/esle时间相等
        TX_BIT1(); 
    }
    else {
        TX_BIT0();
        DELAY2NOP();  //if/esle时间相等
    }
    
    //函数返回3个NOP  
}

/**************************************************************************
* 函数名称：plc_send_proc
* 功能描述：发射流程
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_send_proc(void)
{
    uint8_t i;
    
    if (_send_buf.valid) {
        PLC_MOD = 0x22;     //发射使能
        
        plc_tx_preamble();  //导频信号
        
        for (i = 0; i < 4; i++) {
            plc_tx_nextbit();  
            if (_send_buf.bitdata) {       
                DELAY1NOP();  //if/esle时间相等
                plc_tx1_pn15();
            }
            else
            {
                plc_tx0_pn15();
                DELAY2NOP();  //if/esle时间相等
            }
            
            //调整承接码片时间长度为50us
            DELAY100NOP();
            DELAY100NOP();
            DELAY100NOP();
            DELAY60NOP();
            DELAY4NOP();
        }
        //调整承接码片时间长度为50us
        DELAY100NOP();
        DELAY10NOP();
        DELAY8NOP();
        PLC_MOD = 0x00; //发射禁能
        
        if (!_send_buf.valid) { //发射结束转接收 
            tx_pa_enable(0);   //PA禁能
                            
            PLC_RW = 0x02;      //写使能
            PLC_ADDR = PLC_06H_LPFT;      //间接寻址
            PLC_DATA = 0xB1;	            //赋值地址
            PLC_ADDR = PLC_11H_DACT0;      //间接寻址
            PLC_DATA = 0x13;	            //赋值地址   
            PLC_ADDR = PLC_12H_PAMPT0;      //间接寻址
            PLC_DATA = 0x03;	            //赋值地址
            PLC_RW = 0x00;      //读写禁止
                
            for (i = 0; i < 62; i++) {     //清空缓冲区
                _chip_flag.chipnum[i] = 0;
            } 
     
            _frame_sync.valid = 0;
            _frame_sync.bitcnt = 0;
            _frame_sync.syncword = 0;
    
            _recv_buf.valid = 0;
            _recv_buf.bitcnt = 0;
            _recv_buf.length = 0;
            _recv_buf.length = 0;

            _listen_buf.valid = 0;
            _listen_buf.rssi = 0x81;
        
            _plc_state = RECV;
        }     
    }    
}

/**************************************************************************
* 函数名称：plc_tx_nextbit
* 功能描述：发射比特数据读取
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_tx_nextbit(void) 
{
    _send_buf.bitdata = _send_buf.data[_send_buf.byteoff] & 0x80; // 22nop
    _send_buf.data[_send_buf.byteoff] <<= 1; // 26 nop
    _send_buf.bitoff++; // 8nop
    
    if (_send_buf.bitoff > 7) {   // 3nop // 2 nop      
        _send_buf.bitoff = 0;    // 2nop
        _send_buf.byteoff++; // 7nop
    }
    else {
        DELAY10NOP();
    }
         
    if (_send_buf.byteoff == _send_buf.length) { // 3nop 2nop
        _send_buf.valid = 0;        //清空发射缓冲区
        _send_buf.bitoff = 0; 
        _send_buf.byteoff = 0; 
        _send_buf.length = 0;    
    }
    else {
        DELAY9NOP();
    }
}

/**************************************************************************
* 函数名称：plc_data_send
* 功能描述：发射比特数据读取
* 输入参数：data   数据
            length 数据长度
* 返回参数：FAILED   数据长度错误
            SUCCEED  符合发射要求
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
uint8_t plc_data_send(uint8_t *data, uint8_t length)
{
    uint8_t i;
    uint16_t crcval;
    
    if ((length > 63) || (length == 0)) {
        return 0;  //数据长度错误
    }
    //如果正在发送或已经接收到同步信号
    if (_plc_state == SEND || _frame_sync.valid == 1)
	 return 0;
    MMemcpy(&_send_buf.data[2], data, length);
    crcval = plc_crc_tx(data[0], 0xFFFF);
    for (i = 1; i < length; i++) {
        crcval = plc_crc_tx(data[i], crcval);
    }
    
    _send_buf.data[0] = 0xD3;               //帧同步信号
    _send_buf.data[1] = 0x91;               //帧同步信号
    _send_buf.data[2+length] = crcval/256;  //CRC校验结果
    _send_buf.data[3+length] = crcval%256;  //CRC校验结果
    _send_buf.valid = 1;                    //配置发射使能
    _send_buf.bitdata = 0; 
    _send_buf.bitoff = 0; 
    _send_buf.byteoff = 0; 
    _send_buf.length = length+4;   

	plc_tx_en();
    _plc_state = SEND;  // 开始发送数据
    
    return length;
}

/**************************************************************************
* 函数名称：plc_rx_1bit
* 功能描述：接收1比特数据
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_rx_1bit(void)
{
    uint8_t i;
    
    //延时用于比特采样同步，约1320NOP
 #if 0   
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
    DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
    DELAY100NOP();
	DELAY100NOP();
    DELAY20NOP();
 #else
	DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
	DELAY100NOP();
	DELAY10NOP();
 #endif
     //do {
     for (i = 0; i < 62; i++) { 
        if (PLC_FSK_RXD) {   // 3nop               //sample 0
            DELAY1NOP();    //if/else 对齐
            
			_chip_flag.chipnum[i] = 1;// 16nops
			DELAY7NOP();
        }
        else {
			_chip_flag.chipnum[i] = 0;// 16nops
            DELAY9NOP();
        }
        
        //采样间隔62NOP
        DELAY30NOP();
        DELAY4NOP();

        if (PLC_FSK_RXD) {
            DELAY1NOP();    //if/else 对齐
           
            _chip_flag.chipnum[i]++;// 23
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else 对齐
        }
        
        //采样间隔63NOP
        DELAY30NOP();
        DELAY5NOP();
                       
        if (PLC_FSK_RXD) {                  //sample 2   
            DELAY1NOP();    //if/else 对齐
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else 对齐
        }
        
        //采样间隔62NOP
        DELAY30NOP();
        DELAY4NOP();
                
        if (PLC_FSK_RXD) {                  //sample 3
            DELAY1NOP();    //if/else 对齐
            
            _chip_flag.chipnum[i]++;
        }
        else {
        DELAY20NOP();
        DELAY3NOP();
            
            DELAY2NOP();    //if/else 对齐
        }
        
        //采样间隔63NOP
        DELAY30NOP();
        DELAY5NOP();
                  
        if (PLC_FSK_RXD) {                  //sample 4
            DELAY1NOP();    //if/else 对齐
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else 对齐
        }
        
        //采样间隔62NOP
        DELAY30NOP();
        DELAY4NOP();
                 
        if (PLC_FSK_RXD) {                   //sample 5
            DELAY1NOP();    //if/else 对齐
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else 对齐
        }
        
        //采样间隔63NOP
        DELAY30NOP();
        DELAY5NOP();
              
        if (PLC_FSK_RXD) {                  //sample 6
            DELAY1NOP();    //if/else 对齐
           
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else 对齐
        }
        
        //采样间隔62NOP
        DELAY30NOP();
        DELAY4NOP();
                 
        if (PLC_FSK_RXD) {                  //sample 7
            DELAY1NOP();    //if/else 对齐
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else 对齐
        }
        
        //采样间隔63NOP 
        DELAY20NOP();
        DELAY8NOP();
    }
}

/**************************************************************************
* 函数名称：plc_bit_sum
* 功能描述：扩频解扩
* 输入参数：无
* 返回参数：sum 接扩结果
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
uint8_t plc_bit_sum(uint8_t *chipnum)
{
    uint8_t sum, sum_x, sum_y;

    sum_x = chipnum[0]; 
    sum_x += chipnum[2];     
    sum_x += chipnum[5]; 
    sum_x += chipnum[6];
    sum_x += chipnum[8];
    sum_x += chipnum[9];
    sum_x += chipnum[10];

    sum_y = chipnum[1]; 
    sum_y += chipnum[3];
    sum_y += chipnum[4];
    sum_y += chipnum[7];
    sum_y += chipnum[11];
    sum_y += chipnum[12];
    sum_y += chipnum[13];
    sum_y += chipnum[14];
    
    sum = sum_x + 64- sum_y;
    
    return sum;
}


/**************************************************************************
* 函数名称：plc_bit_recv
* 功能描述：比特数据判决
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_bit_recv(void)
{
    uint8_t i;
    uint8_t sum;
    
    for (i = 0; i < 4; i++) {
        _chip_flag.bitdata <<= 1;
        
        sum =  plc_bit_sum(&_chip_flag.chipnum[15*i+2]);
        if (sum > 60) {
            _chip_flag.bitdata |= 0x01;    
        }
        else {
            _chip_flag.bitdata &= 0xFE; 
        }
    }
    
    //for (i = 0; i < 62; i++) {     //清空缓冲区
        //_chip_flag.chipnum[i] = 0;
    //} 
}

/**************************************************************************
* 函数名称：plc_frame_sync
* 功能描述：接收帧同步
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_frame_sync(void)
{
    _frame_sync.bitcnt++;
    
    _frame_sync.syncword <<= 4;
    _frame_sync.syncword |= _chip_flag.bitdata;

#if 0
    if (_frame_sync.bitcnt >= 3) {  //保证一个sys_tick接收指示有效     
        _recv_buf.valid = 0;        //超过则丢弃，开始接收下一帧  
    }
#endif    
    if (_frame_sync.bitcnt >= 4) {
        _frame_sync.bitcnt = 4;
        if (_frame_sync.syncword == 0xD391) {
            _frame_sync.valid = 1;  //帧同步   
            _frame_sync.bitcnt = 0;
        }
    }
}

/**************************************************************************
* 函数名称：plc_frame_recv
* 功能描述：帧接收
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_frame_recv(void)
{
    uint8_t i;
    uint16_t crcval;
    
    _recv_buf.bitcnt++;
    _recv_buf.data[_recv_buf.length] <<=4;
    _recv_buf.data[_recv_buf.length] |= _chip_flag.bitdata;
        
    if (_recv_buf.bitcnt >= 2) {
        _recv_buf.length++;     //帧长度统计    
        _recv_buf.bitcnt = 0;
        
        if (_recv_buf.length == (_recv_buf.data[0] + 2) ) {
            //完成一帧接收
            crcval = plc_crc_rx(_recv_buf.data[0], 0xFFFF);
            for (i = 1; i < _recv_buf.data[0]; i++) {
                crcval = plc_crc_rx(_recv_buf.data[i], crcval);
            }
    
            if (crcval == _recv_buf.data[_recv_buf.data[0]]*256
                        + _recv_buf.data[_recv_buf.data[0] + 1]) {           
                _recv_buf.valid = 1;  
                _recv_buf.rssiv = _listen_buf.rssi;
            }
            else {
                _recv_buf.valid = 0xAA; //CRC校验错误    
            }
            
            _recv_buf.bitcnt = 0;
            _recv_buf.length = 0; 
         
            _frame_sync.valid = 0;
            _frame_sync.bitcnt = 0;
            _frame_sync.syncword = 0;
        }
        else if (_recv_buf.length > 65) {
            _recv_buf.valid = 0x55;     //接收数据长度错误
            _recv_buf.bitcnt = 0;
            _recv_buf.length = 0; 
            
            _frame_sync.valid = 0;
            _frame_sync.bitcnt = 0;
            _frame_sync.syncword = 0;
        }
    }
}

/**************************************************************************
* 函数名称：plc_recv_proc
* 功能描述：接收流程
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void plc_recv_proc(void)
{
    PLC_MOD = 0x59;     //FSK接收使能
    plc_rx_1bit();      //比特接收
    _listen_buf.valid = 1;
    _listen_buf.rssi = plc_read_reg(PLC_51H_RSSIV);
    PLC_MOD = 0x00;     //接收禁能
        
    plc_bit_recv();     //比特同步
    if (_frame_sync.valid == 0) {
        plc_frame_sync();   //帧同步   
    }
    else if (_recv_buf.valid == 0) {
        plc_frame_recv();   //接收一帧数据   
    }
    else {        
        _frame_sync.valid = 0;
        _frame_sync.bitcnt = 0;
        _frame_sync.syncword = 0;
    
        _recv_buf.bitcnt = 0;
        _recv_buf.length = 0;        
    }

}
    
/**************************************************************************
* 函数名称：plc_crc_rx
* 功能描述：crc校验
* 输入参数：data     输入数据
            regval   前一字节校验结果,初始值取0xFFFF
* 返回参数：regval   校验结果
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
uint16_t plc_crc_rx(uint8_t data, uint16_t regval)
{ 
    uint8_t i;
 
    for (i = 0; i < 8; i++) 
    { 
        if (((regval & 0x8000) >> 8) ^ (data & 0x80) ) 
            regval = (regval << 1) ^ 0x8005; 
        else 
            regval = (regval << 1); 
        
        data <<= 1; 
    } 
    
    return regval; 
}

/**************************************************************************
* 函数名称：plc_crc_tx
* 功能描述：crc校验
* 输入参数：data     输入数据
            regval   前一字节校验结果,初始值取0xFFFF
* 返回参数：regval   校验结果
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
uint16_t plc_crc_tx(uint8_t data, uint16_t regval)
{ 
    uint8_t i;
 
    for (i = 0; i < 8; i++) 
    { 
        if (((regval & 0x8000) >> 8) ^ (data & 0x80) ) 
            regval = (regval << 1) ^ 0x8005; 
        else 
            regval = (regval << 1); 
        
        data <<= 1; 
    } 
    
    return regval; 
}

/**************************************************************************
* 函数名称：isr
* 功能描述：中断入口函数
* 输入参数：无
* 返回参数：无
* 函数作者：
* 完成日期：
* 修订历史：
* 修订日期：
**************************************************************************/
void isr(void) interrupt 
{	
	 if (T16G1IF && T16G1IE) {
		 _sys_tick++;
		 
		 _last_time = _now_time;			 //记录上一次过零时刻
		 _now_time = T16G1RH*256 | T16G1RL;  //记录当前过零时刻
		 
		 if (_now_time > _last_time) {		  
			 _half_time = _now_time - _last_time; 
		 }
		 else
		 {
			 _half_time = 0xFFFF - _last_time;
			 _half_time += _now_time + 1;
		 }		  
		 
		 init_t16g2(0);
		 _t16g1_valid = 1;
		  T16G1IF = 0;	

    }

    if (T16G2IF && T16G2IE) {       //1   
        T16G2IE = 0; 
	    T16G2IF = 0;        
        T16G1IF = 0;
        
        switch (_plc_state) {        
        case SEND: {    
           DELAY3NOP();  //与REC对齐
           if (_t16g1_valid) {          
                init_t16g2(1);
                _t16g1_valid = 0;
            }else {
            	DELAY100NOP();
                DELAY5NOP();
                DELAY2NOP();    //if/else 对齐
            }
            plc_send_proc();    
            break;            
            }            
        case RECV: {
            if (_t16g1_valid) {          
                init_t16g2(1);
                _t16g1_valid = 0;
            }else {
            	DELAY100NOP();
                DELAY5NOP();
                DELAY2NOP();    //if/else 对齐
            }
            plc_recv_proc();
            DELAY1NOP();
            break;
            }
        case IDLE: {
            NOP();
            break;
            }
        default:
            NOP();
            break;        
        }
    }

}





