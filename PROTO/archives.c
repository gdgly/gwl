
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "SQLite.h"
#include "archives.h"


#define ARCHIVES_DB		"/mnt/data/archives.db"
#define TABLE_NAME		"ArchData"
sTableItem cTableItem[] = 
{
//	{"ID","unsigned short"},
//	{"EquipNum","unsigned short"},
	{"EquipSquenceNo","unsigned short"},
	{"CirrentNo","unsigned short"},
	{"ComRateAndPort","unsigned char"},
	{"ProtolType","unsigned char"},
	{"Addr","unsigned char(6)"},
	{"TariffNum","unsigned char"},
	{"DispIntDeciNumunsigned","char"},
	{"CollectorAddr","unsigned char(6)"},
	{"UserInfo","unsigned char"},
};

sList *ArchList;

sArchPara ArchPara;


sArchPara *GetArchPara(void)
{
	return &ArchPara;
}


int CreateArchTable(void)
{
	int len,i,ret = 0;
	u8 *sql,*pTmpSql;
	char *errmsg;
	sqlite3 *mpDB = NULL;
	const int masize = 500;
	
    ret = sqlite3_open(ARCHIVES_DB,&mpDB);
    if(mpDB == NULL)
        return -1;
	sql = malloc(masize);
	if(NULL == sql)
	{
		printf("create table malloc fail\n");
		return -1;
	}
	memset(sql,0,masize);
	snprintf(sql,masize,"create table %s(",TABLE_NAME);
	pTmpSql = sql+strlen(sql);
	len = sizeof(cTableItem)/sizeof(cTableItem[0]);
	for(i=0;i<len;i++)
	{
		if(i < len-1)
		{
			snprintf(pTmpSql,masize,"%s %s,",cTableItem[i].name,cTableItem[i].type);
		}
		else
		{
			snprintf(pTmpSql,masize,"%s %s",cTableItem[i].name,cTableItem[i].type);
		}
		pTmpSql = sql+strlen(sql);
	}
	snprintf(pTmpSql,masize,");");
 	ret = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(ret != SQLITE_OK)	
	{			
		free(sql);
		sqlite3_close(mpDB);
		printf("create archives table failled for: %s\n",errmsg);		
		return -1;
	}	
	free(sql);	
	sqlite3_close(mpDB);
	return 0;
}

