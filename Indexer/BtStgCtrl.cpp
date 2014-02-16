// BtStgCtrl.cpp : Defines the entry point for the DLL application.
//


#include "WriteLog.h"
#include "../Storage/btree.h"
#include "../Storage/sqliteInt.h"
#include "BtStgCtrl.h"




const  int     DOC_MAXID        = 7;
const  int     KEYWORD_MAXID    = 8;
const  int     INDEX_DOC_NUM    = 9;
const  int     POSITION_MAX_INDEX = 10;
const  int     ITABLE_FIELDS_META = 11;
const  int     ITABLE_POS_META    = 12;
const  int     ITABLE_INDEX_META	   = 13;
const  int     ITABLE_DOC_META         = 14;
const  int     ITABLE_KEYWORD_META     = 15;

//
//default value is 10 for sqlite,which is 10*1024bytes(10k)
//set the cache size to 100M
const  int     MAX_CACHE_PAGE_SIZE     = 1024*100;




bool CBtStgCtrl::Open(const char* fileName,bool isWrite)
{
  if(!fileName) return false;
 
  strncpy(m_szFileName,fileName,MAX_PATH);

  char pLogInfo[1024]={0};

  int	rc = sqlite3BtreeOpen( m_szFileName, &m_pBt,0);
  if( rc != SQLITE_OK )
  {
	  sprintf(pLogInfo, "sqlite3BtreeOpen Error, ErrorCode=%d!\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  return false;
  }
  rc = sqlite3BtreeSetCacheSize(m_pBt,MAX_CACHE_PAGE_SIZE);
  if(rc != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqliteBtreeSetCacheSize Error, ErrorCode=%d!\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  return false;
  }

  rc= sqlite3BtreeBeginTrans(m_pBt,isWrite? 2:0);
  if( rc != SQLITE_OK )
  {
	  sprintf( pLogInfo, "sqlite3BtreeBeginTrans Error, ErrorCode=%d\n", rc );
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }

  if((rc = sqlite3BtreeGetMeta(m_pBt,DOC_MAXID,(UINT*)&m_nMaxDocId)) != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta max doc id Error, ErrorCode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }
  if((rc = sqlite3BtreeGetMeta(m_pBt,KEYWORD_MAXID,(UINT*)&m_nMaxKeyId)) != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta max key word id Error, ErrorCode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }

  if((rc = sqlite3BtreeGetMeta(m_pBt,INDEX_DOC_NUM,(UINT*)&m_nIndexDocNum)) != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta doc numbers Error, ErrorCode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }

  if((rc = sqlite3BtreeGetMeta(m_pBt,POSITION_MAX_INDEX,(UINT*)&m_nPosMaxIndex)) != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta doc numbers Error, ErrorCode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }

  
  if((rc = sqlite3BtreeGetMeta(m_pBt,ITABLE_FIELDS_META,(UINT*)&m_nFieldsTableId)) != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta doc numbers Error, ErrorCode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }
  if(m_nFieldsTableId == 0 && isWrite)
  {
	   rc = sqlite3BtreeCreateTable(m_pBt,&m_nFieldsTableId,0);
	   if(rc != SQLITE_OK)
	   {
		   sprintf( pLogInfo, "sqlite3BtreeCreateTable Create IndexTable Error, ErrorCode=%d\n", rc );
		   WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		   goto ERROR_EXIT;
	   }
	   sqlite3BtreeUpdateMeta(m_pBt,ITABLE_FIELDS_META,m_nFieldsTableId);
  }


  if((rc = sqlite3BtreeGetMeta(m_pBt,ITABLE_POS_META,(UINT*)&m_nPosTableId)) != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta doc numbers Error, ErrorCode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }
  if(m_nPosTableId == 0 && isWrite)
  {
	  rc = sqlite3BtreeCreateTable(m_pBt,&m_nPosTableId,0);
	  if(rc != SQLITE_OK)
	  {
		  sprintf( pLogInfo, "sqlite3BtreeCreateTable Create IndexTable Error, ErrorCode=%d\n", rc );
		  WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		  goto ERROR_EXIT;
	  }
	  sqlite3BtreeUpdateMeta(m_pBt,ITABLE_POS_META,m_nPosTableId);
  }
  

  if( (rc=sqlite3BtreeGetMeta(m_pBt, ITABLE_INDEX_META, (unsigned int*)&m_nIndexTableId) ) != SQLITE_OK )
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta IndexTable Error, ErrorCode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }
  if( m_nIndexTableId == 0 && isWrite)
  {
	  rc=	sqlite3BtreeCreateTable(m_pBt, &m_nIndexTableId, 0 );
	  if( rc != SQLITE_OK  )
	  {
		  sprintf( pLogInfo, "sqlite3BtreeCreateTable Create IndexTable Error, ErrorCode=%d\n", rc );
		  WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		  goto ERROR_EXIT ;
	  }
	  sqlite3BtreeUpdateMeta(m_pBt, ITABLE_INDEX_META, m_nIndexTableId );
  }

  if( (rc=sqlite3BtreeGetMeta(m_pBt, ITABLE_DOC_META, (unsigned int*)&m_nDocTableId) ) != SQLITE_OK)
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta DocTable Error, Errorcode=%d\n", rc);
	  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }
  if( m_nDocTableId == 0 && isWrite)
  {
	  rc=sqlite3BtreeCreateTable(m_pBt, &m_nDocTableId, BTREE_INTKEY|BTREE_LEAFDATA);
	  if( rc != SQLITE_OK)
	  {
		  sprintf( pLogInfo, "sqlite3BtreeCreateTable DocTable Error, ErrorCode=%d\n", rc );
		  WriteLogInfo( pLogInfo, __LINE__, DEBUG_NORMAL);
		  goto ERROR_EXIT;
	  }
	  sqlite3BtreeUpdateMeta(m_pBt, ITABLE_DOC_META, m_nDocTableId);
  }

  //打开关键字数据的表
  if( (rc=sqlite3BtreeGetMeta(m_pBt, ITABLE_KEYWORD_META, (unsigned int*)&m_nKeywordTableId) ) != SQLITE_OK )
  {
	  sprintf(pLogInfo, "sqlite3BtreeGetMeta KeywordTable Error, ErrorCode=%d\n", rc );
	  WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
	  goto ERROR_EXIT;
  }
  if( m_nKeywordTableId == 0 && isWrite )
  {
	  rc=sqlite3BtreeCreateTable(m_pBt, &m_nKeywordTableId, 0);
	  if( rc != SQLITE_OK )
	  {
		  sprintf(pLogInfo, "sqlite3BtreeCreateTable keywordTable Error, ErrorCode=%d\n", rc);
		  WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		  goto ERROR_EXIT;
	  }
	  sqlite3BtreeUpdateMeta(m_pBt, ITABLE_KEYWORD_META, m_nKeywordTableId);
  }

  sqlite3BtreeCommit(m_pBt);

  chkIndexCr();
  chkDocCr();
  chkKeywordCr();
  chkFieldCr();
  chkPosCr();
  if(!isWrite)
  {
       if(m_nIndexTableId == 0 || m_nDocTableId == 0 || m_nKeywordTableId == 0)
		   return false;
  }

  return true;

