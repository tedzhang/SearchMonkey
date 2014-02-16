/****************************************************************************
 *
 * Copyright (c) 2000, 2001 
 *     Software Research Lab.
 *     Institute of Computing Tech.
 *     Chinese Academy of Sciences
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Institute of Computing Tech. and the posession or use of this file requires 
 * a written license from the author.
 *
 * Abstract:
 *           Dynamic array, and the array is generally great and sparse.
 *           Dynamic Array Definition 
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
// DynamicArray.h: interface for the CDynamicArray class.
//
//////////////////////////////////////////////////////////////////////

#pragma  once 

#include "Global.h"
#include "../include/MemoryPool/BaseAllocator.h"

#define INFINITE_VALUE 10000.00			

struct array_node:public CBaseAlloc
{
	int	nCol;//row
	int	nRow;// column
    ELEMENT_TYPE	fValue;//The value of the array
	int				nPOS;
	int				nWordLen;
	char			szWord[MAX_WORD_SIZE];
	
    struct array_node* pNext;

	array_node():nCol(0),nPOS(0),nRow(0),fValue(INFINITE_VALUE),nWordLen(0),pNext(NULL)
	{
		memset( szWord, 0, MAX_WORD_SIZE);
	}

	
};
typedef array_node array_node_t;;


class CDynamicArray  
{
public:
	CDynamicArray();
	~CDynamicArray();

public:
	bool			GetElementInfo(int nRow, int nCol,ELEMENT_TYPE& pRetValue,int& pRetPOS, char* pRetWord=NULL);
	ELEMENT_TYPE	GetElement( int nRow, int nCol, array_node_t* pStart=NULL, array_node_t** pRet=NULL);
	int				SetElement( int nRow, int nCol, ELEMENT_TYPE fValue,int nPOS,char *sWord=0);
	void			SetEmpty();

	const CDynamicArray & operator =(const CDynamicArray &);
	bool  operator ==(const CDynamicArray &array);

	int m_nCol;
	int m_nRow;
private:
    array_node_t* m_pHead;//The head pointer of array chain
};


//
//

inline void CDynamicArray::SetEmpty()
{
	array_node_t *pCur = m_pHead,*pTemp;
	while( pCur)
	{
		pTemp = pCur->pNext;
	    delete pCur;
		pCur=pTemp;
	}
	m_pHead=NULL;
	m_nCol=0;
	m_nRow=0;
}

inline CDynamicArray::CDynamicArray():m_pHead(NULL),m_nRow(0),m_nCol(0)
{
	
}


inline CDynamicArray::~CDynamicArray()
{
  SetEmpty();
}

