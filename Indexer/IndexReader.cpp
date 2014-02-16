#include "IndexReader.h"
#include "BtStgCtrl.h"
#include "FieldInfos.h"
#include "IndexInput.h"
#include <iostream>
//
//CIndexReader* CIndexReader::m_pthis = NULL;

CIndexReader::CIndexReader()
{
	m_pStorage = new CBtStgCtrl;
	m_fieldInfos = new CFieldInfos;
	//m_pthis = this;
}
CIndexReader::~CIndexReader()
{
    m_pStorage->Close();
	delete m_pStorage;
	delete m_fieldInfos;
}
bool CIndexReader::Open(const char* indexFullfilename)
{
     if(!m_pStorage->Open(indexFullfilename,false))
		 return false;
	 m_fieldInfos->Read(m_pStorage);
}
UINT CIndexReader::GetTotalDocCount()
{
	return m_pStorage->GetMaxDocnum();
}
bool CIndexReader::GetIndexInfo(const char *strword,const char* field,char *result,UINT &nResultBufferSize)
{
	int nFieldNum = m_fieldInfos->GetFieldNumber(field);
	assert(nFieldNum >= 0);

	//
	//find it the keyword table

	UINT nKeyID;
	if(0 != m_pStorage->getKeyword(strword,nFieldNum,nKeyID))
		return false;

	//
	//find in the tis table
	//
	if(0 != m_pStorage->getKeyIndex(nKeyID,result,nResultBufferSize));
		return false;

	/*for(TISInfoList::iterator it = m_indexInfoList.begin();
		it != m_indexInfoList.end();++it)
	{
		TISInfo &info = *it;
		char buffer[MAX_INDEXDATA_SIZE] = { 0};
		UINT nSize = MAX_INDEXDATA_SIZE;
		if(0 != m_pStorage->getDocSrc(info.nDocID,buffer,nSize))
			continue;

		CIndexInputBuffer inputBuffer(buffer);
		UINT nFieldNum = inputBuffer.ReadVInt();
		std::string str = inputBuffer.ReadString();

		char posInfo[MAX_INDEXDATA_SIZE];
		nSize = MAX_INDEXDATA_SIZE;
		GetTermPosInfo(info.nPosID,posInfo,nSize);
		CIndexInputBuffer posbuffer(posInfo);
		
		while(posbuffer.Length() < nSize)
		{
			UINT iPos = posbuffer.ReadVInt();
#ifdef _DEBUG
			std::cout << iPos << " ";
#endif
			
		}
#ifdef _DEBUG
			std::cout << std::endl;
#endif
	


	}*/
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


////
////
////
//void CIndexReader::getIndexCallback(UINT nDocId, char* pIndexInfo, int nSize)
//{
//    CIndexInputBuffer buffer(pIndexInfo);
//	UINT DocId = buffer.ReadVInt();
//	UINT tf     = buffer.ReadVInt();
//	UINT iPosID = buffer.ReadVInt();
//	if(m_pthis->m_indexInfoList.size() != 0)
//	{
//         DocId += m_pthis->m_indexInfoList.begin()->nDocID;
//	}
//	m_pthis->m_indexInfoList.push_back(TISInfo(DocId,tf,iPosID));
//}