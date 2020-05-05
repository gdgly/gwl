#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "spidev.h"
#include "MESamBase.h"
#include "util.h"
#include <errno.h>

int m_MFd;
unsigned char m_MSendBuf[MESAM_SEND_BUF_MAX_LEN];
//unsigned char m_MRecvBuf[MESAM_RECV_BUF_MAX_LEN];


int MEsamDevOpen(void)
{
	if (m_MFd >= 0)
	{
		close(m_MFd);
		m_MFd = -1;
	}

	m_MFd = open((char *)"/dev/spidev0.0",O_RDWR);
	if (m_MFd < 0)
	{
		//#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamDevOpen error (open dev failed.\n)");
		//#endif
		//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDevOpen open dev failed!!!!\n"));
		return -1;
	}
	if (MEsamParaSet() != 0)
	{
		close(m_MFd);
		m_MFd = -1;
		//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamParaSet failed!!!!\n"));
		return -1;
	}
	//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDevOpen open dev success!!!!\n"));
	printf("MEsamDevOpen open dev success!!!!\n");
	return 0;
};

int MEsamDevClose(void)
{
	if (m_MFd >= 0)
	{
		close(m_MFd);
		//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDevClose close dev!!!!\n"));
	}
	m_MFd = -1;
	return 0;
};

int MEsamDataSend(unsigned char* _MBuf,int _MBufLen)
{
	int _len,_pos,_rs,i, m_MSendPos=0, m_MSendLen;
	if (m_MFd < 0)
	{
		#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamDataSend error(m_MFd = %d)\n",m_MFd);
		#endif
		return -1;
	}
	memset(m_MSendBuf,0,MESAM_SEND_BUF_MAX_LEN);
	m_MSendPos = 0;
	m_MSendBuf[m_MSendPos++] = 0x55;
	if (MEsamGetLRCl(_MBuf,_MBufLen, &m_MSendPos) < 0)
	{
		printf("TMESAMBaseClass::MEsamDataSend MEsamGetLRCl failed _MBufLen %d\n",_MBufLen);
		return -1;
	}
	m_MSendLen = m_MSendPos;
#if 1//def MESAM_CLASS_DEBUG
	printf("MEsamDataSend m_MSendLen=%d\n", m_MSendLen);
	printf("MEsamSend:");
	for (i = 0;i < m_MSendLen ; i++)
	{
		printf("%02X ", m_MSendBuf[i]);
	}
	printf("\n");
#endif
	_pos = 0;
	struct spi_ioc_transfer tr[1];
	memset(tr,0,sizeof(tr));

	tr[0].tx_buf = (unsigned long)m_MSendBuf;
	tr[0].len = m_MSendLen;

	tr[0].delay_usecs = 5;	//original is 100	

	_rs = ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);
	if (_rs < 1)
	{
		MEsamDevOpen();
		printf("MEsamDataSend failed!!! ERRNO %d\n", errno);
		return -1;
	}
	printf("MEsamDataSend success!!!\n");		
	return 0;
}

