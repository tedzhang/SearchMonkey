#pragma  once 

#include "StopwordFilter.h"
#include "Stem.h"
#include "Global.h"

class  CWordPreProcessor
{
public:
	CWordPreProcessor();
	bool LoadStopwordsList(const char* path);
	bool Process(char* word,int nLen,bool keeplogicalword=false);
private:
	CStopwordFilter m_filter;
	
};