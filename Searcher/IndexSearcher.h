#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "../include/MemoryPool/BaseAllocator.h"
#include "Global.h"
#include "Searcher_Export.h"

class CQuery;
class CIndexReader;
class CTopDocs;
class CHits;


class SEARCHER_API CIndexSearcher:public CBaseAlloc
{
public:
	CIndexSearcher(CIndexReader *pReader);
	CTopDocs* GetSearchResult( CQuery *pQuery,UINT nDocs);
	CHits*    Search( CQuery *pQuery);

	CIndexReader* GetReader(){ return m_pReader;}
private:
	CQuery* Optimize(CQuery *pQuery);

private:
	CIndexReader *m_pReader;
};

inline CIndexSearcher::CIndexSearcher(CIndexReader *pReader)
:m_pReader(pReader)
{
}

