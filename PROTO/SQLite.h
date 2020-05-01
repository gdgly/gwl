#ifndef _SQLite_H_
#define _SQLite_H_

#include "sqlite3.h"

#define FILENAME	"./Event.db"

sqlite3* DBopen(const char* szFile);

void DBclose(sqlite3* mpDB);
int create_table();

int execFormatDML(sqlite3* mpDB, const char* format, ...);

int TableIsExit(char *TableName, sqlite3* mpDB);
int getTableIsExit(void *para, int ncolumn, char **columnvalue, char **columnname);

int InsertImportanceEvent(int ERC, char *data, unsigned long long time, int report);

int InsertordinaryEvent(int ERC, char *data, unsigned long long time );

int DeleteImportanceEvent(int ERC);
int DeleteordinaryEvent(int ERC);

int GetImportanceEvent_Num();
int GetordinaryEvent_Num();
int getImportancenum(void *para, int ncolumn, char **columnvalue, char **columnname);
int getordinarynum(void *para, int ncolumn, char **columnvalue, char **columnname);


int GetImportanceEvent(int Pm, int Pn);
int GetImportanceEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname);
int GetordinaryEvent(int Pm, int Pn);
int GetordinaryEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname);


int GetErcEvent(int ERC);
int GetERCEvent_back(void *para, int ncolumn, char **columnvalue, char **columnname);




int GetEventRow();
int GetEventRowback(void *para, int ncolumn, char **columnvalue, char **columnname);



#endif

