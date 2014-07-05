/**************************************************************************
* ��Ȩ������Copyright@2014 �Ϻ��������ɵ�·���޹�˾
* �ļ����ƣ�plc.h
* ����������plc����㴫����ƺ�������	
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
#ifndef _PLC_H_
#define _PLC_H_
#include "type.h"

//-------------------------------------------------------------------------
// ����3040 PLC�������⹦�ܼĴ���                         
#define  PLC_00H_LNA1T0	     0x00    //����Ԥ��
#define	PLC_01H_LNA1T1      0x01    //����Ԥ��                                   
#define	PLC_02H_LNA1T2      0x02    //����Ԥ��
#define	PLC_03H_LNA2T0      0x03    //����Ԥ��
#define	PLC_04H_LNA2T1      0x04    //����Ԥ��
#define	PLC_05H_LNA2T2	     0x05    //����Ԥ��
#define	PLC_06H_LPFT        0x06    //����Ԥ��
#define	PLC_07H_PGA1T0  	  0x07    //����Ԥ��
#define	PLC_08H_PGA1T1	     0x08    //����Ԥ��
#define	PLC_09H_PGA1T2	     0x09    //����Ԥ��
#define	PLC_0AH_PGA2T0	     0x0A    //����Ԥ��
#define	PLC_0BH_PGA2T1	     0x0B    //����Ԥ��
#define	PLC_0CH_PGA2T2	     0x0C    //����Ԥ��
#define	PLC_0DH_ADCT0	     0x0D    //����Ԥ��
#define	PLC_0EH_ADCT1	     0x0E    //����Ԥ��
#define	PLC_0FH_ADCT2	     0x0F    //����Ԥ��


#define	PLC_10H_ADCT3	     0x10    //����Ԥ��
#define	PLC_11H_DACT0	     0x11    //����Ԥ��
#define	PLC_12H_PAMPT0	     0x12    //���͹��ʿ��ƼĴ���	
#define	PLC_13H_PDT0	     0x13    //����Ԥ��
#define	PLC_14H_PLCT0	     0x14    //����Ԥ��
#define	PLC_15H_ADC_D0	     0x15    //����Ԥ��
#define	PLC_16H_ADC_D1	     0x16    //����Ԥ��
#define	PLC_17H_ADC_DIN	  0x17    //����Ԥ��
#define	PLC_18H_RESERVED	  0x18    //����
#define	PLC_19H_RESERVED	  0x19    //����
#define	PLC_1AH_RESERVED	  0x1A    //����
#define	PLC_1BH_RESERVED	  0x1B    //����
#define	PLC_1CH_RESERVED	  0x1C    //����
#define	PLC_1DH_RESERVED	  0x1D    //����
#define	PLC_1EH_RESERVED	  0x1E    //����
#define	PLC_1FH_RESERVED	  0x1F    //����


#define	PLC_20H_AGCP61LNA   0x20    //PLC����Ƶ���ּĴ���0
#define	PLC_21H_AGCP55LNA   0x21    //PLC����Ƶ���ּĴ���1
#define	PLC_22H_AGCP49LNA   0x22    //PLC����Ƶ���ּĴ���2
#define	PLC_23H_AGCP43LNA   0x23    //PLC����Ƶ���ּĴ���3
#define	PLC_24H_AGCP37LNA   0x24    //PLC����Ƶ���ּĴ���0
#define	PLC_25H_AGCP31LNA   0x25    //PLC����Ƶ���ּĴ���1
#define	PLC_26H_AGCP25LNA   0x26    //������ʾ�Ĵ���
#define	PLC_27H_AGCP19LNA   0x27    //PLC RSSI��ʾ�Ĵ���
#define	PLC_28H_AGCP13LNA   0x28    //PLC CSMA���üĴ���
#define	PLC_29H_AGCP07LNA   0x29    //PLC CSMA����ʱ��Ĵ���
#define	PLC_2AH_AGCP01LNA   0x2A    //PLC AFE���ƼĴ���
#define	PLC_2BH_AGCP05LNA   0x2B    //����Ԥ��
#define	PLC_2CH_AGCP11LNA   0x2C    //����Ԥ��
#define	PLC_2DH_AGCP17LNA   0x2D    //����Ԥ��
#define	PLC_2EH_AGCP23LNA   0x2E    //����Ԥ��
#define	PLC_2FH_RESERVED    0x2F    //����


#define	PLC_30H_AGCN61PGA   0x30    //PLC����Ƶ���ּĴ���0
#define	PLC_31H_AGCN55PGA   0x31    //PLC����Ƶ���ּĴ���1
#define	PLC_32H_AGCN49PGA   0x32    //PLC����Ƶ���ּĴ���2
#define	PLC_33H_AGCN43PGA   0x33    //PLC����Ƶ���ּĴ���3
#define	PLC_34H_AGCN37PGA   0x34    //PLC����Ƶ���ּĴ���0
#define	PLC_35H_AGCN31PGA   0x35    //PLC����Ƶ���ּĴ���1
#define	PLC_36H_AGCN25PGA   0x36    //������ʵ�Ĵ���
#define	PLC_37H_AGCN19PGA   0x37    //PLC RSSI��ʾ�Ĵ���
#define	PLC_38H_AGCN13PGA   0x38    //PLC CSMA���üĴ���
#define	PLC_39H_AGCN07PGA   0x39    //PLC CSMA����ʱ��Ĵ���
#define	PLC_3AH_AGCN01PGA   0x3A    //PLC AFE���ƼĴ���
#define	PLC_3BH_AGCN05PGA   0x3B    //����Ԥ��
#define	PLC_3CH_AGCN11PGA   0x3C    //����Ԥ��
#define	PLC_3DH_AGCN17PGA   0x3D    //����Ԥ��
#define	PLC_3EH_AGCN23PGA   0x3E    //����Ԥ��
#define	PLC_3FH_RESERVED    0x3F    //����


#define	PLC_40H_FREQ_TX0	  0x40    //����FSK�±��Ƶ���ֵ�8λ
#define	PLC_41H_FREQ_TX1	  0x41    //����FSK�±��Ƶ���ָ�6λ
#define	PLC_42H_FREQ_TX2	  0x42    //����FSK�ϱ��Ƶ���ֵ�8λ
#define  PLC_43H_FREQ_TX3	  0x43    //����FSK�ϱ��Ƶ���ָ�6λ
#define	PLC_44H_FREQ_RX0	  0x44    //����Ƶ���ָ�8λ�Ĵ���
#define  PLC_45H_FREQ_RX1    0x45    //����Ƶ���ֵ�8λ�Ĵ���
#define 	PLC_46H_CSMAS	     0x46    //�ز���������ֵ���üĴ���
#define 	PLC_47H_CSMAT	     0x47    //�ز�����ʱ����ƺ�ˢ��ģʽѡ��Ĵ���  
#define 	PLC_48H_AFEC	     0x48    //ģ��ǰ�˿��ƼĴ���  
#define 	PLC_49H_IIRT	     0x49    //PSK��·��������  
#define 	PLC_4AH_DKG		     0x4a    //PSK�ڲ���·������ƼĴ���  
#define 	PLC_4BH_DAG		     0x4b    //DA����������������üĴ���  
#define 	PLC_4CH_MIXFWL	     0x4C    //��Ƶ��Ƶ���ֵ�8λ  
#define 	PLC_4DH_MIXFWH	     0x4D    //��Ƶ��Ƶ���ָ�8λ 
#define 	PLC_4EH_DIGTEST	  0x4E    //����Ԥ�� 
#define 	PLC_4FH_DST	        0x4F    //����Ԥ�� 


#define 	PLC_50H_AGCGAIN	  0x50    //������ʾ�Ĵ���    
#define 	PLC_51H_RSSIV	     0x51    //PLC RSSI��ʾ�Ĵ���  
#define 	PLC_52H_RSSIT	     0x52    //PLC RSSI���ԼĴ���  
#define 	PLC_53H_RESERVED	  0x53    //����     
#define 	PLC_54H_RESERVED	  0x54    //����     
#define 	PLC_55H_RESERVED	  0x55    //����     
#define 	PLC_56H_RESERVED	  0x56    //����     
#define 	PLC_57H_RESERVED	  0x57    //����     
#define 	PLC_58H_RESERVED	  0x58    //����     
#define 	PLC_59H_RESERVED	  0x59    //����     
#define 	PLC_5AH_RESERVED	  0x5A    //����     
#define 	PLC_5BH_RESERVED	  0x5B    //����     
#define 	PLC_5CH_RESERVED	  0x5C    //����     
#define 	PLC_5DH_RESERVED	  0x5D    //����     
#define 	PLC_5EH_RESERVED	  0x5E    //����     
#define 	PLC_5FH_RESERVED	  0x5F    //����  

#define  TX_BIT1()  PLC_MOD |= 0x01;
#define  TX_BIT0()  PLC_MOD &= 0xFE;

static volatile section64 sbit PLC_FSK_RXD @ (unsigned) &PLC_MOD* 8 + 3;

//״̬����
enum plc_state
{
    SEND = 0,       //����̬
    RECV = 1,       //����̬
    IDLE = 2,       //����̬
}section32 _plc_state;

//����������
struct listen_buf
{
    uint8_t valid;      //����״̬�ж�:1��Ч,0��Ч    
    uint8_t  rssi;       //�����ŵ�rssiֵ
}section32 _listen_buf;

//���仺����
struct send_buf
{
    uint8_t valid;      //����״̬�ж�:1��Ч,0��Ч
    uint8_t bitdata;    //��ǰ�������λ����0/1
    uint8_t bitoff;     //bitƫ��
    uint8_t byteoff;    //byteƫ��
    uint8_t length;     //���ݳ��ȣ��64
    uint8_t data[68];   //���ݻ�����,�û������64
}section32 _send_buf;

//֡ͬ��������
struct frame_sync
{
    uint8_t  valid;       //ͬ��״̬�ж�:1ͬ�����,0δ���
    uint8_t  bitcnt;      //ͬ������     
    uint16_t syncword;    //ͬ���ֻ���
}section32 _frame_sync;

//���ջ�����
struct recv_buf
{
    uint8_t valid;      //���������Ƿ���Ч:1��Ч,0��Ч
    uint8_t bitcnt;     //���ձ��ؼ��� 
    uint8_t length;     //���ݳ��ȣ��64
    uint8_t data[65];   //���ݻ��������û��64
    uint8_t rssiv;
}section33 _recv_buf;

//��Ƭ������������־λ
struct chip_flag
{         
    uint8_t bitdata;        //���ձ������ݣ��8��
    uint8_t chipnum[62];   //ͳ��chipdataÿbyte��1�ĸ���
}section34 _chip_flag;


/**************************************************************************
* �������ƣ�plc_write_reg
* ����������plc���Ѱַд����
* ���������addr ���Ѱַ��ַ
            data ����
* ���ز�������
**************************************************************************/
void plc_write_reg(uint8_t addr, uint8_t data);

