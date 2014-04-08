/***** DEFINED **************************/
#ifndef __PLC_FSK__H_
#define __PLC_FSK__H_

/*****其它输出口**************/
#define  R_LED PB5		 
#define  S_LED PC0  



/******PLC****************/

#define	    STA     PB6



 union SVR_LONG_B08{	
	unsigned char NumChar[4];
	unsigned long NumLong;
};
union SVR_INT_B08{
unsigned char NumChar[2];	
unsigned int NumInt;	
};

void IO_Init(void);
void PlcSendTask(void);
void DelayBit(void);

#endif/* __PLC_FSK__H_ */