int InsertDataStruct(sArchivesDataUnit *ArchDb)
{
	if(!ArchDb) return -1;
	
	int i,ret = 0;
	u8 *pTmpSql,*sql=NULL;
	char *errmsg;
	sqlite3 *mpDB = NULL;
	const int masize = 500;
	
    ret = sqlite3_open(ARCHIVES_DB,&mpDB);
    if(mpDB == NULL)
        return -1;

	sql = malloc(masize);
	if(NULL == sql)
	{
		printf("insert malloc fail\n");
		return -1;
	}
	memset(sql,0,masize);
	
	snprintf(sql,masize,"insert into %s values(",TABLE_NAME);
	pTmpSql = sql+strlen(sql);
	snprintf(pTmpSql,masize,"%d,",ArchDb->EquipSquenceNo);
	pTmpSql = sql+strlen(sql);
	snprintf(pTmpSql,masize,"%d,",ArchDb->CirrentNo);
	pTmpSql = sql+strlen(sql);
	snprintf(pTmpSql,masize,"%d,",ArchDb->ComRateAndPort);
	pTmpSql = sql+strlen(sql);
	snprintf(pTmpSql,masize,"%d,",ArchDb->ProtolType);
	pTmpSql = sql+strlen(sql);
	*pTmpSql = '\'';
	pTmpSql++;
	for(i=0;i<ADDR_LEN;i++)
	{
		if(i<ADDR_LEN-1)
		{
			snprintf(pTmpSql,masize,"%02d",ArchDb->Addr[i]);
		}
		else
		{
			snprintf(pTmpSql,masize,"%02d\',",ArchDb->Addr[i]);
		}
		pTmpSql = sql+strlen(sql);
	}	
	snprintf(pTmpSql,masize,"%d,",ArchDb->TariffNum);
	pTmpSql = sql+strlen(sql);
	snprintf(pTmpSql,masize,"%d,",ArchDb->DispIntDeciNum);
	pTmpSql = sql+strlen(sql);
	*pTmpSql = '\'';
	pTmpSql++;	
	for(i=0;i<ADDR_LEN;i++)
	{
		if(i<ADDR_LEN-1)
		{
			snprintf(pTmpSql,masize,"%02d",ArchDb->CollectorAddr[i]);
		}
		else
		{
			snprintf(pTmpSql,masize,"%02d\',",ArchDb->CollectorAddr[i]);
		}		
		pTmpSql = sql+strlen(sql);
	}
	snprintf(pTmpSql,masize,"%d",ArchDb->UserInfo);
	pTmpSql = sql+strlen(sql);

	snprintf(pTmpSql,masize,");");
	pTmpSql = sql+strlen(sql);
	
 	ret = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
 	if(ret != SQLITE_OK)	
	{	
		free(sql);
		sqlite3_close(mpDB);
		printf("insert archives table failled for: %s\n",errmsg);		
		return -1;
	}
	free(sql);
	sqlite3_close(mpDB);
	return 0;	
}
/*********************************************************
@function:get data from database by EquipSquenceNo

@input:ArchDb--data of what you want to get
		seq--the equipment sequence number
		MasterKey--select by macro:ASC_ROW_ID,ASC_SEQ
		
@output:0--success,other--fail
*********************************************************/
int ReadArchFromDb(sArchivesDataUnit *ArchDb,u8 *MasterKey,int seq)
{
	if(!ArchDb) return -1;
	
	int i,j,ret = 0;
	u8 *sql=NULL;
	char **result;
	char *errmsg;
	sqlite3 *mpDB = NULL;
	int row=0,column=0;
	const int masize = 500;
	
    ret = sqlite3_open(ARCHIVES_DB,&mpDB);
    if(mpDB == NULL)
        return -1;
	sql = malloc(masize);
	if(NULL == sql)
	{
		printf("read malloc fail\n");
		return -1;
	}
	memset(sql,0,masize);
	snprintf(sql,masize,"select * from %s where %s = '%d' limit 1",TABLE_NAME,MasterKey,seq);
	ret = sqlite3_get_table(mpDB,sql,&result,&row,&column,&errmsg);
	if(ret) return -1;
	if(!row||!column) return -1;

	i=column;
	ArchDb->EquipSquenceNo = atoi(result[i++]);
	ArchDb->CirrentNo = atoi(result[i++]);
	ArchDb->ComRateAndPort = atoi(result[i++]);
	ArchDb->ProtolType = atoi(result[i++]);

	Str2Hex(result[i++],ArchDb->Addr);
	ArchDb->TariffNum = atoi(result[i++]);
	ArchDb->DispIntDeciNum = atoi(result[i++]);

	Str2Hex(result[i++],ArchDb->CollectorAddr);
	ArchDb->UserInfo = atoi(result[i++]);


	printf("[debug]sArchivesDataUnit:\n");
	printhexdata((void*)ArchDb, sizeof(sArchivesDataUnit));
	free(sql);
	sqlite3_free_table(result);//释放result的内存空间
	sqlite3_close(mpDB);
}
int DeleteTable(u8 *taname)
{
	u8 *sql;
	const int masize=500;
	char *errmsg;
	sqlite3 *mpDB = NULL;
	int ret = 0;

    ret = sqlite3_open(ARCHIVES_DB,&mpDB);
    if(mpDB == NULL)
        return -1;

	sql = malloc(masize);
	snprintf(sql,masize,"drop table %s",taname);
	ret = sqlite3_exec(mpDB, sql, NULL, NULL, &errmsg);
	free(sql);	
	sqlite3_close(mpDB);

	return ret;
}
int ClearDataBak(void *src,void *extra)
{
	if(!src) return -1;

	free(src);
	return 0;
}
int ArchDataClear(sList *DataList)
{
	int ret =0;

	
	ret = ListClear(DataList, ClearDataBak, NULL);
	return ret;
}
int ArchVisit(void *src,void *extra)
{
	if(!src || !extra) return -1;
	
	sArchListStruct *ArchListStruct = (sArchListStruct*)src;
	sArchRequire *Data = (sArchRequire*)extra;
	
	if(ArchListStruct->ArchivesDataUnit.EquipSquenceNo == Data->seq)
	{
		Data->Data = (void*)&ArchListStruct->ArchivesDataUnit;
		return 0;
	}
	return -1;
}
//get data by sequences number
int ArchSearchData(u16 seq,sArchivesDataUnit *data)
{
	int i,ret = 0;
	sArchRequire ArchRequire;
	
	ret = ListTraverse(&ArchPara.ArchList,ArchVisit,&ArchRequire);
	if(!ret) 
	{
		data = (sArchivesDataUnit*)ArchRequire.Data;
		return 0;
	}
	return -1;
}

int ArchDataReset(void)
{
	int ret = 0;

	ret += DeleteTable(TABLE_NAME);
	ret += ArchDataClear(&ArchPara.ArchList);

	return ret;
}

int ArchivesInit(void)
{
	int line,ret = 0;
	sArchivesDataUnit ArchData;
	sArchListStruct *ArchListStruct;

	memset(&ArchPara,0,sizeof(ArchPara));
	ListInit(&ArchPara.ArchList);
	printf("[debug]ArchivesInit");
	CreateArchTable();//不判断创建是否成功，可能存在重复创建的问题。
	line = 1;//rowid start from 1
	while(1)
	{
		printf("[debug]while(1)");
		ret = ReadArchFromDb(&ArchData,ASC_ROW_ID,line++);
		if(!ret)
		{
			ArchListStruct = malloc(sizeof(sArchListStruct));
			ArchListStruct->ArchivesDataUnit =	ArchData;
			ListInsertTail(&ArchPara.ArchList,&ArchListStruct->ArchList);
			ArchPara.EquipNum++;
		}
		else
			break;
	}
	return 0;
}






