ERROR_EXIT:
  sqlite3BtreeClose(m_pBt);
  m_pBt=NULL;

  return false;
}

bool CBtStgCtrl::UpdateMetaInfo()
{
	int rc= sqlite3BtreeBeginTrans(m_pBt, 2);
	if((rc = sqlite3BtreeUpdateMeta(m_pBt,DOC_MAXID,m_nMaxDocId)) != SQLITE_OK)
	{
		sqlite3BtreeRollback(m_pBt);
		return false;
	}

	if((rc = sqlite3BtreeUpdateMeta(m_pBt,KEYWORD_MAXID,m_nMaxKeyId)) != SQLITE_OK)
	{
		sqlite3BtreeRollback(m_pBt);
		return false;
	}
	if((rc = sqlite3BtreeUpdateMeta(m_pBt,INDEX_DOC_NUM,m_nIndexDocNum)) != SQLITE_OK)
	{
		sqlite3BtreeRollback(m_pBt);
		return false;
	}

	if((rc = sqlite3BtreeUpdateMeta(m_pBt,POSITION_MAX_INDEX,m_nPosMaxIndex)) != SQLITE_OK)
	{
       sqlite3BtreeRollback(m_pBt);
	   return false;
	}
	sqlite3BtreeCommit(m_pBt);
	return true;
}

