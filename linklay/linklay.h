/***** DEFINED **************************/
#ifndef __LINKLAY__H_
#define __LINKLAY__H_

/* 上层拿这个buf进行数据发送 */
uchar *linklay_get_send_buf(int8u mac_type);
int8u linklay_set_send_len(int8u mac_type, int8u need_send);
void *linklay_get_recv_buf(int8u mac_type, int8u *pLen);
void *linklay_put_recv_buf(int8u mac_type);

#endif/* __LINKLAY__H_ */