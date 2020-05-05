#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "file.h"
#include <sys/file.h>


/*************************************************
函 数 名:       
功能描述: 读取文件        
输    入: filename:文件名,buf:读取缓存,len:读取长度   , mode:打开方式        
输    出:      
返    回:                  
*************************************************/
int readfile(char *filename,char *buf,int len , char *mode)
{
    FILE *fd;
    fd = fopen(filename,mode);		//"r+"
    if(fd<=0)
    {
        printf("read %s failed!\n",filename);
        return -1;
    }

    fread(buf,len,1,fd);
    fclose(fd);
    //DPRINT("read %s success:\n%s\n",filename,buf);
    return 0;
}

/*************************************************
函 数 名:       
功能描述: 写文件        
输    入:           
输    出:      
返    回:                  
*************************************************/
int writefile(char *filename,char *buf,int len,char *mode)
{
    FILE *fd;
    fd = fopen(filename,mode);			//"w+"
    if(fd<=0)
    {
        printf("write %s failed!\n",filename);
        return -1;
    }

    fwrite(buf,len,1,fd);
    fclose(fd);
    return 0;
}

int mygetline(char **lineptr, unsigned int *n, FILE *stream)
{
	if (0 == lineptr || 0 == n || 0 == stream)
		return -1;
	if (0 == *lineptr)
	{
		*lineptr = (char*)malloc(1024);
		*n = 1024;
	}
	if (0 == *lineptr)
	{
		*n = 0;
		return -1;
	}
	long pos = 0;
	size_t leftspace = *n;
	while(!feof(stream) && leftspace > 1)
	{
		if (fread(*lineptr + pos,1,1,stream) < 1)
			break;
		if ((*lineptr)[pos++]  == '\n')
		{
			break;
		}
		-- leftspace;
		if (leftspace == 1)
		{
			char* tmp = (char*)realloc(*lineptr,*n + 256);
			if (0 != tmp)
			{
				*lineptr = tmp;
				*n += 256;
				leftspace += 256;
			}
			else
				return -1;
		}
	}
	//for windows text file.\r\n
	if (pos > 1 && (*lineptr)[pos-1] == '\n' && (*lineptr)[pos-2] == '\r' )
	{
		(*lineptr)[pos-2] = '\n';
		-- pos;
	}
	else if (pos > 0 && (*lineptr)[pos-1] == '\r')//\r without \n at eof
	{
		-- pos;
	}
	(*lineptr)[pos] = 0;
	return pos;
}
/*************************************************
函 数 名: saveConfPara     
功能描述: 写参数到指定文件        
输    入:           
输    出:      
返    回:                  
*************************************************/
int saveConfPara(const char* confile, const char*name, const char*value, char delimiter)
{
	if (0 == confile || 0 == name || 0 == value)
		return -1;
	return delsaveconfparam(confile, name, -1, value, delimiter);
}

int cpyline(char* dest,long pos,const char* src)
{
	strcpy(dest + pos,src);
	pos += strlen(src);
	return pos;
}

int cpykeyvalue(char* dest,long pos,const char* name,const char* value,char delimiter)
{
	if (pos > 0)
	{
		if (dest[pos-1] != '\n')
		{
			dest[pos++] = '\n';
		}
	}
	strcpy(dest + pos,name);
	pos += strlen(name);
	dest[pos++] = delimiter;

	strcpy(dest + pos,value);
	pos += strlen(value);
	const char* newlinepos = strchr(value,'\n');
	if (0 != newlinepos)//already has newline
		return pos;
	dest[pos++] = '\n';
	dest[pos] = 0;

	return pos;
}


