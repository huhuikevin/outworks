/* 2.4G module  spi driver */
#include <hic.h>
#include "type.h"
#include "config.h"
#include "system.h"
#include "tool.h"
#include "spi_drv.h"
#ifdef CONFIG_W2_4G

#define FIFO_SIZE 63
#define FIFO_THRES_EMPTY 16
#define FIFO_THRES_FULL    16

#define MAX_BUF_LEN 60
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
	uchar tx_len;
	uchar tx_data[MAX_BUF_LEN];
}hw2000_t;


section30 hw2000_t hw2000;
section17 unsigned short agcTab[18]={0x0200,0x0200,0x0200,0x0200,0x0208,0x0210,0x0440,0x0480,0x04C0,0x04C8,0x04D0,0x04D8,0x04D9,0x04DA,0x06D1,0x06D9,0x06E1,0x06E2};

void hw2000_read_register(uchar addr , uchar *pValue)
{
	spi_read(addr, pValue, 2);
}

void hw2000_write_register(uchar addr , uchar *pValue)
{
	spi_write(addr, pValue, 2);
}

void hw2000_init()
{
	unsigned char pStr[2], j = 0;   

	//delete write protect ,write enable ,only 203
	pStr[1] = 0x55;
	pStr[0] = 0xAA; 
	hw2000_write_register(0x4C, &pStr[2]);

	//debug mode open or close /agc change
	pStr[1] = 0x01;
	pStr[0] = 0x21;//0x151:RX_data 0x121:tX_data  0x1F1:CD_signal  0x1a1:agc_rst  0x0171:ds_rest  0x0191:agc_gain 0x161  0x50:close debug mode
	hw2000_write_register( 0x4F, &pStr[2]);
	
	pStr[1] = 0x00;//00
	pStr[0] = 0x60;
	hw2000_write_register(0x00, &pStr[2]);
	
	pStr[1] = 0x46;
	pStr[0] = 0x80;
	hw2000_write_register(0x04, &pStr[2]);

	pStr[1] = 0x40;
	pStr[0] = 0x21;	
	hw2000_write_register(0x05, &pStr[2]);
	
	//env_able env_start ADC_data	
	pStr[1] = 0x10;//env=0x10:close  0x90:open
	pStr[0] = 0x00;	
	hw2000_write_register(0x06, &pStr[2]);
	
	pStr[1] = 0x40;
	pStr[0] = 0xE0; 
	hw2000_write_register(0x07, &pStr[2]);
	
	//RX for analog configuration
	pStr[1] = 0x33;
	pStr[0] = 0xC4;
	hw2000_write_register(0x08, &pStr[2]);
	
	//TXDAC_DC
	pStr[1] = 0xD6;
	pStr[0] = 0x20;
	hw2000_write_register(0x0A, &pStr[2]);
	
	
	//agc_delay_len
	pStr[1] = 0xC5;
	pStr[0] = 0x54;
	hw2000_write_register(0x1B, &pStr[2]);

	//FIFO half full set 	//empty and full thres is 16
	pStr[1] = 0x84;
	pStr[0] = 0x03; 
	hw2000_write_register(0x28, &pStr[2]);

	//preamble len
	pStr[1] = 0xF0; //preamble_len[2:0] 111,8bytes 48bits
// 	pStr[3] = 0x10; //1byte,48bits
// 	pStr[3] = 0x50; //3byte,48bits

 	pStr[0] = 0x00; //no  FEC 
// 	pStr[2] = 0x40; //turn on mancester package
// 	pStr[2] = 0x80; //8 bit /10 bit line data 
//	pStr[2] = 0xc0; //interleave data typle
// 	pStr[2] = 0x10; //FEC13
// 	pStr[2] = 0x20; //FEC23
	hw2000_write_register(0x20, &pStr[2]);


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
	pStr[1] = 0x07;
	pStr[0] = 0x08;
	hw2000_write_register(0x11, &pStr[2]);
	
	//agc threshold set and pa gain set
	pStr[1] = 0x30;
	pStr[0] = 0x43;
	hw2000_write_register(0x12, &pStr[2]);
	
	pStr[1] = 0x40;
	pStr[0] = 0x00;
	hw2000_write_register(0x13,  &pStr[2]);
	
	pStr[1] = 0x60;
	pStr[0] = 0x32;
	hw2000_write_register(0x14, &pStr[2]);
	
	pStr[1] = 0xFC;   
	pStr[0] = 0x0C;
	hw2000_write_register(0x15, &pStr[2]);
	
 	//sdm_amp  default:879B
 	pStr[1] = 0x51;
 	pStr[0] = 0x9B;
 	hw2000_write_register(0x1C, &pStr[2]);
	
	//RATE_1M
	pStr[1] = 0x80;
	pStr[0] = 0xFF;
	hw2000_write_register(0x2A, &pStr[2]);	
	
	//cd_th
	pStr[1] = 0x88;
	pStr[0] = 0x83;
 	hw2000_write_register(0x2C, &pStr[2]);
#elif RATE_250K
//agc threshold set and pa gain set
	pStr[1] = 0x00;
	pStr[0] = 0x43;
	hw2000_write_register(0x12, &pStr[2]);
	
	pStr[1] = 0x00;
	pStr[0] = 0x00;
	hw2000_write_register(0x13, &pStr[2]);
	
	pStr[1] = 0x00;
	pStr[0] = 0x32;
	hw2000_write_register(0x14, &pStr[2]);
	
	pStr[1] = 0x00;   
	pStr[0] = 0x0C;
	hw2000_write_register(0x15, &pStr[2]);

	pStr[1] = 0xC4;
	pStr[0] = 0x0B;
	hw2000_write_register(0x16, &pStr[2]);

	pStr[1] = 0x06;
	pStr[0] = 0x31;    //0x09
	hw2000_write_register(0x1A, &pStr[2]);

	pStr[1] = 0x51;
	pStr[0] = 0x8C;
	hw2000_write_register(0x1C, &pStr[2]);

	//RATE_250K
	pStr[1] = 0x01;
	pStr[0] = 0xFF;
	hw2000_write_register(0x2A, &pStr[2]);	
	
	//cd_th
	pStr[1] = 0x88;
	pStr[0] = 0x83;
	hw2000_write_register(0x2C, &pStr[2]);
#endif
	
	//vco_on_dly
	pStr[1] = 0x82;
	pStr[0] = 0xff; 
	hw2000_write_register(0x4D, &pStr[2]);

	//READ TEST
	pStr[1] = 0x00;
	pStr[0] = 0x00;
	hw2000_read_register(0x28, &pStr[2]);
	
	//agc table begin
	for(j=0; j<18; j++)
	{
		pStr[0] = 0x50 + (unsigned char)j;
		hw2000_write_register(pStr[0] , (unsigned char*)&agcTab[j]);
	}

/*********************配置频点和接收使能位*********************************/
	pStr[1] = 0xF0;
	pStr[0] = 0x00;
	hw2000_write_register(0x3c, &pStr[0]);
	
	pStr[1] = 0x18;
	pStr[0] = 0x30;
	hw2000_write_register(0x22, &pStr[0]);	
	
	//enable receive
	pStr[1] = 0x00;
	pStr[0] = 0x80;
	hw2000_write_register(0x21,  &pStr[0]);

/*********************配置36 37 38 39***********************/		
	pStr[1] = 0x00;
	pStr[0] = 0x80;
	hw2000_write_register(0x36, &pStr[0]); //FIFO0_EN = 1, FIFO0 rx

	pStr[1] = 0x00;
	pStr[0] = 0x00;
	hw2000_write_register(0x37, &pStr[0]); //FIFO1_EN = 0, FIFO1 disabled
	
}

