#pragma  once 

#include "Global.h"
#include "../include/Utilities/Lock.h"
#include "TermInfo.h"
#include "FieldInfos.h"
#include <string>
#include <map>
#include <set>
#include <list>
#include "IndexExports.h"


class CBtStgCtrl;//btree database

class CDocument;
class CFieldInfos;
class CIndexDocInfo;

class INDEXREXPORT_API CIndexWriter
{
public:
	enum { MAX_MERGE_DOCNUMS = 25000};
	CIndexWriter():m_pBtStgCtrl(NULL),m_nMergeFactor(MAX_MERGE_DOCNUMS) { m_pThis  = this; }
	~CIndexWriter();
	bool Open(const char* IndexfullFilename);
	void Close();
    void AddDocument(const CDocument &Doc);

	void SetMergeFactor(UINT nDocs){ m_nMergeFactor = nDocs;}
	UINT GetMergeFactor()          { return m_nMergeFactor;}

private:
	void   InvertDocument(const CDocument &doc,CIndexDocInfo *pDoc);
	void   MergeDocs();
	static void TermCallBack(const char* pKeyWord, UINT nStartPos, 
		                     USHORT nWordIndex, float fScore, int nAttr);

private:
	UINT  m_nMergeFactor;

	struct PosInfo2Frequency
	{
		UINT nFreq;
		CPosInfo *posInfo;
	};
  
	typedef std::map<std::string,PosInfo2Frequency> KEYWORD2POSMAP;
	KEYWORD2POSMAP m_keywords;



private:
	
	CBtStgCtrl*		 m_pBtStgCtrl;
	CLock            m_lock;
	static          CIndexWriter* m_pThis;
private:

	CFieldInfos m_fieldInfos;
	typedef std::list<CIndexDocInfo*> INDEXDOCCOLLECTION;
	INDEXDOCCOLLECTION m_indexedDocs;
	
	

};