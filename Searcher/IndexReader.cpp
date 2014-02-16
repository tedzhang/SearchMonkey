
#include "../Indexer/BtStgCtrl.h"
#include "../Indexer/FieldInfos.h"
#include "../Indexer/Field.h"
#include "IndexReader.h"
#include "IndexInput.h"
#include "Query.h"
#include "ResultList.h"
#include "PhraseDoc.h"
#include <math.h>

CBtStgCtrl*  CIndexReader::m_pStorage    = NULL;
CFieldInfos* CIndexReader::m_fieldInfos = NULL;
char*        CIndexReader::m_postingBuffer = NULL;

CIndexReader* CIndexReader::m_pThis = NULL;


CIndexReader::CIndexReader(CQuery *pQuery)
{
	Init();
	m_nTermCount = pQuery->GetTermNum();
	m_pThis = this;
}
CIndexReader::~CIndexReader()
{
    
}

void CIndexReader::Close()
{
	return m_pStorage->Close();
}

void CIndexReader::Init()
{
	if(!m_pStorage)
		m_pStorage = new CBtStgCtrl;
	if(!m_fieldInfos)
		m_fieldInfos = new CFieldInfos;
	if(!m_postingBuffer)
		m_postingBuffer = new char[MAX_POSTITING_BUFFER_SIZE];

}

bool CIndexReader::Open(const char* indexFullfilename)
{
	 Init();

     if(!m_pStorage->Open(indexFullfilename,false))
		 return false;
	  m_fieldInfos->Read(m_pStorage);
	  return true;
}


inline float CIndexReader::GetTermWeight(term_id id)
{
	return (float)(m_nTermCount- id+1)/m_nTermCount;
}

inline term_id CIndexReader::GetTermId(const CTerm &term)
{
  
	term_id id;
	TERMMAPS::iterator it = m_termmaps.find(term);
	if(it == m_termmaps.end())
	{
		id = m_termmaps.size()+1;
		m_termmaps.insert(std::make_pair(term,id));
	}
	else
		id = it->second;

	return id;
}

void CIndexReader::Score(const CTerm &term,CResultList *pDocList)
{
	int     df         = pDocList->getNodeCount();
	float   idf        = log((float)((float)GetTotalDocCount()/(df+1) + 1.0f));
	float   term_weight= GetTermWeight(GetTermId(term));

	CTermDoc *p = pDocList->getFirst();
	while(p)
	{
		p->SetScore(p->GetScore()*idf*term_weight);
		p = pDocList->getNext();
	}
}

UINT        CIndexReader::GetTermDocs  (const CTerm& term,CResultList   *&result)
{
   UINT nBufferSize = MAX_POSTITING_BUFFER_SIZE;
   UINT nKeyID = GetKeyID(term.GetText().c_str(),term.GetField().c_str());
   if(nKeyID == UINT_MAX)
	   return 0;

   m_pResult = NULL;
   SetBoost(m_fieldInfos->GetFieldBoost(term.GetText().c_str()));

   if(0 != m_pStorage->getKeyIndex(nKeyID,GetTermDocCallback))
	   return false;

   result = m_pResult;

   Score(term,result);

   return result->getNodeCount();
}


UINT     CIndexReader::GetPhraseDocs(const CTerm& term,CResultList *&result)
{
	UINT nKeyID = GetKeyID(term.GetText().c_str(),term.GetField().c_str());
	if(nKeyID == UINT_MAX) 
		return 0;

	SetBoost(m_fieldInfos->GetFieldBoost(term.GetField().c_str()));

	m_pResult = NULL;
	if(0 != m_pStorage->getKeyIndex(nKeyID,GetPhraseDocCallback))
		return 0;

	result = m_pResult;
	Score(term,result);

	return result->getNodeCount();
}



UINT CIndexReader::GetTotalDocCount()
{
	return m_pStorage->GetMaxDocnum();
}

UINT   CIndexReader::GetKeyID(const char *strword,const char *field)
{
	int nFieldNum = m_fieldInfos->GetFieldNumber(field);
	assert(nFieldNum >= 0);

	//
	//find it the keyword table

	UINT nKeyID;
	if(0 != m_pStorage->getKeyword(strword,nFieldNum,nKeyID))
		return UINT_MAX;

	return nKeyID;
}

