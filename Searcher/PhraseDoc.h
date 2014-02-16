#include "TermDoc.h"

class CPhraseDoc: public CTermDoc
{
public:
	CPhraseDoc(UINT nDocId,float score,UINT nPosID):CTermDoc(nDocId,score),m_nPosID(nPosID)
	{

	}
	UINT GetPosID() const { return m_nPosID;}

	CTermDoc *Clone()
	{
		return new CPhraseDoc(GetDocID(),GetScore(),GetPosID());
	}

private:
	int m_nPosID;
};