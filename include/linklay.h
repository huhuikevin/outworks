/***** DEFINED **************************/
#ifndef __LINKLAY__H_
#define __LINKLAY__H_

#define MAX_APP_DATA_LEN MSG_MAX_LEN
#define MAC_ADDR_LEN 4

typedef enum {
    MacPlc = 0,
    MacHw2000,
    MacTypeEnd
}uMacType;

void linklay_init();
int8u linklay_send_data(int8u *pdata, int8u len, int8u mac);
int8u linklay_recv_data(int8u *pdata, int8u mac);

#endif/* __LINKLAY__H_ */

