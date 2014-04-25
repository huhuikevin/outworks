/* 2.4G module  spi driver */
#include <hic.h>
#include "type.h"
#include "config.h"
#include "system.h"
#include "tool.h"
#include "spi_drv.h"
#ifdef CONFIG_HW2000

#define FIFO_SIZE 63
#define FIFO_THRES_EMPTY 16
#define FIFO_THRES_FULL    16

#define MAX_BUF_LEN 64
enum hw2000_stat{
	hw2000_idle = 0,
	hw2000_txing,
	hw2000_txfinish,
	hw2000_rxing,
	hw2000_rxfinish,
};

typedef struct{
	uchar stat:3;
	uchar tx_err:1;
	uchar rx_err:1;
	uchar res:3;
	uchar rx_len;
	uchar rx_data[MAX_BUF_LEN];
}hw2000_t;


section30 hw2000_t hw2000;
section17 unsigned short agcTab[18]={0x0200,0x0200,0x0200,0x0200,0x0208,0x0210,0x0440,0x0480,0x04C0,0x04C8,0x04D0,0x04D8,0x04D9,0x04DA,0x06D1,0x06D9,0x06E1,0x06E2};

void hw2000_delay4us(void)
{
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
    // NOP();NOP();NOP();NOP();NOP();
    // NOP();NOP();NOP();NOP();NOP();

    //NOP();NOP();NOP();
    //NOP();NOP();
}


void hw2000_read_register(uchar addr , uchar *pValue)
{
	spi_read(addr, pValue, 2);
}

void hw2000_write_register(uchar addr , uchar *pValue)
{
	spi_write(addr, pValue, 2);
}

void hw2000_rx_enable(uchar enable)
{
	uchar val[2];

	val[1] = 0x00;
	if (enable)
		val[0] = 0x80;
	else
		val[0] = 0x00;
	hw2000_write_register(0x21, &val[0]); 
}

void hw2000_tx_enable(uchar enable)
{
	uchar val[2];
	if (enable)
		val[1] = 0x01;
	else
		val[1] = 0x00;
	val[0] = 0x00;
	hw2000_write_register(0x21, &val[0]);// enable tx
}

void hw2000_rx_reenable()
{
	uchar val[2];
	val[1] = 0x00;
	val[0] = 0x08;
	hw2000_write_register(0x3d, &val[0]);//clear rx irq

	hw2000_rx_enable(0);
	hw2000_delay4us();
	
	hw2000_rx_enable(1);
}