void CBtStgCtrl::Close()
{
 
	UpdateMetaInfo();

	if( m_pIndexCr != NULL )
	{
		sqlite3BtreeCloseCursor( m_pIndexCr );
		m_pIndexCr=NULL;
	}
	if( m_pDocCr != NULL )
	{
		sqlite3BtreeCloseCursor(m_pDocCr);
		m_pDocCr=NULL;
	}
	if( m_pKeywordCr != NULL )
	{
		sqlite3BtreeCloseCursor( m_pKeywordCr);
		m_pKeywordCr=NULL;
	}
	if(m_pFieldCr)
	{
		sqlite3BtreeCloseCursor(m_pFieldCr);
		m_pFieldCr = NULL;
	}
	if(m_pPosCr)
	{
		sqlite3BtreeCloseCursor(m_pPosCr);
		m_pPosCr = NULL;
	}
	if( m_pBt != NULL )
	{
		sqlite3BtreeClose(m_pBt);
		m_pBt=NULL;
	}

}

//
//
//
int CBtStgCtrl::beginLock()
{
	return sqlite3BtreeBeginTrans( m_pBt, 2 );
}

int	CBtStgCtrl::endLock()
{
	return sqlite3BtreeCommit( m_pBt);
}


//获取关键字的索引信息
int		CBtStgCtrl::getKeyIndex( UINT nKeyId, getIndexCallback* pCallback  )
{
	int		rc=0;
	char	pTemp[MAX_INDEXDATA_SIZE]={0};

	if( chkIndexCr( ) != 0 )
	{
		return -1;
	}
	
	int	nRes=0;
	index_key_t stKey;
	stKey.nDocId=0;
	stKey.nKeywordId=nKeyId;

	rc=	sqlite3BtreeMoveto(m_pIndexCr, (void*)&stKey, sizeof(stKey), &nRes);
	if( rc != SQLITE_OK || nRes != 0)
	{
		return -1;
	}
	LONGLONG nSize=0;
	while( nRes == 0 )
	{
		if( sqlite3BtreeKey(m_pIndexCr, 0, sizeof(stKey), &stKey) != SQLITE_OK )
		{
			break;
		}
		if( stKey.nKeywordId != nKeyId )
		{
			break;
		}

		unsigned int nSize=0;
		sqlite3BtreeDataSize(m_pIndexCr, &nSize);
		if( MAX_INDEXDATA_SIZE < (int)nSize )
		{
			nSize=MAX_INDEXDATA_SIZE;
		}
		sqlite3BtreeData(m_pIndexCr, 0, nSize, pTemp);
		pCallback(stKey.nDocId, pTemp, nSize);

		sqlite3BtreeNext(m_pIndexCr, &nRes);
	}
	return 0;
}

