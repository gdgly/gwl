#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdarg.h>
#include "SQLite.h"
#include <stdarg.h>
#include "Sgd_types.h"
#include "EventRecord.h"

extern unsigned char m_EC1;
extern unsigned char m_EC2;
extern unsigned char m_ErcNum[64];	//同一类事件的数量，记录最多10个
extern u8 m_ErcReport[EveMax];		//主动上报事件标志


/*************************************************
函 数 名: DBopen
功能描述: 打开数据库
输    入: szFile 文件名
输    出: 无
返    回: 0 失败， 0< 成功
*************************************************/
sqlite3* DBopen(const char* szFile)
{
	sqlite3* mpDB = NULL;
	int nRet = sqlite3_open(szFile, &mpDB);

	if (nRet != SQLITE_OK)
	{
		return NULL;
	}

	sqlite3_busy_timeout(mpDB, 60000);  //60s
	return mpDB;
}


/*************************************************
函 数 名: DBclose
功能描述: 关闭数据库
输    入: mpDB:要关闭的数据库句柄
输    出: 无
返    回: 无
*************************************************/
void DBclose(sqlite3* mpDB)
{
	if (mpDB != NULL)
	{
		sqlite3_close(mpDB);
		mpDB = 0;
	}
}


/*************************************************
函 数 名: create_table
功能描述: 表没有存在，创建表
输    入: db:数据库句柄
输    出: 无
返    回: 成功：0       失败：-1
*************************************************/
int create_table()
{  
	int rsl; 
	char *sql;    
	char *errmsg; 
	
	sqlite3 *mpDB = NULL;
	
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;

	sql = "create table if not exists ImportanceEvent (ERC int , data unsigned char, time long int, Report int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{	
		DBclose(mpDB);
		printf("create ImportanceEvent table error : %s\n",errmsg);		
		return -1;
	}

	sql = "create table if not exists ordinaryEvent (ERC int , data unsigned char, time long int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{		
		DBclose(mpDB);
		printf("create ordinaryEvent table error : %s\n",errmsg);		
		return -1;
	}
	DBclose(mpDB);
	return 0;
}



/*************************************************
函 数 名: DBSetValue
功能描述: 数据写入文件
输    入: str 数据项， pvalue 数据
输    出: 无
返    回: <0 失败， >=0 成功
************************************************
int DBSetValue( char *str, unsigned long long pvalue )
{
    int rsl;
    
    rsl = DBopen(FILENAME);
    if(rsl < 0)
        return -1;
        
    rsl = execFormatDML( "INSERT OR REPLACE INTO module values( '%s', '%lld' );", str, pvalue );
    DBclose();
    return ( rsl == SQLITE_OK ) ? 0 : -1;
}		*/



/*************************************************
函 数 名: execFormatDML
功能描述: 
输    入: 
输    出: 无
返    回: 操作结果
*************************************************/
int execFormatDML(sqlite3* mpDB, const char* format, ...)
{
	char* szError=0;
	int nRet;
	char *mpSqliteStr;
	//clear( );
	va_list va;
	va_start(va, format);
	mpSqliteStr = sqlite3_vmprintf( format, va);
	va_end(va);
	
	nRet = sqlite3_exec(mpDB, mpSqliteStr, 0, 0, &szError);
	if (nRet == SQLITE_OK)
	{
		return sqlite3_changes(mpDB) ? SQLITE_OK : SQLITE_ERROR;	//返回变化的行数目,不为0;
	}
	return SQLITE_ERROR;
}

/*************************************************
函 数 名: TableIsExit
功能描述: 判断表是否存在,不存在创建表
输    入:  TableName：表名     mpDB：数据库句柄
输    出: 无
返    回: 存在：0       不存在：-1
*************************************************/
int TableIsExit(char *TableName, sqlite3* mpDB)
{
	int rsl;
	char *errmsg; 
	char sql[128] = {0};
	if (mpDB == NULL || TableName ==NULL)
	{
		return -1;
	}
	
	sprintf(sql, "SELECT COUNT(*) FROM sqlite_master where type='table' and name='%s';", TableName);
	rsl = sqlite3_exec(mpDB, sql, getTableIsExit, NULL, &errmsg);
	DBclose(mpDB);
    //return ( rsl == SQLITE_OK ) ? 0 : -1;
    
    if(rsl != SQLITE_OK)	
	{		
		printf("TableIsExit table error : %s\n",errmsg);		
		return -1;
	}
	return 0;
}

int getTableIsExit(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;	
	printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	printf("===========================\n"); 	
	return 0;
}



