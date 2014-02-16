#include "PhraseQuery.h"
#include "IndexSearcher.h" //for indexsearcher
#include "ResultList.h" //for result list
#include "Term.h" //for CTerm
#include "IndexReader.h"
#include "PhraseDoc.h"





CPhraseQuery::CPhraseQuery():m_slop(0)
{
	m_positions.reserve(MAX_TERM_SIZE);
	m_terms.reserve(MAX_TERM_SIZE);
}


void  CPhraseQuery::Add(CTerm *term)
{
	int npos = 0;
	size_t isize = m_positions.size();
	if(isize > 0)
		npos = m_positions[isize-1]+1;
	m_positions.push_back(npos);
	m_terms.push_back(term);
}

#ifdef _DEBUG
std::string CPhraseQuery::ToString() 
{
	std::string str;
	str += m_terms.size() > 0 ?m_terms[0]->GetText():"";
	for(size_t i = 1; i < m_terms.size();++i)
	{
		str += ":";
		str += m_terms[i]->GetText();
	}
	return str;
}
#endif

inline bool  CPhraseQuery::GetPhraseResultQue(CIndexReader *pReader,PhraseQueue &que)
{
	size_t termcount = m_terms.size();

	CResultList *pPhraseDocs = NULL;

	size_t  i;
	for(i = 0; i < termcount;++i)
	{
		if(!pReader->GetPhraseDocs(*(m_terms[i]),pPhraseDocs))
			goto clearup;
		que.push(TermResultList(i,pPhraseDocs));
	}
	return true;

clearup:
	while(!que.empty())
	{
		CResultList *pList = que.top().pResult;
		que.pop();
		Delete_ResultList(pList);
	}
	return false;
}


CPhraseQuery::TermPostionsDocList* CPhraseQuery::GetFirstTermPosDocList(CIndexReader *pReader,PhraseQueue &que)
{
	
   CResultList    *result = NULL;
   TermResultList  element;


	if(!que.empty())
	{
		element = que.top();
		que.pop();
		result = element.pResult;
	}

	TermPostionsDocList *pResult = NULL;

	if(!que.empty())
	{
		TermResultList nextElement = que.top();
		que.pop();

		CResultList *pList= nextElement.pResult;

		CPhraseDoc *pDocPrev = (CPhraseDoc*)result->getFirst();
		CPhraseDoc *pDocCur  = (CPhraseDoc*)pList->getFirst();
		while(pDocPrev && pDocCur)
		{
			if(pDocPrev->GetDocID() < pDocCur->GetDocID())
				pDocPrev =(CPhraseDoc*) result->getNext();
			else if(pDocPrev->GetDocID() > pDocCur->GetDocID())
				pDocCur = (CPhraseDoc*)pList->getNext();
			else
			{
				CIndexReader::TermPositionList lstPrev,lstCur;

				bool IsPrevReadSuccess = pReader->GetTermPosition(pDocPrev->GetPosID(),lstPrev);
				bool IsCurReadSuccess = pReader->GetTermPosition(pDocCur->GetPosID(),lstCur);
				
				if(IsCurReadSuccess && IsPrevReadSuccess)
				{
					TermPositionList *pPostionList = GetMatchPosition(element.termIdx,lstPrev,
						                                             nextElement.termIdx,lstCur);
					if(pPostionList)
					{ 
						if(!pResult)
							pResult = new TermPostionsDocList;
						float score = pDocCur->GetScore() + pDocPrev->GetScore();
						pResult->addNodeTail(new CTermPositionsDoc(pDocCur->GetDocID(),score,pPostionList));
					}
				}
			
				pDocPrev= (CPhraseDoc*)result->getNext();
				pDocCur = (CPhraseDoc*)pList->getNext();

			}
		}

		Delete_ResultList(pList);
		Delete_ResultList(result);

	}


	return pResult;
}

inline void CPhraseQuery::DeletePositionDocList(CPhraseQuery::TermPostionsDocList *&pList)
{
	while(pList->rmFirstNode()== 0)
		;
	delete pList;
	pList  = NULL;
}


