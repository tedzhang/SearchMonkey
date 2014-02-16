#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "Token_List.h"
#include "Searcher_Export.h"

class CAnalysis;
namespace QueryParser
{

	class CAnalyzer
	{
	public:
		virtual void  GetTokenList(const char *strBuffer,CTokenList &result) = 0;
	};



	class SEARCHER_API CChineseAnalyzer:public CAnalyzer
	{
	public:
		enum {MAX_BUFFER_SIZE = 2048};
		enum {LEX_UNIT_SIZE = 1024};
		CChineseAnalyzer();
		void  GetTokenList(const char *strBuffer,CTokenList &result) ;
	private:
		bool  LookupReserveWord(const char *str,int iLength,TokenType &type);
		bool  LookupReserveChar(const char ch,TokenType &type);
		void  DelEscapeChar(const char* str);
		bool  PreProcess(const char* str);
		void  ParseString(const char *str,int iStartPos,CTokenList &tokenlist);
	private:
		static void Analysis_Func(const char* pKeyword, unsigned int nStartPos,unsigned short nWordIndex, float fScore, int nAttr);
		static CChineseAnalyzer *m_pThis;
		CTokenList *m_ptokenlist;

	private:
		static  char       m_charbuffer[MAX_BUFFER_SIZE];


		static const char CONST_LPAREN;
		static const char CONST_RPAREN;
		static const char CONST_PLUS;
		static const char CONST_MINUS;
		static const char CONST_EXCLAM;
		static const char CONST_QUOTE;

	};

}
