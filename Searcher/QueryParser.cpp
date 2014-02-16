#include "Query.h"
#include "QueryParser.h"
#include "Global.h"

namespace QueryParser
{
	CTokenStream CQueryParser::m_stream ;
	CTermPool    CQueryParser::m_termPool;

	CQueryParser::~CQueryParser()
	{
		m_termPool.Recycle();
	}


	
	inline void CQueryParser::InitTokenStream(const char* strQuery,CAnalyzer *pAnalyzer)
	{
		m_stream.Init(strQuery,pAnalyzer);
	}
	inline void CQueryParser::InitTermPool()
	{
		m_termPool.Recycle();
	}
	CQuery* CQueryParser::Parse(const char *strQuery,const char *field,CAnalyzer *pAnalyzer)
	{
		CQuery *q = NULL;
		InitTokenStream(strQuery,pAnalyzer);
		InitTermPool();
		SetField(field);

		m_stream.GetNextToken(m_currentToken);
		q =  Query();
		return q;
	}

	CQuery* CQueryParser::Parse(const char  *queries[],int nQueryCount,const char* fields[],int nFieldCount,CAnalyzer* pAnalyzer)
	{
		if(nQueryCount == nFieldCount)
		{
             CBooleanQuery *q = new CBooleanQuery();
			 InitTermPool();
			 for(int i = 0; i < nFieldCount;++i)
			 {
				  InitTokenStream(queries[i],pAnalyzer);
                  SetField(fields[i]);

				  m_stream.GetNextToken(m_currentToken);
				  CQuery *query =  Query();

				  q->Add(new CBooleanClause(query,CBooleanClause::kOptional));
			 }
			 return q;
		}
		return NULL;
	}

	inline bool CQueryParser::match(TokenType expectedToken)
	{
		if(m_currentToken.GetTokenType() == expectedToken)
		{
			m_stream.GetNextToken(m_currentToken);
			return true;
		}
		else
		{
			return false;
			//error happens ,just ignore
			/*m_currentToken = m_stream.GetNextToken();*/
		}
	}

	//
	//
	//Query   ::=  [Modifer]Claus([Conjuction][Modifer]Clause)*
	CQuery* CQueryParser::Query()
	{
		std::vector<CBooleanClause*> clauses;
		clauses.reserve(CBooleanQuery::GetMaxClauseCount());
		CQuery *q = NULL,*pfirstQuery = NULL;
		ModiferType mods;
		ConjuctionType conj;
		mods = Modfier();
		q = Claus();
		AddClause(clauses,mods,CONJ_NONE,q);
		if(mods == MOD_NONE)
			pfirstQuery = q;
		bool bexitloop = false;
		while(true)
		{
			TokenType type = m_currentToken.GetTokenType();
			switch(type )
			{
			case kID:
			case kPlus:
			case kMinus:
			case kQuote:
			case kLParen:
			case kAnd:
			case kOr:
			case kNot:
			case kExclam:
				{
					conj = Conjuction();
					mods = Modfier();
					q    = Claus();
					if(q== NULL)
						bexitloop = true;
					else
					  AddClause(clauses,mods,conj,q);
				}
				break;
			default:
				{
					bexitloop = true;
				}
				break;
			}
			if(bexitloop)
				break;
		}
		size_t clause_size = clauses.size();
		if(clause_size == 1 && pfirstQuery )
			return pfirstQuery;
		else if(clause_size > 1)
		{
			CBooleanQuery *pQuery = new CBooleanQuery;
			for(size_t k = 0; k < clause_size;++k)
			{
				pQuery->Add(clauses[k]);
			}
			return pQuery;
		}
		return NULL;
	}