int MEsamDataRecv(int _WakeDt, unsigned char m_MRecvBuf[MESAM_RECV_BUF_MAX_LEN])
{
	//int _rs = 0;
	int _num = 0;
	int _bytesCount = 0;
	int m_MRecvLen = 0;
	int _len =0;
	int _cs = 0;
#ifndef ACE_WIN32
	if (m_MFd < 0)
	{
		#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamDataRecv error(m_MFd = %d)\n",m_MFd);
		#endif
		return -1;
	}

	long last_char_time = 0;
	long deadline = get_sys_uptime() + _WakeDt;
	unsigned char _Buf[MESAM_RECV_BUF_MAX_LEN];
	int total=0;
	int phrase = 0;

		//memset(_Buf,0,MESAM_RECV_BUF_MAX_LEN);
	 //_rs = 1;//MEsamSelect(_WakeDt);
	
		struct spi_ioc_transfer tr[1];
		memset(tr,0x00,sizeof(struct spi_ioc_transfer));
		tr[0].tx_buf = NULL;
		tr[0].rx_buf = (unsigned long)_Buf;
		
		//tr[0].delay_usecs = 0;
		//tr[0].speed_hz = speed;
		//tr[0].bits_per_word = bits;
		tr[0].cs_change = 1;//change cs between xfer in one message(one ioctl call) and don't change cs after whole message done
		
		mymsleep(10);
#define ESAM_FRAME_BREAK 3//参见TESAM文档
	//if (_rs > 0)
	{
		int frame_got_part = 0;
		_bytesCount = 1;//MESAM_RECV_BUF_MAX_LEN;//15;//
		while ((m_MRecvLen ? frame_got_part < m_MRecvLen : 1) && (long)(deadline - get_sys_uptime()) >= 0)
		{
			tr[0].len = _bytesCount;
			int ret = ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);
			if (ret < _bytesCount)
			{
				++_num;
				if (last_char_time && (long)(get_sys_uptime() - last_char_time) >= ESAM_FRAME_BREAK)
				{
					printf("MEsamDataRecv frame break!!!\n");
					m_MRecvLen = 0;
					break;
				}
				if (_num > 100)
				{
					_num = 0;
					mymsleep(0);
					//ACE_OS::sleep(0);
				}
				continue;
			}
			else//if (_len > 0)
			{
				total += _bytesCount;
				_num = 0;
				frame_got_part += _bytesCount;
				_bytesCount = 0;				
				last_char_time = get_sys_uptime();
				//printf("MEsamDataRecv success: %d!!!\n", frame_got_part);
				//printf("MEsamDataRecv data:");
				int _i;
				for (_i = 0;_i < frame_got_part;_i++ )
				{
					//printf("MEsamDataRecv loop data: 0x%x", _Buf[_i]);
					if (_Buf[_i] == 0x55)//frame start done
					{
						if (phrase == 0)
						{
							printf("MEsamDataRecv found 0x55 !!!\n");
						}
						if (_i)//discard garbage before frame start 
						{
							memmove(_Buf,_Buf+_i,frame_got_part - _i);
							frame_got_part -= _i;
							_i = 0;
						}
						
						phrase = 1;
						if (5 <= frame_got_part)//head done
						{
							if (_Buf[1]==0x90)
							{
								m_MRecvLen = 5 + (_Buf[3]<< 8)+_Buf[4] + 1;
								if (m_MRecvLen > MESAM_RECV_BUF_MAX_LEN)//bad frame len
								{
									printf("MEsamDataRecv frame len %d too large\n", m_MRecvLen);
									//search next 0x55
									_Buf[0] = 0;//don't check it again
									m_MRecvLen = 0;
									phrase = 0;
									frame_got_part = 0;
									continue;//search 55 again
								}

								if (frame_got_part < m_MRecvLen)//frame not enough
								{
									phrase = 2;
									tr[0].rx_buf = (unsigned long)_Buf + frame_got_part;
#if !READ_ESAM_SPI_MULTI_BYTE
									_bytesCount = 1;
#else
									_bytesCount = m_MRecvLen - frame_got_part;//
#endif
									//tr[0].len = _bytesCount;
									break;//recv again
								}
								//whole frame got
								_cs = MEsamCheckLRC2(&_Buf[1],m_MRecvLen - 2);
								if (_cs != _Buf[m_MRecvLen - 1])//bad frame
								{
									printf("MEsamDataRecv LRC wrong m_MRecvLen %d _cs 0x%x->0x%x!!!\n", m_MRecvLen,_cs,_Buf[m_MRecvLen - 1]);
									//dumpHex(NULL,&_Buf[0],m_MRecvLen);
									_Buf[0] = 0;//don't check it again
									m_MRecvLen = 0;
									frame_got_part = 0;
									phrase = 0;
									//_num = 0;
									continue;//search 55 again
								}
								phrase = 3;
								//good frame
								memcpy(m_MRecvBuf,&_Buf[0],m_MRecvLen);
								#ifdef MESAM_CLASS_DEBUG
								MY_ACE_ERROR((LM_ERROR,"MEsamRecv:"));
								for (int _j = 0; _j < m_MRecvLen ; _j++)
								{
									//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("%02X "),m_MRecvBuf[_j]));
									printf("%02X ", m_MRecvBuf[_j]);
								}
								//MY_ACE_ERROR((LM_ERROR,"\n"));
								printf("\n");
								#endif
								//dumpHex("TMESAMBaseClass::MEsamDataRecv whole frame got",m_MRecvBuf,m_MRecvLen);

								//MY_ACE_ERROR((LM_ERROR,"now deactive cs!!!\n"));
								printf("now deactive cs!!!\n");
								//cs deactivate
								tr[0].delay_usecs = 0;
								tr[0].cs_change = 0;
								tr[0].len = 1;//if 0, driver will not return
								tr[0].tx_buf = NULL;
								tr[0].rx_buf = (unsigned long)&_i;
								ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);

								mymsleep(1);


								//return 0;
								return m_MRecvLen;
							}
							else
							{
								printf("MEsamDataRecv _Buf[1] 0x%x 0x%x not 90\n",_Buf[1], _Buf[2]);
							}
						}
						else//head not got
						{
							tr[0].rx_buf = (unsigned long)_Buf + frame_got_part;
#if !READ_ESAM_SPI_MULTI_BYTE
							_bytesCount = 1;
#else
							_bytesCount = 5-frame_got_part;//
#endif
							//tr[0].len = _bytesCount;
							break;//recv again
						}
					}//buf[i] == 0x55
				}//for
				if (_i >= frame_got_part)//no valid frame start found
				{
					//discard all bytes received,retry receive at _Buf[0] to get frame start
					phrase = 0;
					m_MRecvLen = 0;
					frame_got_part = 0;
					_bytesCount = 1;//15;//MESAM_RECV_BUF_MAX_LEN;
					tr[0].rx_buf = (unsigned long)_Buf;
					//tr[0].len = _bytesCount;
					mymsleep(0);
				}
				else
				{
					//ACE_ASSERT(_bytesCount > 0);
					//assert(_bytesCount > 0);
					//receive left frame part
					if (m_MRecvLen)
					{
						//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataRecv  want frame len %d got %d\n",m_MRecvLen,frame_got_part));
					}
					else
					{
						//receiving head
					}
				}
				//MY_ACE_ERROR((LM_ERROR,"\n"));
			}
		}//while
		m_MRecvLen = 0;
		//deactivate cs
		tr[0].cs_change = 0;
		tr[0].delay_usecs = 0;
		tr[0].len = 1;
		tr[0].tx_buf = NULL;
		tr[0].rx_buf = (unsigned long)&frame_got_part;
		ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);
		mymsleep(1);
		//MY_ACE_ERROR((LM_ERROR, "\nTMESAMBaseClass::MEsamDataRecv fail total %d phrase %d!!!",total,phrase));
		printf("MEsamDataRecv fail total %d phrase %d!!!", total,phrase);

	}
