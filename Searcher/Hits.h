#pragma  once 
#include "Global.h"
#include "Searcher_Export.h"
#include "../Indexer/Document.h"
#include "../include/MemoryPool/BaseAllocator.h"
#include <vector>

class CDocument;
class CQuery;
class CIndexSearcher;

class SEARCHER_API CHits:public CBaseAlloc
{
private:
	class CHitDoc:public CBaseAlloc
	{
	public:
		CHitDoc *next;  
		CHitDoc *prev;  
		CHitDoc(float s, UINT i) :m_fscore(s),m_docID(i),m_pDoc(NULL) { }
		~CHitDoc() 
		{
			delete m_pDoc;
		}
		CDocument* GetDocument() { return m_pDoc;}
		void       SetDocument(CDocument* pDoc){ m_pDoc = pDoc;}
		UINT       GetDocID() { return m_docID;}
		float      GetScore() { return m_fscore;}
	private:
		float m_fscore;
		UINT  m_docID;
		CDocument *m_pDoc;

	};

	CIndexSearcher *m_pSearcher;
	CQuery         *m_pQuery;
	int m_nLength;//the total number of hits

	typedef std::vector<CHitDoc*> HitDocs;
	HitDocs m_hitDocs;

	CHitDoc *m_pfirst;         // head of LRU cache
	CHitDoc *m_plast;          // tail of LRU cache
	int     numDocs ;      // number cached
	static const  int maxDocs = 200;    // max to cache

public:

	CHits(CIndexSearcher *s, CQuery *q)
		:m_pSearcher(s),m_pQuery(q),numDocs(0),
		m_pfirst(NULL),m_plast(NULL),m_nLength(0)
	{
		m_hitDocs.reserve(maxDocs);
		GetMoreDocs(50); // retrieve 100 initially
	}
	
	~CHits()
	{
       for(int i = 0; i < m_hitDocs.size();++i)
		   delete m_hitDocs[i];
	}

	void GetMoreDocs(int min) ;

	int  GetLength() 
	{
		return m_nLength;
	}


	CDocument* GetDoc(int n);

	float GetScore(int n); 
	UINT  GetDocID(int n);


private:
	CHitDoc* hitDoc(int n);
	void AddToFront(CHitDoc *hitDoc);
    void Remove(CHitDoc *hitDoc);
};
