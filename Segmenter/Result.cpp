// Result.cpp: implementation of the CResult class.
//
//////////////////////////////////////////////////////////////////////

#include "Result.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "Utility.h"
#include "analysis.h"
#include <vector>
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CResult::CResult(const char* pPath)
{
	//malloc buffer
	m_pResult=new PWORD_RESULT[MAX_SEGMENT_NUM];
	for(int i=0;i<MAX_SEGMENT_NUM;i++)
	{
		m_pResult[i]=new WORD_RESULT[MAX_WORDS];
		memset(m_pResult[i], 0, MAX_WORDS * sizeof( WORD_RESULT) );
	}
	char	szDctPath[MAX_PATH]={0};

	strcpy( szDctPath, pPath);
	strcat( szDctPath, "coreDict.dct");
	m_dictCore.Load(szDctPath);

	strcpy( szDctPath, pPath);
	strcat( szDctPath, "lexical.ctx");
	m_POSTagger.LoadContext(szDctPath);

	m_POSTagger.SetTagType();

	//Set the person recognition configure
	strcpy( szDctPath, pPath);
	strcat( szDctPath, "nr");
	m_Person.Configure(szDctPath,TT_PERSON);


	//Set the person recognition configure
	strcpy( szDctPath, pPath);
	strcat( szDctPath, "tr");
	m_Transliteration.Configure(szDctPath,TT_TRANS);

	//Set the transliteration recognition configure
	strcpy( szDctPath, pPath);
	strcat( szDctPath, "ns");
	m_Place.Configure(szDctPath,TT_PLACE);
	//Set the place recognition configure

	m_nOperateType=2;//0:Only Segment;1: First Tag; 2:Second Type
	m_nOutputFormat=0;//0:PKU criterion;1:973 criterion; 2: XML criterion

	strcpy(szDctPath,pPath);
	strcat(szDctPath,"stopwords.txt");
	m_wordPreProcessor.LoadStopwordsList(szDctPath);
}


