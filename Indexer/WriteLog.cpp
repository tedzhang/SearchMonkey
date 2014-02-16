

#include <stdio.h>
#include "writelog.h"
#include <time.h>

void WriteLogInfo(char* pLogInfo, int Line, int nDebugLev)
{

#ifdef _DEBUG
	printf(pLogInfo);
#endif

	char	pFileName[100];
	char	pDate[20];
//	_strdate(pDate);
	time_t	tt;
	time(&tt);
	struct tm * time1= localtime(&tt);
	strftime(pDate, 20, "%y-%m-%d", time1);

	char	pTime[20];
	_strtime(pTime);

	sprintf(pFileName, "%s\\Log%s.txt", pDate);
	FILE* fp= fopen(pFileName, "a+");
	if(fp != NULL)
	{
		sprintf(pFileName, "time= %s; Line=%d; \n", pTime, Line);
		fputs(pFileName, fp);
		fputs(pLogInfo, fp);
		fclose(fp);
	}
}




