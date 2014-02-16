#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "Token_List.h"
#include "Analyzer.h"

namespace QueryParser
{

	class CTokenStream
	{
	public:
		void Init(const char *buffer,CAnalyzer *pAnalyzer);
		bool GetNextToken(CToken &token);
	private:
		size_t              m_iwordpos;
		static  CTokenList  m_tokenlist;
		size_t              m_tokenlist_size;
	};


	//
	// Token stream's implementation
	//

	inline void CTokenStream::Init(const char *buffer,CAnalyzer *pAnalyzer)
	{
		m_iwordpos = 0;
		m_tokenlist.Recycle();
		pAnalyzer->GetTokenList(buffer,m_tokenlist);
		m_tokenlist_size = m_tokenlist.size();
	}

	inline bool CTokenStream::GetNextToken(CToken &token)
	{
		if(m_iwordpos >= m_tokenlist_size)
		{  
			token.SetTokenType(kEnd);
			return false;
		}
		token = *m_tokenlist[m_iwordpos++];
		return true;
	}

}