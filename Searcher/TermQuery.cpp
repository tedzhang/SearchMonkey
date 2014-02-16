#include "TermQuery.h"
#include "IndexSearcher.h" //for indexsearcher
#include "IndexReader.h"

CResultList* CTermQuery::GetSearchResult(CIndexSearcher* pIndexSearcher)
{
	CIndexReader *pReader = pIndexSearcher->GetReader();
	ASSERT(pReader);
	CResultList *pDocList = NULL;
	pReader->GetTermDocs(*m_pterm,pDocList);
	return pDocList;
}