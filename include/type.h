#ifndef __TYPE__H_
#define __TYPE__H_

typedef unsigned char   uint8_t;     
typedef unsigned int    uint16_t;
typedef unsigned long   uint32_t;

typedef signed char     int8_t;     
typedef signed int      int16_t;
typedef signed long     int32_t;




#define BOOL      char

#define TRUE      1
#define FALSE     0


#define int8u unsigned char
#define int16u unsigned int 
#define int32u unsigned long   
#define int16s signed int 

#define PGM  const int8u  * 
#define LOW_BYTE(n)                     ((int8u)((n) & 0xFF))
#define HIGH_BYTE(n)                    ((int8u)(LOW_BYTE((n) >> 8)))
#define HIGH_LOW_TO_INT(high, low) (                              \
                                    (( (int16u) (high) ) << 8) +  \
                                    (  (int16u) ( (low) & 0xFF))  \
                                   )             
                                   
#define BYTE_0(n)                    ((int8u)((n) & 0xFF))
#define BYTE_1(n)                    ((int8u)(BYTE_0((n) >> 8)))
#define BYTE_2(n)                    ((int8u)(BYTE_0((n) >> 16)))
#define BYTE_3(n)                    ((int8u)(BYTE_0((n) >> 24)))

#define BIT0    (1 << 0)
#define BIT1    (1 << 1)
#define BIT2    (1 << 2)
#define BIT3    (1 << 3)
#define BIT4    (1 << 4)
#define BIT5    (1 << 5)
#define BIT6    (1 << 6)
#define BIT7    (1 << 7)

#define TRUE    1
#define FALSE   0

#define SUCCEED 1
#define FAILED  0


union SVR_LONG_B08{	
	unsigned char NumChar[4];
	unsigned long NumLong;
};
union SVR_INT_B08{
    unsigned char NumChar[2];	
    unsigned int NumInt;	
};

#define MAC_ADDR_LEN 4
typedef union {	
	unsigned char caddr[MAC_ADDR_LEN];
	unsigned long laddr;
}mac_addr;

#endif /* __TYPE__H_ */

