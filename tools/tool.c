#include "type.h"
#include "system.h"
#include "tool.h"
#include "uart.h"



 /**************************************************************************
 * �������ƣ�crc16
 * ����������crcУ��
 * ���������data	  ��������
			 regval   ǰһ�ֽ�У����,��ʼֵȡ0xFFFF
 * ���ز�����regval   У����
 * �������ߣ�
 * ������ڣ�
 * �޶���ʷ��
 * �޶����ڣ�
 **************************************************************************/
 uint16_t calc_crc16(uint8_t data, uint16_t regval)
 { 
	 uint8_t i;
  
	 for (i = 0; i < 8; i++) 
	 { 
		 if (((regval & 0x8000) >> 8) ^ (data & 0x80) ) 
			 regval = (regval << 1) ^ 0x8005; 
		 else 
			 regval = (regval << 1); 
		 
		 data <<= 1; 
	 } 
	 
	 return regval; 
 }

uint8_t CalChecksum( uint8_t *p_data, uint16_t len)
{
  uint8_t checksum = 0;
  uint16_t i;

  for(i=0;i<len;i++)
    checksum += p_data[i];

  return checksum;
}

void* memset00(uint8_t * p1, uint8_t n) 
{
	while(n--)
  {
		*p1++ = 0x00;
  }
}
 void MMemSet(void *dest,uint8_t value,uint8_t count)
{
   uint8_t *d=(uint8_t *)dest;
   uint8_t i;
   for(i=0;i<count;i++)
   	*d++=value;
}

 void MMemcpy( void *dest,  void *src, uint16_t count) 
{
	uint16_t i;
	uint8_t *d = (uint8_t *)dest;
	uint8_t *s = (uint8_t *)src;

	for(i=0;i<count;i++)
		*d++ = *s++;

}

uint8_t MMemcmp( void *buf1,  void *buf2, uint8_t count)
{
	uint8_t i;
	uint8_t *d = (uint8_t *)buf1;
	uint8_t *s = (uint8_t *)buf2;

	for(i=0;i<count;i++)
	{
        
		if(*d++ != *s++)
		{

		break;
		}
	}
  
	if(i < count)
		return 1;
	else
		return 0;
}

/************************************
** ����ԭ��: void Delay(uchar MS); **
** ��    ��: ��ʱtime=MS ms.     **
************************************/
void DelayMs(uint32_t MS)
{
    uint32_t aa;//,ucB;
    uint8_t bb;
    for(aa=0;aa<MS;aa++) 
    {
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFC;)
        {
            bb++;
            NOP();
        }
        for(bb=0;bb<0xFc;)
        {
            bb++;
        }
    }
}


void print_num(uint32_t num)
{
	uint32_t t = num;
	uint8_t tail[10];
	uint8_t i = 0;
	do {
		tail[i] = t % 10;
		t = t / 10;
		i++;
	}while(t != 0);
	while(i-- != 0){
		console_tx_one_byte('0'+tail[i]);
	}
}

void print_char(uint8_t ch)
{
	console_tx_one_byte(ch);
}


/********************���ֽ�BCD��תHEX��***********************
�� �� ����  Bcd_To_Hex                        
����������  ���ֽ�BCD��תHEX��  
���������  ��ת����BCD��                                    
��������ֵ��ת�����HEX��                                           
***********************************************************/
uint8_t Bcd_To_Hex( uint8_t value )	
{
  return ( ((value&0xF0)>>4)*10+(value&0x0F) );
}


/********************���ֽ�HEX��תBCD��***********************
�� �� ����  Hex_To_Bcd                        
����������  ���ֽ�HEX��תBCD�룬Len���ܳ���4  
���������  HEX��ָ�룬BCD��ָ�룬�ֽڳ���                                    
��������ֵ����                                           
***********************************************************/
void Hex2Bcd( uint8_t *Hexopt, uint8_t *BCDopt, uint8_t Len ) //Ҫ��Len���ܴ���4
{ 
	MMemcpy(BCDopt, Hexopt, Len);
}
void Hex2Bcd_4(uint8_t *HEXopt, uint8_t *BCDopt)
{
  Hex2Bcd( HEXopt, BCDopt, 4 );
}
/********************���ֽ�BCD��תHEX��***********************
�� �� ����  Bcd2Hex                        
����������  ���ֽ�BCD��תHEX��  
���������  BCD��ָ�룬HEX��ָ�룬�ֽڳ���                                    
��������ֵ����                                           
***********************************************************/
void Bcd2Hex( uint8_t *BCDopt, uint8_t *HEXopt, uint8_t Len ) 
{
	MMemcpy(HEXopt, BCDopt, Len);
}
void Bcd2Hex_4(uint8_t *BCDopt, uint8_t *HEXopt)
{
 	Bcd2Hex( BCDopt, HEXopt, 4 );
}
