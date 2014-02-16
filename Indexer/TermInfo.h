#pragma once 

#include <string.h>
#include "TermPosInfo.h"

//
//
//关键字表结构如下：
//
//
//< <词，fieldID1>, m_nKeyID>
class CTerm 
{
public:
	CTerm(const char *szKeyword,int nFieldID):m_nKeyId(0),m_nFieldID(nFieldID)
	{
		strncpy(m_szKeyword,szKeyword,MAX_KEYWORD_SIZE);
	};
	~CTerm()
	{
	};
	
    void  SetFieldID(int nFieldID)
	{
		m_nFieldID = nFieldID;
	}
	int   GetFieldID() const
	{
		return m_nFieldID;
	}

	void	SetKeyword(const char* pWord)
	{
		strncpy( m_szKeyword, pWord, MAX_KEYWORD_SIZE );
	};
	
	const char* GetKeyword()const
	{
		return m_szKeyword;
	};
	
	UINT GetKeyID()const
	{
		return m_nKeyId;
	};
   
	void SetKeyID(UINT nKeyId)
	{
		m_nKeyId= nKeyId;
	}
private:
	char	m_szKeyword[MAX_KEYWORD_SIZE];
	UINT    m_nKeyId;
	int     m_nFieldID;
};



//
//
//一个词的完整信息
//
class CTermInfo
{
public:
	CTermInfo(CTerm* pKeyword,CPosInfo *pPosNode,int nFreq)
		:m_pKeyword(pKeyword),m_pPosNode(pPosNode),m_nFreq(nFreq)

	{
	}
	CTerm*	m_pKeyword;		//分词关键字
	CPosInfo*	m_pPosNode;		//位置链表
	UINT         m_nFreq;
};