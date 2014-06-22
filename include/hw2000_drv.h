#ifndef HW2000_DRV__H_
#define HW2000_DRV__H_

void hw2000_init();
void hw2000_driver_process(void);
uint8_t hw2000_tx_bytes(uint8_t *pdata, uint8_t len);
uint8_t hw2000_rx_bytes(uint8_t *pdata);

#endif /* HW2000_DRV__H_*/

