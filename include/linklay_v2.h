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
	uint8_t Version:2;
	uint8_t ack_pkg:1;// 0:normal packaget, 1: ack package
	uint8_t protocol:1;//0: normal package, 1:route package
	uint8_t seq:1; // 
	uint8_t need_ack:1;// 1:need ack
	//uchar sender:1;//0:send by device, 1:send by gateway
	uint8_t res:2;
	//mac_addr dst_addr;//may be the next of the router or real dst
	mac_addr rtdst_addr;// must be real dst
}linkhead_t; // 6 bytes
#define PROTOCOL_NORMAL 0
#define PROTOCOL_ROUTER 1


typedef struct {
	linkhead_t head;
	uint8_t link_data[MAX_APP_DATA_LEN];
}link_frame_t;

typedef enum {
    MacPlc = 0,
    MacHw2000,
    MacTypeEnd
}uMacType;

void linklay_init();
uint8_t linklay_send_data(int8u *pdata, int8u len, linkaddr_t *plinkaddr);
uint8_t linklay_recv_data(uint8_t *pdata, uint8_t mac);
uint8_t linklay_recv_data_with_rssi(uint8_t *pdata, uint8_t mac, uint8_t *prssi);
uint8_t linklay_send_route_data(mac_addr *pdst, uint8_t *pdata, uint8_t len, uint8_t mac);
#endif/* __LINKLAY__H_ */