//
//
// nKeyId:词的信息
// result:结果所在buffer,其中结果为 <nDocID,tf,PosId>的集合
// nResultBufferSize:输入输出参数，返回结果集的大小
int    CBtStgCtrl::getKeyIndex(UINT nKeyId,char *result,UINT &nResultBufferSize)
{

	int		rc=0;
	char	pTemp[MAX_INDEXDATA_SIZE]={0};

	if( chkIndexCr( ) != 0 )
	{
		return -1;
	}

	int	nRes=0;
	index_key_t stKey;
	stKey.nDocId=0;
	stKey.nKeywordId=nKeyId;

	rc=	sqlite3BtreeMoveto(m_pIndexCr, (void*)&stKey, sizeof(stKey), &nRes);
	if( rc != SQLITE_OK || nRes != 0)
	{
		return -1;
	}
	LONGLONG nSize=0;
	UINT     nBufferPos = 0;
	while( nRes == 0 )
	{
		if( sqlite3BtreeKey(m_pIndexCr, 0, sizeof(stKey), &stKey) != SQLITE_OK )
		{
			break;
		}
		if( stKey.nKeywordId != nKeyId )
		{
			break;
		}

		unsigned int nSize=0;
		sqlite3BtreeDataSize(m_pIndexCr, &nSize);
		if( MAX_INDEXDATA_SIZE < (int)nSize )
		{
			nSize=MAX_INDEXDATA_SIZE;
		}
		sqlite3BtreeData(m_pIndexCr, 0, nSize, pTemp);
    
		//
		//Exceed the buffer's size ,omit other result
		//
		if(nBufferPos + nSize > nResultBufferSize)
			break;

		memcpy(result+nBufferPos,pTemp,nSize);
		nBufferPos += nSize;

		sqlite3BtreeNext(m_pIndexCr, &nRes);
	}
	nResultBufferSize = nBufferPos;

	return 0;
}

//
//保存关键字的索引信息
//
int		CBtStgCtrl::setKeyIndex( UINT nKeyId, UINT nDocId, const char* pIndex, int nIndexSize)
{
	int rc=0;
	if( chkIndexCr( ) != 0 )
	{
		return -1;
	}
	index_key_t stKey={0};
	stKey.nDocId=		nDocId;
	stKey.nKeywordId=	nKeyId;
	rc=	sqlite3BtreeInsert(m_pIndexCr, &stKey, sizeof(stKey), pIndex, nIndexSize);
	if( rc != SQLITE_OK )
	{
		return -1;
	}
	return 0;
}


//删除关键字某个文档的索引信息
int		CBtStgCtrl::rmKeyIndex(UINT nKeyId, UINT nDocId)
{	
	if( chkIndexCr( ) != 0 )
	{
		return -1;
	}
	index_key_t stKey={0};
	stKey.nDocId=		nDocId;
	stKey.nKeywordId=	nKeyId;

	int	nRes=0;
	int	rc=0;
	rc=	sqlite3BtreeMoveto( m_pIndexCr, &stKey, sizeof(stKey), &nRes);
	if( rc != SQLITE_OK || nRes != 0 )
	{
		return -1;
	}
	return sqlite3BtreeDelete( m_pIndexCr );
}

int		CBtStgCtrl::rmKeyIndex(unsigned int nKeyId)
{
	index_key_t stKey={0};
	if( chkIndexCr() )
	{
		return -1;
	}
	int rc=0;
	int nRes=0;
	while( true )
	{
		stKey.nDocId=	0;
		stKey.nKeywordId= nKeyId;
		rc= sqlite3BtreeMoveto(m_pIndexCr, (void*)&stKey, sizeof(stKey), &nRes );
		if( rc != SQLITE_OK || nRes != 0 )
		{
			break;
		}
		rc= sqlite3BtreeNext(m_pIndexCr, &nRes );
		if( rc != SQLITE_OK || nRes != 0 )
		{
			break;
		}
		if( sqlite3BtreeKey(m_pIndexCr, 0, sizeof(stKey), &stKey ) != SQLITE_OK )
		{
			break;
		}
		if( stKey.nKeywordId == nKeyId )
		{
			sqlite3BtreeDelete(m_pIndexCr);
		}
		else
		{
			rc= sqlite3BtreePrevious(m_pIndexCr, &nRes );
			if( rc == SQLITE_OK && nRes == 0 )
			{
				sqlite3BtreeDelete(m_pIndexCr);
			}
			break;
		}
	}
	return 0;

}

