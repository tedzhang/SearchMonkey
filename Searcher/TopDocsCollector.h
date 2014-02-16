#pragma  once 
#include <algorithm>
#include <vector>
#include "Topdocs.h"
//
//
//基于doc的分数的最小堆
//类似于优先队列的实现
//


class CTopDocsCollector
{
public:
	 CTopDocsCollector(int nNumHits):m_nNumHits(nNumHits),m_totalHits(0),m_fMinScore(0.0f)
	 {
			m_hits.reserve(nNumHits);
	 }
	 void      Collect( float score,UINT docID);
	 int       GetNumHits()const   { return m_hits.size();}
	 int       GetTotalHits()const { return m_totalHits;}
	 CTopDocs* GetTopDocs() ;
	

private:
	typedef std::vector<CScoreDoc*> HitQueue;
	HitQueue        m_hits;
	
	struct ScoreGreater
	{
		bool operator()(const CScoreDoc* &lhs,const CScoreDoc *& rhs) const 
		{
			return lhs->GetScore() > rhs->GetScore();
		}
	};

	float     m_fMinScore;
	int       m_totalHits;
	int       m_nNumHits;
private:
	CScoreDoc* Pop();	
	bool       IsEmpty();
};

inline void CTopDocsCollector:: Collect( float score,UINT docID)
{
	if(score > 0.0f)
	{
		m_totalHits++;
		if(m_hits.size() < m_nNumHits)
		{
			m_hits.push_back(new CScoreDoc(docID,score));
			std::push_heap(m_hits.begin(),m_hits.end(),ScoreGreater());
			m_fMinScore = m_hits[0]->GetScore();
		}
		else if(score > m_fMinScore)
		{
			std::pop_heap(m_hits.begin(),m_hits.end(),ScoreGreater());
			m_hits[m_hits.size()-1] = new CScoreDoc(docID,score);
			std::push_heap(m_hits.begin(),m_hits.end(),ScoreGreater());
			m_fMinScore = m_hits[0]->GetScore();
		}

	}
}

inline CScoreDoc* CTopDocsCollector::Pop()
{
   std::pop_heap(m_hits.begin(),m_hits.end(),ScoreGreater());
   CScoreDoc *p = m_hits.back();
   m_hits.pop_back();
   return p;
}

inline bool CTopDocsCollector::IsEmpty()
{
	return m_hits.size() == 0;
}


CTopDocs* CTopDocsCollector::GetTopDocs()
{
	 CTopDocs *pTopDocs = new CTopDocs(m_totalHits,0.0f);
     while(!IsEmpty())
	 { 
		 pTopDocs->scoreDocs.insert(pTopDocs->scoreDocs.begin(),Pop());
	 }

	 float maxScore = (m_totalHits== 0)?(-1.0f*UINT_MAX):pTopDocs->scoreDocs[0]->GetScore();
	 pTopDocs->SetMaxScore(maxScore);

	 return pTopDocs;
}

