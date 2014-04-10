#include "system.h"
#include "tool.h"
//#include "uart.h"

 int8u CalChecksum( int8u *p_data, int16u len)
{
  int8u checksum = 0;
  int16u i;

  for(i=0;i<len;i++)
    checksum += p_data[i];

  return checksum;
}

 void MMemSet(void *dest,int8u value,int8u count)
{
   int8u *d=(int8u *)dest;
   int8u i;
   for(i=0;i<count;i++)
   	*d++=value;
}

 void MMemcpy( void *dest,  void *src, int16u count) 
{
	int16u i;
	int8u *d = (int8u *)dest;
	int8u *s = (int8u *)src;

	for(i=0;i<count;i++)
		*d++ = *s++;

}

int8u MMemcmp( void *buf1,  void *buf2, int8u count)
{
	int8u i;
	int8u *d = (int8u *)buf1;
	int8u *s = (int8u *)buf2;

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
void DelayMs(uint MS)
{
    uint aa;//,ucB;
    uchar bb;
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