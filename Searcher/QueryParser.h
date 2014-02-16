#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include <vector>
#include "Token_Stream.h"
#include "Term.h"
#include <string>
#include "Searcher_Export.h"

class CQuery;
class CBooleanClause;

namespace QueryParser
{
	//
	// 对查询字符串进行分析,结果为一个个的query对象
	// 如" A AND B"分析后结果为 一个布尔查询对象，其中包含
	// 两个boolean clause,1个boolean cluase是一个termQuery(查询词A)
	// 另外一个也包含一个termQuery(查询词B),然后两者进行求交操作.
	// 该分析器使用LL(1)递归下降的方法分析
	//
	class SEARCHER_API CQueryParser
	{
	public:
		typedef enum {OP_OR,OP_AND}Operator;
		CQueryParser::CQueryParser( ):m_defaultOp(OP_OR)
		{

		}
		~CQueryParser();
		//
		//单个域的查询
		CQuery   *Parse(const char *query,const  char *field,CAnalyzer *pAnalyzer);

		//
		//多个域的查询
		CQuery  *Parse(const char  *queries[],int nQueryCount,const char* fields[],int nFieldCount,CAnalyzer* pAnalyzer);


		void      SetDefaultOperator(Operator op){ m_defaultOp = op;}
		Operator  GetDeaultOperator()            { return m_defaultOp;}
		size_t    GetTermsNumber() { return m_termPool.size(); }//return the total number of search keyword in one query

	private:
		typedef enum { MOD_NONE,MOD_REQ/*+*/,MOD_NOT/*-'*/} ModiferType;
		typedef enum { CONJ_NONE,CONJ_AND,CONJ_OR} ConjuctionType;
		ModiferType    Modfier();
		ConjuctionType Conjuction();
		CQuery *Query();
		CQuery *Claus();
		void AddClause(std::vector<CBooleanClause*> & clauses,ModiferType mods,ConjuctionType conj,CQuery *query);
		void SetField(const char *field) { m_field = field;}

	private:
		bool      match(TokenType expectedToken);
		void      InitTokenStream(const char *strQuery,CAnalyzer *pAnalyzer);
		void      InitTermPool();
	private:
		Operator             m_defaultOp;
		CToken               m_currentToken;
		std::string          m_field;
		static               CTokenStream        m_stream;
		static               CTermPool           m_termPool;

	};
}
