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
 *
 * Abstract:
 *           Dynamic array, and the array is generally great and sparse.
 *           Dynamic Array Implement
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
// DynamicArray.cpp: implementation of the CDynamicArray class.
//
//////////////////////////////////////////////////////////////////////
#include "DynamicArray.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//获取数组中的元素
//如果nCol=-1;那么表示获取Row相应的第一个;
ELEMENT_TYPE CDynamicArray::GetElement(int nRow, int nCol, array_node_t* pStart, array_node_t** pRet)
{
	array_node_t* pCur=pStart;
	if(pStart==0)
	{
		pCur=m_pHead;
	}
	if(pRet!=0)
	{
		*pRet=NULL;
	}
	if( nRow > m_nRow || nCol > m_nCol )//Judge if the nRow and nCol is overflow
	{
		return INFINITE_VALUE;
	}
	
	while(pCur!=NULL
		&& ( nCol!=-1 && pCur->nCol < nCol 
				|| ( pCur->nCol==nCol && nRow!=-1 && pCur->nRow < nRow ) ) )
	{
		pCur=pCur->pNext;
	}

    //huangbin 这个地方可能会有点问题
	if( pRet != NULL )
	{
		*pRet=pCur;
	}
	
	//如果找到你想找的一样的东西,返回一个值, 如果不是一样的话返回后面一个,而且值为INFINITE_VALUE
	if( pCur!=NULL
		&&( pCur->nRow==nRow||nRow==-1  )
		&&( pCur->nCol==nCol||nCol==-1) )//Find the same position
	{
/*
 		//Find it and return the fValue
		if(pRet!=0)
			*pRet=pCur;
*/		return pCur->fValue;
	}
	return INFINITE_VALUE;
}

//插入到 Row 和 Col 位置, 以nRow为主
int CDynamicArray::SetElement(int nRow,  int nCol, ELEMENT_TYPE fValue,int nPOS,char *sWord)
{
   array_node_t* pCur=m_pHead, *pPre=NULL, *pAdd=NULL;//The pointer of array chain
   if(nRow > m_nRow)//Set the array nRow
   {
	   m_nRow=nRow;
   }
   if(nCol>m_nCol)//Set the array nCol
   {
	   m_nCol=nCol;
   }
   while( pCur!=NULL
			&&( pCur->nCol<nCol || ( pCur->nCol==nCol && pCur->nRow < nRow)) )
   {
	   pPre=pCur;
	   pCur=pCur->pNext;
   }

   if(pCur!=NULL && pCur->nRow==nRow && pCur->nCol==nCol)//Find the same position
   {
	   pCur->fValue=fValue;//Set the fValue
	   pCur->nPOS=nPOS;//Set the possible POS
   }
   else
   {
	   pAdd= new array_node_t;
	   pAdd->nCol=nCol;//get the fValue
	   pAdd->nRow=nRow;
	   pAdd->fValue=fValue;
	   pAdd->nPOS=nPOS;
	   if(sWord)//sWord is not empty
	   {
		   pAdd->nWordLen = strlen(sWord);
		   strncpy( pAdd->szWord,sWord, MAX_WORD_SIZE );
	   }
	   pAdd->pNext=pCur;
	   
	   if(pPre == NULL)//link pAdd after the pPre
		   m_pHead=pAdd;
	   else
		   pPre->pNext=pAdd;
   }
   return 0;
}


const CDynamicArray& CDynamicArray::operator=(const CDynamicArray &array)
{
	if(this == &array)
		return *this;

	SetEmpty();

	m_nCol = array.m_nCol;
	m_nRow = array.m_nRow;
	array_node_t *pCur = array.m_pHead;

	array_node_t *pAdd,*pTail=NULL;//The pointer of array chain

	while(pCur!=NULL)
	{
		pAdd= new array_node_t;
		pAdd->nCol  = pCur->nCol;//get the fValue
		pAdd->nRow  = pCur->nRow;
		pAdd->fValue= pCur->fValue;
		pAdd->nPOS  = pCur->nPOS;
		pAdd->pNext = NULL;
		
		if(pTail == NULL)
			m_pHead=pAdd;//The head element
		else
			pTail->pNext=pAdd;

		pTail=pAdd;

		pCur=pCur->pNext;
	}
	return *this;
}



bool CDynamicArray::operator ==(const CDynamicArray &array)
{
	if(m_nCol != array.m_nCol || m_nRow != array.m_nRow)//Row or Col not equal
	{
		return false;
	}
	
	array_node_t* pFirst=	array.m_pHead;
	array_node_t* pSecond=	m_pHead;

	//判断二者是否相等
	while(pFirst && pSecond && pFirst->nRow==pSecond->nRow 
			&& pFirst->nCol==pSecond->nCol
			&& pFirst->fValue==pSecond->fValue )
	{
		pFirst=pFirst->pNext;
		pSecond=pSecond->pNext;
	}

	if( pFirst==NULL && pSecond==NULL )
	{
		return true;
	}

	return false;
}


bool CDynamicArray::GetElementInfo(int nRow, int nCol, ELEMENT_TYPE& pRetValue, int& pRetPOS,char* sRetWord)
{
	array_node_t* pCur=m_pHead;
	pRetValue=INFINITE_VALUE;
	pRetPOS=0;
	if(nRow > m_nRow || nCol > m_nCol)//Judge if the nRow and nCol is overflow
	{
		return false;
	}
	
	//先比较Col,再比较Row, 如果Col都不等.那么Row等也没有意思. 如果Col等,着一个Row大于或等于查找条件的节点
	while(pCur!=NULL &&( nCol!=-1 && pCur->nCol<nCol || ( pCur->nCol== nCol && nRow!=-1 && pCur->nRow < nRow)))
	{
		pCur=pCur->pNext;
	}

	if( pCur!=NULL  && ( pCur->nRow==nRow || nRow==-1) 
					&& ( pCur->nCol==nCol || nCol==-1 ) )//Find the same position
	{	//Find it and return the fValue
		pRetValue=pCur->fValue;
		if(pRetPOS)
		{
			pRetPOS=pCur->nPOS;
		}
		if(sRetWord)//sWord is not empty
		{
			strcpy(sRetWord,pCur->szWord);
		}
	}
	return false;
}
