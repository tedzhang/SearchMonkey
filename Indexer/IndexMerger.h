#pragma  once 
#include<map>
#include<set>
#include<list>
#include "TermInfo.h"

class CIndexDocInfo;
class CBtStgCtrl;

class CIndexMerger
{
public:
	CIndexMerger(CBtStgCtrl* pStorage);
	~CIndexMerger(){ }
	void Add(CIndexDocInfo *pDoc);
	void Close();

private:

	struct TermComparator
	{
		bool operator()(const CTerm *lhs,const CTerm *rhs) const 
		{
			if(lhs->GetFieldID() == rhs->GetFieldID())
				return strcmp(lhs->GetKeyword(),rhs->GetKeyword()) < 0;
			return lhs->GetFieldID() - rhs->GetFieldID();
		}
	};
	typedef std::map<CTerm*,UINT,TermComparator> Term2IDMap;

	struct TisItem
	{
		UINT    docID;
		UINT   tf;
		UINT    posID;
		TisItem(UINT doc,UINT tf,UINT posID,CIndexDocInfo* pDoc)
			:docID(doc),tf(tf),posID(posID),pDocInfo(pDoc)

		{

		}
		bool operator<(const TisItem& rhs) const
		{
			return docID < rhs.docID;
		}
		CIndexDocInfo* pDocInfo;

	};
	typedef std::map<UINT,std::set<TisItem> > TISMAP;

	//
	//该map中的值是tismap中的key
	Term2IDMap  m_term2IDMap;
	TISMAP      m_tismap;
	CBtStgCtrl *m_pStg;

	typedef std::list<CIndexDocInfo*> INDEXDOCS;
	INDEXDOCS m_indexDocs;

private:
	static void getKeywordCallback(const char* pKeyword, int nFieldID,UINT nKeyId);
	
};


inline CIndexMerger::CIndexMerger(CBtStgCtrl* pStorage):m_pStg(pStorage)
{
	m_term2IDMap.clear();
	m_tismap.clear();
}

