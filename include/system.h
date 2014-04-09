#ifndef __SYSTEM__H_
#define __SYSTEM__H_

#define MAX_DATA_LEN 32
#define MAC_ADDR_LEN 4
#define VER_LEN_LEN 1
#define TRAN_SEQ_LEN 1
#define CHECKSUM_LEN 1

#define LINK_LAY_HEAD_LEN (MAC_ADDR_LEN + VER_LEN_LEN + TRAN_SEQ_LEN + CHECKSUM_LEN)
#define LINK_LAY_MAX_LEN (MAX_DATA_LEN + LINK_LAY_HEAD_LEN)

#define MAC_LAY_HEAD_LEN 2
#define MAC_LAY_MAX_LEN     (LINK_LAY_MAX_LEN + MAC_LAY_HEAD_LEN)

#define LINK_LAY_OFFSET MAC_LAY_HEAD_LEN

typedef enum {
    MacPlc = 0,
    MacWireless_2_4G,
    MacTypeEnd;
}uMacType;


typedef enum {
    LinkLayIdle = 0,
    LinkLayTxing,
    LinkLayTxPending,
    LinkLayTxFinish,
    LinkLayWaitAck,
    LinkLayRecvedAck,    
}uLinkLayStatMachine;


#endif /* __SYSTEM__H_ */