//获取文档源的信息
int		CBtStgCtrl::getDocSrcAll(getDocSrcCallback* Callback)
{
	unsigned int	nId=0;
	LONGLONG		nKeySize=0;
	char			szTemp[MAX_INDEXDATA_SIZE]={0};
	unsigned int	nSize=0;
	if( chkDocCr( ) != 0 )
	{
		return -1;
	}
	int	nRes=0;
	int rc=0;
	rc=	sqlite3BtreeFirst(m_pDocCr, &nRes);
	if( rc != SQLITE_OK || nRes != 0)
	{
		return 0;
	}
	while( nRes == 0 )
	{
		//获取关键字信息
		if( sqlite3BtreeKeySize(m_pDocCr, &nKeySize) != SQLITE_OK )
		{
			break;
		}
		nId=	(UINT)nKeySize;

		//获取数据信息
		sqlite3BtreeDataSize(m_pDocCr, &nSize);
		if( MAX_INDEXDATA_SIZE < (int)nSize )
		{
			nSize = MAX_INDEXDATA_SIZE;
		}
		if( sqlite3BtreeData(m_pDocCr, 0, nSize, szTemp) != SQLITE_OK )
		{
			break;
		}

		//回掉查询结果
		Callback(nId, szTemp, nSize);

		//获取下一个数据
		if( sqlite3BtreeNext(m_pDocCr, &nRes) != SQLITE_OK) 
		{
			break;
		}
	}
	return 0;
}


int    CBtStgCtrl::getDocSrc( UINT nDocID,void *pData,UINT &nDataSize)
{
	
	UINT	        nSize=0;
	if( chkDocCr( ) != 0 )
	{
		return -1;
	}
	
	int rc,nRes;
	rc = sqlite3BtreeMoveto(m_pDocCr,NULL,nDocID,&nRes);
	if( rc != SQLITE_OK || nRes != 0)
	{
		return 0;
	}
	if( nRes == 0 )
	{
		//获取数据信息
		sqlite3BtreeDataSize(m_pDocCr, &nSize);
		if( nDataSize < (int)nSize )
		{
			nSize = nDataSize;
		}
		if( sqlite3BtreeData(m_pDocCr, 0, nSize, pData) != SQLITE_OK )
		{
			return -1;
		}
		nDataSize = nSize;

	}
	return 0;
}

//修改文档源的信息
int		CBtStgCtrl::setDocSrc(UINT nKey, void* pData, int nDataSize)
{
	int rc=0;
	if( chkDocCr() != 0 )
	{
		return -1;
	}
	rc=	sqlite3BtreeInsert(m_pDocCr, NULL, nKey, pData, nDataSize);
	if( rc != SQLITE_OK )
	{
		return -1;
	}
	m_nIndexDocNum++;
	return 0;
}


//删除文档源的信息
int		CBtStgCtrl::rmDocSrc(UINT nKey)
{
	int	rc=0;
	if( chkDocCr( ) != 0 )
	{
		return -1;
	}
	int	nRes=0;
	rc=	sqlite3BtreeMoveto( m_pDocCr, NULL, nKey, &nRes);
	if( rc != SQLITE_OK || nRes != 0 )
	{
		return -1;
	}
	rc= sqlite3BtreeDelete( m_pDocCr );
	return rc;
}




struct TermFieldId2KeyID
{
	int fieldID;
	UINT nKeyID;
};


