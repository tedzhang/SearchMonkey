// analysis.cpp : Defines the entry point for the DLL application.
//

#include "analysis.h"
#include "result.h"

CResult*	CAnalysis::m_pAnalysis=NULL;
bool		CAnalysis::m_bInitFlag= false;


int		CAnalysis::initAnalysis(const char* szDictRoot)
{
	if( m_bInitFlag )
	{
		return 0;
	}
	if( m_pAnalysis != NULL )
	{
		delete []m_pAnalysis;
		m_pAnalysis=NULL;
	}

	m_pAnalysis= new CResult(szDictRoot);
	if( m_pAnalysis == NULL )
	{
		return -1;
	}

	m_bInitFlag= true;

	return 0;
}


void	CAnalysis::releaseAnalysis()
{
	if(m_pAnalysis != NULL )
	{
		delete m_pAnalysis;
	}
	m_pAnalysis= NULL;
	m_bInitFlag= false;

	return;
}

int		CAnalysis::analysisText(const char* pText, int nTextSize, AnalysisCallBack* pCallBackFuct,bool bKeepLogicalword)
{
	if( m_pAnalysis == NULL || pText == NULL )
	{
		return -1;
	}
	unsigned short nWordIndex=0;
	return m_pAnalysis->Processing(pText, 0, nWordIndex, pCallBackFuct, bKeepLogicalword);

}

int		CAnalysis::analysisFile(const char* pText, int nTextSize, AnalysisCallBack* pCallBackFuct)
{
	if( m_pAnalysis == NULL || pText == NULL )
	{
		return -1;
	}
	return m_pAnalysis->ParagraphProcessing(pText, nTextSize, pCallBackFuct);
}


//////////////////////////////////////////////////////////////////////////
//
// Dictionary exported function
//////////////////////////////////////////////////////////////////////////

#include "Dictionary.h"

CCnDictionary::CCnDictionary()
{
     m_pDictionary = new CDictionary;
}
CCnDictionary::~CCnDictionary()
{
      if(m_pDictionary)
		  delete m_pDictionary;
}

bool CCnDictionary::OpenDictFile( char *sFileName)
{
   return m_pDictionary->Load(sFileName);
}


bool  CCnDictionary::AddWord(char *strWord,const char* sPos,int nFrequence)
{
   int nAttribute = CDictionary::GetPOSValue(sPos);
   return m_pDictionary->AddItem(strWord,nAttribute,nFrequence);
}

bool CCnDictionary::DelWord(char *strWord,const char* sPOS)
{
	return m_pDictionary->DelItem(strWord,CDictionary::GetPOSValue(sPOS));
}

bool CCnDictionary::SaveDictFile( char *sFileName)
{
  return  m_pDictionary->Save(sFileName);
}