void hw2000_init()
{
	unsigned char val[2], j = 0;   

	//delete write protect ,write enable ,only 203
	val[1] = 0x55;
	val[0] = 0xAA; 
	hw2000_write_register(0x4C, &val[2]);

	//debug mode open or close /agc change
	val[1] = 0x01;
	val[0] = 0x21;//0x151:RX_data 0x121:tX_data  0x1F1:CD_signal  0x1a1:agc_rst  0x0171:ds_rest  0x0191:agc_gain 0x161  0x50:close debug mode
	hw2000_write_register( 0x4F, &val[2]);
	
	val[1] = 0x00;//00
	val[0] = 0x60;
	hw2000_write_register(0x00, &val[2]);
	
	val[1] = 0x46;
	val[0] = 0x80;
	hw2000_write_register(0x04, &val[2]);

	val[1] = 0x40;
	val[0] = 0x21;	
	hw2000_write_register(0x05, &val[2]);
	
	//env_able env_start ADC_data	
	val[1] = 0x10;//env=0x10:close  0x90:open
	val[0] = 0x00;	
	hw2000_write_register(0x06, &val[2]);
	
	val[1] = 0x40;
	val[0] = 0xE0; 
	hw2000_write_register(0x07, &val[2]);
	
	//RX for analog configuration
	val[1] = 0x33;
	val[0] = 0xC4;
	hw2000_write_register(0x08, &val[2]);
	
	//TXDAC_DC
	val[1] = 0xD6;
	val[0] = 0x20;
	hw2000_write_register(0x0A, &val[2]);
	
	
	//agc_delay_len
	val[1] = 0xC5;
	val[0] = 0x54;
	hw2000_write_register(0x1B, &val[2]);

	//FIFO half full set 	//empty and full thres is 16
	val[1] = 0x84;
	val[0] = 0x03; 
	hw2000_write_register(0x28, &val[2]);

	//preamble len
	val[1] = 0xF0; //preamble_len[2:0] 111,8bytes 48bits
// 	pStr[3] = 0x10; //1byte,48bits
// 	pStr[3] = 0x50; //3byte,48bits

 	val[0] = 0x00; //no  FEC 
// 	pStr[2] = 0x40; //turn on mancester package
// 	pStr[2] = 0x80; //8 bit /10 bit line data 
//	pStr[2] = 0xc0; //interleave data typle
// 	pStr[2] = 0x10; //FEC13
// 	pStr[2] = 0x20; //FEC23
	hw2000_write_register(0x20, &val[2]);


// 	pStr[1] = 0x23;
// 	pStr[3] = 0x03;
// 	pStr[2] = 0x21;
// 	spiAcc(WRITE, &pStr[0], &pStr[2]);

// 	
// 	pStr[1] = 0x29;
// 	pStr[3] = 0x18;//default
// // 	pStr[3] = 0x58;//crc 8
// // 	pStr[3] = 0x38;//enable raoma
// 	pStr[2] = 0x00;
// 	spiAcc(WRITE, &pStr[0], &pStr[2]);

 
#if RATE_1M	
	//pa_off_dly
	val[1] = 0x07;
	val[0] = 0x08;
	hw2000_write_register(0x11, &val[2]);
	
	//agc threshold set and pa gain set
	val[1] = 0x30;
	val[0] = 0x43;
	hw2000_write_register(0x12, &val[2]);
	
	val[1] = 0x40;
	val[0] = 0x00;
	hw2000_write_register(0x13,  &val[2]);
	
	val[1] = 0x60;
	val[0] = 0x32;
	hw2000_write_register(0x14, &val[2]);
	
	val[1] = 0xFC;   
	val[0] = 0x0C;
	hw2000_write_register(0x15, &val[2]);
	
 	//sdm_amp  default:879B
 	val[1] = 0x51;
 	val[0] = 0x9B;
 	hw2000_write_register(0x1C, &val[2]);
	
	//RATE_1M
	val[1] = 0x80;
	val[0] = 0xFF;
	hw2000_write_register(0x2A, &val[2]);	
	
	//cd_th
	val[1] = 0x88;
	val[0] = 0x83;
 	hw2000_write_register(0x2C, &val[2]);
#elif RATE_250K
//agc threshold set and pa gain set
	val[1] = 0x00;
	val[0] = 0x43;
	hw2000_write_register(0x12, &val[2]);
	
	val[1] = 0x00;
	val[0] = 0x00;
	hw2000_write_register(0x13, &val[2]);
	
	val[1] = 0x00;
	val[0] = 0x32;
	hw2000_write_register(0x14, &val[2]);
	
	val[1] = 0x00;   
	val[0] = 0x0C;
	hw2000_write_register(0x15, &val[2]);

	val[1] = 0xC4;
	val[0] = 0x0B;
	hw2000_write_register(0x16, &val[2]);

	val[1] = 0x06;
	val[0] = 0x31;    //0x09
	hw2000_write_register(0x1A, &val[2]);

	val[1] = 0x51;
	val[0] = 0x8C;
	hw2000_write_register(0x1C, &val[2]);

	//RATE_250K
	val[1] = 0x01;
	val[0] = 0xFF;
	hw2000_write_register(0x2A, &val[2]);	
	
	//cd_th
	val[1] = 0x88;
	val[0] = 0x83;
	hw2000_write_register(0x2C, &val[2]);
#endif
	
	//vco_on_dly
	val[1] = 0x82;
	val[0] = 0xff; 
	hw2000_write_register(0x4D, &val[2]);

	//READ TEST
	val[1] = 0x00;
	val[0] = 0x00;
	hw2000_read_register(0x28, &val[2]);
	
	//agc table begin
	for(j=0; j<18; j++)
	{
		val[0] = 0x50 + (unsigned char)j;
		hw2000_write_register(val[0] , (unsigned char*)&agcTab[j]);
	}

/*********************配置频点和接收使能位*********************************/
	val[1] = 0xF0;
	val[0] = 0x00;
	hw2000_write_register(0x3c, &val[0]);
	
	val[1] = 0x18;
	val[0] = 0x30;
	hw2000_write_register(0x22, &val[0]);	

	val[1] = 0x00;
	val[0] = 0x00;
	hw2000_write_register(0x37, &val[0]); //FIFO1_EN = 0, PTX_FIFO1_OCPY = 0

	
	hw2000_rx_enable();
	
}

