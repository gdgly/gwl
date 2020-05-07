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
extern unsigned char m_ErcNum[64];	//ͬһ���¼�����������¼���10��
extern u8 m_ErcReport[EveMax];		//�����ϱ��¼���־
extern char EventBuf[EveMax][528];


int sqlite_lock_inited = -1;
pthread_mutex_t sqlite_lock;
//�ӻ�����
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

//����
void unlock_sqlite()
{
  if (sqlite_lock_inited >= 0)
    pthread_mutex_unlock(&sqlite_lock);
  return;
}

/*************************************************
�� �� ��: DBopen
��������: �����ݿ�
��    ��: szFile �ļ���
��    ��: ��
��    ��: 0 ʧ�ܣ� 0< �ɹ�
*************************************************/
sqlite3* DBopen(const char* szFile)
{
	lock_sqlite();			//�ӻ�����
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
�� �� ��: DBclose
��������: �ر����ݿ�
��    ��: mpDB:Ҫ�رյ����ݿ���
��    ��: ��
��    ��: ��
*************************************************/
void DBclose(sqlite3* mpDB)
{
	if (mpDB != NULL)
	{
		sqlite3_close(mpDB);
		mpDB = 0;
	}
	unlock_sqlite();		//����
}


/*************************************************
�� �� ��: create_table
��������: ��û�д��ڣ�������
��    ��: db:���ݿ���
��    ��: ��
��    ��: �ɹ���0       ʧ�ܣ�-1
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

	//������Ҫ�¼��洢��
	sql = "create table if not exists ImportanceEvent (ERC int , data unsigned char, time long int, Report int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{	
		DBclose(mpDB);
		printf("create ImportanceEvent table error : %s\n",errmsg);		
		return -1;
	}

	//����һ���¼��洢��
	sql = "create table if not exists ordinaryEvent (ERC int , data unsigned char, time long int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{		
		DBclose(mpDB);
		printf("create ordinaryEvent table error : %s\n",errmsg);		
		return -1;
	}

	//������Ҫ�¼��������洢��
	sql = "create table if not exists importanceEC (EC1 int);";
 	rsl = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(rsl != SQLITE_OK)	
	{		
		DBclose(mpDB);
		printf("create ordinaryEvent table error : %s\n",errmsg);		
		return -1;
	}

	//����һ���¼��������洢��
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
�� �� ��: DBSetValue
��������: ����д���ļ�
��    ��: str ����� pvalue ����
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: execFormatDML
��������: 
��    ��: 
��    ��: ��
��    ��: �������
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
		return sqlite3_changes(mpDB) ? SQLITE_OK : SQLITE_ERROR;	//���ر仯������Ŀ,��Ϊ0;
	}
	return SQLITE_ERROR;
}

/*************************************************
�� �� ��: TableIsExit
��������: �жϱ��Ƿ����,�����ڴ�����
��    ��:  TableName������     mpDB�����ݿ���
��    ��: ��
��    ��: ���ڣ�0       �����ڣ�-1
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
�� �� ��: InsertordinaryEvent
��������: д��һ���¼�����
��    ��:  ERC:�¼�����  , data:����, time:��¼�¼�ʱ��
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: InsertImportanceEvent
��������: д����Ҫ�¼�����
��    ��: ERC:�¼�����  , data:����, time:��¼�¼�ʱ��
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: DeleteImportanceEvent
��������: ɾ���������Ҫ�¼�
��    ��:   ERC��Ҫɾ�����¼�����,(0~64)      0:ɾ�������¼� old = 1 ɾ������һ��
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: DeleteordinaryEvent
��������: ɾ�������һ���¼�
��    ��:   ERC��Ҫɾ�����¼�����(0~64)      0:ɾ�������¼�
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: DelimpERC
��������: ɾ����Ҫ�¼�ȫ��ERC��¼
��    ��:   ERC��Ҫɾ�����¼�����(0~64)      
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: DelordERC
��������: ɾ��һ���¼�ȫ��ERC��¼
��    ��:   ERC��Ҫɾ�����¼�����(0~64)      
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: GetImportanceEvent_Num
��������: ��ȡ��¼��Ҫ�¼�������
��    ��:   ERC:�¼�����(0~64), 0 ʱΪ��ȡ�����¼��ĸ���
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: GetordinaryEvent_Num
��������: ��ȡ��¼һ���¼�������
��    ��:   ERC:�¼�����(0~64), 0 ʱΪ��ȡ�����¼��ĸ���
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
�� �� ��: GetImportanceEvent
��������: ��ȡ��¼��Ҫ�¼�
��    ��: Pm:��ʼָ��  Pn:����ָ��
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
	
	(*(int *)para)++;		//��ȡ��¼�ĸ���
	return 0;
}

/*************************************************
�� �� ��: GetordinaryEvent
��������: ��ȡ��¼һ���¼�
��    ��: Pm:��ʼָ��  Pn:����ָ��
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
	
	(*(int *)para)++;		//��ȡ��¼�ĸ���
	return 0;
}	



/*************************************************
�� �� ��: GetErcEvent
��������: ����ERC�����ȡ�¼���¼
��    ��: Pm:��ʼָ��  Pn:����ָ��
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
	return GetERCEvent_sum;		//���ػ�ȡ�¼��ĸ���
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
	
	(*(int *)para)++;		//��ȡ��¼�ĸ���
	return 0;
}	





/*************************************************
�� �� ��: GetEventROW
��������: ��ȡ�¼����ϱ���־
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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

	//��ձ�־λ
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
�� �� ��: UpdateEventRow
��������: �����¼����ϱ���־
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
	

	sprintf(sql, "select rowid from ImportanceEvent limit %d,%d;", 0, row+1);	//��ȡ�к�
		
	rsl = sqlite3_exec(mpDB, sql, UpdateEventRow_back, (void *)&back_row, &errmsg); 
	memset(sql, 0, sizeof(sql));
	printf("back_row = %d\n\n",back_row);
	sprintf(sql, "update ImportanceEvent set Report = 0 where rowid = %d;", back_row);			//�еĿ���ִ�У��еĲ���
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
�� �� ��: GetEC1
��������: ��ȡ�¼�������
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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

	//��ձ�־λ
	memset(m_ErcReport, 0, sizeof(m_ErcReport));
	
	sql = "select * from importanceEC;" ;		
	rsl = sqlite3_exec(mpDB, sql, GetEC1back, (void *)&EC1, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return EC1;			//������Ҫ�¼�������
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
�� �� ��: GetEC2
��������: ��ȡһ���¼�������
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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

	//��ձ�־λ
	memset(m_ErcReport, 0, sizeof(m_ErcReport));
	
	sql = "select * from ordinaryEC;" ;		
	rsl = sqlite3_exec(mpDB, sql, GetEC2back, (void *)&EC2, &errmsg); 
	DBclose(mpDB);
	
	if(rsl != SQLITE_OK)	
	{		
		printf("select error : %s\n",errmsg);	
		return -1;
	} 
	return EC2;		//����һ���¼�������
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
�� �� ��: UpdateEC1
��������: ������Ҫ�¼�������
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
	
	//��ձ�־λ
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
	return 0;			//������Ҫ�¼�������
}





/*************************************************
�� �� ��: UpdateEC2
��������: ������Ҫ�¼�������
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
	
	//��ձ�־λ
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
	return 0;			//������Ҫ�¼�������
}





/*************************************************
�� �� ��: UpdatePower
��������: ����ͣ�ϵ��¼�
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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
	return 0;			//������Ҫ�¼�������
}
























#if 0
/*************************************************
�� �� ��: GetEventByhang
��������: 
��    ��: 
��    ��: ��
��    ��: <0 ʧ�ܣ� >=0 �ɹ�
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



