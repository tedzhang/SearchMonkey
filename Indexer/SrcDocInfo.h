#pragma  once 

#include "Global.h"

class CDocSrcInfo
{
public:
	CDocSrcInfo(UINT nDocID):m_nDocId(nDocID)
	{
		memset( m_szUrl, 0, MAX_URL_SIZE);
	};
	UINT getDocID()      { return m_nDocId;}
	const char* getUrl() { return m_szUrl;}
private:
	UINT	        m_nDocId;					//�ĵ�ID
	char			m_szUrl[MAX_URL_SIZE];		//�ĵ�URL
};