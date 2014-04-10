/***** DEFINED **************************/
#ifndef __LINKLAY__H_
#define __LINKLAY__H_

#define MAX_APP_DATA_LEN 32

typedef enum {
    MacPlc = 0,
    MacWireless_2_4G,
    MacTypeEnd;
}uMacType;




int8u linklay_send_data(int8u *pdata, int8u len, int8u mac);
int8u linklay_recv_data(int8u pdata, int8u mac);

#endif/* __LINKLAY__H_ */
