/****************************************************************************
 *
 * Copyright (c) 2000, 2001 
 *     Machine Group
 *     Software Research Lab.
 *     Institute of Computing Tech.
 *     Chinese Academy of Sciences
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Institute of Computing Tech. and the posession or use of this file requires 
 * a written license from the author.
 * Filename: ContextStat.h
 * Abstract:
 *           
 *			 interface for the CContextStat class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-1-24
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
#pragma  once 

#include "Global.h"
#include "Utility.h"
#include <hash_map>

struct tagContext
{
	int					nKey;					//The key word
	int					**aContextArray;		//The context array
	int					*aTagFreq;				//The total number a tag appears
	int					nTotalFreq;				//The total number of all the tags
	struct tagContext	*next;	//The chain pointer to next Context
};
typedef struct tagContext MYCONTEXT, *PMYCONTEXT;


class  CContextStat  
{

public:
	CContextStat();
	~CContextStat();

public:
	int		GetFrequency(int nKey,int nSymbol);
	double	GetContextPossibility(int nKey,int nPrev,int nCur);
	bool	Load(char *sFilename);


private:
	int m_nTableLen;
	int *m_pSymbolTable;
	PMYCONTEXT m_pContext;
	int m_nCategory;
	bool	GetItem(int nKey,PMYCONTEXT * pItemRet);
private:
	int FindConextIndex(int nKey);

	typedef stdext::hash_map<int,PMYCONTEXT> CONTEXTCACHE;
	CONTEXTCACHE m_cache;

	typedef stdext::hash_map<int,int> SYMKEY2INDEXCACHE;
	SYMKEY2INDEXCACHE m_symbolKey2indexCache;
	

};


////////////////////////////////////////////////////////////

inline CContextStat::CContextStat():m_pSymbolTable(NULL),m_pContext(NULL)
{

}


//Get the frequency which nSymbol appears
inline int CContextStat::GetFrequency(int nKey, int nSymbol)
{
	PMYCONTEXT pFound = NULL;

	if( !GetItem(nKey, &pFound) )
		return 0;
	int nIndex = BinarySearch(nSymbol,m_pSymbolTable,m_nTableLen);
	if(nIndex == -1)
		return 0;
	return pFound->aTagFreq[nIndex];
}

inline bool CContextStat::GetItem(int nKey,PMYCONTEXT *pItemRet)
{
	if(nKey == 0)
	{
		*pItemRet = m_pContext;
		return true;
	}
	CONTEXTCACHE::iterator it = m_cache.find(nKey);
	if(it != m_cache.end())
	{
		*pItemRet =  it->second;
		return true;
	}
	*pItemRet = NULL;
	return false;
}

inline int CContextStat::FindConextIndex(int nKey)
{
	SYMKEY2INDEXCACHE::iterator it = m_symbolKey2indexCache.find(nKey) ;
	if(it == m_symbolKey2indexCache.end())
		return -1;
	return it->second;
}

inline double CContextStat::GetContextPossibility(int nKey, int nPrev, int nCur)
{
	PMYCONTEXT pCur;
#ifdef _DEBUG
	int nCurIndex=	BinarySearch(nCur,m_pSymbolTable,m_nTableLen);
	int nPrevIndex=	BinarySearch(nPrev,m_pSymbolTable,m_nTableLen);
#else
    int nCurIndex  =  FindConextIndex(nCur);
	int nPrevIndex =  FindConextIndex(nPrev);
#endif


	if( !GetItem(nKey,&pCur)
		||nCurIndex == -1
		||nPrevIndex==-1
		||pCur->aTagFreq[nPrevIndex]==0
		||pCur->aContextArray[nPrevIndex][nCurIndex]==0 )
	{
		return 0.000001;//return a lower value, not 0 to prevent data sparse
	}

	int nPrevCurConFreq=pCur->aContextArray[nPrevIndex][nCurIndex];
	int nPrevFreq=pCur->aTagFreq[nPrevIndex];
	return 0.9*(double)nPrevCurConFreq / (double)nPrevFreq+0.1*(double)nPrevFreq/(double)pCur->nTotalFreq;
}


