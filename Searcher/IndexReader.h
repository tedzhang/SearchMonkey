#pragma  once 

#include "Global.h"
#include <map>
#include <vector>
#include "Searcher_Export.h"
#include "Term.h"


class CBtStgCtrl;
class CFieldInfos;
class CQuery;
class CResultList;
class CDocument;

class SEARCHER_API CIndexReader
{
public:
	typedef std::vector<int> TermPositionList;

	static const int MAX_POSTITING_BUFFER_SIZE =1024*10*1024*10; //100M 

	CIndexReader(CQuery *pQuery);
	~CIndexReader();
	static bool Open(const char* indexFullfilename);
	static void Close();

    UINT        GetTotalDocCount();
	UINT        GetTermDocs  (const CTerm& term,CResultList   *&result);
	UINT        GetPhraseDocs(const CTerm& term,CResultList   *&result);
	bool        GetTermPosition(UINT posId,TermPositionList &termPositions);
	CDocument*  GetDocument(UINT nDocId);
	static      CBtStgCtrl* GetStorage() { return m_pStorage;}
	UINT        GetKeyID(const char *strword,const char *field);
private:
	bool  GetIndexInfo(const char *strword,const char* field,char *result,UINT &nResultBufferSize);
	bool  GetTermPosInfo(UINT nPosID,char *PosInfo,UINT &nsize);
    bool  GetUnTokenizedFieldInfo(UINT nDocID,char *value,UINT &nValueSize);
	static void  Init();
private:
	float   GetTermWeight(term_id id);
	term_id GetTermId(const CTerm &term);
	void    Score(const CTerm &term,CResultList *pDocList);
private:
	static CBtStgCtrl   *m_pStorage;
	static CFieldInfos  *m_fieldInfos;
	static char         *m_postingBuffer;
private:
	typedef std::map<CTerm,term_id> TERMMAPS;
	TERMMAPS m_termmaps;

	static CIndexReader *m_pThis;
	int   m_nTermCount;
	//
	//only call internal
	void  SetBoost(float fBoost=1.0f) { m_fBoost = fBoost;}
	float GetBoost() const            { return m_fBoost;}
	float m_fBoost;
private:
	static void GetTermDocCallback(UINT nDocId, char* pIndexInfo, int nSize);
	static void GetPhraseDocCallback(UINT nDocId,char *pIndexInfo,int nSize);
	CResultList *m_pResult;
};