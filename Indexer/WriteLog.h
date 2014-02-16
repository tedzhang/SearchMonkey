/////////////////////////////

#ifndef __WRITELOG_H__
#define __WRITELOG_H__



#define DEBUG_ALL		4
#define DEBUG_PROG		3
#define DEBUG_WARN		2
#define DEBUG_NORMAL	1
#define DEBUG_DISABLE	0

void WriteLogInfo(char* pLogInfo, int Line, int nDebugLev);


#endif
