#ifndef TO_TOOL_H
#define TO_TOOL_H
#include "type.h"
void MMemcpy( void *dest, void *src, unsigned int count);
int8u MMemcmp( void *buf1, void *buf2, int8u count);
int8u CalChecksum(int8u *p_data, unsigned int len);
void MMemSet(void *dest,int8u value,int8u count);
void DelayMs(int32u MS);
#endif