CResultList *CPhraseQuery::GetPositionMatchResult(CIndexReader *pReader,TermPostionsDocList *pDocList,PhraseQueue &que)
{
	TermPostionsDocList *pList = pDocList;
	CResultList         *pResultList = NULL;
	TermPostionsDocList *pBoolResult = NULL;


	TermResultList nextElement;
	
	while(!que.empty())
	{
		nextElement = que.top();
		que.pop();

		pResultList = nextElement.pResult;

		CTermPositionsDoc *pTermPositionDoc =  pList->getFirst();
		CPhraseDoc        *pPhraseDoc       =  (CPhraseDoc*)pResultList->getFirst();
		pBoolResult = NULL;

		while(pTermPositionDoc && pPhraseDoc)
		{
			if(pTermPositionDoc->GetDocID() < pPhraseDoc->GetDocID())
			{
				pTermPositionDoc = pList->getNext();
			}
			else if(pTermPositionDoc->GetDocID() >pPhraseDoc->GetDocID())
			{
				pPhraseDoc =(CPhraseDoc*) pResultList->getNext();
			}
			else
			{

				CIndexReader::TermPositionList lstPrev,lstCur;
				bool IsCurReadSuccess  = pReader->GetTermPosition(pPhraseDoc->GetPosID(),lstCur);
				if(IsCurReadSuccess)
				{
					TermPositionList *pPostionList = GetMatchPosition(pTermPositionDoc->pTermPositions,
						                                              nextElement.termIdx,lstCur);
					if(pPostionList)
					{ 
						if(!pBoolResult)
							pBoolResult = new TermPostionsDocList;
						float score = pTermPositionDoc->GetScore()+ pPhraseDoc->GetScore();
						pBoolResult->addNodeTail(new CTermPositionsDoc(pTermPositionDoc->GetDocID(),score,pPostionList));
					}

				}
				pTermPositionDoc = pList->getNext();
				pPhraseDoc       = (CPhraseDoc*) pResultList->getNext();


			}
		}
		
	   Delete_ResultList(pResultList);
	   DeletePositionDocList(pList);

	   if(!pBoolResult)
		   goto clearup;

	   pList = pBoolResult;

	}

clearup:
	while(!que.empty())
	{
		CResultList *pRemainList = que.top().pResult;
		que.pop();
		Delete_ResultList(pRemainList);
	}
	if(pList)
	{
		CResultList *pRet = new CResultList;
		CTermPositionsDoc *pPosDoc = pList->getFirst();
		while(pPosDoc)
		{
			pRet->addNodeTail(new CTermDoc(pPosDoc->GetDocID(),pPosDoc->GetScore()));
			pPosDoc= pList->getNext();
		}

		//
		//delete the list's memory
		DeletePositionDocList(pList);

		return pRet;
	}
	return NULL;
}
//
//
//�����ѯ���㷨���˴�����ʹ�õ��Ǿ�ȷƥ��Ķ����ѯ
//�����ѯ"term1 term2 term3"
//step1: �������ļ��ж�ȡterm1��resultlist������Ҫ��term1��Ӧ��λ����ϢҲ��ȡ���������Ϊresult1
//step2: �������ļ��ж�ȡterm2��resultlist�Լ�λ����Ϣ,���Ϊresult2
//step3: result1��result2�󽻲���������docid��ȣ����õ�result3
//result3��ÿ���ڵ����������
//     | docID               |
//     | <term1_id,λ����Ϣ1>  |
//     | <term2_id,λ����Ϣ2>   |
//  ��ÿ���ڵ㣬�������²���
  // 1)��λ����Ϣ1�е�ÿ��λ������pos1��ѭ������λ����Ϣ2������2�ֲ��Ҳ���(pos1+1)������
  //   2)������������1����pos1����λ����Ϣ����a,������2������pos1+1�ļ���λ����Ϣ����b
 // result3��ÿ���ڵ����������������ʾ����Ϊ�½�� result3'
 //  |docID   |
//   |<term1_id,λ����Ϣa|
//   |<term2_id,λ����Ϣb|
//step4:�������ѯterm3,�õ����result4,result3����result3����step3��һ���Ĳ����������
// ��������Ľ��
//
//
//

CResultList* CPhraseQuery::GetSearchResult(CIndexSearcher* pIndexSearcher)
{
	CIndexReader *pReader = pIndexSearcher->GetReader();
	ASSERT(pReader);

	PhraseQueue que;
	if(!GetPhraseResultQue(pReader,que))
		return NULL;

	TermPostionsDocList *pDocList = GetFirstTermPosDocList(pReader,que);
	if(pDocList)
		return GetPositionMatchResult(pReader,pDocList,que);

	return NULL;	
}


CPhraseQuery::TermPositionList*  CPhraseQuery::GetMatchPosition(int  nPrevIdx,const CIndexReader::TermPositionList &prev,
							                                     int  nCurIdx, const CIndexReader::TermPositionList &cur)
{
	size_t prevSize = prev.size();
	size_t curSize  = cur.size();

	size_t r = 0;  
	int low = 0,mid = 0,high = curSize-1;

	int findVal ;
	int curVal;

	TermPositionList *pResult = NULL;

	for(size_t i = 0; i < prevSize;++i)
	{
		findVal = prev[i]-nPrevIdx;
		if(low >high)
			break;

		while(low <= high)
		{
			r =mid = (low +high) >> 1;
			curVal = cur[mid]-nCurIdx;
		
			if(curVal > findVal)
			{
                high = mid -1;
			}
			else if(curVal< findVal)
			{
				low = mid+1;
			}
			else
			{
				if(pResult == NULL)
					pResult = new TermPositionList;
				pResult->push_back(findVal);
				break;

			}

		}
		low = r;
	}

	return pResult;

}


CPhraseQuery::TermPositionList*  CPhraseQuery::GetMatchPosition(TermPositionList *pTermPositions,
																int  nCurIdx, 
																const CIndexReader::TermPositionList &cur)
{
	size_t prevSize = pTermPositions->size();
	size_t curSize  = cur.size();

	size_t r = 0;  
	int low = 0,mid = 0,high = curSize-1;

	int findVal ;
	int curVal;

	TermPositionList *pResult = NULL;

	for(size_t i = 0; i < prevSize;++i)
	{
		findVal = pTermPositions->at(i);
		if(low >high)
			break;

		while(low <= high)
		{
			r =mid = (low +high) >> 1;
			curVal = cur[mid]-nCurIdx;

			if(curVal > findVal)
			{
				high = mid -1;
			}
			else if(curVal< findVal)
			{
				low = mid+1;
			}
			else
			{
				if(pResult == NULL)
					pResult = new TermPositionList;
				pResult->push_back(findVal);
				break;
			}

		}
		low = r;
	}

	return pResult;

}