/**************************************************************************
* �������ƣ�plc_read_reg
* ����������plc���Ѱַ������
* ���������addr ���Ѱַ��ַ
* ���ز������Ĵ�������
**************************************************************************/
uint8_t plc_read_reg(uint8_t addr);

/**************************************************************************
* �������ƣ�plc_init
* ����������plc��ʼ��
* �����������
* ���ز�������
**************************************************************************/
void plc_init(void);

/**************************************************************************
* �������ƣ�plc_tx_en
* ��������������ʹ��
* �����������
* ���ز�������
**************************************************************************/
void plc_tx_en(void);

/**************************************************************************
* �������ƣ�plc_rx_en
* ��������������ʹ��
* �����������
* ���ز�������
**************************************************************************/
void plc_rx_en(void);

/**************************************************************************
* �������ƣ�plc_listen_en
* �����������ŵ�����
* �����������
* ���ز�������
**************************************************************************/
void plc_listen_en(void);

/**************************************************************************
* �������ƣ�plc_idle_en
* �������������տ���
* �����������
* ���ز�������
**************************************************************************/
void plc_idle_en(void);

/**************************************************************************
* �������ƣ�plc_tx_preamble
* ��������������2λ��Ƶ�ź�,ÿλ50us
* �����������
* ���ز�������
**************************************************************************/
void plc_tx_preamble(void);

