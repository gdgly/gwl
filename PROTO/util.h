#ifndef __UTIL_H_
#define	__UTIL_H_
long get_sys_uptime_by_sysinfo();
long get_sys_uptime();
long get_sys_uptime_ms();
int mymsleep(unsigned int ms);
unsigned char DEC_To_BCD(unsigned char Data_DEC);	
void Array_Mirror(unsigned char* P_Array, int Length);
void Protocol_645_Unpack(unsigned char *P_String, unsigned char Length);
void Protocol_645_Pack(unsigned char *P_String, unsigned char Length);
unsigned char GetSum8(unsigned char *P_Data, unsigned int Length);
int BitReverse(unsigned char *srcData, unsigned int srcSize, unsigned char *destData);
#endif