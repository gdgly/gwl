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
//#define APN_CONF_FILE_PATH "/mnt/conf/apn_para.conf"
#define REMOTE_MODULE_CONF_PATH 	"/mnt/data/concentrator/remote_com_modal_version.conf"
#define REMOTE_PARA_FILE_PATH 		"/mnt/conf/remote_para.conf"
//设置电流回路使能参数
#define CURRENT_CIRCUIT_ENABLE_PATH    "./config/current_circuit_param.conf"
#define TERM_SOFT_VERSION				"./terminal_version.conf"
#define TERM3761_PARA_CONF			"./conf/term3761.conf"
#define TERM3761_PARA_CONF_INIT		"./conf/term3761_init.conf"
#define TERM_IP_CONF_FILE			"/etc/network/interfaces"


int writefile(char *filename,char *buf,int len, char* mode);
int readfile(char *filename,char *buf,int len, char * mode);
int cpyline(char* dest,long pos,const char* src);
int cpykeyvalue(char* dest,long pos,const char* name,const char* value,char delimiter);								//由delsaveconfparam调用
int mygetline(char **lineptr, unsigned int *n, FILE *stream);
int delsaveconfparam(const char* confile, const char*name, int namelen, const char*value, char delimiter);			//由saveConfPara调用
int saveConfPara(const char* confile, const char*name, const char*value, char delimiter);							//写入参数-x，终端的参数都通过这个文件
int getConfPara(const char* confile, const char*name, char*value, int value_size, char delimiter);					//读取参数-x
int GetFileDataGroup(const char* confile,char *para[],int group);


int SaveConfMultiGroup(const char* confile, char *para[],int group);


#endif

