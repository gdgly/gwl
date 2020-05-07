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

#include "EventRecord.h"

extern unsigned char m_EC1;
extern unsigned char m_EC2;
extern unsigned char m_ErcNum[64];	//同一类事件的数量，记录最多10个
extern u8 m_ErcReport[EveMax];		//主动上报事件标志
extern char EventBuf[EveMax][528];


int sqlite_lock_inited = -1;
pthread_mutex_t sqlite_lock;
//加互斥锁
void lock_sqlite()
{
  if(sqlite_lock_inited < 0)
  {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,0);
    sqlite_lock_inited = pthread_mutex_init(&sqlite_lock,&attr);
    pthread_mutexattr_destroy(&attr);
    printf("init sqlite lock result %d\n",sqlite_lock_inited);
  }
  if(sqlite_lock_inited >= 0)
  	pthread_mutex_lock(&sqlite_lock);
  return;
}

//解锁
void unlock_sqlite()
{
  if (sqlite_lock_inited >= 0)
    pthread_mutex_unlock(&sqlite_lock);
  return;
}

/*************************************************
函 数 名: DBopen
功能描述: 打开数据库
输    入: szFile 文件名
输    出: 无
返    回: 0 失败， 0< 成功
*************************************************/
sqlite3* DBopen(const char* szFile)
{
	lock_sqlite();			//加互斥锁
	sqlite3* mpDB = NULL;
	int nRet = sqlite3_open(szFile, &mpDB);

	if (nRet != SQLITE_OK)
	{
		unlock_sqlite();
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
	unlock_sqlite();		//解锁
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

	//创建重要事件存储表
	sql = "create table if not exists ImportanceEvent (ERC int , data unsigned char, time long int, Report int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{	
		DBclose(mpDB);
		printf("create ImportanceEvent table error : %s\n",errmsg);		
		return -1;
	}

	//创建一般事件存储表
	sql = "create table if not exists ordinaryEvent (ERC int , data unsigned char, time long int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{		
		DBclose(mpDB);
		printf("create ordinaryEvent table error : %s\n",errmsg);		
		return -1;
	}

	//创建重要事件计数器存储表
	sql = "create table if not exists importanceEC (EC1 int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{		
		DBclose(mpDB);
		printf("create ordinaryEvent table error : %s\n",errmsg);		
		return -1;
	}

	//创建一般事件计数器存储表
	sql = "create table if not exists ordinaryEC (EC2 int);";
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
	//printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	//printf("===========================\n"); 	
	return 0;
}





