
#include "Hits.h"
#include "IndexSearcher.h"
#include "TopDocs.h"
#include "IndexReader.h"

void CHits::GetMoreDocs(int min) 
{
	if (m_hitDocs.size() > min) 
	{
		min = m_hitDocs.size();
	}

	int n = min * 2;
	CTopDocs *pTopDocs = m_pSearcher->GetSearchResult(m_pQuery,n);
	if(!pTopDocs) return;

	m_nLength = pTopDocs->GetTotalHits();


	float scoreNorm = 1.0f;

	if (pTopDocs->GetTotalHits() > 0 && pTopDocs->GetMaxScore() > 1.0f) 
	{
		scoreNorm = 1.0f / pTopDocs->GetMaxScore();
	}

	int scoreDocsSize = pTopDocs->scoreDocs.size();
	int totalHits     = pTopDocs->GetTotalHits();

	int end = scoreDocsSize <totalHits ? scoreDocsSize:totalHits;
	ScoreDocs &scoreDocs = pTopDocs->scoreDocs;
	for (int i = m_hitDocs.size(); i < end; i++) 
	{
		CHitDoc *pDoc = new CHitDoc(scoreDocs[i]->GetScore() * scoreNorm,scoreDocs[i]->GetDocID());
		m_hitDocs.push_back(pDoc);
	}

	delete pTopDocs;

}




CDocument* CHits::GetDoc(int n) 
{
	CHitDoc* pHitDoc = hitDoc(n);

	// Update LRU cache of documents
	Remove(pHitDoc);               // remove from list, if there
	AddToFront(pHitDoc);           // add to front of list
	if (numDocs > maxDocs) {      // if cache is full
		CHitDoc *oldLast = m_plast;
		Remove(m_plast);             // flush last
		delete oldLast;
	}

	if (pHitDoc->GetDocument() == NULL) 
	{
		CDocument* pDoc = m_pSearcher->GetReader()->GetDocument(pHitDoc->GetDocID());
		ASSERT(pDoc);
		pHitDoc->SetDocument(pDoc);
	}

	return pHitDoc->GetDocument();
}

float CHits::GetScore(int n) 
{
	return hitDoc(n)->GetScore();
}

UINT CHits::GetDocID(int n) 
{
	return hitDoc(n)->GetDocID();
}


CHits::CHitDoc* CHits::hitDoc(int n) 
{
    if(n > GetLength()) 
			return NULL;
     if (n >= m_hitDocs.size()) 
	{
			GetMoreDocs(n);
	}
    return m_hitDocs[n];
}

void CHits::AddToFront(CHitDoc *hitDoc) 
{  
		if (m_pfirst == NULL) 
		{
			m_plast = hitDoc;
		} 
		else 
		{
			m_pfirst->prev = hitDoc;
		}

		hitDoc->next = m_pfirst;
		m_pfirst = hitDoc;
		hitDoc->prev = NULL;

		numDocs++;
}


void CHits::Remove(CHitDoc *hitDoc) 
{	  // remove from cache
		if (hitDoc->GetDocument() == NULL) 
		{     // it's not in the list
			return;					  // abort
		}

		if (hitDoc->next == NULL) 
		{
			m_plast = hitDoc->prev;
		} 
		else 
		{
			hitDoc->next->prev = hitDoc->prev;
		}

		if (hitDoc->prev == NULL) 
		{
			m_pfirst = hitDoc->next;
		} 
		else 
		{
			hitDoc->prev->next = hitDoc->next;
		}

		numDocs--;
}

