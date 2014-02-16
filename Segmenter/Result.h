// Result.h: interface for the CResult class.
//
//////////////////////////////////////////////////////////////////////

#pragma  once 

#include "Dictionary.h"
#include "Segment.h"
#include "Span.h"
#include "UnknowWord.h"
#include "WordPreProcessor.h"
#include "analysis.h"



class CResult  
{
public:
	CResult(const char* pPath);
	~CResult();

	//nFlag=1,表示内存从TextPool内存池中分配. =0:表示从MemPool中分配
	bool Processing(const char *sSentence,int nStartPos, unsigned short& nWordIndex, AnalysisCallBack* pCallBack,bool bKeepLogicalWord = false);
	bool ParagraphProcessing(const char *sParagraph, int nSize, AnalysisCallBack* pCallBack);
	
	PWORD_RESULT *m_pResult;

	//The buffer which store the segment and POS result
	//and They stored order by its possibility

	ELEMENT_TYPE m_dResultPossibility[MAX_SEGMENT_NUM];
	int m_nResultCount;
	int m_nOperateType;//0:Only Segment;1: First Tag; 2:Second Type
	int m_nOutputFormat;//0:PKU criterion;1:973 criterion; 2: XML criterion

	char	m_szSentence[MAX_SENTENCE_SIZE];


private:
	CSegment		m_Seg;//Seg class
	CDictionary		m_dictCore;//Core dictionary
	CSpan			m_POSTagger;//POS tagger
	CUnknowWord		m_Person,m_Transliteration,m_Place;//Person recognition
protected:
	bool ChineseNameSplit(char *sPersonName,char *sSurname, char *sSurname2,char *sGivenName,CDictionary &personDict);
	bool Adjust(PWORD_RESULT pItem,PWORD_RESULT pItemRet);
	ELEMENT_TYPE ComputePossibility(PWORD_RESULT pItem);
	void GetMaxPossibilityResult();
private:
	CWordPreProcessor m_wordPreProcessor;
};

//
//
//implementation 

inline CResult::~CResult()
{
	//free buffer
	for(int i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete [] m_pResult[i];
	}
	delete m_pResult;

}