/*************************************************
函 数 名: InsertordinaryEvent
功能描述: 写入一般事件内容
输    入:  ERC:事件类型  , data:数据, time:记录事件时间
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int InsertordinaryEvent(u8 ERC, char *data, u32 time)
{
    int rsl;
    sqlite3* mpDB = NULL; 
	char *errmsg; 
	char sql[128] = {0};
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;

	sprintf(sql, "insert into ordinaryEvent values('%d', '%s', '%d');", ERC, data, time);	
	
	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg); 
	
    DBclose(mpDB);
    if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
 	return 0;
}


/*************************************************
函 数 名: InsertImportanceEvent
功能描述: 写入重要事件内容
输    入: ERC:事件类型  , data:数据, time:记录事件时间
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int InsertImportanceEvent(u8 ERC, char *data, u32 time, int report)
{
	sqlite3* mpDB = NULL; 
	char *errmsg = NULL; 
	char sql[128] = {0};
    int rsl;    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;      

	sprintf(sql, "insert into ImportanceEvent values('%d', '%s', '%d', '%d');", ERC, data, time,report);	

	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg); 
	
	DBclose(mpDB);
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
 	return 0;
}



/*************************************************
函 数 名: DeleteImportanceEvent
功能描述: 删除最早的重要事件
输    入:   ERC：要删除的事件类型,(0~64)      0:删除所有事件 old = 1 删除最早一个
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
函 数 名: DelimpERC
功能描述: 删除重要事件全部ERC记录
输    入:   ERC：要删除的事件类型(0~64)      
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int DelimpERC(int ERC)
{
    int rsl;
    sqlite3* mpDB = NULL; 
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL || ERC<1 || ERC > 64)
        return -1;

	rsl = execFormatDML(mpDB, "delete from ImportanceEvent where ERC == %d;", ERC);

    DBclose(mpDB);
    return ( rsl == SQLITE_OK ) ? 0 : -1;
}


/*************************************************
函 数 名: DelordERC
功能描述: 删除一般事件全部ERC记录
输    入:   ERC：要删除的事件类型(0~64)      
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int DelordERC(int ERC)
{
    int rsl;
    sqlite3* mpDB = NULL; 
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL || ERC<1 || ERC > 64)
        return -1;

	rsl = execFormatDML(mpDB, "delete from ordinaryEvent where ERC == %d;", ERC);

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
	char sql[128] = {0};
	int ercname = ERC;
	char *errmsg; 	
    sqlite3* mpDB = NULL; 
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL || ERC < 0 || ERC > 64)
        return -1;
    if(ERC == 0)
		sprintf(sql, "select count(*) from ImportanceEvent;");
	else	
		sprintf(sql, "select count(*) from ImportanceEvent where ERC = %d;",ERC);
	rsl = sqlite3_exec(mpDB, sql, getImportancenum, (void *)&ercname, &errmsg); 
	
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
		//printf("m_EC1 = %d\n", m_EC1);	
	}
	else
	{
		sscanf(columnvalue[0], "%d", (int *)&m_ErcNum[*(int *)para - 1]);	
		//printf("m_ErcNum[%d] = %d\n", ERC-1, m_ErcNum[*(int *)para - 1]);	
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
	char sql[128] = {0};    
	char *errmsg;  
    sqlite3* mpDB = NULL;
	int ercname = ERC;
    mpDB  = DBopen(FILENAME);
	if(mpDB == NULL || ERC < 0 || ERC > 64)
        return -1;
    if(ERC == 0)
		sprintf(sql, "select count(*) from ordinaryEvent;");
	else	
		sprintf(sql, "select count(*) from ordinaryEvent where ERC = %d;",ERC);
		
	rsl = sqlite3_exec(mpDB, sql, getordinarynum, (void *)&ercname, &errmsg); 
	
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
		//printf("m_EC1 = %d\n", m_EC2);	
	}
	else
	{
		sscanf(columnvalue[0], "%d", (int *)&m_ErcNum[*(int *)para - 1]);	
		//printf("m_ErcNum[%d] = %d\n", ERC-1, m_ErcNum[*(int *)para  - 1]);	
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
int GetImportanceEvent(int Pm, int Pn)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	int GetImportanceEvent_sum = 0;
	memset(EventBuf, 0, sizeof(EventBuf));
	if(Pm <= Pn)
	{ 
		sprintf(sql, "select * from ImportanceEvent limit %d,%d;", Pm, Pn);
	}

	if(Pm > Pn)
	{
		sprintf(sql, "select * from ImportanceEvent limit 0,%d;",Pn);
		rsl = sqlite3_exec(mpDB, sql, GetImportanceEvent_back, (void *)&GetImportanceEvent_sum, &errmsg); 
		sprintf(sql, "select * from ImportanceEvent limit %d,80;",Pm);
	}
      		
	rsl = sqlite3_exec(mpDB, sql, GetImportanceEvent_back, (void *)&GetImportanceEvent_sum, &errmsg); 
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
	
	//printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	printf("===========================\n");

	int len = strlen(columnvalue[1]);

	memcpy(EventBuf[*(int *)para], columnvalue[1], len);
	
	(*(int *)para)++;		//获取记录的个数
	return 0;
}

/*************************************************
函 数 名: GetordinaryEvent
功能描述: 获取记录一般事件
输    入: Pm:起始指针  Pn:结束指针
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetordinaryEvent(u8 Pm, u8 Pn)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;
	
    mpDB  = DBopen(FILENAME);
	if(mpDB == NULL)
		return -1;
	int GetordinaryEvent_sum = 0;
	memset(EventBuf, 0, sizeof(EventBuf));
	if(Pm < Pn)
	{ 
		sprintf(sql, "select * from ordinaryEvent limit %d,%d;", Pm, Pn);
	}

	if(Pm > Pn)
	{
		sprintf(sql, "select * from ordinaryEvent limit 0,%d;",Pn);
		rsl = sqlite3_exec(mpDB, sql, GetordinaryEvent_back, (void *)&GetordinaryEvent_sum, &errmsg); 
		sprintf(sql, "select * from ordinaryEvent limit %d,80;",Pm);
	}
	
	rsl = sqlite3_exec(mpDB, sql, GetordinaryEvent_back, (void *)&GetordinaryEvent_sum, &errmsg); 
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
	
	//printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	//printf("===========================\n");

	int len = strlen(columnvalue[1]);

	memcpy(EventBuf[*(int *)para], columnvalue[1], len);
	
	(*(int *)para)++;		//获取记录的个数
	return 0;
}	



/*************************************************
函 数 名: GetErcEvent
功能描述: 按照ERC代码获取事件记录
输    入: Pm:起始指针  Pn:结束指针
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int	GetERCEvent_sum;
int GetErcEvent(int ERC)
{
	int rsl;
	char sql[128] = {0};	
	char *errmsg; 
	sqlite3* mpDB = NULL;

	GetERCEvent_sum = 0;
	
	mpDB  = DBopen(FILENAME);
	if(mpDB == NULL)
		return -1;
	memset(EventBuf, 0, sizeof(EventBuf));
 
	sprintf(sql, "select * from ImportanceEvent where ERC=%d;", ERC);
	rsl = sqlite3_exec(mpDB, sql, GetERCEvent_back, (void *)&GetERCEvent_sum, &errmsg); 
	sprintf(sql, "select * from ordinaryEvent where ERC=%d;", ERC);
	rsl = sqlite3_exec(mpDB, sql, GetERCEvent_back, (void *)&GetERCEvent_sum, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return GetERCEvent_sum;		//返回获取事件的个数
}

int GetERCEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;
	//printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	//printf("===========================\n");

	int len = strlen(columnvalue[1]);

	memcpy(EventBuf[*(int *)para], columnvalue[1], len);
	
	(*(int *)para)++;		//获取记录的个数
	return 0;
}	





/*************************************************
函 数 名: GetEventROW
功能描述: 获取事件的上报标志
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetEventRow()
{
    int rsl;
   	char *sql;    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	
	int reprotnum = 0;

	//清空标志位
	memset(m_ErcReport, 0, sizeof(m_ErcReport));
	
	sql = "select Report from ImportanceEvent;" ;		
	rsl = sqlite3_exec(mpDB, sql, GetEventRowback, (void *)&reprotnum, &errmsg); 
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
	//printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	sscanf(columnvalue[0],"%d", (int *)&m_ErcReport[*(int *)para]);
	//printf("m_ErcReport = %d\n",m_ErcReport[*(int *)para]);
	(*(int *)para)++;
	return 0;
}



/*************************************************
函 数 名: UpdateEventRow
功能描述: 更新事件的上报标志
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int UpdateEventRow(u8 row)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;  

	int back_row = 0;
	
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	

	sprintf(sql, "select rowid from ImportanceEvent limit %d,%d;", 0, row+1);	//获取行号
		
	rsl = sqlite3_exec(mpDB, sql, UpdateEventRow_back, (void *)&back_row, &errmsg); 
	memset(sql, 0, sizeof(sql));
	printf("back_row = %d\n\n",back_row);
	sprintf(sql, "update ImportanceEvent set Report = 0 where rowid = %d;", back_row);			//有的可以执行，有的不行
	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg); 
	
	DBclose(mpDB);
	if(rsl != SQLITE_OK)	
	{		
		printf("update error : %s\n",errmsg);	
		return -1;
	} 
	return 0;
}


int UpdateEventRow_back(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	

	*(int *)para = atol(columnvalue[0]);
	return 0;
}





/*************************************************
函 数 名: GetEC1
功能描述: 获取事件计数器
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetEC1()
{
    int rsl;
   	char *sql;    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	
	int EC1 = 0;

	//清空标志位
	memset(m_ErcReport, 0, sizeof(m_ErcReport));
	
	sql = "select * from importanceEC;" ;		
	rsl = sqlite3_exec(mpDB, sql, GetEC1back, (void *)&EC1, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return EC1;			//返回重要事件计数器
}
	
int GetEC1back(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;	
	//printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	sscanf(columnvalue[0],"%d", (int *)para);
	return 0;
}

/*************************************************
函 数 名: GetEC2
功能描述: 获取一般事件计数器
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetEC2()
{
    int rsl;
   	char *sql;    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	
	int EC2 = 0;

	//清空标志位
	memset(m_ErcReport, 0, sizeof(m_ErcReport));
	
	sql = "select * from ordinaryEC;" ;		
	rsl = sqlite3_exec(mpDB, sql, GetEC2back, (void *)&EC2, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return EC2;		//返回一般事件计数器
}
	
int GetEC2back(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;	
	//printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++)	
	{		
		//printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]);	
	}	
	sscanf(columnvalue[0],"%d", (int *)para);
	return 0;
}


/*************************************************
函 数 名: UpdateEC1
功能描述: 更新重要事件计数器
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int UpdateEC1(u8 EC1)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	
	//清空标志位
	memset(m_ErcReport, 0, sizeof(m_ErcReport));

	rsl = sqlite3_exec(mpDB, "delete from importanceEC", NULL, NULL, &errmsg); 

	sprintf(sql, "insert into importanceEC values('%d');",EC1);
	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;			//返回重要事件计数器
}





/*************************************************
函 数 名: UpdateEC2
功能描述: 更新重要事件计数器
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int UpdateEC2(u8 EC2)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	
	//清空标志位
	memset(m_ErcReport, 0, sizeof(m_ErcReport));

	rsl = sqlite3_exec(mpDB, "delete from ordinaryEC", NULL, NULL, &errmsg); 

	sprintf(sql, "insert into ordinaryEC values('%d');",EC2);
	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;			//返回重要事件计数器
}





/*************************************************
函 数 名: UpdatePower
功能描述: 更新停上电事件
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int UpdatePower(char *data, u32 time)
{
    int rsl;
   	char sql[128] = {0};    
	char *errmsg; 
    sqlite3* mpDB = NULL;  
    
    mpDB = DBopen(FILENAME);
    if(mpDB == NULL)
        return -1;
	
	sprintf(sql, "update ImportanceEvent set Report = 1  date=%s time=%d ;", data, time);
	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg); 

	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;			//返回重要事件计数器
}
























#if 0
/*************************************************
函 数 名: GetEventByhang
功能描述: 
输    入: 
输    出: 无
返    回: <0 失败， >=0 成功
*************************************************/
int GetEventByhang(int row)
{
	int rsl;
	char sql[128] = {0};	  
	char *errmsg; 
	sqlite3* mpDB = NULL;  
	
	mpDB = DBopen(FILENAME);
	if(mpDB == NULL)
		return -1;
	reprotnum = 0;

	sprintf(sql, "select * from ImportanceEvent limit %d,%d;", row, row);
	rsl = sqlite3_exec(mpDB, sql, GetEventhangback, NULL, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return 0;
}
	
int GetEventhangback(void *para, int ncolumn, char **columnvalue, char **columnname)
{
	int i;	
	printf("total column is %d\n",ncolumn); 	
	for(i = 0;i < ncolumn; i++) 
	{		
		printf("col_name:%s----> clo_value:%s\n",columnname[i],columnvalue[i]); 
	}	

	return 0;
}

#endif



