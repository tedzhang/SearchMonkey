/****************************************************************************
 *
 * Copyright (c) 2000, 2001 
 *     Machine Group
 *     Software Research Lab.
 *     Institute of Computing Tech.
 *     Chinese Academy of Sciences
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Institute of Computing Tech. and the posession or use of this file requires 
 * a written license from the author.
 * Filename: Segment.cpp
 * Abstract:
 *           implementation of the CSegment class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-4-23
 *
 * Notes:  N-Shortest paths Word segmentation
 *                
 ****************************************************************************/

#include "Segment.h"
#include "Utility.h"
#include "NShortPath.h"
#include <string.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSegment::CSegment()
{
	//malloc buffer
	m_pWordSeg = new PWORD_RESULT[MAX_SEGMENT_NUM];
	int i;
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		m_pWordSeg[i]=new WORD_RESULT[MAX_WORDS];
	}
	m_nSegRoute=new int*[MAX_SEGMENT_NUM];
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		m_nSegRoute[i]=new int[MAX_SENTENCE_LEN/2];
	}

}

CSegment::~CSegment()
{
	//free buffer
	int i;
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete[] m_pWordSeg[i];
	}
	delete[] m_pWordSeg;
	for( i=0; i< MAX_SEGMENT_NUM; i++ )
	{
		delete[] m_nSegRoute[i];
	}
	delete[] m_nSegRoute;
}


//After unknown word recognition
bool CSegment::OptimumSegmet(int nResultCount)
{
	int **nSegRoute;//The segmentation route
	nSegRoute=new int*[MAX_SEGMENT_NUM];
	int i;
	for(i=0;i < MAX_SEGMENT_NUM;i++)
	{
		nSegRoute[i]=new int[MAX_SENTENCE_LEN/2];
	}
	CNShortPath sp(&m_graphOptimum);
	sp.ShortPath();
	sp.Output((int **)nSegRoute,false,&m_nSegmentCount);
	i=0;
	m_graphSeg.m_segGraph=m_graphOptimum;
	m_graphOptimum.SetEmpty();//Set graph optimum empty
	while(i<m_nSegmentCount)
	{
		GenerateWord(nSegRoute,i);
		//Generate word according the Segmentation route
		i++;
	}
	
	//free the memory
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		delete [] nSegRoute[i];//free the pointer memory
	}
	delete [] nSegRoute;//free the pointer array
	return true;
}


