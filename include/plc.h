/***** DEFINED **************************/
#ifndef  __PLC_FSK__H_
#define   __PLC_FSK__H_

int8u plc_tx_bytes(uchar *pdata ,int8u num);
int8u plc_rx_bytes(uchar *pdata);
void plc_init(void);
void plc_driver_process(void);

#endif
