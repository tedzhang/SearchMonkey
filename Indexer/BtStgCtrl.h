
/*
 * begin time：2007-1-9 
 * end time:
 * author: huangbin
 * email: binhuang@vip.sina.com
 * 解决利用btree结构存储搜索索引的问题，并能够对索引进行合并等
 * 
*/
#ifndef _BTSTGCTRL_DLL_H
#define _BTSTGCTRL_DLL_H

#include "Global.h"
#include "IndexExports.h"

typedef void	getDocSrcCallback(UINT nDocId, char* pDocSrc, int nSize);
typedef void	getKeywordCallback(const char* pKeyword, int nFieldID,UINT nKeyId);
typedef void	getIndexCallback(UINT nDocId, char* pIndexInfo, int nSize);
typedef void    getFieldCallback(const char* fieldName, char *value);
typedef struct  
{
	unsigned int nKeywordId;
	unsigned int nDocId;
}index_key_t;

struct BtCursor;
struct Btree;



//存储操作类
class INDEXREXPORT_API CBtStgCtrl
{
public:
	CBtStgCtrl():m_nMaxKeyId(0),m_pIndexCr(NULL),m_pDocCr(NULL),
		         m_pKeywordCr(NULL),m_pBt(NULL),m_nIndexTableId(0),
				 m_nDocTableId(0),m_nKeywordTableId(0),
				 m_nMaxDocId(0),m_nIndexDocNum(0),
				 m_pPosCr(NULL),m_pFieldCr(NULL),
				 m_nPosTableId(0),m_nFieldsTableId(0),
				 m_nPosMaxIndex(0)
	{
		
	}		
	~CBtStgCtrl()
	{
		if(m_pBt != NULL )
		{
			Close();
		}
	}

    
	bool Open(const char* fileName,bool isWrite);
	void Close();
	bool IsClosed()        { return m_pBt == NULL;}
	UINT GetMaxKeywordID() { return m_nMaxKeyId;}
	UINT GetMaxDocID()     { return m_nMaxDocId;}
	UINT GetMaxDocnum()    { return m_nIndexDocNum;}
	void SetMaxDocID(UINT nMaxDocID);

	BtCursor *GetIndexCursor()   { return m_pIndexCr;}
	BtCursor *GetTermPosCursor() { return m_pPosCr;}
private:
	BtCursor*	m_pIndexCr;				//索引光标
	BtCursor*	m_pDocCr;				//文档信息光标
	BtCursor*	m_pKeywordCr;			//关键字光标
	BtCursor*   m_pFieldCr;             //域表光标
	BtCursor*   m_pPosCr;               //词位置信息表光标
	Btree*	    m_pBt;				    //索引树


	int			m_nIndexTableId;		//索引表ID
	int			m_nDocTableId;			//文档信息表ID
	int			m_nKeywordTableId;		//关键字表ID
	int         m_nFieldsTableId;       //域——>数字映射关键字id
	int         m_nPosTableId;          //词位置信息表id

    UINT        m_nMaxKeyId;			//词的最大id
    UINT        m_nMaxDocId;            //文档最大id
	UINT        m_nIndexDocNum;           //当前索引了多少篇文档
	UINT        m_nPosMaxIndex;
	char		m_szFileName[MAX_PATH];	//表保存路径
	

public:
	

	int		setKeyIndex(UINT nKeyId, UINT nDocId, const char* pIndexInfo, int nIndexSize);
	//删除关键字的索引信息
	int		rmKeyIndex(UINT nKeyId, UINT nDocId);
	int		rmKeyIndex(UINT nKeyId);
	//获取关键字的索引信息
	int		getKeyIndex(UINT nKeyId, getIndexCallback* Callback);
	int     getKeyIndex(UINT nKeyId,char *result,UINT &nResultBufferSize);


	int		getDocSrcAll( getDocSrcCallback* Callback );
	int		setDocSrc( UINT nKey, void* pData, int nDataSize);
	int     getDocSrc( UINT nDocID,void *pData,UINT &nDataSize);
	int		rmDocSrc ( UINT nKey);

	//关键字信息
	//<词，域>作为一个关键字

	int		getKeywordAll(getKeywordCallback* Callback);
	int		setKeyword(const char* pKey,int nFieldID, UINT& nKeyId);
	int     getKeyword(const char* pKey,int nField,UINT &nKeyId);
	int		rmKeyword(const char* pKey, UINT nKeyId);


    


	int     SetField(const char *pKey,const char* value,int nValueSize);
	int     GetField(const char *pKey,char *value);
	int     GetAllFields(getFieldCallback *pCallback);


	int     setPosInfo(const char* posBuffer,UINT nbufferSize,UINT &posID);
	int     getPosInfo(UINT posID,char *Posvalue,UINT &posBufferSize);

	int		beginLock();
	int		endLock();

private:
	int		chkIndexCr();
	int		chkDocCr();
	int		chkKeywordCr();
	int     chkFieldCr();
	int     chkPosCr();
	bool    UpdateMetaInfo();


};



#endif