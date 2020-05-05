#ifndef __UTIL_H_
#define	__UTIL_H_
#define	debug_info	1

#define MRS_STREAM_MAX_LEN       (4096)
#define MRS_PROTO645_DATAGRAM_DATA_LEN_MAX     240
#define MRS_PROTO645_DATAGRAM_HEADER_LEN_MAX   12
#define MRS_PROTO645_DATAGRAM_LEN_MAX   (MRS_PROTO645_DATAGRAM_DATA_LEN_MAX\
                                        +MRS_PROTO645_DATAGRAM_HEADER_LEN_MAX)

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
unsigned char mrsToolsCalcCheckSum2(unsigned char* ptr, int len);
unsigned char mrsToolsCalcCheckSum(unsigned char* ptr, int len);
unsigned char* mrsToolsStreamFindByte(unsigned char* ptr, int len, int v);
int mrsToolsCheck645Frame(unsigned char* ptr, int len);
unsigned char* mrsToolsStreamFindByte(unsigned char* ptr, int len, int v);
#endif