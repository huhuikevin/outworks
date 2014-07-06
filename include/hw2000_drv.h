#ifndef HW2000_DRV__H_
#define HW2000_DRV__H_

#define HW2000_SPEED_BPS_250K 1
#define HW2000_SPEED_BPS_1M 2

void hw2000_init(uint8_t bps);
void hw2000_driver_process(void);
uint8_t hw2000_tx_bytes(uint8_t *pdata, uint8_t len);
uint8_t hw2000_rx_bytes(uint8_t *pdata);
uint8_t hw2000_read_rssi();

#endif /* HW2000_DRV__H_*/

