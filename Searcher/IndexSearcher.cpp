#include "ResultList.h"
#include "Query.h" 
#include "IndexSearcher.h"
#include "TopDocsCollector.h"
#include "Hits.h"
#include "TopDocs.h"
#include "TermDoc.h"


CTopDocs* CIndexSearcher::GetSearchResult(CQuery *pQuery,UINT nDocs)
{
  CQuery* q= Optimize(pQuery);
  CResultList *pTotalResultList = pQuery->GetSearchResult(this);

  if(pTotalResultList && pTotalResultList->getNodeCount() > 0)
  {
     CTopDocsCollector hc(nDocs);

	 CTermDoc *p = pTotalResultList->getFirst();
	 while(p)
	 {
		 hc.Collect(p->GetScore(),p->GetDocID());
		 p = pTotalResultList->getNext();
	 }
	 Delete_ResultList(pTotalResultList);
	 return hc.GetTopDocs();
  }
  return (CTopDocs*)NULL;
  
}

CHits* CIndexSearcher::Search(CQuery *pQuery)
{
	return new CHits(this,pQuery);
}

inline CQuery* CIndexSearcher::Optimize(CQuery *pQuery)
{
	//todo : optimize the query
	return pQuery;
}

