#ifndef _SQLite_H_
#define _SQLite_H_

#include "Sgd_types.h"

#include "sqlite3.h"

#define FILENAME	"./Event.db"

sqlite3* DBopen(const char* szFile);

void DBclose(sqlite3* mpDB);
int create_table();

int execFormatDML(sqlite3* mpDB, const char* format, ...);

int TableIsExit(char *TableName, sqlite3* mpDB);
int getTableIsExit(void *para, int ncolumn, char **columnvalue, char **columnname);

int InsertImportanceEvent(u8 ERC, char *data, u32 time, int report);

int InsertordinaryEvent(u8 ERC, char *data, u32 time);


int DeleteImportanceEvent(int ERC);
int DeleteordinaryEvent(int ERC);

int DelimpERC(int ERC);
int DelordERC(int ERC);


int GetImportanceEvent_Num();
int GetordinaryEvent_Num();
int getImportancenum(void *para, int ncolumn, char **columnvalue, char **columnname);
int getordinarynum(void *para, int ncolumn, char **columnvalue, char **columnname);


int GetImportanceEvent(int Pm, int Pn);
int GetImportanceEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname);
int GetordinaryEvent(u8 Pm, u8 Pn);


int GetordinaryEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname);


int GetErcEvent(int ERC);
int GetERCEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname);




int GetEventRow();
int GetEventRowback(void *para, int ncolumn, char **columnvalue, char **columnname);

int UpdateEventRow(u8 row);


int GetEC1();
int GetEC1back(void *para, int ncolumn, char **columnvalue, char **columnname);

int GetEC2();
int GetEC2back(void *para, int ncolumn, char **columnvalue, char **columnname);

int UpdateEC1(u8 EC1);
int UpdateEC2(u8 EC2);

int UpdatePower(char *data, u32 time);


#endif

