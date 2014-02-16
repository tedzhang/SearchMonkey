#pragma  once 

#include "../MemoryPool/BaseAllocator.h"

template<class T>
class CLinkNode:public CBaseAlloc
{
public:
	CLinkNode()
	{
		pData=NULL;
		pNext=NULL;
	};
	~CLinkNode()
	{
		delete pData;
		pData = NULL;
	};
	T*				pData;
	CLinkNode<T>*	pNext;
};

template<class T>
class CSingleLink:public CBaseAlloc
{
public:
	CSingleLink()
	{
		m_pHead=NULL;
		m_pTail=NULL;
		m_pCurrent=NULL;
		m_nCount=0;
	};
	~CSingleLink()
	{
		CLinkNode<T>* pNode=m_pHead;
		while( pNode != NULL )
		{
			m_pHead=	m_pHead->pNext;
			delete		pNode;
			pNode = NULL;
			pNode=		m_pHead;
		}
		m_pTail=NULL;
		m_pCurrent=NULL;
		m_nCount=0;
	};

	T*		getFirst();
	T*		getLast();

	T*		getNext();

	T*		getCurrent();
	int		addNodeTail(T* pNode);
	int		addNodeHead(T* pNode);
	int		rmFirstNode();
	int		getNodeCount()const;

	


protected:
	CLinkNode<T>*	m_pHead;
	CLinkNode<T>*	m_pTail;
	CLinkNode<T>*	m_pCurrent;
	int	m_nCount;
};


template<class T>
inline T*	CSingleLink<T>::getFirst()
{
	m_pCurrent=m_pHead;
	if( m_pHead == NULL )
	{
		return NULL;
	}
	return m_pHead->pData;
}

template<class T>
inline T*	CSingleLink<T>::getLast()
{
	m_pCurrent=	m_pTail;
	if( m_pTail == NULL )
	{
		return NULL;
	}
	return m_pTail->pData;
}

template<class T>
inline T*	CSingleLink<T>::getNext()
{
	if( m_pCurrent == NULL)
	{
		m_pCurrent=m_pHead;
	}
	else
	{
		m_pCurrent=	m_pCurrent->pNext;
	}
	if( m_pCurrent == NULL )
	{
		return NULL;
	}
	return m_pCurrent->pData;
}

template<class T>
inline T*	CSingleLink<T>::getCurrent()
{
	if( m_pCurrent == NULL )
	{
		return NULL;
	}
	return m_pCurrent->pData;
}



template<class T>
inline int	CSingleLink<T>::addNodeTail(T* pNode)
{
	if( pNode == NULL )
	{
		return -1;
	}
	CLinkNode<T>* pLinkNode= new CLinkNode<T>();
	if( pLinkNode == NULL )
	{
		return -1;
	}
	pLinkNode->pData=	pNode;
	//Êý¾ÝÎª¿Õ
	if( m_pTail == NULL )
	{
		m_pHead= pLinkNode;
		m_pTail= pLinkNode;
	}
	else
	{
		m_pTail->pNext=	pLinkNode;
		m_pTail=	m_pTail->pNext;
	}
	m_nCount++;

	return 0;
}


template<class T>
inline int	CSingleLink<T>::addNodeHead(T* pNode)
{
	if( pNode == NULL )
	{
		return -1;
	}
	CLinkNode<T>* pLinkNode= new CLinkNode<T>();
	if( pLinkNode == NULL )
	{
		return -1;
	}
	pLinkNode->pData=	pNode;

	pLinkNode->pNext=	m_pHead;
	m_pHead=pLinkNode;
	if( m_pTail == NULL )
	{
		m_pTail=m_pHead;
	}
	m_nCount++;
	return 0;
}


template<class T>
inline int	CSingleLink<T>::rmFirstNode()
{
	CLinkNode<T>* pNode=m_pHead;
	if( m_pHead == NULL )
	{
		m_pTail=NULL;
		return -1;
	}
	if( m_pTail == m_pHead)
	{
		m_pTail=NULL;
	}
	m_pHead=m_pHead->pNext;
	delete pNode;
	pNode = NULL;
	m_nCount--;
	return 0;
}

template<class T>
inline int	CSingleLink<T>::getNodeCount() const
{
	return m_nCount;
}




