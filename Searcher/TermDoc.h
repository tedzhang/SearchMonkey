#pragma  once 
#include "../include/MemoryPool/BaseAllocator.h"
#include "Global.h"



//
//
//ƥ����ĵ�����
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
	UINT    m_docID;//�ĵ�id��Ψһ
	float   m_fScore;

};




