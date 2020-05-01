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
	m_MFd = open((char *)"/dev/spidev1.3",O_RDWR);
	if (m_MFd < 0)
	{
		#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamDevOpen error (open dev failed.\n)");
		#endif
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
#ifndef ACE_WIN32
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
	//m_MSendBuf[m_MSendPos++] = 0;//0xaa?//gabbage,2016-3-10
	m_MSendBuf[m_MSendPos++] = 0x55;
	if (MEsamGetLRCl(_MBuf,_MBufLen, &m_MSendPos) < 0)
	{
		printf("TMESAMBaseClass::MEsamDataSend MEsamGetLRCl failed _MBufLen %d\n",_MBufLen);
		return -1;
	}
	m_MSendLen = m_MSendPos;
	//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("1111m_MSendLen=%d\n"), m_MSendLen));
#ifdef MESAM_CLASS_DEBUG
	//printf("MEsamSend:");
	MY_ACE_ERROR((LM_ERROR,ACE_TEXT("TMESAMBaseClass::MEsamDataSend m_MSendLen=%d\n"), m_MSendLen));
	MY_ACE_ERROR((LM_ERROR,"MEsamSend:"));
	for (i = 0;i < m_MSendLen ; i++)
	{
		//printf("%02X ",m_MSendBuf[i]);
		MY_ACE_ERROR((LM_ERROR,ACE_TEXT("%02X "), m_MSendBuf[i]));
	}
	//printf("\n");
	MY_ACE_ERROR((LM_ERROR,"\n"));
#endif
	_pos = 0;
	/*
	while (_pos < m_MSendLen)
	{
		_len = m_MSendLen - _pos;
		_rs = write(m_MFd,&m_MSendBuf[_pos],_len);
		if (_rs > 0)
		{
			_pos = _pos + _rs;
		}
		else
		{
			return -1;
		}
	}
	*/
	//芯片手册:
	//南京南瑞集团通信与用电技术分公司 10
	//严格按照流程中对SSN信号的处理方式，禁止在数据传输中，将SSN置高
	//	SCK推荐速率2MHz---本程序1Mhz,见上面构造函数
	//在SSN置高后，T-ESAM将进入低功耗状态，高电平保持时间至少10μs以上（见图7），才能置低。接口设备将SSN再次置低后，需等待50μs以上时间（见图8），才能开始发送数据
	//发送数据字节之间延时需大于15μs

	struct spi_ioc_transfer tr[1];
		memset(tr,0,sizeof(tr));

		//spidev_sync->spi_async->__spi_async-->atmel_spi_transfer	
		//加入队列list_add_tail(&msg->queue, &as->queue)没传输(!as->current_transfer)调atmel_spi_next_message开始传输

		//atmel_spi_cleanup(文件关闭时spidev_release调用)若保存上次spi是被关闭的设备,清上次设备指针,cs置无效
		//atmel_spi_setup(spi_add_device调用,增加从设备,不是接口)若已有atmel_spi_device,也清此指针,cs置无效

		//atmel_spi.c message->transfer			atmel_spi_next_message/atmel_spi_tasklet_func->atmel_spi_next_xfer(0长度第归)
		//第一个atmel_spi_next_message开始传输如果本次spi设备不是该spi接口上次结束传输的话,上次spi设备置cs无效,无论异同都清保存的上次设备指针;
		//目前NANDFLASH/ESAM是不同的spi接口,同一接口上发起传输的spi不会不同
		//本次spi设备cs置有效

		tr[0].tx_buf = (unsigned long)m_MSendBuf;
		//tr[0].rx_buf = NULL;
		tr[0].len = m_MSendLen;

		//delay_usecs:
		//atmel_spi_tasklet_func(dma传输完成中断调用)中传输完成后(无论收发)延迟,
		//cs_change为1时(无论收发)也用作cs脉宽
		//发送前延时:
		//atmel_spi_next_xfer_pio(内核config中dma配置为关闭或dma错误时调用)中发送每字节前延时
		//atmel_spi_pump_pio_data发送每字节前延时(中断处理程序RDRF时调用,接收/发送字节)
		//atmel_spi_next_xfer_dma每次都既启动发送dma也启动接收dma,如果是发送,发送开始发送dma前延时
		//	(接收dma开始前不延时)
		//xfer的tx_buf/rx_buf(只有一个非空)用的是4096的struct spidev_data* spidev的buffer内部缓冲区(PAGE_SIZE),空的那个用
		//struct atmel_spi	*as的buffer_dma内部缓冲区(PAGE_SIZE),dma长度收发都是xfer->len
		//收时会先清0发dma缓冲(即接收期间MOSI保持0)

		tr[0].delay_usecs = 100;	

		//tr[0].speed_hz = speed;
		//tr[0].bits_per_word = bits;

		//cs_change为1:atmel_spi_tasklet_func中多transfer时非最后一个transfer完成时继续传输前脉冲一次cs(先无效再有效,宽度为delay_usecs,至少1us),
		//最后一个transfer完成且无错时则保持cs不变,并记住此spi设备
		//cs_change为0:atmel_spi_tasklet_func中多transfer时非最后一个transfer前不改变cs,
		//最后一个transfer完成或出错时cs置无效
		//tr[0].cs_change = 0;	

		//1次ioctl调用spidev.c形成一个message,里面可有多个transfer,spidev_sync同步传输操作(等待传输完成)
						//spidev_sync返回actual_length

		_rs = ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);
		if (_rs < 1)
		{
			MEsamDevOpen();
			//printf("can't send spi message");
			//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("TMESAMBaseClass::MEsamDataSend failed!!! ERRNO %d\n"),errno));
			return -1;
		}
		//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataSend success!!!\n"));
		