bool CSegment::Segment(char *sSentence,CDictionary &dictCore,int nResultCount)
{
	int i  = 0;
	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
		 //memset(m_nSegRoute[i],0,MAX_SENTENCE_LEN/2*sizeof(int));
		memzeroquad(m_nSegRoute[i],MAX_SENTENCE_LEN/2);
	}

	for(i=0;i<MAX_SEGMENT_NUM;i++)
	{
	   //memset( m_pWordSeg[i], 0, sizeof(WORD_RESULT) * MAX_WORDS );
		memzeroquad(m_pWordSeg[i],sizeof(WORD_RESULT)*MAX_WORDS/4);
	}
	

 	m_graphSeg.GenerateWordNet(sSentence,dictCore);
	CNShortPath sp(&m_graphSeg.m_segGraph);
	sp.ShortPath();
	sp.Output((int **)m_nSegRoute, false, &m_nSegmentCount);
	m_graphOptimum.SetEmpty();//Set graph optimum empty
	i=0;
	while(i<m_nSegmentCount)
	{
		GenerateWord(m_nSegRoute,i);
		//Generate word according the Segmentation route
		i++;
	}
	
	return true;
}
//Generate Word according the segmentation route
bool CSegment::GenerateWord(int **nSegRoute, int nIndex)
{
	unsigned int i=0,k=0;
	int j,nStartVertex,nEndVertex,nPOS;
	char sAtom[WORD_MAXLENGTH],sNumCandidate[100];
	ELEMENT_TYPE fValue;
	while(nSegRoute[nIndex][i]!=-1&&nSegRoute[nIndex][i+1]!=-1&&nSegRoute[nIndex][i]<nSegRoute[nIndex][i+1])
	{
		nStartVertex=nSegRoute[nIndex][i];
		j=nStartVertex;//Set the start vertex
		nEndVertex=nSegRoute[nIndex][i+1];//Set the end vertex
		nPOS=0;
		m_graphSeg.m_segGraph.GetElementInfo(nStartVertex,nEndVertex,fValue,nPOS);
		sAtom[0]=0;

		while(j < nEndVertex)
		{
			//Generate the word according the segmentation route
			strcat(sAtom,m_graphSeg.m_sAtom[j]);
			j++;
		}
		m_pWordSeg[nIndex][k].sWord[0]=0;//Init the result ending
		strcpy(sNumCandidate,sAtom);
		while(sAtom[0]!=0 && (IsAllNum((unsigned char *)sNumCandidate)||IsAllChineseNum(sNumCandidate)))
		{
			//Merge all seperate continue num into one number
		    //sAtom[0]!=0: add in 2002-5-9
			strcpy(m_pWordSeg[nIndex][k].sWord,sNumCandidate);
			//Save them in the result segmentation
			i++;//Skip to next atom now 
			sAtom[0]=0;
			
			while(j<nSegRoute[nIndex][i+1])
			{//Generate the word according the segmentation route
				strcat(sAtom,m_graphSeg.m_sAtom[j]);
				j++;
			}
			strcat(sNumCandidate,sAtom);
		}
		if(m_pWordSeg[nIndex][k].sWord[0]==0)//Have never entering the while loop
		{
			strcpy(m_pWordSeg[nIndex][k].sWord,sAtom);
			//Save them in the result segmentation
		}
		else
		{//It is a num
			if(strcmp("－－",m_pWordSeg[nIndex][k].sWord)==0||strcmp("—",m_pWordSeg[nIndex][k].sWord)==0||m_pWordSeg[nIndex][k].sWord[0]=='-')//The delimiter "－－"
			{
				nPOS=30464;//'w'*256;Set the POS with 'w'
				i--;//Not num, back to previous word
			}
			else
			{//Adding time suffix

				char sInitChar[3];
				unsigned int nCharIndex=0;//Get first char
				sInitChar[nCharIndex]=m_pWordSeg[nIndex][k].sWord[nCharIndex];
				if(sInitChar[nCharIndex]<0)
				{
					nCharIndex+=1;
					sInitChar[nCharIndex]=m_pWordSeg[nIndex][k].sWord[nCharIndex];
				}
				nCharIndex+=1;
				sInitChar[nCharIndex]='\0';
				if(k>0&&m_pWordSeg[nIndex][k-1].nHandle==27904&&(strcmp(sInitChar,"—")==0||sInitChar[0]=='-')&&(strlen(m_pWordSeg[nIndex][k].sWord)>nCharIndex))
				{//3-4月                                 //27904='m'*256
				   //Split the sInitChar from the original word
					strcpy(m_pWordSeg[nIndex][k+1].sWord,m_pWordSeg[nIndex][k].sWord+nCharIndex);
					m_pWordSeg[nIndex][k+1].dValue=m_pWordSeg[nIndex][k].dValue;
					m_pWordSeg[nIndex][k+1].nHandle=27904;
					m_pWordSeg[nIndex][k].sWord[nCharIndex]=0;
					m_pWordSeg[nIndex][k].dValue=0;
					m_pWordSeg[nIndex][k].nHandle=30464;//'w'*256;
					m_graphOptimum.SetElement(nStartVertex,nStartVertex+1,m_pWordSeg[nIndex][k].dValue,m_pWordSeg[nIndex][k].nHandle);
					nStartVertex+=1;
					k+=1;
				}
				unsigned int nLen=strlen(m_pWordSeg[nIndex][k].sWord);
				if((strlen(sAtom)==2&&CC_Find("月日时分秒",sAtom))||strcmp(sAtom,"月份")==0)
				{//2001年
					strcat(m_pWordSeg[nIndex][k].sWord,sAtom);
					nPOS=29696;//'t'*256;//Set the POS with 'm'
				}
				else if(strcmp(sAtom,"年")==0)
				{
					 if(IsYearTime(m_pWordSeg[nIndex][k].sWord))//strncmp(sAtom,"年",2)==0&&
					 {//1998年，
						strcat(m_pWordSeg[nIndex][k++].sWord,sAtom);
						nPOS='t'*256;//Set the POS with 'm'
					 }
					 else
					    i--;//Can not be a time word
				}
     			else
				{
					//早晨/t  五点/t 
					if(strcmp(m_pWordSeg[nIndex][k].sWord+strlen(m_pWordSeg[nIndex][k].sWord)-2,"点")==0)
					{
						nPOS='t'*256;//Set the POS with 'm'
					}	
					else 
					{
						if(m_pWordSeg[nIndex][k].sWord[0]!='.')
							nPOS='m'*256;//Set the POS with 'm'
						if(nLen>1&&m_pWordSeg[nIndex][k].sWord[nLen-1]=='.')
						{//Get rid of . example 1.
							m_pWordSeg[nIndex][k].sWord[nLen-1]=0;
							i--;
						}
					}
					i--;//Not num, back to previous word
				}
			}
			fValue=0;
			nEndVertex=nSegRoute[nIndex][i+1];//Ending POS changed to latter
	}
		m_pWordSeg[nIndex][k].nHandle=nPOS;//Get the POS of current word
		m_pWordSeg[nIndex][k].dValue=fValue;//(int)(MAX_FREQUENCE*exp(-fValue));//Return the frequency of current word
		m_graphOptimum.SetElement(nStartVertex,nEndVertex,fValue,nPOS);
		//Generate optimum segmentation graph according the segmentation result
		i++;//Skip to next atom
		k++;//Accept next word
	}
	m_pWordSeg[nIndex][k].sWord[0]=0;
	m_pWordSeg[nIndex][k].nHandle=-1;//Set ending
	return true;
}




int CSegment::GetResultCount(PWORD_RESULT pItem)
{
	int nCount=0;
	while(pItem[nCount].sWord[0]!=0)
	{
		nCount+=1;
	}
	return nCount;
}

bool CSegment::GetLastWord(PWORD_RESULT pItem, char *sWordRet)
{
	int nCount=0;
	sWordRet[0]=0;
	while(pItem[nCount].sWord[0]!=0)
	{
		strcpy(sWordRet,pItem[nCount].sWord);
		nCount+=1;
	}
	return !sWordRet[0];
}

bool CSegment::IsYearTime(char *sNum)
{
	//Judge whether the sNum is a num genearating year
	unsigned int nLen=strlen(sNum);
	char sTemp[3];
	strncpy(sTemp,sNum,2);
	sTemp[2]=0;
	if(IsAllSingleByte((unsigned char *)sNum)&&(nLen>=3||nLen==2&&sNum[0]>'4'))//1992年, 90年
		return true;
	if(IsAllNum((unsigned char *)sNum)&&(nLen>=6||nLen==4&&CC_Find("５６７８９",sTemp)))
		return true;
	if(GetCharCount("零○一二三四五六七八九壹贰叁肆伍陆柒捌玖",sNum)==(int)nLen/2&&nLen>=3)
		return true;
	if(nLen==8&&GetCharCount("千仟零○",sNum)==2)//二仟零二年
		return true;
	return false;
}
