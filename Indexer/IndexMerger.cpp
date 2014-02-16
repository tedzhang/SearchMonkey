#include "IndexMerger.h"
#include "IndexDocInfo.h"
#include "BtStgCtrl.h"
#include "../Storage/sqliteInt.h"
#include "WriteLog.h"
#include "IndexOutput.h"

void CIndexMerger::Add(CIndexDocInfo *pDoc)
{
	COutputBuffer buffer;

	CIndexDocInfo::TERMSLIST::iterator it = pDoc->m_terms.begin();
	for(; it != pDoc->m_terms.end();++it)
	{
		 CTermInfo *pTermInfo = *it;
		 CTerm     *pTerm = pTermInfo->m_pKeyword;
		 
		 UINT nKeyID;
		 Term2IDMap::iterator iter = m_term2IDMap.find(pTerm); 
		 if(iter == m_term2IDMap.end())
		 {
			 if(SQLITE_OK != m_pStg->setKeyword(pTerm->GetKeyword(),pTerm->GetFieldID(),nKeyID))
			 {
				 WriteLogInfo( "CBtStgCtrl::setKeyword() Error\n", __LINE__, DEBUG_NORMAL);
				 return;
			 }
			 m_term2IDMap.insert(std::make_pair(pTerm,nKeyID));
		 }
		 else
		 {
			 nKeyID = iter->second;
			 
		 }

		 //
		 //insert the pos into the position table
		 //
		 //write How many pos data in the table
		 //
		 //buffer.WriteVInt(pTermInfo->m_nFreq);
		 CPosInfo *pPos = pTermInfo->m_pPosNode;
		 USHORT   lastPos = 0;
		 while(pPos)
		 {
			 USHORT nWordPos = pPos->GetWordPos();
			 USHORT nWordGap = nWordPos-lastPos;
			 buffer.WriteVInt(nWordGap);
			 lastPos = nWordPos;
			 pPos = pPos->m_pNext;
		 }

		 UINT nPosID;
		 if(m_pStg->setPosInfo(buffer.GetBuffer(),buffer.Length(),nPosID) == -1)
			 return;

		 buffer.Flush();//now we can reuse the buffer


		 TISMAP::iterator tiIt = m_tismap.find(nKeyID);
		 if(tiIt == m_tismap.end())
		 {
			 std::set<TisItem> tisSet;
			 tisSet.insert(TisItem(pDoc->GetDocID(),pTermInfo->m_nFreq,nPosID,pDoc));
			 m_tismap.insert(std::make_pair(nKeyID,tisSet));
		 }
		 else
		 {
			 std::set<TisItem> &tisSet = tiIt->second;
			 tisSet.insert(TisItem(pDoc->GetDocID(),pTermInfo->m_nFreq,nPosID,pDoc));
		 }

	}
	m_indexDocs.push_back(pDoc);
}

void CIndexMerger::Close()
{

	COutputBuffer buffer;
	UINT startDocID = m_pStg->GetMaxDocID();

	TISMAP::iterator it = m_tismap.begin();
	for(; it != m_tismap.end();++it)
	{
		UINT nKeyID = it->first;
		std::set<TisItem> &postingSet = it->second;
		std::set<TisItem>::iterator setIt = postingSet.begin();
		
		//first clear the buffer
		buffer.Flush();

		UINT  nLastPos  = 0;
		UINT  nLastDocId = 0;

		for(;setIt != postingSet.end();++setIt)
		{
			TisItem &item = *setIt;
			//
			//
			item.docID += startDocID;
			item.pDocInfo->SetDocID(item.docID);
			
			buffer.WriteVInt(item.docID-nLastDocId);
			buffer.WriteVInt(item.tf);
			buffer.WriteVInt(item.posID-nLastPos);

			nLastDocId = item.docID;
			nLastPos   = item.posID;
		}
		if(SQLITE_OK != m_pStg->setKeyIndex(nKeyID,startDocID,buffer.GetBuffer(),buffer.Length()))
			continue;

		
	}
	buffer.Flush();
	

	//
	//保存不需要分析的域的数据
	//
	INDEXDOCS::iterator indexDocit = m_indexDocs.begin();
	for(;indexDocit != m_indexDocs.end();++indexDocit)
	{
		CIndexDocInfo *pdoc = *indexDocit;

		CIndexDocInfo::FieldID2UnTokenizedMap::iterator unIt = pdoc->m_UnTokenizedInfo.begin();
	

		for(;unIt != pdoc->m_UnTokenizedInfo.end();++unIt)
		{
			UINT nFieldID = unIt->first;
			const std::string &strValue = unIt->second;
			buffer.WriteVInt(nFieldID);
			buffer.WriteString(strValue);
		}
		if(SQLITE_OK != m_pStg->setDocSrc(pdoc->GetDocID(),(void*)buffer.GetBuffer(),buffer.Length()))
			return;
		buffer.Flush();
	}

	m_pStg->SetMaxDocID(startDocID+m_indexDocs.size());

}