int		CBtStgCtrl::getKeywordAll(getKeywordCallback Callback)
{

	LONGLONG		nKeySize=0;
	UINT	        nSize=0;
	char	szTemp[MAX_KEYWORD_SIZE]={0};

	if( chkKeywordCr( ) != 0 )
	{
		return -1;
	}
	int rc=0;
	int	nRes=0;
	rc=	sqlite3BtreeFirst(m_pKeywordCr, &nRes);
	if( rc != SQLITE_OK || nRes != 0)
	{
		return 0;
	}
	while( nRes == 0 )
	{
		//获取关键字信息
		LONGLONG	nSize=0;
		memset( szTemp, 0, sizeof(szTemp) );
		sqlite3BtreeKeySize(m_pKeywordCr, &nSize);
		if( MAX_KEYWORD_SIZE < (int)nSize )
		{
			nSize= MAX_KEYWORD_SIZE;
		}
		if( sqlite3BtreeKey(m_pKeywordCr, 0, (int)nSize, szTemp) != SQLITE_OK )
		{
			break;
		}
        TermFieldId2KeyID tfk;

		if( sqlite3BtreeData(m_pKeywordCr, 0, sizeof(TermFieldId2KeyID), (void*)&tfk ) != SQLITE_OK )
		{
			break;
		}

		Callback(szTemp,tfk.fieldID,tfk.nKeyID);
		
		//查找下一个
		if( sqlite3BtreeNext(m_pKeywordCr, &nRes ) != SQLITE_OK )
		{
			break;
		}
	}
	return 0;
}


int    CBtStgCtrl:: getKeyword(const char* pKey,int nFieldID,UINT &nKeyId)
{

	if( chkKeywordCr() != 0 )
	{
		return -1;
	}

	int res;
	sqlite3BtreeMoveto(m_pKeywordCr,pKey,strlen(pKey),&res);
	TermFieldId2KeyID tfk;
	while(res == 0)
	{
		//获取ID信息
		if( sqlite3BtreeData(m_pKeywordCr, 0, sizeof(TermFieldId2KeyID), (void*)&tfk ) != SQLITE_OK )
		{
			break;
		}
		if(tfk.fieldID == nFieldID)
		{
			nKeyId = tfk.nKeyID;
			return 0;
		}

		if( sqlite3BtreeNext(m_pKeywordCr, &res ) != SQLITE_OK )
		{
			break;
		}
	}
	return -1;
}


int		CBtStgCtrl::setKeyword( const char* pKey,int nFieldID, UINT& nKeyId)
{
	if( chkKeywordCr() != 0 )
	{
		return -1;
	}
	
	int res;
	sqlite3BtreeMoveto(m_pKeywordCr,pKey,strlen(pKey),&res);
	TermFieldId2KeyID tfk;
	while(res == 0)
	{
		//获取ID信息
		if( sqlite3BtreeData(m_pKeywordCr, 0, sizeof(TermFieldId2KeyID), (void*)&tfk ) != SQLITE_OK )
		{
			break;
		}
		if(tfk.fieldID == nFieldID)
		{
			nKeyId = tfk.nKeyID;
			return 0;
		}
		
		if( sqlite3BtreeNext(m_pKeywordCr, &res ) != SQLITE_OK )
		{
			break;
		}
	}
	m_nMaxKeyId++;
	nKeyId=	m_nMaxKeyId;

	tfk.fieldID = nFieldID;
	tfk.nKeyID  = nKeyId;

	if( sqlite3BtreeInsert(m_pKeywordCr, pKey, strlen(pKey), &tfk, sizeof(TermFieldId2KeyID) ) != SQLITE_OK )
	{
		return -1;
	}

	//index_key_t stKey={0};
	//stKey.nKeywordId= nKeyId;
	//stKey.nDocId=	0;
	//if( sqlite3BtreeInsert(m_pIndexCr, &stKey, sizeof(stKey), NULL, 0 ) != SQLITE_OK )
	//{
	//	return -1;
	//}
	return 0;
}


