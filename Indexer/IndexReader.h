#pragma  once 

#include "Global.h"
#include "IndexExports.h"
#include <list>

class CBtStgCtrl;
class CFieldInfos;

class INDEXREXPORT_API CIndexReader
{
public:
	/*typedef struct TISInfo
	{
		UINT nDocID;
		UINT tf;
		UINT nPosID;
		TISInfo(UINT doc,UINT tf,UINT nPosID):nDocID(doc),tf(tf),nPosID(nPosID)
		{
		}
	}TISInfo;
	typedef std::list<TISInfo> TISInfoList;*/

	CIndexReader();
	~CIndexReader();
	bool Open(const char* indexFullfilename);
	UINT GetTotalDocCount();
	bool GetIndexInfo(const char *strword,const char* field,char *result,UINT &nResultBufferSize);
	bool GetTermPosInfo(UINT nPosID,char *PosInfo,UINT &nsize);
    bool GetUnTokenizedFieldInfo(UINT nDocID,char *value,UINT &nValueSize);

private:
	CBtStgCtrl *m_pStorage;
	CFieldInfos  *m_fieldInfos;
private:
	/*static void	 getSrcCallback(UINT nDocId, char* pDocSrc, int nSize);
	static void  getKeywordCallback(const char* pKeyword, UINT nKeyId);
	static void	 getIndexCallback(UINT nDocId, char* pIndexInfo, int nSize);
	static  CIndexReader *m_pthis;
	TISInfoList   m_indexInfoList;*/
};