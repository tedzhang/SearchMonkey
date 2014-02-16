#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "Token.h"
#include "../include/Utilities/SmallObjectPool.h"

namespace QueryParser
{

	//
	//
	//token list implementation

	class CTokenList:public CStaticSmallObjectPool<CToken*,1024>
	{
	public:
		CTokenList() 
		{
			for(int i = 0;i < 1024;++i)
				m_pool[i] = new CToken();
		}

	};

}