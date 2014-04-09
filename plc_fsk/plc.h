/***** DEFINED **************************/
#ifndef __PLC_FSK__H_
#define __PLC_FSK__H_

uchar plc_tx_bytes(uchar *pdata ,uchar num);
int8u plc_rx_bytes(uchar *pdata);
void plc_init(void);
void plc_driver_txrx(void);

#endif/* __PLC_FSK__H_ */