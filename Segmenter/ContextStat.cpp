
#include "ContextStat.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CContextStat::~CContextStat()
{
	delete [] m_pSymbolTable;
	PMYCONTEXT pCur=m_pContext,pTemp;
	while(pCur!=NULL)
	{
		pTemp=pCur->next;
		for(int i=0;i<m_nTableLen;i++)
			delete [] pCur->aContextArray[i];
		delete [] pCur->aContextArray;
		delete [] pCur->aTagFreq;
		delete pCur;
		pCur=pTemp;
	}

}


/*
 *	Context File�ṹ
 *		TableLen:			sizeof(int)
 *		SymbolTable:		sizeof(int) * tableLen 
 *		Context(�ظ�):
 *			nKey:			sizeof(int) 
 *			TotalFreq:		sizeof(int)
 *			TagTreq:		sizeof(int) * tableLen
 *			ContextArray:	sizeof(int) * tableLen * tableLen
 */
bool CContextStat::Load(char *sFilename)
{
	FILE *fp;
	if((fp=fopen(sFilename,"rb"))==NULL)
	{
		return false;
	}
	PMYCONTEXT pCur=m_pContext,pTemp,pPre=NULL;
	while(pCur!=NULL)
	{
		pTemp=pCur->next;
		for(int i=0;i<m_nTableLen;i++)
			delete [] pCur->aContextArray[i];
		delete [] pCur->aContextArray;
		delete [] pCur->aTagFreq;
		delete pCur;
		pCur=pTemp;
	}
	
	if(m_pSymbolTable!=NULL)
	{
		delete [] m_pSymbolTable;
	}
	
	//ǰ4�ֽ�,table����
	fread(&m_nTableLen, sizeof(m_nTableLen), 1,fp);
	m_pSymbolTable=		new int[m_nTableLen];
	
	//��ȡsymbolTable
	fread( m_pSymbolTable, sizeof(int), m_nTableLen,fp);//write the symbol table

	for(int i  = 0; i < m_nTableLen;++i)
	{
		m_symbolKey2indexCache.insert(std::make_pair(m_pSymbolTable[i],i));
	}
	
	
	//��ȡcontext
	while( !feof(fp) )
	{
		pCur=new MYCONTEXT;
		pCur->next=NULL;
		
		//ȡKey
		if(fread(&pCur->nKey,sizeof(int),1,fp) < 1)//Read error
		{
			delete pCur;
			break;
		}
		//ȡTotalFreq
		fread(&pCur->nTotalFreq,sizeof(int),1,fp);
		//ȡTagFreq
		pCur->aTagFreq=new int [m_nTableLen];
		fread( pCur->aTagFreq,sizeof(int),m_nTableLen,fp );//the every POS frequency
		
		//ȡContextArray
		pCur->aContextArray=new int *[m_nTableLen];
		for(int i=0;i<m_nTableLen;i++)
		{
			pCur->aContextArray[i]=new int[m_nTableLen];
			fread( pCur->aContextArray[i],sizeof(int),m_nTableLen,fp );
		}

		//add to the cache
		//added by yongzhang 2007/4/24
		m_cache.insert(std::make_pair(pCur->nKey,pCur));

		//���뵽������
		if(pPre==NULL)
			m_pContext=pCur;
		else
			pPre->next=pCur;
		pPre=pCur;
	}

	fclose(fp);
	
	return true;
}