#endif
	return 0;
}

int MEsamDataRecv(int _WakeDt, unsigned char m_MRecvBuf[MESAM_RECV_BUF_MAX_LEN])
{
	//int _rs = 0;
	int _num = 0;
	int _bytesCount = 0;
	int m_MRecvLen = 0;//帧长度
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
			//_len = read(m_MFd,_Buf,_bytesCount);
			//memset((void*)tr[0].rx_buf,0,tr[0].len);
			//usleep(10);
			tr[0].len = _bytesCount;
			int ret = ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);
			if (ret < _bytesCount)//不应该发生,片选有效后spi采样MISO总收到东西
			{
				//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataRecv failed!!!\n"));
				++_num;
				if (last_char_time && (long)(get_sys_uptime() - last_char_time) >= ESAM_FRAME_BREAK)
				{
					//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataRecv frame break!!!\n"));
					printf("MEsamDataRecv frame break!!!\n");
					m_MRecvLen = 0;
					break;
				}
				if (_num > 100)
				{
					_num = 0;
					mymsleep(0);//广播和组地址测试个别不合格(设置组地址,广播地址设置参数)
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
				//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataRecv success!!!\n"));
				//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataRecv data:"));
				int _i;
				for (_i = 0;_i < frame_got_part;_i++ )
				{
					if (_Buf[_i] == 0x55)//frame start done
					{
						if (phrase == 0)
						{
							//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataRecv found 0x55 !!!\n"));
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
									//MY_ACE_ERROR((LM_ERROR,"TMESAMBaseClass::MEsamDataRecv frame len %d too large\n",m_MRecvLen));
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
									//MY_ACE_ERROR((LM_ERROR, "\nTMESAMBaseClass::MEsamDataRecv LRC wrong m_MRecvLen %d _cs 0x%x->0x%x!!!\n",m_MRecvLen,_cs,_Buf[m_MRecvLen - 1]));
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
								tr[0].delay_usecs = 0;//内核不延时
								tr[0].cs_change = 0;
								tr[0].len = 1;//if 0, driver will not return
								tr[0].tx_buf = NULL;
								tr[0].rx_buf = (unsigned long)&_i;
								ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);

								mymsleep(1);//用户态延时(cs无效持续一会)


								//return 0;
								return m_MRecvLen;
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
					assert(_bytesCount > 0);
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
		tr[0].delay_usecs = 0;//内核不延时
		tr[0].len = 1;
		tr[0].tx_buf = NULL;
		tr[0].rx_buf = (unsigned long)&frame_got_part;
		ioctl(m_MFd, SPI_IOC_MESSAGE(1), tr);
		mymsleep(1);//用户态延时(cs无效持续一会)
		//MY_ACE_ERROR((LM_ERROR, "\nTMESAMBaseClass::MEsamDataRecv fail total %d phrase %d!!!",total,phrase));
		printf("MEsamDataRecv fail total %d phrase %d!!!", total,phrase);

	}
#endif
	return -1;
}

int MEsamParaSet(void)
{
	unsigned char	m_EsamMode = 3;
	unsigned char	m_EsamBits = 8;
	unsigned int m_EsamSpeed = 1*1000*1000;//1Mhz
#ifndef ACE_WIN32
	if (ioctl(m_MFd,SPI_IOC_WR_MODE,&m_EsamMode)!=0)
	{
		#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_WR_MODE)\n");
		#endif
		return -1;
	}
	if (ioctl(m_MFd,SPI_IOC_WR_BITS_PER_WORD,&m_EsamBits)!=0)
	{
		#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_WR_BITS_PER_WORD)\n");
		#endif
		return -1;
	}

	if (ioctl(m_MFd,SPI_IOC_WR_MAX_SPEED_HZ,&m_EsamSpeed)!=0)
	{
		#ifdef MESAM_CLASS_DEBUG
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_WR_MAX_SPEED_HZ)\n");
		#endif
		return -1;
	}
	#ifdef MESAM_CLASS_DEBUG
	if (ioctl(m_MFd,SPI_IOC_RD_MODE,&m_EsamMode)!=0)
	{
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_RD_MODE)\n");
	}
	if (ioctl(m_MFd,SPI_IOC_RD_BITS_PER_WORD,&m_EsamBits)!=0)
	{
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_RD_BITS_PER_WORD)\n");
	}
	if (ioctl(m_MFd,SPI_IOC_RD_BITS_PER_WORD,&m_EsamSpeed)!=0)
	{
		printf("TMESAMBaseClass::MEsamParaSet error (SPI_IOC_RD_BITS_PER_WORD)\n");
	}
	printf("%s:T-ESAM mode %d,%d bits,%d MHz Max\n",m_EsamDevName,m_EsamMode,m_EsamBits,m_EsamSpeed/1000/1000);
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
	m_MSendBuf[*pos++] = _cs ^ 0xFF;
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

