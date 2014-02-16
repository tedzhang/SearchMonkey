#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "Query_Base.h"
#include "Term.h"
//
//词查询，最简单的查询形式：查询单个的词

class CTermQuery : public CQuery
{
public:
	QueryType GetQueryType()                  { return kTermQuery;}
	CTermQuery( CTerm *term):m_pterm(term)    { };
	CTerm* GetTerm() const   { return m_pterm;}
	virtual CResultList* GetSearchResult(CIndexSearcher* pIndexSearcher);
#ifdef _DEBUG
	std::string ToString() 
	{
		return std::string(m_pterm->GetText());
	}
#endif
	virtual int  GetTermNum() const  { return 1;}
private:
	CTerm *m_pterm;

};