//保存参数
int delsaveconfparam(const char* confile, const char*name, int namelen, const char*value, char delimiter)
{
	FILE* fd;
	char *linebuf = 0;
	size_t linebufsize = 0;
	int strict; 
	if (namelen < 0)
	{
		strict = 0;
	}
	else
	{
		strict = 1;
	}
	if (0 == confile || 0 == name)
		return -1;
	if (namelen < 0 || 0 != value)
	{
		namelen = strlen(name);
		if (namelen <= 0)
			return -1;
	}
	char* colonpos;
	char* reallinestart;
	//char* filebuf = 0;
	

	int valuelen = 0;
	if (0 != value)
	{
		valuelen = strlen(value);
		while(isspace(*value))//remove leading space
		{
			++value;
			--valuelen;
		}
	}

	int ret = -1;

	fd = fopen(confile,"rb");
	if(fd == 0)
	{
		if (0 == value)//delete
			return 0;
		fd = fopen(confile, "w+b");//create
		if (0 == fd)
		{
			//MY_ACE_ERROR((LM_ERROR,ACE_TEXT("saveconfparam open %s for write fail\n"),confile));
			//goto getout;
			free(linebuf);
			if (fd)
			{
				fflush(fd);
				fclose(fd);
			}
			return ret;
		}
		//newfile = true;
	}
	if (fseek(fd,0,SEEK_END) < 0)
		//goto getout;
	{
		free(linebuf);
		if (fd)
		{
			fflush(fd);
			fclose(fd);
		}
		return ret;
	}
	long filesize;
	int found;
	filesize = ftell(fd);
	found = 0;
	if (filesize < 0)
		//goto getout;
	{
		free(linebuf);
		if (fd)
		{
			fflush(fd);
			fclose(fd);
		}
		return ret;
	}
	char filebuf[filesize + namelen + valuelen + 1 + 1 + 1];
	//filebuf = new char[filesize + namelen + valuelen + 1 + 1 + 1];//delimiter, \n and \0
	//if (0 == filebuf)
	//	goto getout;
	long filebufpos;
	long linelen;
	filebufpos = 0;
	rewind(fd);

	while(!feof(fd))
	{
		linelen = mygetline(&linebuf,(unsigned int *)&linebufsize,fd);
		if (linelen < 0)
			break;
		filesize -= linelen;
		reallinestart = linebuf;
		while(isspace(*reallinestart))//remove leading space
		{
			++ reallinestart;
			-- linelen;
		}
		if (*reallinestart == '#' //comment
			)
		{
			filebufpos = cpyline(filebuf,filebufpos,reallinestart);
			continue;
		}
		colonpos = strchr(reallinestart,delimiter);//delimiter
		if (0 == colonpos)
		{
			//garbage line,discard
			continue;
		}
		//too short line
		if (linelen <= namelen)//linelen must >= delimiter + namelen
		{
			filebufpos = cpyline(filebuf,filebufpos,reallinestart);
			continue;
		}
		*colonpos ++ = 0;
		//match name?
		if (0 != strncmp(reallinestart,name,namelen) //not match
			|| (strict && reallinestart[namelen] && !isspace(reallinestart[namelen])) //must match whole key name
			)
		{
			colonpos[-1] = delimiter;
			filebufpos = cpyline(filebuf,filebufpos,reallinestart);
			continue;
		}
		if (0 == value)//delete
			continue;
		if (found)//duplicate,eat this line
			continue;
		//copy value
		found = 1;
		filebufpos = cpykeyvalue(filebuf,filebufpos,name,value,delimiter);
	}
	if (!found && 0 != value)
	{
		//append value
		filebufpos = cpykeyvalue(filebuf,filebufpos,name,value,delimiter);
	}
	if (filesize <= 0 //windows text file will > 0 because getline() return less than real read
		|| feof(fd))//all read
	{
		fclose(fd);
		fd = fopen(confile,"wb");//win32 readonly file can not overwrite or unlink
		if(fd == 0)
		{
			perror("saveconf write");
			ret = -1;
			//goto getout;
			free(linebuf);
			if (fd)
			{
				fflush(fd);
				fclose(fd);
			}
			return ret;
		}
		//newfile = true;

		while(filebufpos > 0)
		{
			ret = fwrite(filebuf,1,filebufpos,fd);
			if (ret <= 0)
			{
				ret = -1;
				break;
			}
			filebufpos -= ret;
		}
		if (filebufpos <= 0)
			ret = 0;
	}
//getout:
	//delete[] filebuf;
	free(linebuf);
	if (fd)
	{
		fflush(fd);
		fclose(fd);
	}
	return ret;
}
/*************************************************
函 数 名: getConfPara     
功能描述: 从指定文件读参数        
输    入:           
输    出:      
返    回:                  
*************************************************/
int getConfPara(const char* confile, const char*name, char*value, int value_size, char delimiter)
{
	if (0 == confile || 0 == name || 0 == value)
		return -1;
	FILE* fd;
	char *linebuf = 0;
	size_t linebufsize = 0;
	int namelen = strlen(name);
	char* colonpos;
	char* newlinepos;
	char* reallinestart;
	int ret = -1;
	
	fd = fopen(confile,"rb");
	if(fd == 0)
	{
		//printf("open %s fail!\n", confile);
		return -1;
	}
	long linelen = 0;
	while(!feof(fd))
	{
		linelen = mygetline(&linebuf,(unsigned int *)&linebufsize,fd);
		if (linelen < 0)
			break;
		reallinestart = linebuf;
		while(isspace(*reallinestart))				//remove leading space
		{
			++ reallinestart;
			--linelen;
		}
		if (*reallinestart == '#' 					//comment
			|| linelen < namelen)					//too short line
			continue;
		colonpos = strchr(reallinestart,delimiter);	//delimiter
		if (0 == colonpos)
			continue;
		*colonpos ++ = 0;
		if (0 != strcmp(reallinestart,name))		//match name?
			continue;
		while(isspace(*colonpos))					//remove leading space
			++ colonpos;
		newlinepos = strchr(colonpos,'\n');
		if (0 != newlinepos)						//remove new line char
			*newlinepos = 0;
		//if (!*colonpos) 							//empty value str
		//	continue;
		if (strlen(colonpos) >= value_size)
		{
			ret = -2;
		}
		else
		{
			strcpy(value,colonpos);
			ret = 0;
		}
		break;
	}
	free(linebuf);
	fclose(fd);
	return ret;
}

