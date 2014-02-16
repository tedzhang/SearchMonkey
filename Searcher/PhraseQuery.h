#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007


#include "Query_Base.h"
#include <vector> //for vector
#include "ResultList.h"
#include "IndexReader.h"
#include <queue>

class CTerm;


class CPhraseQuery:public CQuery
{
public:
	QueryType GetQueryType() { return kPhraseQuery;}
	enum { MAX_TERM_SIZE = 1024};
	CPhraseQuery();
	~CPhraseQuery() { }
	void      SetSlop(int s) { m_slop = s;}
	int       GetSlop()      { return m_slop;}
	void      Add(CTerm *term);
	virtual CResultList* GetSearchResult(CIndexSearcher* pIndexSearcher);
#ifdef _DEBUG
	std::string ToString();
#endif
	virtual int    GetTermNum()const  { return m_terms.size();}


private:
	typedef std::vector<CTerm*> TERMLIST;
	typedef std::vector<int>   POSITIONLIST;
	TERMLIST     m_terms;
	POSITIONLIST m_positions;
	int          m_slop;

private:
	
	struct TermResultList
	{
		int termIdx;
		CResultList* pResult;
		TermResultList(int termidx,CResultList *pList):termIdx(termidx),pResult(pList)
		{

		}
		TermResultList() 
		{
		}
	};
	struct DocFreqComparator
	{
		bool operator()(const TermResultList& lhs,const TermResultList &rhs)const
		{
			return lhs.pResult->getNodeCount() > rhs.pResult->getNodeCount();
		}
	};


	

private:
	
	typedef std::vector<int> TermPositionList;

	class CTermPositionsDoc:public CBaseAlloc
	{
	public:
		UINT  docID;
		float fScore;
		TermPositionList *pTermPositions;
		CTermPositionsDoc(UINT nDocId,float score,TermPositionList* pPosition)
			:docID(nDocId),fScore(score),pTermPositions(pPosition)
		{

		}
		UINT  GetDocID() { return docID;}
		float GetScore() { return fScore;}
		~CTermPositionsDoc()
		{
			delete pTermPositions;
		}
	};
	typedef CSingleLink<CTermPositionsDoc> TermPostionsDocList;

	typedef std::priority_queue<TermResultList,std::vector<TermResultList>,DocFreqComparator> PhraseQueue;
	bool                       GetPhraseResultQue(CIndexReader *pReader,PhraseQueue &que);
	TermPostionsDocList*       GetFirstTermPosDocList(CIndexReader *pReader,PhraseQueue &que);
	
	CResultList *GetPositionMatchResult(CIndexReader *pReader,TermPostionsDocList *pDocList,PhraseQueue &que);



	TermPositionList*          GetMatchPosition(int nPrevIdx,const CIndexReader::TermPositionList &prev,
		                                        int nCurIdx,const CIndexReader::TermPositionList&cur);

	TermPositionList*          GetMatchPosition(TermPositionList *pTermPositions,
		                                        int  nCurIdx, 
		                                        const CIndexReader::TermPositionList &cur);

	void                       DeletePositionDocList(CPhraseQuery::TermPostionsDocList *&pList);

};
