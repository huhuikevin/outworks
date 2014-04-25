#ifndef HW2000_DRV__H_
#define HW2000_DRV__H_

void hw2000_driver_process(void);
void hw2000_tx_bytes(uchar *pdata, uchar len);
uchar hw2000_rx_bytes(uchar *pdata);

#endif /* HW2000_DRV__H_*/

