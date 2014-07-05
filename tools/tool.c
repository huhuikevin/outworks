#include "type.h"
#include "system.h"
#include "tool.h"
#include "uart.h"



 /**************************************************************************
 * 函数名称：crc16
 * 功能描述：crc校验
 * 输入参数：data	  输入数据
			 regval   前一字节校验结果,初始值取0xFFFF
 * 返回参数：regval   校验结果
 * 函数作者：
 * 完成日期：
 * 修订历史：
 * 修订日期：
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
** 函数原型: void Delay(uchar MS); **
** 功    能: 延时time=MS ms.     **
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


/********************单字节BCD码转HEX码***********************
函 数 名：  Bcd_To_Hex                        
功能描述：  单字节BCD码转HEX码  
输入参数：  待转换的BCD码                                    
函数返回值：转换后的HEX码                                           
***********************************************************/
uint8_t Bcd_To_Hex( uint8_t value )	
{
  return ( ((value&0xF0)>>4)*10+(value&0x0F) );
}


/********************多字节HEX码转BCD码***********************
函 数 名：  Hex_To_Bcd                        
功能描述：  多字节HEX码转BCD码，Len不能超过4  
输入参数：  HEX码指针，BCD码指针，字节长度                                    
函数返回值：无                                           
***********************************************************/
void Hex2Bcd( uint8_t *Hexopt, uint8_t *BCDopt, uint8_t Len ) //要求Len不能大于4
{ 
	MMemcpy(BCDopt, Hexopt, Len);
}
void Hex2Bcd_4(uint8_t *HEXopt, uint8_t *BCDopt)
{
  Hex2Bcd( HEXopt, BCDopt, 4 );
}
/********************多字节BCD码转HEX码***********************
函 数 名：  Bcd2Hex                        
功能描述：  多字节BCD码转HEX码  
输入参数：  BCD码指针，HEX码指针，字节长度                                    
函数返回值：无                                           
***********************************************************/
void Bcd2Hex( uint8_t *BCDopt, uint8_t *HEXopt, uint8_t Len ) 
{
	MMemcpy(HEXopt, BCDopt, Len);
}
void Bcd2Hex_4(uint8_t *BCDopt, uint8_t *HEXopt)
{
 	Bcd2Hex( BCDopt, HEXopt, 4 );
}