#endif
	return -1;
}

int MEsamParaSet(void)
{
	unsigned int	m_EsamMode = 3;
	unsigned int	m_EsamBits = 8;
	unsigned int m_EsamSpeed = 1*1000*1000;//1Mhz
	int	ret = 0;
#ifndef ACE_WIN32
	if ((ret = ioctl(m_MFd,SPI_IOC_WR_MODE,&m_EsamMode))!=0)	//
	{
		//#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_WR_MODE) ret =%d\n", ret);
		//#endif
		return -1;
	}
	if ((ret = ioctl(m_MFd,SPI_IOC_WR_BITS_PER_WORD,&m_EsamBits))!=0)	//�?每字多少�?8
	{
		//#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_WR_BITS_PER_WORD) ret =%d\n", ret);
		//#endif
		return -1;
	}

	if ((ret = ioctl(m_MFd,SPI_IOC_WR_MAX_SPEED_HZ,&m_EsamSpeed))!=0)	//写最大速率1Mhz
	{
		//#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_WR_MAX_SPEED_HZ) ret =%d\n", ret);
		//#endif
		return -1;
	}
	#if 1//def MESAM_CLASS_DEBUG
	if (ioctl(m_MFd,SPI_IOC_RD_MODE,&m_EsamMode)!=0)
	{
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_RD_MODE)\n");
	}
	if (ioctl(m_MFd,SPI_IOC_RD_BITS_PER_WORD,&m_EsamBits)!=0)
	{
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_RD_BITS_PER_WORD)\n");
	}
	if (ioctl(m_MFd,SPI_IOC_RD_MAX_SPEED_HZ,&m_EsamSpeed)!=0)
	{
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_RD_BITS_PER_WORD)\n");
	}
	printf("T-ESAM mode %d,%d bits,%d MHz Max\n", m_EsamMode,m_EsamBits,m_EsamSpeed);
	#endif
#endif
	return 0;	
}

int MEsamGetLRCl(unsigned char* _MBuf,int _MBufLen, int *pos)
{
	int _cs = 0, _i=0;
	if (*pos + _MBufLen + 1 > sizeof(m_MSendBuf))
		return -1;	
	memcpy(&m_MSendBuf[*pos],_MBuf,_MBufLen);
	*pos = *pos + _MBufLen;
	for (_i = 0;_i < _MBufLen ; _i++)
	{
		_cs = _cs ^ _MBuf[_i];
	}
	//m_MSendBuf[*pos++] = _cs ^ 0xFF;	
	m_MSendBuf[*pos] = _cs ^ 0xFF;
	*pos += 1;
	return 0;
};

int MEsamCheckLRC2(unsigned char* _MBuf,int _MBufLen)
{
	int _cs = 0, _i=0;
	for (_i = 0;_i < _MBufLen ; _i++)
	{
		_cs = _cs^_MBuf[_i];
	}
	_cs = _cs ^ 0xFF;
	return _cs;
};

