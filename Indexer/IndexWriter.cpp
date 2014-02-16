#include "IndexWriter.h"
#include "Document.h"
#include "FieldInfos.h"
#include "BtStgCtrl.h"
#include "../Segmenter/analysis.h"
#include "IndexDocInfo.h"
#include "IndexMerger.h"
#include<iostream>

#define DICT_PATH "F:\\temp\\Data\\"

CIndexWriter* CIndexWriter::m_pThis = NULL;

bool CIndexWriter::Open(const char* IndexfullFilename)
{
	if(!m_pBtStgCtrl)
		m_pBtStgCtrl = new CBtStgCtrl;
	if(!m_pBtStgCtrl->Open(IndexfullFilename,true))
	{
		delete m_pBtStgCtrl;
		return false;
	}
	//m_nMaxKeyWordID = m_pBtStgCtrl->GetMaxKeywordID();
	//m_nMaxDocID     = m_pBtStgCtrl->GetMaxDocID();

	if(CAnalysis::initAnalysis(DICT_PATH) != 0)
		return false;

	return true;

}

void CIndexWriter::Close()
{
	if(m_indexedDocs.size() > 0)
	{
		MergeDocs();//write to the database
	}
	m_fieldInfos.Write(m_pBtStgCtrl);

	if(m_pBtStgCtrl)
	{
		m_pBtStgCtrl->Close();
	}
}

CIndexWriter::~CIndexWriter()
{
	if(!m_pBtStgCtrl->IsClosed())
	    Close();
	if(m_pBtStgCtrl)
		delete m_pBtStgCtrl;
	CAnalysis::releaseAnalysis();
}



void CIndexWriter::AddDocument(const CDocument &Doc)
{
	if(m_indexedDocs.size() > GetMergeFactor())
	{
		MergeDocs();
	}
	
	//
	//new segment
	m_fieldInfos.Add(Doc);
	UINT nDocID = m_indexedDocs.size();
	CIndexDocInfo *pDocInfo = new CIndexDocInfo(nDocID);
	InvertDocument(Doc,pDocInfo);
	m_indexedDocs.push_back(pDocInfo);

	
}


void CIndexWriter::InvertDocument(const CDocument &doc,CIndexDocInfo *pDoc)
{

	for(int i = 0; i < doc.GetFields().size();++i)
	{
		const std::string &fieldName = doc.GetFields().at(i).GetName();
		int          fieldNumber = m_fieldInfos.GetFieldNumber(fieldName);
		const CField &field = doc.GetFields().at(i);

		if (field.IsIndexed())
		{
			const std::string& value = field.GetValue();

			if (!field.IsTokenized())
			{
				//
				//simply save
				pDoc->AddUntokenizedInfo(fieldNumber,value);

			}
			else
			{
				m_keywords.clear();
				CAnalysis::analysisFile(value.c_str(),value.length(),TermCallBack);
				
				KEYWORD2POSMAP::iterator it = m_keywords.begin();
				for(; it != m_keywords.end();++it)
				{
					const std::string &Term = it->first;
					CTerm *pTerm = new CTerm(Term.c_str(),fieldNumber);

					PosInfo2Frequency &fq = it->second;
					CTermInfo *pTermInfo = new CTermInfo(pTerm,fq.posInfo,fq.nFreq);
					pDoc->AddTerm(pTermInfo);
					
				}


			}

		}
	}
}

void CIndexWriter::TermCallBack( const char* pKeyWord, UINT nStartPos, 
								 USHORT nWordIndex, 
							     float fScore, int nAttr)
{
  if(nAttr == ATTR_STOPWORD)
	  return;


  std::string strWord(pKeyWord);
  CPosInfo* pPosInfo = new CPosInfo(nWordIndex);

  KEYWORD2POSMAP::iterator it = m_pThis->m_keywords.find(strWord);
  if(it == m_pThis->m_keywords.end())
  {
      PosInfo2Frequency pf;
	  pf.nFreq = 1;
	  pf.posInfo = pPosInfo;
	  m_pThis->m_keywords.insert(std::make_pair(strWord,pf));
#ifdef _DEBUG

	  //std::cout << strWord <<std::endl;
#endif
  }
  else
  {
	  PosInfo2Frequency &pf = it->second;
	  pf.nFreq++;
	  pf.posInfo->m_pNext = pPosInfo;
  }
}

void CIndexWriter::MergeDocs()
{

	CIndexMerger indexMerger(m_pBtStgCtrl);


	//
	//start transaction
	m_pBtStgCtrl->beginLock();

	INDEXDOCCOLLECTION::iterator it = m_indexedDocs.begin();
	for(; it != m_indexedDocs.end();++it)
	{
		CIndexDocInfo *pDoc = *it;
		indexMerger.Add(pDoc);

	}
	//
	//write <term,docid,tf> table to the database
	indexMerger.Close();

	m_pBtStgCtrl->endLock();

	//
	//free all memory
	it = m_indexedDocs.begin();
	for(; it != m_indexedDocs.end();++it)
	{
		CIndexDocInfo *pDoc = *it;
		CIndexDocInfo::TERMSLIST::iterator termIt = pDoc->m_terms.begin();
		for(;termIt != pDoc->m_terms.end();++termIt)
		{
			CTermInfo *ptf = *termIt;
			delete ptf->m_pKeyword;

			CPosInfo *pPosInfo = ptf->m_pPosNode;
			CPosInfo *pPrev ;
			while(pPosInfo)
			{
				pPrev = pPosInfo;
				pPosInfo = pPosInfo->m_pNext;
				delete pPrev;
			}

			delete ptf;

		}
		delete pDoc;
	}
	m_indexedDocs.clear();

}