/**************************************************************************
* �������ƣ�plc_tx1_pn15
* ��������������1��������1(010 1100 1000 1111)
* �����������
* ���ز�������
**************************************************************************/
void plc_tx1_pn15(void);

/**************************************************************************
* �������ƣ�plc_tx0_pn15
* ��������������1��������0(101 0011 0111 0000)
* �����������
* ���ز�������
**************************************************************************/
void plc_tx0_pn15(void);

/**************************************************************************
* �������ƣ�plc_send_proc
* ������������������
* �����������
* ���ز�������
**************************************************************************/
void plc_send_proc(void);

/**************************************************************************
* �������ƣ�plc_tx_nextbit
* ��������������������ݶ�ȡ
* �����������
* ���ز�������
**************************************************************************/
void plc_tx_nextbit(void);

/**************************************************************************
* �������ƣ�plc_tx_nextbit
* ��������������������ݶ�ȡ
* ���������data   ����
            length ���ݳ���
* ���ز�����FAILED   ���ݳ��ȴ���
            SUCCEED  ���Ϸ���Ҫ��
**************************************************************************/
uint8_t plc_data_send(uint8_t *data, uint8_t length);

/**************************************************************************
* �������ƣ�plc_rx_1bit
* ��������������1��������
* �����������
* ���ز�������
**************************************************************************/
void plc_rx_1bit(void);