//删除已经删除的文档源的信息
int		CBtStgCtrl::rmKeyword(const char* pKey, unsigned int nKeyId)
{
	//删除关键字相关的文档
	int	nRes=0;
	int rc=0;

	rmKeyIndex(nKeyId);

	//删除关键字信息
	if( chkKeywordCr( ) != 0 )
	{
		return -1;
	}
	rc=	sqlite3BtreeMoveto(m_pKeywordCr, pKey, strlen(pKey), &nRes);
	if( rc != SQLITE_OK || nRes != 0)
	{
		return -1;
	}
	return sqlite3BtreeDelete(m_pKeywordCr);
}


int   CBtStgCtrl:: SetField(const char *pKey,const char* value,int nValueSize)
{
	if(chkFieldCr() != 0)
		return -1 ;
	return sqlite3BtreeInsert(m_pFieldCr,pKey,strlen(pKey),value,nValueSize);
}
int    CBtStgCtrl::GetField(const char *pKey,char *value)
{
	if(chkFieldCr() != 0)
		return -1;
	int nRes;
	sqlite3BtreeMoveto(m_pFieldCr,pKey,strlen(pKey),&nRes);
	if(nRes != 0)
		return -1;
	u32 nSize;
	sqlite3BtreeDataSize(m_pFieldCr,&nSize);
	if(SQLITE_OK != sqlite3BtreeData(m_pFieldCr,0,nSize,(void*)value))
		return -1;
	return 0;
}

int CBtStgCtrl::GetAllFields(getFieldCallback *pCallback)
{
	if(chkFieldCr() != 0)
		return -1;

	int rc=0;
	int	nRes=0;
	
	

	rc=	sqlite3BtreeFirst(m_pFieldCr, &nRes);
	if( rc != SQLITE_OK || nRes != 0)
	{
		return 0;
	}

	char szTemp[MAX_KEYWORD_SIZE] = { 0};
	char buffer[MAX_KEYWORD_SIZE] = { 0};//one float and one vint
		
	while( nRes == 0 )
	{
		//获取关键字信息
		i64 nSize;
		sqlite3BtreeKeySize(m_pFieldCr, &nSize);

		memset(szTemp,0,MAX_KEYWORD_SIZE);
		memset(buffer,0,MAX_KEYWORD_SIZE);
		
		if( sqlite3BtreeKey(m_pFieldCr, 0, (int)nSize, szTemp) != SQLITE_OK )
		{
			break;
		}

		u32 nDataSize;
		sqlite3BtreeDataSize(m_pFieldCr,&nDataSize);
		if( sqlite3BtreeData(m_pFieldCr, 0,nDataSize, (void*)buffer ) != SQLITE_OK )
		{
			break;
		}

        pCallback(szTemp,buffer);

		//查找下一个
		if( sqlite3BtreeNext(m_pFieldCr, &nRes ) != SQLITE_OK )
		{
			break;
		}
	}
	return 0;
}

int     CBtStgCtrl::setPosInfo(const char* posBuffer,UINT nbufferSize,UINT &posID)
{
	if(chkPosCr() != 0)
		return -1;

	posID =	m_nPosMaxIndex;
	m_nPosMaxIndex++;
	if( sqlite3BtreeInsert(m_pPosCr, &posID, sizeof(UINT), posBuffer, nbufferSize ) != SQLITE_OK )
	{
		return -1;
	}
	return 0;

}
int    CBtStgCtrl::getPosInfo(UINT posID,char *Posvalue,UINT &posBufferSize)
{
	if(chkPosCr() != 0)
	{
		return -1;
	}
	int nRes;
	sqlite3BtreeMoveto(m_pPosCr,&posID,sizeof(UINT),&nRes);
	if(nRes == 0)
	{
		u32 nsize= 0;
		sqlite3BtreeDataSize(m_pPosCr,&nsize);
		posBufferSize = nsize;
		sqlite3BtreeData(m_pPosCr,0,nsize,Posvalue);
	}
	else 
		return -1;
	return 0;
}





/*
 *	索引比较函数
 */