uchar hw2000_tx_bytes(uchar *pdata, uchar len)
{
	uchar data[2];
	uchar reman;
	//enable transmit 
	if (hw2000.stat != hw2000_idle)
		return 0;
	hw2000.stat = hw2000_txing;
	data[1] = 0x01;
	data[0] = 0x00;
	hw2000_write_register(0x37, &data[0]);

	spi_write(0x32, &len, 1);// write the send len;

	if (len <= FIFO_SIZE){
		spi_write(0x32, pdata, len);
		reman = 0;
	}else{
		spi_write(0x32, pdata, FIFO_SIZE);
		reman = len - FIFO_SIZE;
	}

	data[1] = 0x00;
	data[0] = 0x81;//pipe0
	hw2000_write_register(0x36, &data[0]); //FIFO0_EN = 1, PTX_FIFO0_OCPY = 1, FIFO0 tx

	data[1] = 0x00;
	data[0] = 0x00;
	hw2000_write_register(0x37, &data[0]); //FIFO1_EN = 0, PTX_FIFO1_OCPY = 0

	while(reman != 0){
		do {
			data[1] = 0;
			hw2000_read_register(0x3a, &data[0]);//wait tx empty thres
		}while(data[1] & 0x1 == 0);
		if (reman <= FIFO_THRES_EMPTY){
			spi_write(0x32, pdata+len-reman, reman);
			reman = 0;
		}else{
			spi_write(0x32, pdata+len-reman, FIFO_THRES_EMPTY);
			reman -= FIFO_THRES_EMPTY;
		}
	}

	do {
		data[0] = 0x00;
		hw2000_read_register(0x3d, &data[0]);
	}while(data[0]& 0x1 == 0);

	data[1] = 0x00;
	data[0] = 0x08;
	hw2000_write_register(0x3d, &data[0]);
	hw2000.stat = hw2000_txfinish;
	return len;
}


#endif /* CONFIG_W2_4G */