bool CIndexReader::GetIndexInfo(const char *strword,const char* field,char *result,UINT &nResultBufferSize)
{
	

	UINT nKeyID = GetKeyID(strword,field);

	//
	//find in the tis table
	//
	if(0 != m_pStorage->getKeyIndex(nKeyID,result,nResultBufferSize))
		return false;

	return true;

}
bool CIndexReader::GetTermPosInfo(UINT nPosID,char *PosInfo,UINT &nBuffersize)
{
  return 0== m_pStorage->getPosInfo(nPosID,PosInfo,nBuffersize);
}

bool CIndexReader::GetUnTokenizedFieldInfo(UINT nDocID,char *value,UINT &nValueSize)
{
	return 0== m_pStorage->getDocSrc(nDocID,value,nValueSize);
}

bool CIndexReader::GetTermPosition(UINT posId,TermPositionList &termPositions)
{
	 UINT nBufferSize = MAX_POSTITING_BUFFER_SIZE;
     if(!GetTermPosInfo(posId,m_postingBuffer,nBufferSize))
		 return false;

	 CIndexInputBuffer posbuffer(m_postingBuffer);

	 UINT iPos;
	 UINT iLastPos = 0,iNewPos;

	 while(posbuffer.Length() < nBufferSize)
	 {
		iPos = posbuffer.ReadVInt();
		iNewPos = iPos + iLastPos;
		termPositions.push_back(iNewPos);
		iLastPos = iNewPos;
	 }
	 return true;
}

CDocument* CIndexReader::GetDocument(UINT nDocId)
{
	UINT nBufferSize = MAX_POSTITING_BUFFER_SIZE;
	if(0 != m_pStorage->getDocSrc(nDocId,m_postingBuffer,nBufferSize))
		return NULL;

	CIndexInputBuffer inputBuffer(m_postingBuffer);

	CDocument *pDoc = new CDocument;

	while(inputBuffer.Length() < nBufferSize)
	{
		UINT nFieldNum = inputBuffer.ReadVInt();
		std::string str = inputBuffer.ReadString();
		pDoc->Add(CField(m_fieldInfos->GetFieldName(nFieldNum),str));
	}
	return pDoc;

}

void CIndexReader::GetTermDocCallback(UINT nDocId, char* pIndexInfo, int nSize)
{
	

	UINT DocId,tf;
	float fOriginalScore = 1.0f;

	float fFieldBoost = m_pThis->GetBoost();

	if(!m_pThis->m_pResult)
	    m_pThis->m_pResult = new CResultList;

	UINT   lastDocId = 0;
	UINT    newDocId = 0;

	CIndexInputBuffer buffer(pIndexInfo);

	while(buffer.Length() < nSize)
	{
		DocId  = buffer.ReadVInt();
		tf     = buffer.ReadVInt();
		buffer.ReadVInt();
		newDocId = lastDocId +DocId;
		//todo : add original score in the index
		m_pThis->m_pResult->addNodeTail(new CTermDoc(newDocId,tf*fFieldBoost*fOriginalScore));
		lastDocId = newDocId;
	}
}

void CIndexReader::GetPhraseDocCallback(UINT nDocId,char *pIndexInfo,int nSize)
{
	float fOriginalScore = 1.0f;

	float fFieldBoost = m_pThis->GetBoost();

	if(!m_pThis->m_pResult)
		m_pThis->m_pResult = new CResultList;

	UINT DocId,tf,nPosID,newDocId,newPosId;

	CIndexInputBuffer buffer(pIndexInfo);

	UINT lastDocId = 0,lastPosId = 0;
	while(buffer.Length() < nSize)
	{
		DocId  = buffer.ReadVInt();
		tf     = buffer.ReadVInt();
		nPosID = buffer.ReadVInt();

		newDocId = DocId+ lastDocId;
		newPosId = nPosID +lastPosId;

		m_pThis->m_pResult->addNodeTail((CTermDoc*) new CPhraseDoc(newDocId,tf*fFieldBoost*fOriginalScore,newPosId));
		lastDocId = newDocId;
		lastPosId = newPosId;
	}
}
