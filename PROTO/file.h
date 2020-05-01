#ifndef _FILE_H_
#define _FILE_H_
#include <stdio.h>
#define GPRS_SIGNAL_PATH   	 		"/var/run/gprs-signal.txt"
#define GPRS_PIN_PATH       		"/var/run/gprs-pin.txt"
#define GPRS_REG_PATH       		"/var/run/gprs-reg.txt"
#define GPRS_OPERATOR_PATH  		"/var/run/gprs-operator.txt"
#define CONNECTMAINSTATION_PATH 	"/var/run/mainstation_active.log"
#define MAINSTATION_CONN_FILE 		"/var/run/mainstation_conn_conc.log"
#define CONNECTMAINSTATION_LOG_PATH "/mnt/mainstation_connect.log"
#define TERM698_CONF_FILE_PATH 		"./term698.conf"			//"/mnt/data/concentrator/term698.conf"
//#define APN_CONF_FILE_PATH "/mnt/conf/apn_para.conf"
#define REMOTE_MODULE_CONF_PATH 	"/mnt/data/concentrator/remote_com_modal_version.conf"
#define REMOTE_PARA_FILE_PATH 		"/mnt/conf/remote_para.conf"

int writefile(char *filename,char *buf,int len, char* mode);
int readfile(char *filename,char *buf,int len, char * mode);
int cpyline(char* dest,long pos,const char* src);
int cpykeyvalue(char* dest,long pos,const char* name,const char* value,char delimiter);								//由delsaveconfparam调用
int mygetline(char **lineptr, unsigned int *n, FILE *stream);
int delsaveconfparam(const char* confile, const char*name, int namelen, const char*value, char delimiter);			//由saveConfPara调用
extern int saveConfPara(const char* confile, const char*name, const char*value, char delimiter);							//写入参数-x，终端的参数都通过这个文件
extern int getConfPara(const char* confile, const char*name, char*value, int value_size, char delimiter);					//读取参数-x
int SaveConfMultiGroup(const char* confile, char *para[],int group);

#endif

