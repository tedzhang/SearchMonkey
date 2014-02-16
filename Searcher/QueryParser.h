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
	// �Բ�ѯ�ַ������з���,���Ϊһ������query����
	// ��" A AND B"��������Ϊ һ��������ѯ�������а���
	// ����boolean clause,1��boolean cluase��һ��termQuery(��ѯ��A)
	// ����һ��Ҳ����һ��termQuery(��ѯ��B),Ȼ�����߽����󽻲���.
	// �÷�����ʹ��LL(1)�ݹ��½��ķ�������
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
		//������Ĳ�ѯ
		CQuery   *Parse(const char *query,const  char *field,CAnalyzer *pAnalyzer);

		//
		//�����Ĳ�ѯ
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
