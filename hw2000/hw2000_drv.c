/* 2.4G module  spi driver */
#include <hic.h>
#include "type.h"
#include "config.h"
#include "system.h"
#include "tool.h"
#include "spi_drv.h"

#ifdef CONFIG_HW2000

#define FIFO_SIZE 63
#define FIFO_THRES_EMPTY 32
#define FIFO_THRES_FULL    8

#define MAX_BUF_LEN 64
enum hw2000_stat{
	hw2000_idle = 0,
	hw2000_txing,
	hw2000_txfinish,
	hw2000_rxing,
	hw2000_rxfinish,
};

typedef struct{
	uint8_t stat:3;
	uint8_t tx_err:1;
	uint8_t rx_err:1;
	uint8_t res:3;
	uint8_t rx_len;
	uint8_t rx_data[MAX_BUF_LEN];
}hw2000_t;


section30 hw2000_t hw2000;
unsigned short agcTab[18]={0x0200,0x0200,0x0200,0x0200,0x0208,0x0210,
									 0x0440,0x0480,0x04C0,0x04C8,0x04D0,0x04D8,
									 0x04D9,0x04DA,0x06D1,0x06D9,0x06E1,0x06E2};

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


uint16_t hw2000_read_register(uint8_t addr)
{
	return spi_rx_word(addr);
}

void hw2000_write_register(uint8_t addr , uint16_t Value)
{
	spi_tx_word(addr, Value);
}

void hw2000_rx_enable()
{
	hw2000_write_register(0x36, 0x0080); 
	hw2000_write_register(0x21, 0x0080); 
}

void hw2000_tx_enable(uint8_t enable)
{
	hw2000_write_register(0x23, 0x0000);
	hw2000_write_register(0x36, 0x0000);
	if (enable)
		hw2000_write_register(0x21, 0x0100);// enable tx
	else
		hw2000_write_register(0x21, 0x0000);
}

void hw2000_rx_reenable()
{
	hw2000_write_register(0x3d, 0x0008);//clear rx irq

	hw2000_write_register(0x21,0x0000);
	hw2000_delay4us();
	
	hw2000_write_register(0x21,0x0080);
}

void hw2000_port_init()
{
	PAT &= ~(BIT2 + BIT3 + BIT4 + BIT5);
	PAT |= (BIT6 + BIT7);
		
	CONFIG_SPI_CE = 1;
		
	CONFIG_SPI_CSN = 1;
	CONFIG_SPI_SCK = 0;
	CONFIG_SPI_MOSI = 0; 
		
	CONFIG_3_3V_CTRL = 1; //电源开
}

void hw2000_init()
{
	uint8_t j = 0;   
	uint8_t addr = 0;

	hw2000_port_init();
		
	//delete write protect ,write enable ,only 203
	hw2000_write_register(0x4C, 0x55AA);

	hw2000_write_register(0x01, 0x8FFF);
#if 0	
	//debug mode open or close /agc change
	hw2000_write_register( 0x4F, 0x0121);
	
	hw2000_write_register(0x00, 0x0060);
	
	hw2000_write_register(0x04, 0x4680);

	hw2000_write_register(0x05, 0x4021);
	
	//env_able env_start ADC_data	
	hw2000_write_register(0x06, 0x1000);
	
	hw2000_write_register(0x07, 0x40E0);
#endif	
	//RX for analog configuration
	hw2000_write_register(0x08, 0x73C4);
	hw2000_write_register(0x09, 0xC481);
#if 0	
	//TXDAC_DC
	hw2000_write_register(0x0A, 0xD620);
	
	
	//agc_delay_len
	hw2000_write_register(0x1B, 0xC554);
#endif
	//FIFO half full set 	//empty and full thres is 16
	hw2000_write_register(0x28, ((FIFO_THRES_EMPTY - 1)<<11) | ((FIFO_THRES_FULL -1) << 6) | 0x2);//(0x28, 0x8403);
	hw2000_write_register(0x2C, 0x918B);


#if 0
#if RATE_1M	
	//pa_off_dly
	hw2000_write_register(0x11, 0x0708);
	
	//agc threshold set and pa gain set
	hw2000_write_register(0x12, 0x3043);
	
	hw2000_write_register(0x13,  0x4000);
	
	hw2000_write_register(0x14, 0x6032);
	
	hw2000_write_register(0x15, 0xFC0C);
	
 	//sdm_amp  default:879B
 	hw2000_write_register(0x1C, 0x519B);
	
	//RATE_1M
	hw2000_write_register(0x2A, 0x80FF);	
	
	//cd_th
 	hw2000_write_register(0x2C, 0x8883);
#elif RATE_250K
//agc threshold set and pa gain set
	hw2000_write_register(0x12, 0x0043);
	
	hw2000_write_register(0x13, 0x0000);
	
	hw2000_write_register(0x14, 0x0032);
	
	hw2000_write_register(0x15, 0x000C);

	hw2000_write_register(0x16, 0xC40B);

	hw2000_write_register(0x1A, 0x0631);

	hw2000_write_register(0x1C, 0x518C);

	//RATE_250K
	hw2000_write_register(0x2A, 0x01FF);	
	
	//cd_th
	hw2000_write_register(0x2C, 0x8883);
#endif
#endif
#if 0
	//vco_on_dly
	hw2000_write_register(0x4D, 0x82FF);
#endif	
	//agc table begin
	for(j=0; j<18; j++)
	{
		addr = 0x50 + j;
		hw2000_write_register(addr , agcTab[j]);
	}
	//preamble len
	hw2000_write_register(0x20, 0xF060); //preamble 16bytes sync 48bits trailer 4bits

/*********************配置频点和接收使能位*********************************/

	hw2000_write_register(0x3c, 0xF000);

	hw2000_write_register(0x22, 0x1830);	

	hw2000_write_register(0x37, 0x0000); //FIFO1_EN = 0, PTX_FIFO1_OCPY = 0

	
	hw2000_rx_enable();
	
}