bool CResult::Processing(const char *m_szSentence, int nStartPos,unsigned short& nWordIndex, AnalysisCallBack* pCallBack,bool bKeepLogicalWord)
{
	int nIndex=0;

	m_Seg.Segment((char*)m_szSentence,m_dictCore);

	m_nResultCount=m_Seg.m_nSegmentCount;

	//Record the number of result
	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
	{
		m_POSTagger.POSTagging(m_Seg.m_pWordSeg[nIndex],m_dictCore,m_dictCore);
		m_Person.Recognition(m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
		m_Transliteration.Recognition(m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
		m_Place.Recognition(m_Seg.m_pWordSeg[nIndex],m_Seg.m_graphOptimum,m_Seg.m_graphSeg,m_dictCore);
	}		

	m_Seg.OptimumSegmet();

	for(nIndex=0;nIndex<m_Seg.m_nSegmentCount;nIndex++)
	{
		m_POSTagger.POSTagging(m_Seg.m_pWordSeg[nIndex],m_dictCore,m_dictCore);
	}

	GetMaxPossibilityResult();

	try
	{


		int i=0;
		int nWordLen = 0;
		while( m_pResult[0][i].sWord[0] !=0 )//Not sentence ending flag
		{
			nWordLen = strlen(m_pResult[0][i].sWord);
			if(!m_wordPreProcessor.Process(m_pResult[0][i].sWord,nWordLen,bKeepLogicalWord))
			{
				pCallBack(m_pResult[0][i].sWord, nStartPos, nWordIndex, 1.0f, ATTR_STOPWORD);
			}
			else
			{
				pCallBack(m_pResult[0][i].sWord,nStartPos, nWordIndex, 1.0f,0);
			}

			nStartPos += nWordLen;
			nWordIndex++;
			i++;
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
	return true;

}


//Sort the segmentation and POS result according its possibility

inline void  CResult::GetMaxPossibilityResult()
{
	//find the largest possibility result
	ELEMENT_TYPE dMaxPossibility = 0.0;
	int          nMaxIndex = 0;

	ELEMENT_TYPE dPossibility ;

	for(int i=0;i<m_Seg.m_nSegmentCount;i++)
	{
		dPossibility = ComputePossibility(m_Seg.m_pWordSeg[i]);
		if(dPossibility > dMaxPossibility)
		{
			nMaxIndex = i;
			dMaxPossibility = dPossibility; 
		}
	}

	if(m_Seg.m_nSegmentCount > 0)
	{
		//
		//adjust the result according to some rules
		Adjust(m_Seg.m_pWordSeg[nMaxIndex],m_pResult[0]);
	}

}


//Compute the possibility of current segmentation and POS result
ELEMENT_TYPE CResult::ComputePossibility(PWORD_RESULT pItem)
{
	int i=0;
	ELEMENT_TYPE dResultPossibility=0;
	while(pItem[i].sWord[0]!=0)
	{
		dResultPossibility+=pItem[i].dValue;
		//Compute the possibility of logP(Wi|Ti)
		if(pItem[i+1].sWord[0]!=0)//Not the last one
		{
			//Compute the possibility of logP(Ti|Ti-1)
			dResultPossibility +=log((double)(m_POSTagger.m_context.GetContextPossibility(0,pItem[i].nHandle,pItem[i+1].nHandle)+1));
			dResultPossibility -=log((double)(m_POSTagger.m_context.GetFrequency(0,pItem[i].nHandle)+1));
		}
		i++;
	}
	return dResultPossibility;
}

//Adjust the result with some rules
bool CResult::Adjust(PWORD_RESULT pItem,PWORD_RESULT pItemRet)
{
	int i=0,j=0;
	unsigned int nLen;
	bool bProcessed=false;//Have been processed
	while(pItem[i].sWord[0]!=0)
	{
		nLen=strlen(pItem[i].sWord);
		bProcessed=false;

		////Rule1: adjust person name
		//if(pItem[i].nHandle==28274&&ChineseNameSplit(pItem[i].sWord,sSurName,sSurName2,sGivenName,m_Person.m_dict)&&strcmp(pItem[i].sWord,"叶利钦")!=0)//'nr'
		//{
		//	//Divide name into surname and given name

		//   if(sSurName[0])
		//	{
		//		strcpy(pItemRet[j].sWord,sSurName);
		//		pItemRet[j++].nHandle=28274;
		//	}
		//	if(sSurName2[0])
		//	{
		//		strcpy(pItemRet[j].sWord,sSurName2);
		//		pItemRet[j++].nHandle=28274;
		//	}
		//	if(sGivenName[0])
		//	{
		//		strcpy(pItemRet[j].sWord,sGivenName);
		//		pItemRet[j++].nHandle=28274;
		//	}
		//	bProcessed=true;
		//}
		//Rule2 for overlap words ABB 一段段、一片片
		if(pItem[i].nHandle==256*'m'&&pItem[i+1].nHandle/256=='q'&&strcmp(pItem[i+1].sWord,pItem[i+2].sWord)==0)
		{
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			strcat(pItemRet[j].sWord,pItem[i+1].sWord);
			strcat(pItemRet[j].sWord,pItem[i+2].sWord);
			pItemRet[j].nHandle=256*'m';
			j+=1;
			i+=2;
			bProcessed=true;
		}
		//Rule3 for overlap words AA or AABB
		else if(nLen==2&&strcmp(pItem[i].sWord,pItem[i+1].sWord)==0)
		{
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			strcat(pItemRet[j].sWord,pItem[i+1].sWord);
			//24832=='a'*256
			pItemRet[j].nHandle=24832;//a
			if(pItem[i].nHandle/256=='v'||pItem[i+1].nHandle/256=='v')//30208='v'8256
			{
				pItemRet[j].nHandle=30208;
			}
			if(pItem[i].nHandle/256=='n'||pItem[i+1].nHandle/256=='n')//30208='v'8256
			{
				pItemRet[j].nHandle='n'*256;
			}			
			i+=1;
			if(strlen(pItem[i+1].sWord)==2)
			{//AAB:洗/洗/脸、蒙蒙亮
				if((pItemRet[j].nHandle==30208&&pItem[i+1].nHandle/256=='n')||
					(pItemRet[j].nHandle==24832&&pItem[i+1].nHandle/256=='a')
					)
				{
					strcat(pItemRet[j].sWord,pItem[i+1].sWord);
					i+=1;
				}
			}
			j+=1;
			bProcessed=true;
		}

		//Rule 4: AAB 洗/洗澡
		else if(nLen==2&&strncmp(pItem[i].sWord,pItem[i+1].sWord,2)==0&&strlen(pItem[i+1].sWord)==4&&(pItem[i].nHandle/256=='v'||pItem[i].nHandle/256=='a'))
		{
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			strcat(pItemRet[j].sWord,pItem[i+1].sWord);
			//24832=='a'*256
			pItemRet[j].nHandle=24832;		//'a'
			if(pItem[i].nHandle/256=='v'||pItem[i+1].nHandle/256=='v')//30208='v'8256
			{
				pItemRet[j].nHandle=30208;
			}

			i+=1;
			j+=1;
			bProcessed=true;
		}
		else if(pItem[i].nHandle/256=='u'&&pItem[i].nHandle%256)//uj,ud,uv,uz,ul,ug->u
			pItem[i].nHandle='u'*256;
		else if(pItem[i].nHandle/256=='s'&&pItem[i].nHandle%256=='s')//ss->nx
			pItem[i].nHandle='n'*256+'x';
		else if(pItem[i].nHandle==28275&&m_Transliteration.m_dict.IsExist(pItem[i+1].sWord,30))
		{
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			strcat(pItemRet[j].sWord,pItem[i+1].sWord);
			pItemRet[j].nHandle=28275;
			i+=1;
			j+=1;
			bProcessed=true;
		}
		if(!bProcessed)
		{//If not processed,that's mean: not need to adjust;
			//just copy to the final result
			strcpy(pItemRet[j].sWord,pItem[i].sWord);
			pItemRet[j++].nHandle=pItem[i].nHandle;
		}
		i++;
	}
	pItemRet[j].sWord[0]=0;//Set ending
	return true;
}

/*
#define  SEPERATOR_C_SENTENCE "。！？：；…"
#define  SEPERATOR_E_SENTENCE "!?:;"
#define  SEPERATOR_C_SUB_SENTENCE "、，（）“”‘’"
#define  SEPERATOR_E_SUB_SENTENCE ",()\042'"
#define  SEPERATOR_LINK "\n\r 　"
*/

//Paragraph Segment and POS Tagging
//分析一个句子,如果由特殊字符,也作为一个句子
bool CResult::ParagraphProcessing(const char *sParagraph, int nSize, AnalysisCallBack* pCallBack)
{
	char sChar[3];
	int nLen=nSize + 13;

	m_szSentence[0]=0;

	unsigned int nPosIndex=0;
	unsigned int nParagraphLen=nSize;
	unsigned int nSentenceIndex=0;


	sChar[2]=0;

	bool	bEnd=false;
	int		nStartPos=0;
	unsigned short nWordIndex=0;
	while( nPosIndex < nParagraphLen )
	{
		//Find a whole sentence which separated by ! . \n \r
		sChar[0]=sParagraph[nPosIndex];//Get a char
		sChar[1]=0;
		if( sParagraph[nPosIndex] < 0 )
		{
			//double byte char
			nPosIndex+=1;
			sChar[1]=sParagraph[nPosIndex];
		}
		//CC_Find全角查找, 句子结束标志
		//strstr半角查找,
		if( strstr(SEPERATOR_LINK,sChar) )
		{
			bEnd=true;
		}
		else if( CC_Find(SEPERATOR_C_SENTENCE,sChar)
			||CC_Find(SEPERATOR_C_SUB_SENTENCE,sChar) )
		{
			bEnd=true;
		}
		else if( strstr(SEPERATOR_E_SENTENCE,sChar)
			||strstr(SEPERATOR_E_SUB_SENTENCE,sChar) )
		{
			bEnd=true;
		}
		else //Other chars and store in the sentence buffer
		{
			strcat(m_szSentence,sChar);
		}
		if( bEnd)
		{
			if(m_szSentence[0] != 0)
			{
				Processing(m_szSentence, nStartPos, nWordIndex, pCallBack);//Processing and output the result of current sentence.
				nStartPos=	nPosIndex+1;
				m_szSentence[0]=0;
				bEnd=false;
			}
		}
		nPosIndex+=1;
	}

	if(m_szSentence[0]!=0&&strcmp(m_szSentence,SENTENCE_BEGIN)!=0)
	{
		Processing( m_szSentence, nStartPos, nWordIndex, pCallBack);//Processing and output the result of current sentence.
	}
	return true;
}





bool CResult::ChineseNameSplit(char *sPersonName, char *sSurname, char *sSurname2, char *sGivenName, CDictionary &personDict)
{
	int nSurNameLen=4,nLen=strlen(sPersonName),nFreq,i=0,nCharType;
	char sTemp[3];
	if(nLen<3||nLen>8)//Not a traditional Chinese person name
		return false;
	while(i<nLen)//No Including non-CHinese char
	{
		nCharType=charType((unsigned char*)sPersonName+i);
		if(nCharType!=CT_CHINESE&&nCharType!=CT_OTHER)
			return false;
		i+=2;
	}
	sSurname2[0]=0;//init 
	strncpy(sSurname,sPersonName,nSurNameLen);	
	sSurname[nSurNameLen]=0;
	if(!personDict.IsExist(sSurname,1))
	{
		nSurNameLen=2;
		sSurname[nSurNameLen]=0;
		if(!personDict.IsExist(sSurname,1))
		{
			nSurNameLen=0;
			sSurname[nSurNameLen]=0;
		}
	}
	strcpy(sGivenName,sPersonName+nSurNameLen);
	if(nLen>6)
	{
		strncpy(sTemp,sPersonName+nSurNameLen,2);
		sTemp[2]=0;//Get the second possible surname
		if(personDict.IsExist(sTemp,1))
		{
			//Hongkong women's name: Surname+surname+given name
			strcpy(sSurname2,sTemp);
			strcpy(sGivenName,sPersonName+nSurNameLen+2);
		}
	}
	nFreq=personDict.GetFrequency(sSurname,1);
	if(nSurNameLen!=4&&((nSurNameLen==0&&nLen>4)||strlen(sGivenName)>4||(GetForeignCharCount(sPersonName)==3&&nFreq<personDict.GetFrequency("张",1)/4)||(nFreq<10&&GetForeignCharCount(sGivenName)==(nLen-nSurNameLen)/2)))
		return false;
	if(nLen==4&&m_Person.IsGivenName(sPersonName))
	{//Single Surname+given name
		return false;
	}
	return true;
}

