//ϵͳ��һЩ����
#include <hic.h>
#include "type.h"


void RAM_Clr(void) //ramȫ��
{
	__asm 
  { 
	CLR   IAAL;
	CLR   IAAH;
	CLR   IAD;
	INC   IAAL,1;
	JBS   PSW,C
	GOTO  $-3;
	INC   IAAH,1;
	JBS   IAAH,4
	GOTO  $-6;
  } 
}