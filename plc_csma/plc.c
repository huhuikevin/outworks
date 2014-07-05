/**************************************************************************
* ��Ȩ������Copyright@2014 �Ϻ��������ɵ�·���޹�˾
* �ļ����ƣ�plc.c
* ����������plc����㴫����ƺ���		
* �����б�plc_write_reg
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
* �ļ����ߣ�sundy  	
* �ļ��汾��1.0			
* ������ڣ�2014-4-24 			
* �޶���ʷ��
* �޶����ڣ�
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
* �������ƣ�plc_write_reg
* ����������plc���Ѱַд����
* ���������addr ���Ѱַ��ַ
            data ����
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_write_reg(uint8_t addr, uint8_t data)
{
    PLC_RW = 0x02;      //дʹ��
	
    PLC_ADDR = addr;    //���Ѱַ
    PLC_DATA = data;	   //��ֵ��ַ
      
    PLC_RW = 0x00;      //��д��ֹ
}

/**************************************************************************
* �������ƣ�plc_read_reg
* ����������plc���Ѱַ������
* ���������addr ���Ѱַ��ַ
* ���ز������Ĵ�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
uint8_t plc_read_reg(uint8_t addr)
{
    uint8_t data;
	
    PLC_RW = 0x01;      //��ʹ��
      
    PLC_ADDR = addr;    //��ֵ��ַ
    data = PLC_DATA;    //������
      
    PLC_RW = 0x00;      //��д��ֹ
      
    return data;                                                        
}

/**************************************************************************
* �������ƣ�plc_init
* ����������plc��ʼ��
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
    
    plc_write_reg(PLC_11H_DACT0,0x13);  //DA������ʿ��ƼĴ���
    plc_write_reg(PLC_12H_PAMPT0,0x03); //���书������9.8dB
    
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
    
    plc_write_reg(PLC_40H_FREQ_TX0,0x06);   //����PSK��Ƶ��F0 = 131.5KHZ
    plc_write_reg(PLC_41H_FREQ_TX1,0x02);                                        
    plc_write_reg(PLC_42H_FREQ_TX2,0x30);                                      
    plc_write_reg(PLC_43H_FREQ_TX3,0x02);  
    
    plc_write_reg(PLC_44H_FREQ_RX0,0x1B);   //����PSKƵ��F0 = 131.5KHZ
    plc_write_reg(PLC_45H_FREQ_RX1,0x02);      
    
    plc_write_reg(PLC_46H_CSMAS,0xC8); //�ز���������ֵ���üĴ�����-56dB��
    plc_write_reg(PLC_47H_CSMAT,0xC9); //�ز�����ʱ����ƺ�ˢ��ģʽ    
    
    plc_write_reg(PLC_48H_AFEC,0x30); //ģ��ǰ��������ƼĴ���������6dB��  
	 plc_write_reg(PLC_49H_IIRT,0x06); //��·�˲��� 
    plc_write_reg(PLC_4AH_DKG,0x0B);  //��·������ƼĴ���(5-0):11dB
    plc_write_reg(PLC_4BH_DAG,0x1F);  //D/A���������������� 
	 plc_write_reg(PLC_4CH_MIXFWL,0x00);         
	 plc_write_reg(PLC_4DH_MIXFWH,0x80); 
    plc_write_reg(PLC_4EH_DIGTEST,0x04);                                   
    plc_write_reg(PLC_4FH_DST,0x00);                                        
    plc_write_reg(PLC_50H_AGCGAIN,0x00); //AGC����   
        
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
* �������ƣ�plc_tx_en
* ��������������ʹ��
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
    
    plc_write_reg(PLC_4BH_DAG,0x36);  //D/A���������������� 
    plc_write_reg(PLC_11H_DACT0,0x32);  //DA������ʿ��ƼĴ���
    plc_write_reg(PLC_12H_PAMPT0,0x23); //���书������
    
    tx_pa_enable(1);                            //PAʹ��
}

/**************************************************************************
* �������ƣ�plc_rx_en
* ��������������ʹ��
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_rx_en(void)
{
    uint8_t i;
    
    tx_pa_enable(0);   //PA����
    
    plc_write_reg(PLC_06H_LPFT,0xB1);   // deadbeef
    plc_write_reg(PLC_11H_DACT0,0x13);  //DA������ʿ��ƼĴ���
    plc_write_reg(PLC_12H_PAMPT0,0x03); //���书������
    
    for (i = 0; i < 62; i++) {     //��ջ�����
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
* �������ƣ�plc_idle_en
* �������������տ���
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_idle_en(void)
{    
    PLC_MOD = 0x00;     //PLC����
               
  
    _plc_state = IDLE;
}

/**************************************************************************
* �������ƣ�plc_tx_preamble
* ��������������2λ��Ƶ�źţ�ÿλ50us
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
* �������ƣ�plc_tx1_pn15
* ��������������1��������1(010 1100 1000 1111)
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_tx1_pn15(void)
{
    uint8_t i, j;
    uint16_t pn15 = 0b0101100100011110; //��λ���㣬��������
   
    //��Ƶ����ʱ����50us����500��NOP
    for (i = 0; i < 14; i++) {
        if ((pn15 & 0x8000) != 0) {
            DELAY1NOP();  //if/esleʱ�����
            
            DELAY4NOP(); //����ѭ��NOPȷ�����һλ��Ƭʱ�����
            TX_BIT1();   
        }
        else {       
            DELAY4NOP(); //����ѭ��NOPȷ�����һλ��Ƭʱ�����
            TX_BIT0();  
            DELAY2NOP();  //if/esleʱ�����
        }
        pn15 <<= 1; // 5nop
 
        //��Ƭ��֤50us��ʱ
        DELAY100NOP(); 
        DELAY100NOP();
        DELAY100NOP();
        DELAY100NOP();
        DELAY70NOP();
        DELAY8NOP();
        
    }  
    
    //�����н���Ƭʱ�䳤��Ϊ50us
    DELAY3NOP();
    //������Ƭ��Ҫ�ϼ�������в�����ʱ
    if ((pn15 & 0x8000) != 0) {
        DELAY1NOP();  //if/esleʱ�����
        TX_BIT1(); 
    }
    else {
        TX_BIT0();
        DELAY2NOP();  //if/esleʱ�����
    }
    
    //��������3��NOP  
}

/**************************************************************************
* �������ƣ�plc_tx0_pn15
* ��������������1��������0(101 0011 0111 0000)
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_tx0_pn15(void)
{ 
    uint8_t i, j;
    uint16_t pn15 = 0b1010011011100000; //��λ���㣬��������
   
    //��Ƶ����ʱ����50us����500��NOP
    for (i = 0; i < 14; i++) {
        if ((pn15 & 0x8000) != 0) {
            DELAY1NOP();  //if/esleʱ�����
            
            DELAY4NOP(); //����ѭ��NOPȷ�����һλ��Ƭʱ�����
            TX_BIT1();   
        }
        else {       
            DELAY4NOP(); //����ѭ��NOPȷ�����һλ��Ƭʱ�����
            TX_BIT0();  
            DELAY2NOP();  //if/esleʱ�����
        }
        pn15 <<= 1;
 
        //��Ƭ��֤50us��ʱ
        DELAY100NOP(); 
        DELAY100NOP();
        DELAY100NOP();
        DELAY100NOP();
        DELAY70NOP();
        DELAY8NOP();
        
    }  
    
    //�����н���Ƭʱ�䳤��Ϊ50us
    DELAY3NOP();
    //������Ƭ��Ҫ�ϼ�������в�����ʱ
    if ((pn15 & 0x8000) != 0) {
        DELAY1NOP();  //if/esleʱ�����
        TX_BIT1(); 
    }
    else {
        TX_BIT0();
        DELAY2NOP();  //if/esleʱ�����
    }
    
    //��������3��NOP  
}

/**************************************************************************
* �������ƣ�plc_send_proc
* ������������������
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_send_proc(void)
{
    uint8_t i;
    
    if (_send_buf.valid) {
        PLC_MOD = 0x22;     //����ʹ��
        
        plc_tx_preamble();  //��Ƶ�ź�
        
        for (i = 0; i < 4; i++) {
            plc_tx_nextbit();  
            if (_send_buf.bitdata) {       
                DELAY1NOP();  //if/esleʱ�����
                plc_tx1_pn15();
            }
            else
            {
                plc_tx0_pn15();
                DELAY2NOP();  //if/esleʱ�����
            }
            
            //�����н���Ƭʱ�䳤��Ϊ50us
            DELAY100NOP();
            DELAY100NOP();
            DELAY100NOP();
            DELAY60NOP();
            DELAY4NOP();
        }
        //�����н���Ƭʱ�䳤��Ϊ50us
        DELAY100NOP();
        DELAY10NOP();
        DELAY8NOP();
        PLC_MOD = 0x00; //�������
        
        if (!_send_buf.valid) { //�������ת���� 
            tx_pa_enable(0);   //PA����
                            
            PLC_RW = 0x02;      //дʹ��
            PLC_ADDR = PLC_06H_LPFT;      //���Ѱַ
            PLC_DATA = 0xB1;	            //��ֵ��ַ
            PLC_ADDR = PLC_11H_DACT0;      //���Ѱַ
            PLC_DATA = 0x13;	            //��ֵ��ַ   
            PLC_ADDR = PLC_12H_PAMPT0;      //���Ѱַ
            PLC_DATA = 0x03;	            //��ֵ��ַ
            PLC_RW = 0x00;      //��д��ֹ
                
            for (i = 0; i < 62; i++) {     //��ջ�����
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
* �������ƣ�plc_tx_nextbit
* ��������������������ݶ�ȡ
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
        _send_buf.valid = 0;        //��շ��仺����
        _send_buf.bitoff = 0; 
        _send_buf.byteoff = 0; 
        _send_buf.length = 0;    
    }
    else {
        DELAY9NOP();
    }
}

/**************************************************************************
* �������ƣ�plc_data_send
* ��������������������ݶ�ȡ
* ���������data   ����
            length ���ݳ���
* ���ز�����FAILED   ���ݳ��ȴ���
            SUCCEED  ���Ϸ���Ҫ��
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
uint8_t plc_data_send(uint8_t *data, uint8_t length)
{
    uint8_t i;
    uint16_t crcval;
    
    if ((length > 63) || (length == 0)) {
        return 0;  //���ݳ��ȴ���
    }
    //������ڷ��ͻ��Ѿ����յ�ͬ���ź�
    if (_plc_state == SEND || _frame_sync.valid == 1)
	 return 0;
    MMemcpy(&_send_buf.data[2], data, length);
    crcval = plc_crc_tx(data[0], 0xFFFF);
    for (i = 1; i < length; i++) {
        crcval = plc_crc_tx(data[i], crcval);
    }
    
    _send_buf.data[0] = 0xD3;               //֡ͬ���ź�
    _send_buf.data[1] = 0x91;               //֡ͬ���ź�
    _send_buf.data[2+length] = crcval/256;  //CRCУ����
    _send_buf.data[3+length] = crcval%256;  //CRCУ����
    _send_buf.valid = 1;                    //���÷���ʹ��
    _send_buf.bitdata = 0; 
    _send_buf.bitoff = 0; 
    _send_buf.byteoff = 0; 
    _send_buf.length = length+4;   

	plc_tx_en();
    _plc_state = SEND;  // ��ʼ��������
    
    return length;
}

/**************************************************************************
* �������ƣ�plc_rx_1bit
* ��������������1��������
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_rx_1bit(void)
{
    uint8_t i;
    
    //��ʱ���ڱ��ز���ͬ����Լ1320NOP
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
            DELAY1NOP();    //if/else ����
            
			_chip_flag.chipnum[i] = 1;// 16nops
			DELAY7NOP();
        }
        else {
			_chip_flag.chipnum[i] = 0;// 16nops
            DELAY9NOP();
        }
        
        //�������62NOP
        DELAY30NOP();
        DELAY4NOP();

        if (PLC_FSK_RXD) {
            DELAY1NOP();    //if/else ����
           
            _chip_flag.chipnum[i]++;// 23
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else ����
        }
        
        //�������63NOP
        DELAY30NOP();
        DELAY5NOP();
                       
        if (PLC_FSK_RXD) {                  //sample 2   
            DELAY1NOP();    //if/else ����
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else ����
        }
        
        //�������62NOP
        DELAY30NOP();
        DELAY4NOP();
                
        if (PLC_FSK_RXD) {                  //sample 3
            DELAY1NOP();    //if/else ����
            
            _chip_flag.chipnum[i]++;
        }
        else {
        DELAY20NOP();
        DELAY3NOP();
            
            DELAY2NOP();    //if/else ����
        }
        
        //�������63NOP
        DELAY30NOP();
        DELAY5NOP();
                  
        if (PLC_FSK_RXD) {                  //sample 4
            DELAY1NOP();    //if/else ����
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else ����
        }
        
        //�������62NOP
        DELAY30NOP();
        DELAY4NOP();
                 
        if (PLC_FSK_RXD) {                   //sample 5
            DELAY1NOP();    //if/else ����
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else ����
        }
        
        //�������63NOP
        DELAY30NOP();
        DELAY5NOP();
              
        if (PLC_FSK_RXD) {                  //sample 6
            DELAY1NOP();    //if/else ����
           
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else ����
        }
        
        //�������62NOP
        DELAY30NOP();
        DELAY4NOP();
                 
        if (PLC_FSK_RXD) {                  //sample 7
            DELAY1NOP();    //if/else ����
            
            _chip_flag.chipnum[i]++;
        }
        else {
            DELAY20NOP();
            DELAY3NOP();
            
            DELAY2NOP();    //if/else ����
        }
        
        //�������63NOP 
        DELAY20NOP();
        DELAY8NOP();
    }
}

/**************************************************************************
* �������ƣ�plc_bit_sum
* ������������Ƶ����
* �����������
* ���ز�����sum �������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
* �������ƣ�plc_bit_recv
* �������������������о�
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
    
    //for (i = 0; i < 62; i++) {     //��ջ�����
        //_chip_flag.chipnum[i] = 0;
    //} 
}

/**************************************************************************
* �������ƣ�plc_frame_sync
* ��������������֡ͬ��
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_frame_sync(void)
{
    _frame_sync.bitcnt++;
    
    _frame_sync.syncword <<= 4;
    _frame_sync.syncword |= _chip_flag.bitdata;

#if 0
    if (_frame_sync.bitcnt >= 3) {  //��֤һ��sys_tick����ָʾ��Ч     
        _recv_buf.valid = 0;        //������������ʼ������һ֡  
    }
#endif    
    if (_frame_sync.bitcnt >= 4) {
        _frame_sync.bitcnt = 4;
        if (_frame_sync.syncword == 0xD391) {
            _frame_sync.valid = 1;  //֡ͬ��   
            _frame_sync.bitcnt = 0;
        }
    }
}

/**************************************************************************
* �������ƣ�plc_frame_recv
* ����������֡����
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_frame_recv(void)
{
    uint8_t i;
    uint16_t crcval;
    
    _recv_buf.bitcnt++;
    _recv_buf.data[_recv_buf.length] <<=4;
    _recv_buf.data[_recv_buf.length] |= _chip_flag.bitdata;
        
    if (_recv_buf.bitcnt >= 2) {
        _recv_buf.length++;     //֡����ͳ��    
        _recv_buf.bitcnt = 0;
        
        if (_recv_buf.length == (_recv_buf.data[0] + 2) ) {
            //���һ֡����
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
                _recv_buf.valid = 0xAA; //CRCУ�����    
            }
            
            _recv_buf.bitcnt = 0;
            _recv_buf.length = 0; 
         
            _frame_sync.valid = 0;
            _frame_sync.bitcnt = 0;
            _frame_sync.syncword = 0;
        }
        else if (_recv_buf.length > 65) {
            _recv_buf.valid = 0x55;     //�������ݳ��ȴ���
            _recv_buf.bitcnt = 0;
            _recv_buf.length = 0; 
            
            _frame_sync.valid = 0;
            _frame_sync.bitcnt = 0;
            _frame_sync.syncword = 0;
        }
    }
}

/**************************************************************************
* �������ƣ�plc_recv_proc
* ������������������
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void plc_recv_proc(void)
{
    PLC_MOD = 0x59;     //FSK����ʹ��
    plc_rx_1bit();      //���ؽ���
    _listen_buf.valid = 1;
    _listen_buf.rssi = plc_read_reg(PLC_51H_RSSIV);
    PLC_MOD = 0x00;     //���ս���
        
    plc_bit_recv();     //����ͬ��
    if (_frame_sync.valid == 0) {
        plc_frame_sync();   //֡ͬ��   
    }
    else if (_recv_buf.valid == 0) {
        plc_frame_recv();   //����һ֡����   
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
* �������ƣ�plc_crc_rx
* ����������crcУ��
* ���������data     ��������
            regval   ǰһ�ֽ�У����,��ʼֵȡ0xFFFF
* ���ز�����regval   У����
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
* �������ƣ�plc_crc_tx
* ����������crcУ��
* ���������data     ��������
            regval   ǰһ�ֽ�У����,��ʼֵȡ0xFFFF
* ���ز�����regval   У����
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
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
* �������ƣ�isr
* �����������ж���ں���
* �����������
* ���ز�������
* �������ߣ�
* ������ڣ�
* �޶���ʷ��
* �޶����ڣ�
**************************************************************************/
void isr(void) interrupt 
{	
	 if (T16G1IF && T16G1IE) {
		 _sys_tick++;
		 
		 _last_time = _now_time;			 //��¼��һ�ι���ʱ��
		 _now_time = T16G1RH*256 | T16G1RL;  //��¼��ǰ����ʱ��
		 
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
           DELAY3NOP();  //��REC����
           if (_t16g1_valid) {          
                init_t16g2(1);
                _t16g1_valid = 0;
            }else {
            	DELAY100NOP();
                DELAY5NOP();
                DELAY2NOP();    //if/else ����
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
                DELAY2NOP();    //if/else ����
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





