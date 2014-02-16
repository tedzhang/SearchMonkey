#include "Analyzer.h"
#include "../Segmenter/analysis.h"
#include <ctype.h> //for isspace
//#ifdef _DEBUG
//#pragma comment (lib, "../lib/debug/analysis.lib")
//#else
//#pragma comment (lib, "../lib/release/analysis.lib")
//#endif

namespace QueryParser
{
	//
	//
	// Chinese analyzer's implementation
	const char CChineseAnalyzer::CONST_LPAREN = '(';
	const char CChineseAnalyzer::CONST_RPAREN = ')';
	const char CChineseAnalyzer::CONST_PLUS   = '+';
	const char CChineseAnalyzer::CONST_MINUS  = '-';
	const char CChineseAnalyzer::CONST_EXCLAM = '!';
	const char CChineseAnalyzer::CONST_QUOTE  ='\"';
	char       CChineseAnalyzer::m_charbuffer[MAX_BUFFER_SIZE];


	CChineseAnalyzer* CChineseAnalyzer::m_pThis = NULL;

	CChineseAnalyzer::CChineseAnalyzer()
	{
		m_pThis = this;  
	}
	void CChineseAnalyzer::Analysis_Func(const char* pKeyword, unsigned int nStartPos, unsigned short nWordIndex, float fScore, int nAttr)
	{
		if(nAttr & ATTR_STOPWORD) return;
		TokenType type;
		int iLength;

		type =kID;
		iLength = strlen(pKeyword);
		m_pThis->LookupReserveWord(pKeyword,iLength,type);
		CToken * pToken = m_pThis->m_ptokenlist->Get();
		pToken->SetText(pKeyword);
		pToken->SetTokenType(type);
		pToken->SetStartPos(nStartPos);
		pToken->SetEndPos(nStartPos+iLength);

	}

	inline void CChineseAnalyzer::ParseString(const char* str,int iStartPos,CTokenList &tokenlist)
	{
		m_ptokenlist = &tokenlist;
		CAnalysis::analysisText((char*)str,(int)strlen(str),Analysis_Func,true);
		//CAnalysis::analysisText((char*)str,strlen(str),Analysis_Func,true);

	}

	void CChineseAnalyzer::GetTokenList(const char *strBuffer,CTokenList &result)
	{
		if(!PreProcess(strBuffer))
			return;
		int iStartPos = 0,iEndPos = strlen(m_charbuffer)-1;
		int iPos = iStartPos;
		TokenType type;
		int iCount = 0;
		static char strText[LEX_UNIT_SIZE];
		while(iPos <= iEndPos)
		{
			char current_char = m_charbuffer[iPos];
			if(LookupReserveChar(current_char,type))
			{
				if(iPos-iStartPos > 0)
				{
					memcpy(strText,m_charbuffer+iStartPos,(iPos-iStartPos));
					strText[iPos-iStartPos] ='\0';
					ParseString(strText,iStartPos,result);

				}
				//be careful.the token is from the pool
				CToken * pToken = result.Get();
				pToken->SetText(current_char);
				pToken->SetTokenType(type);
				pToken->SetStartPos(iPos);
				pToken->SetEndPos(iPos);
				iStartPos = iPos+1;

			}
			iPos++;
		}
		if(iPos-1 > iStartPos)
		{
			memcpy(strText,m_charbuffer+iStartPos,(iPos-iStartPos));
			strText[iPos-iStartPos] ='\0';
			ParseString(strText,iStartPos,result);
		}

	}



	inline bool CChineseAnalyzer::PreProcess(const char* str)
	{
		DelEscapeChar(str);
		//todo: remove the stop words from the list

		return true;

	}

	inline void CChineseAnalyzer::DelEscapeChar(const char* str)
	{
		char current_char = 0;
		int i = 0;
		//trim the left white space
		while((current_char = str[i]) &&  (current_char > 0 && isspace(current_char)))
			i++;
		int ibuffer_length = strlen(str)-i;
		memcpy(m_charbuffer,str+i,ibuffer_length+1);

		//todo: fix the defect can not eliminate other white space
		for(i= ibuffer_length-1; i >= 0; --i)
		{
			current_char = m_charbuffer[i];
			if( current_char == '\t' || current_char =='\n' ||current_char == '\\'
				|| current_char == '*'||current_char == '?'|| current_char == '/'
				|| current_char == '#')
			{
				int iLen = strlen(m_charbuffer);
				for(int k = i+1; k < iLen;k++)
					m_charbuffer[k-1] = m_charbuffer[k];
			}
		}

	}


	inline bool CChineseAnalyzer::LookupReserveWord(const char *str,int iLength,TokenType& type)
	{
		static struct  
		{
			const char *words;
			int   iStrLen;
			TokenType    type;
		}ReserveWords[] = {  { "AND",3,kAnd},{ "OR",2,kOr},{ "NOT",3,kNot}};
		static const int words_size = sizeof(ReserveWords)/sizeof(ReserveWords[0]);
		char ch = str[0];
		if(ch == 'A' || ch == 'N'||ch == 'O')
		{
			for(int i = 0; i < words_size;++i)
			{
				if(iLength != ReserveWords[i].iStrLen)
					continue;
				if(strcmp(ReserveWords[i].words,str) == 0)
				{
					type = ReserveWords[i].type;
					return true;
				}
			}
		}
		return false;
	}


	inline bool CChineseAnalyzer::LookupReserveChar(const char ch, TokenType &type)
	{

		static struct 
		{
			const char ch;
			TokenType  type;
		}g_ReservedChars[] = { {CONST_EXCLAM,kExclam},
		{ CONST_QUOTE,kQuote},
		{ CONST_LPAREN,kLParen},
		{ CONST_RPAREN,kRParen},
		{ CONST_PLUS,kPlus},
		{ CONST_MINUS,kMinus},
		};
		static const size_t size = sizeof(g_ReservedChars)/sizeof(g_ReservedChars[0]);
		int low = 0,high = size-1,mid;
		while(low <= high)
		{
			mid =(low +high) >>1;
			if(ch > g_ReservedChars[mid].ch)
			{ 
				low = mid+1;
			}
			else if(ch < g_ReservedChars[mid].ch)
			{
				high = mid-1;
			}
			else
			{
				type = g_ReservedChars[mid].type;
				return true;
			}
		}
		return false;

	}
}



