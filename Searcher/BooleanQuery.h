#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "Query_Base.h"
#include "../include/MemoryPool/BaseAllocator.h"
#include <vector>

class CBooleanClause:public CBaseAlloc
{
public:
	typedef enum {kOptional,kRequired,kPhrobited} ClauseType;

	CBooleanClause(CQuery *query,ClauseType type)
		:m_pquery(query)
		,m_type(type)
	{
	}
	CQuery    *GetQuery()                { return m_pquery;}
	ClauseType GetType()                 { return m_type;  }
	void       SetType(ClauseType type)  { m_type = type;  }
	~CBooleanClause()                    { delete m_pquery;}
#ifdef _DEBUG
	std::string ToString();
#endif

private:
	CQuery    *m_pquery;
	ClauseType m_type;

};

class CBooleanQuery:public CQuery
{
public:
	typedef std::vector<CBooleanClause*> QUERYCLAUSES;
	QueryType GetQueryType() { return kBooleanQuery;}
	CBooleanQuery()          { m_clauses.reserve(m_maxClausesCount);};

	CBooleanQuery::~CBooleanQuery()
	{
		size_t isize = m_clauses.size();
		for(size_t i = 0; i < isize;++i)
			delete m_clauses[i];
	}

	static void SetMaxClauseCount(int maxClauseCount){ m_maxClausesCount = maxClauseCount;}
	static int  GetMaxClauseCount()                  { return m_maxClausesCount;}

	bool   Add( CBooleanClause *clause);
	QUERYCLAUSES GetClauses()  { return m_clauses;}
	virtual CResultList* GetSearchResult(CIndexSearcher* pIndexSearcher);

#ifdef _DEBUG
	std::string  ToString();
#endif

	virtual int    GetTermNum()const;
private:

	QUERYCLAUSES m_clauses;
	static int m_maxClausesCount;
};