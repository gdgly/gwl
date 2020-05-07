#ifndef __SGD_TYPES_H_
#define	__SGD_TYPES_H_

/* type definitions */
typedef unsigned char                                       UINT8;
typedef unsigned short                                      UINT16;
typedef unsigned int                                        UINT32;
typedef signed char                                         INT8;
typedef signed short                                        INT16;
typedef signed int                                          INT32;
typedef float                                               FLOAT;
typedef double                                              DOUBLE;
typedef char                                                CHAR;

typedef unsigned int                                        BOOL;
typedef unsigned long long                                  UINT64;
typedef signed long long                                    INT64;
typedef unsigned int                                        UINTPTR;
typedef signed int                                          INTPTR;

typedef enum {false=0,true = !false}bool;
//typedef INT32                                               ssize_t;
//typedef UINT32                                              size_t;

#ifndef uint8_t
typedef unsigned char   uint8_t;
#endif
#ifndef int8_t
typedef signed char int8_t;
#endif
#ifndef uint16_t
typedef unsigned short  uint16_t;
#endif
#ifndef int16_t
typedef signed short    int16_t;
#endif
#ifndef uint32_t
typedef unsigned int    uint32_t;
#endif
#ifndef int32_t
typedef signed int  int32_t;
#endif
#if 0

#ifndef int64_t
typedef long long  int64_t;
#endif
#ifndef uint64_t
typedef unsigned long long  uint64_t;
#endif

#endif
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

//645Э�����������ռ�õ��ֽ�������λ: �ֽڣ�
#define PROTOCOL_645_SIZE                         256       //һ֡645����Э�����󳤶�



//���ڻ�ȡʱ������ݵ�ʱ���õ��ĺ�
#define TYPE_DATA_NONE           0   //��

//��������
#define ERROR_TYPE_NONE        0       //��



#endif