/*************************************************
函 数 名: InsertImportanceEvent
功能描述: 写入重要事件内容
输    入: ERC:事件类型  , data:数据, time:记录事件时间
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int InsertImportanceEvent(int ERC, char *data, unsigned long long time, int report)
{
	sqlite3* mpDB = NULL; 
    int rsl;    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
        
    rsl = execFormatDML(mpDB, "insert into ImportanceEvent values('%d', '%s', '%lld', '%d');", ERC, data, time,report);
    DBclose(mpDB);
    return ( rsl == SQLITE_OK ) ? 0 : -1;
}


/*************************************************
函 数 名: InsertordinaryEvent
功能描述: 写入一般事件内容
输    入:  ERC:事件类型  , data:数据, time:记录事件时间
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int InsertordinaryEvent(int ERC, char *data, unsigned long long time )
{
    int rsl;
    sqlite3* mpDB = NULL; 
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
        
    rsl = execFormatDML(mpDB, "insert into ordinaryEvent values('%d', '%s', '%lld');", ERC, data, time);
    DBclose(mpDB);
    return ( rsl == SQLITE_OK ) ? 0 : -1;
}


/*************************************************
函 数 名: DeleteImportanceEvent
功能描述: 删除最早的重要事件
输    入:   ERC：要删除的事件类型,(0~64)      0:删除所有事件
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int DeleteImportanceEvent(int ERC)
{
    int rsl;   
    sqlite3* mpDB = NULL; 
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL || ERC<0 || ERC > 64)
        return -1;
    if(0 == ERC)
    {
		rsl = execFormatDML(mpDB, "delete from ImportanceEvent;", ERC);
	}
	else
	{
		rsl = execFormatDML(mpDB, "delete from ImportanceEvent where ERC == %d limit 0,1;", ERC);
	}
    DBclose(mpDB);
    return ( rsl == SQLITE_OK ) ? 0 : -1;
}



/*************************************************
函 数 名: DeleteordinaryEvent
功能描述: 删除最早的一般事件
输    入:   ERC：要删除的事件类型(0~64)      0:删除所有事件
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int DeleteordinaryEvent(int ERC)
{
    int rsl;
    sqlite3* mpDB = NULL; 
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL || ERC<0 || ERC > 64)
        return -1;
    if(0 == ERC)
    {
		rsl = execFormatDML(mpDB, "delete from ordinaryEvent;", ERC);
	}
	else
	{
		rsl = execFormatDML(mpDB, "delete from ordinaryEvent where ERC == %d limit 0,1;", ERC);
	}

    DBclose(mpDB);
    return ( rsl == SQLITE_OK ) ? 0 : -1;
}


/*************************************************
函 数 名: GetImportanceEvent_Num
功能描述: 获取记录重要事件的数量
输    入:   ERC:事件类型(0~64), 0 时为获取所有事件的个数
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetImportanceEvent_Num(int ERC)
{
    int rsl; 
	char *sql;    
	char *errmsg; 	
    sqlite3* mpDB = NULL; 
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL || ERC < 0 || ERC > 64)
        return -1;
    if(ERC == 0)
		sql = "select count(*) from ImportanceEvent;";
	else	
		sprintf(sql, "select count(*) from ImportanceEvent where ERC = %d;",ERC);
	rsl = sqlite3_exec(mpDB, sql, getImportancenum, &ERC, &errmsg); 
	
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);
		return -1;
	} 
	return 0;
}

int getImportancenum(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int ERC = 0;;	
	ERC = *(int *)para;
	if(ERC == 0)
	{
		sscanf(columnvalue[0], "%d", (int *)&m_EC1);	
		printf("m_EC1 = %d\n", m_EC1);	
	}
	else
	{
		sscanf(columnvalue[0], "%d", (int *)&m_ErcNum[*(int *)para]);	
		printf("m_ErcNum[%d] = %d\n", ERC, m_ErcNum[*(int *)para]);	
	}
	return 0;
}	


/*************************************************
函 数 名: GetordinaryEvent_Num
功能描述: 获取记录一般事件的数量
输    入:   ERC:事件类型(0~64), 0 时为获取所有事件的个数
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetordinaryEvent_Num(int ERC)
{
    int rsl;
	char *sql;    
	char *errmsg;  
    sqlite3* mpDB = NULL;
	
    mpDB  = DBopen(FILENAME);
	if(mpDB == NULL || ERC < 0 || ERC > 64)
        return -1;
    if(ERC == 0)
		sql = "select count(*) from ordinaryEvent;";
	else	
		sprintf(sql, "select count(*) from ordinaryEvent where ERC = %d;",ERC);
		
	rsl = sqlite3_exec(mpDB, sql, getordinarynum, &ERC, &errmsg); 
	
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);
		return -1;
	} 
	return 0;
}

int getordinarynum(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int ERC = 0;;	
	ERC = *(int *)para;
	if(ERC == 0)
	{
		sscanf(columnvalue[0], "%d", (int *)&m_EC2);	
		printf("m_EC1 = %d\n", m_EC2);	
	}
	else
	{
		sscanf(columnvalue[0], "%d", (int *)&m_ErcNum[*(int *)para]);	
		printf("m_ErcNum[%d] = %d\n", ERC, m_ErcNum[*(int *)para]);	
	}
	return 0;
}	

/*************************************************
函 数 名: GetImportanceEvent
功能描述: 获取记录重要事件
输    入: Pm:起始指针  Pn:结束指针
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetImportanceEvent_sum = 0;
int GetordinaryEvent_sum = 0;
char EventBuf[EveMax][528] = {0};
int GetImportanceEvent(int Pm, int Pn)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	GetImportanceEvent_sum = 0;
	if(Pm <= Pn)
	{ 
		sprintf(sql, "select * from ImportanceEvent limit %d,%d;", Pm, Pn);
	}

	if(Pm > Pn)
	{
		sprintf(sql, "select * from ImportanceEvent limit 0,%d;",Pn);
		rsl = sqlite3_exec(mpDB, sql, GetImportanceEvent_back, NULL, &errmsg); 
		sprintf(sql, "select * from ImportanceEvent limit %d,80;",Pm);
	}
      		
	rsl = sqlite3_exec(mpDB, sql, GetImportanceEvent_back, NULL, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;
}

int GetImportanceEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;
	
	printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	printf("===========================\n");

	int len = strlen(columnvalue[1]);

	memcpy(EventBuf[GetImportanceEvent_sum], columnvalue[1], len);
	
	GetImportanceEvent_sum++;		//获取记录的个数
	return 0;
}



/*************************************************
函 数 名: GetordinaryEvent
功能描述: 获取记录一般事件
输    入: Pm:起始指针  Pn:结束指针
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetordinaryEvent(int Pm, int Pn)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;
	
    mpDB  = DBopen(FILENAME);
	if(mpDB == NULL)
		return -1;
	GetordinaryEvent_sum = 0;
	memset(EventBuf, 0, sizeof(EventBuf));
	if(Pm < Pn)
	{ 
		sprintf(sql, "select * from ordinaryEvent limit %d,%d;", Pm, Pn);
	}

	if(Pm > Pn)
	{
		sprintf(sql, "select * from ordinaryEvent limit 0,%d;",Pn);
		rsl = sqlite3_exec(mpDB, sql, GetordinaryEvent_back, NULL, &errmsg); 
		sprintf(sql, "select * from ordinaryEvent limit %d,80;",Pm);
	}
	
	rsl = sqlite3_exec(mpDB, sql, GetordinaryEvent_back, NULL, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;
}


int GetordinaryEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;
	
	printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	printf("===========================\n");

	int len = strlen(columnvalue[1]);

	memcpy(EventBuf[GetordinaryEvent_sum], columnvalue[1], len);
	
	GetordinaryEvent_sum++;		//获取记录的个数
	return 0;
}	



/*************************************************
函 数 名: GetErcEvent
功能描述: 按照ERC代码获取时间记录
输    入: Pm:起始指针  Pn:结束指针
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetERCEvent_sum = 0;
int GetErcEvent(int ERC)
{
	int rsl;
	char sql[128] = {0};	
	char *errmsg; 
	sqlite3* mpDB = NULL;
	
	mpDB  = DBopen(FILENAME);
	if(mpDB == NULL)
		return -1;
	
	GetERCEvent_sum = 0;
	memset(EventBuf, 0, sizeof(EventBuf));
 
	sprintf(sql, "select * from ImportanceEvent where ERC=%d;", ERC);
	rsl = sqlite3_exec(mpDB, sql, GetERCEvent_back, NULL, &errmsg); 
	sprintf(sql, "select * from ordinaryEvent where ERC=%d;", ERC);
	rsl = sqlite3_exec(mpDB, sql, GetERCEvent_back, NULL, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;
}

int GetERCEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;
	
	printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	printf("===========================\n");

	int len = strlen(columnvalue[1]);

	memcpy(EventBuf[GetERCEvent_sum], columnvalue[1], len);
	
	GetERCEvent_sum++;		//获取记录的个数
	return 0;
}	











/*************************************************
函 数 名: GetEventROW
功能描述: 获取事件的上报标志
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int reprotnum = 0;
int GetEventRow()
{
    int rsl;
   	char *sql;    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	reprotnum = 0;
	sql = "select Report from ImportanceEvent;" ;		
	rsl = sqlite3_exec(mpDB, sql, GetEventRowback, NULL, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;
}
	
int GetEventRowback(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;	
	printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	sscanf(columnvalue[0],"%d", (int *)&m_ErcReport[reprotnum++]);
	printf("m_ErcReport = %d\n",m_ErcReport[reprotnum-1]);
	return 0;
}

