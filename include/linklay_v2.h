/***** DEFINED **************************/
#ifndef __LINKLAY__H_
#define __LINKLAY__H_

#define MAX_APP_DATA_LEN MSG_MAX_LEN
#define LINKLAY_VERSION 0

typedef struct{
	//mac_addr next;
	mac_addr dest;
	uint8_t      protocol;
	uint8_t      mac;
	uint8_t      needack;
}linkaddr_t;

typedef struct {
	uchar Version:2;
	uchar ack_pkg:1;// 0:normal packaget, 1: ack package
	uchar protocol:1;//0: normal package, 1:route package
	uchar seq:1; // 
	uchar need_ack:1;// 1:need ack
	//uchar sender:1;//0:send by device, 1:send by gateway
	uchar res:2;
	//mac_addr dst_addr;//may be the next of the router or real dst
	mac_addr rtdst_addr;// must be real dst
}linkhead_t; // 6 bytes
#define PROTOCOL_NORMAL 0
#define PROTOCOL_ROUTER 1


typedef struct {
	linkhead_t head;
	uchar link_data[MAX_APP_DATA_LEN];
}link_frame_t;

typedef enum {
    MacPlc = 0,
    MacHw2000,
    MacTypeEnd
}uMacType;

void linklay_init();
int8u linklay_send_data(int8u *pdata, int8u len, int8u mac);
int8u linklay_recv_data(int8u *pdata, int8u mac);

#endif/* __LINKLAY__H_ */

