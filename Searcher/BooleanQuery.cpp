#include "BooleanQuery.h"
#include "IndexReader.h"
#include "ResultList.h"
#include "IndexSearcher.h"


int CBooleanQuery::m_maxClausesCount  = 1024;

bool CBooleanQuery::Add( CBooleanClause *clause)
{
	if(m_clauses.size() >= m_maxClausesCount)
		return false;
	m_clauses.push_back(clause);
	return true;
}

int    CBooleanQuery::GetTermNum()const
{
	int nTermNum = 0;
	for(size_t i = 0; i < m_clauses.size();++i)
	{
        nTermNum += m_clauses[i]->GetQuery()->GetTermNum();
	}
	return nTermNum;
}

#ifdef _DEBUG
std::string CBooleanQuery::ToString()
{
	std::string str;
	for(int i = 0; i < m_clauses.size();++i)
	{
		str +="\n";
		str += m_clauses[i]->ToString();
	}
	return str;
}
#endif

//
//
// BooleanClause
#ifdef _DEBUG
std::string CBooleanClause::ToString()
{
	std::string str;
	CBooleanClause::ClauseType type = GetType();
	if(type == kRequired)
		str += "+";
	else if(type == kPhrobited)
		str +="-";
	else
		str += " ";
	return str+ m_pquery->ToString();
}
#endif

CResultList* CBooleanQuery::GetSearchResult(CIndexSearcher* pIndexSearcher)
{
	CIndexReader *pReader = pIndexSearcher->GetReader();
	ASSERT(pReader);
	CResultList *result = NULL;

	CBooleanClause *pClause;
	CQuery *pQuery;
	CBooleanClause::ClauseType clauseType;
	
	CResultList *pBoolResult = NULL;

	size_t isize = m_clauses.size();

	for(size_t i = 0 ; i < isize;++i)
	{
		pClause = m_clauses[i];
		pQuery  = pClause->GetQuery();
		clauseType = pClause->GetType();
		switch(clauseType)
		{
		case CBooleanClause::kOptional:
			{
				CResultList *pList = pQuery->GetSearchResult(pIndexSearcher);
				if(pList  && result)
				{
					pBoolResult = result->Merge(*pList);
					
				    //
					//because pMergeResult's memory is newly allocated
					Delete_ResultList(result);
					Delete_ResultList(pList);

					result = pBoolResult;

				}
				else  if(pList)
					result = pList;
			}
			break;
		case CBooleanClause::kPhrobited:
			{
				CResultList *pList = pQuery->GetSearchResult(pIndexSearcher);
				if(pList)
				{
					ASSERT(result != NULL && result->getNodeCount() > 0);
					if(result && result->getNodeCount() > 0)
					{
						pBoolResult = result->Difference(*pList);
					    
						Delete_ResultList(result);
						Delete_ResultList(pList);

						result = pBoolResult;
					}
				}
				break;
			}
		case CBooleanClause::kRequired:
			{
				CQuery::QueryType querytype = pQuery->GetQueryType();
				CResultList *pList1 = pQuery->GetSearchResult(pIndexSearcher);
				{

					if(!pList1) 
					{ 
						result = NULL; 
						continue;
					}
					if(i == isize  -1)//it's this the  last one 
					{
						if(!result)
						{ 
							result = NULL; 
							continue;
						}
						else if(result && pList1)
						{
							pBoolResult = result->Interset(*pList1);

							Delete_ResultList(result);
							Delete_ResultList(pList1);

							result = pBoolResult;
						}
						break;
					}

					//if the next clause is not required, we need to leave
					//the left task to others
					if(m_clauses[i+1]->GetType() != CBooleanClause::kRequired)
					{
						if(result && pList1)
						{
							pBoolResult = result->Merge(*pList1);

							Delete_ResultList(result);
							Delete_ResultList(pList1);
							result  = pBoolResult;

						}
						else if(pList1)
							result = pList1;
						continue;
					}
					i++;

					CResultList *pList2 = m_clauses[i]->GetQuery()->GetSearchResult(pIndexSearcher);
					if(!pList2)
					{ 
						result = NULL;
						continue;
					}

					CResultList* pRequiredList = NULL;
					if(pList1  && pList2 )
					{
						pRequiredList = pList1->Interset(*pList2);
						Delete_ResultList(pList1);
						Delete_ResultList(pList2);
					}

					if(pRequiredList && result)//because the default operation is or
					{ 
						pBoolResult = result->Merge(*pRequiredList);
						
						Delete_ResultList(result);
						Delete_ResultList(pRequiredList);

						result = pBoolResult;
					}
					else if(pRequiredList)
						result = pRequiredList;

				}


			}
			break;
		}
	}
	return result;
}