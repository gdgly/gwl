#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "Sgd_types.h"


int itoa(int num,char* str,int radix);
u32 intToBcd(u32 data,u8 *bcd,u8 len);
u32 Hex2Bcd(u32 hex);
u32 Bcd2Hex(u32 bcd);
int printhexdata(void *data,int len);
int Str2Hex(u8 *str,u8 *hex);


#endif