	//
	//Clause ::= Term | Phrase |"(" Query ")"
	//
	CQuery* CQueryParser::Claus()
	{
		CQuery *q = NULL;
		switch(m_currentToken.GetTokenType())
		{
		case kID:
		 {
			 CTerm *pterm = m_termPool.Get();
			 pterm->SetText(m_currentToken.GetText());
			 pterm->SetField(m_field);

			 q = new CTermQuery(pterm);
			 match(kID);

		 }
		 break;
		case kQuote:
		 {
			 std::vector<CToken> vec;
			 vec.reserve(CPhraseQuery::MAX_TERM_SIZE);//max phrase size is 255
			 CToken current;
			 bool ret = m_stream.GetNextToken(current);
			 TokenType type ;
			 while(ret)
			 {
				 type = current.GetTokenType();
				 if(type == kQuote)
				 {
					 match(kQuote);
					 break;
				 }
				 if(type == kID)
					 vec.push_back(current);
				 ret = m_stream.GetNextToken(current);
			 }

			 size_t vec_size = vec.size();
			 if(vec_size == 0)
				 return NULL;
			 else if(vec_size == 1)
			 {
				 CTerm *pTerm = m_termPool.Get();
				 pTerm->SetText(vec[0].GetText());
				 pTerm->SetField(m_field);

				 return new CTermQuery(pTerm);
			 }
			 else //phrase query
			 {
				 CPhraseQuery *pQuery = new CPhraseQuery();
				 for(size_t i = 0; i < vec_size;++i)
				 {
					 CTerm *pTerm = m_termPool.Get();
					 pTerm->SetText(vec[i].GetText());
					 pTerm->SetField(m_field);
					 pQuery->Add(pTerm);
				 }
				 pQuery->SetSlop(0);
				 q = pQuery;
			 }	 
		 }
		 break;
		case kLParen:
		 {
			 match(kLParen);
			 q = Query();
			 match(kRParen);
		 }
		default:
			break;
		}
		return q;
	}


	//
	//Modifier    ::= '-' | '+' | '!' | 'NOT'
	//
    CQueryParser::ModiferType   CQueryParser::Modfier()
	{
		ModiferType ret = MOD_NONE;
		switch(m_currentToken.GetTokenType())
		{
		case kMinus:
			{
				match(kMinus);
				ret = MOD_NOT;
			}
			break;
		case kPlus:
			{
				match(kPlus);
				ret = MOD_REQ;
			}
			break;
		case kNot:
		case kExclam:
			{
				match(m_currentToken.GetTokenType());
				ret = MOD_NOT;
			}
			break;
		default:
			break;
		}
		return ret;
	}

	//
	//
	//Conjunction ::= 'AND' | 'OR' 
	CQueryParser::ConjuctionType CQueryParser::Conjuction()
	{
		ConjuctionType conj_type = CONJ_NONE;
		switch(m_currentToken.GetTokenType())
		{
		case kAnd:
			{
				conj_type = CONJ_AND;
				match(kAnd);
			}
			break;
		case kOr:
			{
				conj_type = CONJ_OR;
				match(kOr);
			}
			break;
		default:
			break;
		}
		return conj_type;
	}

	void CQueryParser::AddClause(std::vector<CBooleanClause*> & clauses,ModiferType mods,ConjuctionType conj,CQuery *query)
	{
		size_t clauses_size = clauses.size();
		if(clauses_size > 0 && conj == CONJ_AND)
		{
			CBooleanClause * Claus = clauses[clauses_size-1];
			ASSERT(Claus);
			if(Claus->GetType() != CBooleanClause::kPhrobited )
				Claus->SetType(CBooleanClause::kRequired);
		}
		if(clauses_size > 0 && m_defaultOp  == OP_AND && conj == CONJ_OR)
		{
			CBooleanClause * Claus = clauses[clauses_size-1];
			ASSERT(Claus);
			if(Claus->GetType() != CBooleanClause::kPhrobited )
				Claus->SetType(CBooleanClause::kOptional);
		}
		if(query == NULL)
			return;
		bool bRequried,bProhibited;
		if(m_defaultOp == OP_OR)
		{
			bRequried  =  (mods == MOD_REQ);
			bProhibited = (mods == MOD_NOT);
			if(conj == CONJ_AND && !bProhibited)
				bRequried = true;
		}
		else
		{
			bProhibited = (mods == MOD_NOT);
			bRequried   = (!bProhibited && conj == CONJ_OR);
		}
		if(bRequried && !bProhibited)
			clauses.push_back(new CBooleanClause(query,CBooleanClause::kRequired));
		else if(!bRequried && !bProhibited)
			clauses.push_back(new CBooleanClause(query,CBooleanClause::kOptional));
		else if(!bRequried && bProhibited)
			clauses.push_back(new CBooleanClause(query,CBooleanClause::kPhrobited));
		else
		{
			ASSERT(0);
		}
	}


}
