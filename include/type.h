#ifndef __TYPE__H_
#define __TYPE__H_


//#define TRUE 1
//#define FALSE 0
#define ulong	  unsigned long
#define	uint	  unsigned int
#define	uchar	  unsigned char
#define	uintd	  unsigned int	data
#define	uchard	  unsigned char	data
#define	uintb	  unsigned int	bdata
#define	ucharb	  unsigned char	bdata
#define	uinti	  unsigned int	idata
#define	uchari	  unsigned char	idata
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


union SVR_LONG_B08{	
	unsigned char NumChar[4];
	unsigned long NumLong;
};
union SVR_INT_B08{
    unsigned char NumChar[2];	
    unsigned int NumInt;	
};




#endif /* __TYPE__H_ */
