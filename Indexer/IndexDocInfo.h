#pragma  once 

#include <list>
#include <map>
#include "Global.h"
class CTermInfo;

class CIndexDocInfo
{
public:
	typedef std::list<CTermInfo*> TERMSLIST;
	TERMSLIST m_terms;

	typedef std::map<int,std::string> FieldID2UnTokenizedMap;
	FieldID2UnTokenizedMap m_UnTokenizedInfo;

	CIndexDocInfo(UINT nDocID):m_nDocID(nDocID)
	{

	}
	void AddTerm(CTermInfo* pTerm);
	void AddUntokenizedInfo(int fieldID,const std::string & value);
	UINT GetDocID() { return m_nDocID;}
	void SetDocID(UINT ndocID) { m_nDocID = ndocID;}

private:
	UINT m_nDocID;
	
};


inline void CIndexDocInfo::AddTerm(CTermInfo* pTerm)
{
	m_terms.push_back(pTerm);
}