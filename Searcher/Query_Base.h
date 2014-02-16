#pragma  once 

#include <string>
#include "../include/MemoryPool/BaseAllocator.h"

class CResultList;
class CIndexSearcher;

class CQuery:public CBaseAlloc
{
public:
	typedef enum 
	{
		kTermQuery    = 0 , //Term query,one keyword query
		kPhraseQuery  = 1,  //Phrase query
		kBooleanQuery = 2,  //Boolean Query
	}QueryType;
	CQuery() { }
	virtual ~CQuery() { }

	virtual QueryType GetQueryType()      = 0;
	virtual CResultList* GetSearchResult(CIndexSearcher* pIndexSearcher) = 0;
#ifdef _DEBUG
	virtual std::string ToString()        = 0;//for debug only
#endif
	virtual int    GetTermNum() const { return 0;}
};