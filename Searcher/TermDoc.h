#pragma  once 
#include "../include/MemoryPool/BaseAllocator.h"
#include "Global.h"



//
//
//匹配的文档对象
//
//

class CTermDoc :public CBaseAlloc
{
public:

	virtual ~CTermDoc()
	{

	}

	CTermDoc(UINT docID,float score)
		:m_docID(docID),
		m_fScore(score)
	{
	}

	virtual CTermDoc *Clone()
	{
		return  new CTermDoc(m_docID,m_fScore);
	}
	float  GetScore() const { return m_fScore;}
	UINT   GetDocID() const { return m_docID; }
	void   SetScore(float score){ m_fScore = score;}


private:
	UINT    m_docID;//文档id，唯一
	float   m_fScore;

};