inline int	indexCmpFuct(void* pNotUsed, int nSize1, const void* pKey1, int nSize2, const void* pKey2)
{
	const index_key_t* Key1= (const index_key_t*) pKey1;
	const index_key_t* Key2= (const index_key_t*) pKey2;
	if( Key1->nKeywordId  != Key2->nKeywordId )
	{
		return Key1->nKeywordId- Key2->nKeywordId; 
	}
	if( Key1->nDocId == Key2->nDocId )
	{
		return 0;
	}
	return Key1->nDocId > Key2->nDocId ? 1 : -1;
}


inline int		docCmpFuct(void* pNotUsed, int nSize1, const void* pKey1, int nSize2, const void* pKey2)
{
	return nSize1- nSize2;
}


inline int		KeywordFuct(void* pNotUsed, int nSize1, const void* pKey1, int nSize2, const void* pKey2)
{
	int c = memcmp(pKey1, pKey2, nSize1<nSize2 ? nSize1 : nSize2);
	if( c==0 )
	{
		c = nSize1 - nSize2;
	}
	return c;
}



int		CBtStgCtrl::chkIndexCr()
{
	if( m_pIndexCr != NULL )
	{
		return 0;
	}
	int rc=sqlite3BtreeCursor( m_pBt, m_nIndexTableId, 1, indexCmpFuct, NULL, &m_pIndexCr );
	if( rc != SQLITE_OK)
	{
		char pLogInfo[1024]={0};
		sprintf( pLogInfo, "sqlite3BtreeCursor Error, errorcode=%d\n", rc);
		WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		return -1;
	}
	return 0;
}

int		CBtStgCtrl::chkDocCr()
{
	if( m_pDocCr != NULL )
	{
		return 0;
	}
	int rc=sqlite3BtreeCursor(m_pBt, m_nDocTableId, 1, docCmpFuct, NULL, &m_pDocCr);
	if( rc != SQLITE_OK )
	{
		char pLogInfo[1024]={0};
		sprintf( pLogInfo, "chkDocCr sqlite3BtreeCursor Error, errorcode=%d\n", rc);
		WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		return -1;
	}
	return 0;
}


int		CBtStgCtrl::chkKeywordCr()
{
	if( m_pKeywordCr != NULL )
	{
		return 0;
	}
	int rc=sqlite3BtreeCursor(m_pBt, m_nKeywordTableId, 1, KeywordFuct, NULL, &m_pKeywordCr);
	if( rc != SQLITE_OK )
	{
		char pLogInfo[1024]={0};
		sprintf( pLogInfo, "chkKeywordCr sqlite3BtreeCursor Error, errorcode=%d\n", rc);
		WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		return -1;
	}
	return 0;
}

int    CBtStgCtrl::chkFieldCr()
{
	if(m_pFieldCr)
		return 0;

	int rc=sqlite3BtreeCursor(m_pBt, m_nFieldsTableId, 1, KeywordFuct, NULL, &m_pFieldCr);
	if( rc != SQLITE_OK )
	{
		char pLogInfo[1024]={0};
		sprintf( pLogInfo, "chkKeywordCr sqlite3BtreeCursor Error, errorcode=%d\n", rc);
		WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
		return -1;
	}
	return 0;


}
int     CBtStgCtrl::chkPosCr()
{
   if(m_pPosCr)
	   return 0;

   int rc=sqlite3BtreeCursor(m_pBt, m_nPosTableId, 1, KeywordFuct, NULL, &m_pPosCr);
   if( rc != SQLITE_OK )
   {
	   char pLogInfo[1024]={0};
	   sprintf( pLogInfo, "chkKeywordCr sqlite3BtreeCursor Error, errorcode=%d\n", rc);
	   WriteLogInfo(pLogInfo, __LINE__, DEBUG_NORMAL);
	   return -1;
   }
   return 0;


}


void CBtStgCtrl::SetMaxDocID(UINT nMaxDocID)
{
	 m_nMaxDocId = nMaxDocID;
	 //UpdateMetaInfo();
}