uint8_t hw2000_tx_bytes(uint8_t *pdata, uint8_t len)
{
	uint16_t val;
	uint8_t reman;
	uint8_t *ptx = pdata;
	//enable transmit 
	if (hw2000.stat != hw2000_idle)
		return 0;
	hw2000.stat = hw2000_txing;

	hw2000_tx_enable(1);

	spi_write(0x32, &len, 1);// write the send len;

	if (len <= (FIFO_SIZE-1)){
		spi_write(0x32, pdata, len);
		reman = 0;
		ptx = pdata + len;
	}else{
		spi_write(0x32, pdata, (FIFO_SIZE-1));
		reman = len - (FIFO_SIZE-1);
		ptx = pdata + (FIFO_SIZE-1);
	}

    /* tell hw2000 that data is ready and can be sent */
	hw2000_write_register(0x36, 0x0091); //FIFO0_EN = 1, PTX_FIFO0_OCPY = 1, FIFO0 tx
	hw2000_write_register(0x37, 0x0000);
	
	while(reman != 0){
		do {
			val = hw2000_read_register(0x3a);//wait tx empty thres
		}while(val & 0x100 == 0);
		if (reman <= FIFO_THRES_EMPTY){//pading the tx size = FIFO_THRES_EMPTY-reman
			spi_write(0x32, ptx, reman);
			reman = 0;
			ptx = ptx + reman;
		}else{
			spi_write(0x32, ptx, FIFO_THRES_EMPTY);
			reman -= FIFO_THRES_EMPTY;
			ptx = ptx + FIFO_THRES_EMPTY;
		}
		//hw2000_write_register(0x36, 0x0091); //FIFO0_EN = 1, PTX_FIFO0_OCPY = 1, FIFO0 tx
		//hw2000_write_register(0x37, 0x0000);
	}

	do {
		val = hw2000_read_register(0x30);// FSM_TX_STATE
	}while(val& 0x0020 == 0);

	hw2000_write_register(0x3d, 0x0008);// 清中断
	hw2000.stat = hw2000_txfinish;

	hw2000_rx_reenable();// 默认一直处于接受状态

	hw2000.stat = hw2000_idle;
	return len;
}

uint8_t hw2000_rx_bytes(uint8_t *pdata)
{
	uint8_t len;
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
	uint16_t val;
	uint8_t pkglen=0, readlen = 0, reman = 0;

	if (hw2000.stat != hw2000_idle)
		return;
	
	val = hw2000_read_register(0x3d);
	if (val & 0x1 == 0)// no recv irq 
	{
		return;
	}
	hw2000.stat = hw2000_rxing;

	//hw2000_write_register(0x3b, 0x0080);// clear rx fifo read pointer

	do {
		val = hw2000_read_register(0x3a);//wait rx full thresh, the min tx size is FIFO_THRES_EMPTY
	}while(val & 0x200 == 0);              // so the full thres event must be present

	spi_read(0x32, &pkglen, 1);//read the package len

	if (pkglen > MAX_BUF_LEN){
		hw2000.stat = hw2000_idle;
		hw2000_rx_reenable();
		return ;
	}

	spi_read(0x32, &hw2000.rx_data[0], FIFO_THRES_FULL-1);	
	readlen = FIFO_THRES_FULL-1;
	reman = pkglen = readlen;
	while(reman != 0){
		do {
			val = hw2000_read_register(0x3a);//wait rx full thresh, the min tx size is FIFO_THRES_EMPTY
		}while(val & 0x200 == 0);				// so the full thres event must be present
		if (reman >= FIFO_THRES_EMPTY) {
			spi_read(0x32, &hw2000.rx_data[readlen], FIFO_THRES_FULL);
			readlen += FIFO_THRES_FULL;
			reman -= FIFO_THRES_FULL;
		}
		else {
			spi_read(0x32, &hw2000.rx_data[readlen], reman);
			readlen += reman;
			reman = 0;
		}
	}
	hw2000.rx_len = pkglen;
	hw2000.stat = hw2000_rxfinish;
	hw2000_write_register(0x3d, 0x0008);// 清中断
	hw2000_rx_reenable();
	return ;	
}

void hw2000_driver_process(void)
{
	hw2000_rx();
}


#endif /* CONFIG_W2_4G */