/**************************************************************************
* �������ƣ�plc_bit_sum
* ������������Ƶ����
* �����������
* ���ز�����sum �������
**************************************************************************/
uint8_t plc_bit_sum(uint8_t *chipnum);

/**************************************************************************
* �������ƣ�plc_bit_sync
* �������������ձ���ͬ��
* �����������
* ���ز�������
**************************************************************************/
void plc_bit_recv(void);

/**************************************************************************
* �������ƣ�plc_frame_sync
* ��������������֡ͬ��
* �����������
* ���ز�������
**************************************************************************/
void plc_frame_sync(void);

/**************************************************************************
* �������ƣ�plc_frame_recv
* ����������֡����
* �����������
* ���ز�������
**************************************************************************/
void plc_frame_recv(void);

/**************************************************************************
* �������ƣ�plc_recv_proc
* ������������������
* �����������
* ���ز�������
**************************************************************************/
void plc_recv_proc(void);

/**************************************************************************
* �������ƣ�plc_listen_proc
* ������������������
* �����������
* ���ز�������
**************************************************************************/
void plc_listen_proc(void);

/**************************************************************************
* �������ƣ�plc_crc_rx
* ����������crcУ��
* ���������data     ��������
            regval   ǰһ�ֽ�У����,��ʼֵȡ0xFFFF
* ���ز�����regval   У����
**************************************************************************/
uint16_t plc_crc_rx(uint8_t data, uint16_t regval);

/**************************************************************************
* �������ƣ�plc_crc_tx
* ����������crcУ��
* ���������data     ��������
            regval   ǰһ�ֽ�У����,��ʼֵȡ0xFFFF
* ���ز�����regval   У����
**************************************************************************/
uint16_t plc_crc_tx(uint8_t data, uint16_t regval);

#endif
