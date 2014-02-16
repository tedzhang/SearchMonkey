
/*
 * begin time��2007-1-9 
 * end time:
 * author: huangbin
 * email: binhuang@vip.sina.com
 * �������btree�ṹ�洢�������������⣬���ܹ����������кϲ���
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



//�洢������
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
	BtCursor*	m_pIndexCr;				//�������
	BtCursor*	m_pDocCr;				//�ĵ���Ϣ���
	BtCursor*	m_pKeywordCr;			//�ؼ��ֹ��
	BtCursor*   m_pFieldCr;             //�����
	BtCursor*   m_pPosCr;               //��λ����Ϣ����
	Btree*	    m_pBt;				    //������


	int			m_nIndexTableId;		//������ID
	int			m_nDocTableId;			//�ĵ���Ϣ��ID
	int			m_nKeywordTableId;		//�ؼ��ֱ�ID
	int         m_nFieldsTableId;       //�򡪡�>����ӳ��ؼ���id
	int         m_nPosTableId;          //��λ����Ϣ��id

    UINT        m_nMaxKeyId;			//�ʵ����id
    UINT        m_nMaxDocId;            //�ĵ����id
	UINT        m_nIndexDocNum;           //��ǰ�����˶���ƪ�ĵ�
	UINT        m_nPosMaxIndex;
	char		m_szFileName[MAX_PATH];	//����·��
	

public:
	

	int		setKeyIndex(UINT nKeyId, UINT nDocId, const char* pIndexInfo, int nIndexSize);
	//ɾ���ؼ��ֵ�������Ϣ
	int		rmKeyIndex(UINT nKeyId, UINT nDocId);
	int		rmKeyIndex(UINT nKeyId);
	//��ȡ�ؼ��ֵ�������Ϣ
	int		getKeyIndex(UINT nKeyId, getIndexCallback* Callback);
	int     getKeyIndex(UINT nKeyId,char *result,UINT &nResultBufferSize);


	int		getDocSrcAll( getDocSrcCallback* Callback );
	int		setDocSrc( UINT nKey, void* pData, int nDataSize);
	int     getDocSrc( UINT nDocID,void *pData,UINT &nDataSize);
	int		rmDocSrc ( UINT nKey);

	//�ؼ�����Ϣ
	//<�ʣ���>��Ϊһ���ؼ���

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