int GetFileDataGroup(const char* confile,char *para[],int group)
{
    if(!confile||!para||!group) return 0;

    int i,ret = -1;
    char *pData;
    
    for(i=0;i<group;i++)
    {
        if((para[i*3]==NULL)||(para[1+i*3]==NULL)||(para[2+i*3]==NULL)) break;
        pData = para[2+i*3];
        ret = getConfPara(confile,para[i*3],para[1+i*3],100,*pData);
        if(ret) break;
    }
    if(i>=group) ret = 0;
    return ret;
}



int SaveConfMultiGroup(const char* confile, char *para[],int group)
{
    if(!confile||!para||!group) return 0;


    FILE *fp;
    int i,j,InDataSize,FileSize,TmpLen,MaSize,ret=0;
    char *pTemName,*pTemVal,*pDelimitor;
    char *FileBuf,*FileBufNew,*pFileTem,*pTem;

    
    fp = fopen(confile,"r+");
    if(fp==NULL)
    {
        printf("save open file fail!\n");
        ret = -1;
        return ret;
    }	
    fseek(fp,0,SEEK_END);
    FileSize = ftell(fp);  
	InDataSize = 0;
    for(i=0;i<group;i++)
    {
        pTemName = para[i*3];
        pTemVal = para[1+i*3];

        if(pTemName&&pTemVal)
        {
            printf("[debug]name[%d]=%s\n",i,pTemName);
            printf("[debug]value[%d]=%s\n",i,pTemVal);
            InDataSize += strlen(pTemName);
            InDataSize += strlen(pTemVal);
            InDataSize++;//delimiter
        }
        else
        {
            InDataSize = -1;
            break;
        }
    }
	if(InDataSize<0) 
	{
		if(fp) fclose(fp);
		printf("caculate indata size error!\n");
        ret = -1;
		return ret;
	}
    MaSize = FileSize+InDataSize+group*2;//每组预算'\r'和'\n'
    FileBuf = malloc(MaSize);
	if(FileBuf==NULL)
	{
		printf("first malloc error!!\n");
		if(fp) fclose(fp);
        ret = -1;
		return ret;
	}  
    rewind(fp);
	memset(FileBuf,0,MaSize);
    if(fread(FileBuf,sizeof(char),FileSize,fp)<=0)
    {
        printf("save conf read data fail!\n");
		if(fp) fclose(fp);
		if(FileBuf) free(FileBuf);
        ret = -1;
        return ret;
    }
    fclose(fp);
	
    FileBufNew = malloc(MaSize);
	if(FileBufNew==NULL)
	{
		printf("second malloc error!!\n");
		if(FileBuf) free(FileBuf);	
        ret = -1;
		return ret;
	}
    for(i=0;i<group;i++)
    {
        pTemName = para[i*3];
        pTemVal = para[1+i*3];
        pDelimitor = para[2+i*3];

        
		if((pTemName==NULL)||(pTemVal==NULL))
		{
			break; 
		}
		
        pFileTem = NULL;
        pFileTem = strstr(FileBuf,pTemName);
        if(pFileTem==NULL)//未找到 
        {	
            if(FileBuf[FileSize-1]!='\n')
            {
				printf("add newline\n");
                FileBuf[FileSize++]='\r';
                FileBuf[FileSize++]='\n';
            }
            pTem = &FileBuf[FileSize];
            memcpy(pTem,pTemName,strlen(pTemName));
            pTem += strlen(pTemName);
            *pTem++ = *pDelimitor;
            memcpy(pTem,pTemVal,strlen(pTemVal));
            pTem += strlen(pTemVal);
            FileSize = strlen(FileBuf);	
			if((int)(pTem-FileBuf)>=MaSize)
			{
				printf("exceed max buf\n");
				break;				
			}
            continue;
        }
		memset(FileBufNew,0,MaSize);
        pTem = FileBufNew;
        memcpy(pTem,FileBuf,pFileTem-FileBuf);
        pTem += pFileTem-FileBuf;
        memcpy(pTem,pTemName,strlen(pTemName));
        pTem += strlen(pTemName);
        *pTem++ = *pDelimitor;
        memcpy(pTem,pTemVal,strlen(pTemVal));
        pTem += strlen(pTemVal);
        *pTem++ = '\r';
        *pTem++ = '\n';
        
        while((*pFileTem++!='\n')&&((int)(pFileTem-FileBuf)<FileSize));
        memcpy(pTem,pFileTem,FileSize-(int)(pFileTem-FileBuf));

        FileSize = strlen(FileBufNew);
        memset(FileBuf,0,MaSize);
        memcpy(FileBuf,FileBufNew,MaSize);         	
		printf("the new buffsize = %d\n",FileSize);
    }
	while(i>=group)
	{
		fp = fopen(confile,"w+");
		if(fp==NULL) break;
        if(flock(fileno(fp),LOCK_EX | LOCK_NB))
        {
            printf("file is busy!!\n");
            break;
        }
		if(fwrite(FileBuf,sizeof(char),FileSize,fp)<=0)
		{
			printf("write data error\n");
            ret = -1;
            break;
		}
		break;
	}
    
	if(fp) 
	{
        flock(fileno(fp),LOCK_UN);
        fclose(fp); 
	}
	if(FileBuf) free(FileBuf);
	if(FileBufNew) free(FileBufNew);
	return ret;
}

