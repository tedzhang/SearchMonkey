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
 *           Queue 
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
// Queue.h: interface for the CQueue class.
//
//////////////////////////////////////////////////////////////////////


#pragma  once 

#include "Global.h"
#include "../include/MemoryPool/BaseAllocator.h"


class CQueue:public CBaseAlloc
{
public:
	bool IsSingle();
	bool IsEmpty(bool bBrowsed=false);


	//
	//@param nValue:The value for parent node
	//@param nIndex:number of index in the parent node
	//@param eWeigth:the weight of last path 
	void Push(int nValue=0,int nIndex=0,ELEMENT_TYPE eWeight=0);
    
	//@param  npValue: The value for node
	//@param  npIndex: number of index in the parent node
	//@param  epWeight: the weight of last path
	//@param  bModify : false: not modify the data
	//@param  bFirstGet: first get data,just for browse 
	//

	bool Pop( int& npValue, int& npIndex,ELEMENT_TYPE& epWeight, 
		      bool  bModify=true,bool bFirstGet=true);
	
	CQueue ();
    ~CQueue();
private:
	struct queue_node:public CBaseAlloc
	{
		int nParent;//the weight
		int nIndex;//number of index in the parent node
		ELEMENT_TYPE eWeight;//the weight of last path
		struct queue_node *pNext;
	};
	typedef queue_node queue_node_t;

	queue_node_t* m_pHead;//The chain sort according the weight of shortest path
	queue_node_t* m_pLastAccess;//The node last accessed
};

inline CQueue::CQueue()
{
	m_pHead=NULL;
	m_pLastAccess=NULL;
}

inline CQueue::~CQueue()
{
	queue_node_t*	pCur=m_pHead;
	while( pCur!=NULL )
	{
		m_pHead=pCur->pNext;
		delete pCur;
		pCur=m_pHead;
	}
}



//bBrowsed=true: judge whether the browse pointer got end.
inline bool CQueue::IsEmpty(bool bBrowsed)
{
	if(bBrowsed==true)
	{
		return (m_pLastAccess==NULL);
	}
	return ( m_pHead==NULL );
}


inline bool CQueue::IsSingle()
{
	return (m_pHead != NULL && m_pHead->pNext==NULL);
}


inline void  CQueue::Push(int nValue, int nIndex,ELEMENT_TYPE eWeight)
{
	queue_node_t*	pAdd;
	queue_node_t*	pCur= m_pHead;
	queue_node_t*	pPre=0;//The pointer of queue chain

	while(pCur!=NULL && pCur->eWeight < eWeight)
	{
		//Get the proper position
		pPre=pCur;
		pCur=pCur->pNext;
	}
	pAdd=	new queue_node_t;
	pAdd->nParent=nValue;
	pAdd->nIndex=nIndex;
	pAdd->eWeight=eWeight;
	pAdd->pNext=pCur;

	if(pPre==0)
	{
		m_pHead=pAdd;
	}
	else
	{
		pPre->pNext=pAdd;
	}
}


inline bool  CQueue::Pop(int& npValue,int& npIndex,ELEMENT_TYPE& epWeight,bool  bModify,bool  bFirstGet)
{
	queue_node_t* pTemp;

	if(bModify) 
	{
		pTemp=m_pHead;//The temp buffer
	}
	else 
	{
		if( bFirstGet )//First get the data
		{
			m_pLastAccess=m_pHead;//The temp buffer
		}
		pTemp=m_pLastAccess;
	}

	if(pTemp==NULL)
	{
		return false;//fail get the value
	}

	npValue=pTemp->nParent;
	npIndex=pTemp->nIndex;
	epWeight=pTemp->eWeight;

	if(bModify)
	{
		m_pHead=pTemp->pNext;
		delete pTemp;
	}
	else
	{
		m_pLastAccess=	pTemp->pNext;
	}
	return true;
}