uchar hw2000_tx_bytes(uchar *pdata, uchar len)
{
	uchar val[2];
	uchar reman;
	//enable transmit 
	if (hw2000.stat != hw2000_idle)
		return 0;
	hw2000.stat = hw2000_txing;


	spi_write(0x32, &len, 1);// write the send len;

	if (len <= FIFO_SIZE){
		spi_write(0x32, pdata, len);
		reman = 0;
	}else{
		spi_write(0x32, pdata, FIFO_SIZE);
		reman = len - FIFO_SIZE;
	}
	if (reman == 0 && len < FIFO_THRES_EMPTY){//pading the tx to match the FIFO_THRES_EMPTY
		reman = FIFO_THRES_EMPTY - len;
		val[0] = 0xff;
		while(reman--){
			spi_write(0x32, val, 1);
		}
	}
	val[1] = 0x00;
	val[0] = 0x81;//pipe0
	hw2000_write_register(0x36, &val[0]); //FIFO0_EN = 1, PTX_FIFO0_OCPY = 1, FIFO0 tx

	while(reman != 0){
		do {
			val[1] = 0;
			hw2000_read_register(0x3a, &val[0]);//wait tx empty thres
		}while(val[1] & 0x1 == 0);
		if (reman <= FIFO_THRES_EMPTY){//pading the tx size = FIFO_THRES_EMPTY-reman
			spi_write(0x32, pdata+len-reman, FIFO_THRES_EMPTY);
			reman = 0;
		}else{
			spi_write(0x32, pdata+len-reman, FIFO_THRES_EMPTY);
			reman -= FIFO_THRES_EMPTY;
		}
	}

	do {
		val[0] = 0x00;
		hw2000_read_register(0x30, &val[0]);// FSM_TX_STATE
	}while(val[0]& 0x20 == 0);

	val[1] = 0x00;
	val[0] = 0x80;
	hw2000_write_register(0x36, &val[0]); //FIFO0_EN = 1, PTX_FIFO0_OCPY=0

	val[1] = 0x00;
	val[0] = 0x08;
	hw2000_write_register(0x3d, &val[0]);// 清中断
	hw2000.stat = hw2000_txfinish;

	hw2000_rx_reenable();// 默认一直处于接受状态

	hw2000.stat = hw2000_idle;
	return len;
}

uchar hw2000_rx_bytes(uchar *pdata)
{
	uchar len;
	if (hw2000.stat == hw2000_rxfinish){
		if (hw2000.rx_len != 0){
			len = hw2000.rx_len;
			hw2000.stat = hw2000_idle;
			MMemcpy(pdata, &hw2000.rx_data[0], len);
			return len;
		}
	}
	return 0;
}
void hw2000_rx(void)
{
	uchar val[2];
	uchar pkglen=0, readlen = 0;

	if (hw2000.stat != hw2000_idle)
		return;
	
	val[0] = 0;
	val[1] = 0;
	hw2000_read_register(0x3d, &val[0]);
	if (val[1] & 0x1 == 0)// no recv irq 
	{
		return;
	}
	hw2000.stat = hw2000_rxing;
	val[1] = 0x00;
	val[0] = 0x80;
	hw2000_write_register(0x3b, &val[0]);// clear rx fifo read pointer

	do {
		val[1] = 0;
		hw2000_read_register(0x3a, &val[0]);//wait rx full thresh, the min tx size is FIFO_THRES_EMPTY
	}while(val[1] & 0x2 == 0);              // so the full thres event must be present

	spi_read(0x32, &pkglen, 1);//read the package len

	if (pkglen > MAX_BUF_LEN){
		hw2000.stat = hw2000_idle;
		hw2000_rx_reenable();
		return ;
	}

	spi_read(0x32, &hw2000.rx_data[0], FIFO_THRES_FULL-1);	
	readlen = FIFO_THRES_FULL-1;

	while(readlen<pkglen){
		do {
			val[1] = 0;
			hw2000_read_register(0x3a, &val[0]);//wait rx full thresh, the min tx size is FIFO_THRES_EMPTY
		}while(val[1] & 0x2 == 0);				// so the full thres event must be present
		
		spi_read(0x32, &hw2000.rx_data[readlen], FIFO_THRES_FULL);
		readlen += FIFO_THRES_FULL;
	}
	hw2000.rx_len = pkglen;
	hw2000.stat = hw2000_rxfinish;

	hw2000_rx_reenable();
	return ;	
}

void hw2000_driver_process(void)
{
	hw2000_rx();
}


#endif /* CONFIG_W2_4G */
