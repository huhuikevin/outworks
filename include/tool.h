#ifndef TO_TOOL_H
#define TO_TOOL_H
#include "type.h"

uint16_t calc_crc16(uint8_t data, uint16_t regval);
void MMemcpy( void *dest, void *src, uint16_t count);
int8u MMemcmp( void *buf1, void *buf2, uint8_t count);
int8u CalChecksum(uint8_t *p_data, unsigned int len);
void MMemSet(void *dest,uint8_t value,uint8_t count);
void DelayMs(uint32_t MS);
void print_num(uint32_t num);

void print_char(uint8_t ch);
void* memset00(uint8_t * p1, uint8_t n);

void Bcd2Hex(uint8_t *BCDopt, uint8_t *Hexopt, uint8_t Len );
void Hex2Bcd(uint8_t *Hexopt, uint8_t *BCDopt, uint8_t Len );

void Hex2Bcd_4(uint8_t *HEXopt, uint8_t *BCDopt);
void Bcd2Hex_4(uint8_t *BCDopt, uint8_t *HEXopt);

#endif

