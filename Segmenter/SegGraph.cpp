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
 * Filename: SegGraph.cpp
 * Abstract:
 *            implement for the Word Segmentation Directed Graph.
 *
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-1-8
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
// SegGraph.cpp: implementation of the CSegGraph class.
//
//////////////////////////////////////////////////////////////////////
#include "SegGraph.h"
#include "Utility.h"
#include "Dictionary.h"
#include <string>
#include <string.h>
#include <math.h>



bool CSegGraph::GenerateWordNet(char *sSentence,CDictionary &dictCore)
{

	//Gernerate the word net from the sLine, that's list all the possible word
	int i=0,j,nLen=strlen(sSentence);
	char sWord[WORD_MAXLENGTH]="",sTempWord[WORD_MAXLENGTH]="",sWordMatch[WORD_MAXLENGTH];
	int nWordIndex=0,nHandleTemp,k,nPOS;
	int nMatchFreq[20],nMatchHandle[20],nTotalFreq,nMatchCount;
	
	m_nAtomCount=0;
	m_segGraph.SetEmpty();//Set segmentation graph empty

	AtomSegment(sSentence, nLen);
	//Atomic Segmentation

    for(i=0; i< m_nAtomCount;i++)//Init the cost array
    {
		if(m_nAtomPOS[i]==CT_CHINESE)//The atom is a Chinese Char
			m_segGraph.SetElement(i,i+1,LOG_MAX_FRQUENCE,0,m_sAtom[i]);//init the link with the maximum value
		else//Other atom
		{
			switch(m_nAtomPOS[i])
			{
			case CT_INDEX:
			case CT_NUM:
				nPOS='m'*256; 
				break;
			case CT_DELIMITER:
				nPOS='w'*256;
				break;
			case CT_LETTER:
				nPOS='s'*256+'s';
				break;
			case CT_SINGLE://12021-2129-3121
				if(m_sAtom[i][0]<='9'&&m_sAtom[i][0]>='0')
					nPOS='m'*256;
				else
					nPOS='s'*256+'s';
				break;
			default:
				nPOS=m_nAtomPOS[i];//'?'*256;
				break;
			}
			m_segGraph.SetElement(i,i+1,0,nPOS,m_sAtom[i]);//init the link with minimum
		}
    }
	
	i=0;
	while(i < m_nAtomCount)//All the word
	{
		strcpy(sWord,m_sAtom[i]);//Get the current atom
		j=i+1;
		
		while( j <= m_nAtomCount
			&& dictCore.GetMaxMatch(sWord,sWordMatch,&nHandleTemp) )
		{
			//Add a condition to control the end of string
			//retrieve the dictionary with the word
			if(strcmp(sWordMatch,sWord)==0)//find the current word
			{
				nTotalFreq=0;
				dictCore.GetHandle(sWord,&nMatchCount,nMatchHandle,nMatchFreq);
				for(k=0;k<nMatchCount;k++)//Add the frequency
				{
					nTotalFreq += nMatchFreq[k];
				}
				//Adding a rule to exclude some words to be formed.
				if(strlen(sWord)==4 
					&& i >= 1 
					&&
					( IsAllNum((unsigned char *)m_sAtom[i-1])||IsAllChineseNum(m_sAtom[i-1]))
					&&(strncmp(sWord,"年",2)==0||strncmp(sWord,"月",2)==0)
				  )
				{
					//1年内、1999年末
					if(CC_Find("末内中底前间初",sWord+2))
					{
						break;
					}
				}
				if(nMatchCount==1)//The possible word has only one POS, store it
				{
					m_segGraph.SetElement(i,j,LOG_MAX_FRQUENCE-log((double)nTotalFreq+1),nMatchHandle[0],sWord);
				}
				else
				{
					m_segGraph.SetElement(i,j,LOG_MAX_FRQUENCE-log((double)nTotalFreq+1),0,sWord);
				}
			}
			strcat(sWord,m_sAtom[j++]);
		}
		i+=1;	//Start from i++;
	}
	return true;
}

inline void GetToken(const char *sSentence,unsigned int nSentenceLength,
					 int &iStartIdx,
					 char *sChar, int &type)
{

	sChar[0]= *(sSentence+iStartIdx);		
	sChar[1]=0;
	iStartIdx++;

	if(sChar[0]< 0)					
	{
		sChar[1]= *(sSentence+iStartIdx);	
		iStartIdx++;
	}
	type = charType((unsigned char*) sChar);

	if(sChar[0]=='.' && iStartIdx < nSentenceLength)
	{
		if(charType((unsigned char *)sSentence+iStartIdx) == CT_NUM)
			type = CT_NUM;
		else if( *(sSentence+iStartIdx) >= '0'&& *(sSentence+iStartIdx) <= '9')
			type = CT_SINGLE;
	}
	else if (type == CT_LETTER)
		type = CT_SINGLE;

}


bool CSegGraph::AtomSegment(char *sSentence, int nLenth)
{
    int i=0,j=0,nCurType,nNextType;	
	//i is the pointer of sentence string
	//j is the pointer of pAtoms
	char sChar[3];
	sChar[2]=0;//Set the char ending
	m_sAtom[j][0]=0;//Set the first word as null
	m_nAtomLength[j]=0;

	while( i < nLenth )
	{

		GetToken(sSentence,nLenth,i,sChar,nCurType);

		if( nCurType==CT_CHINESE|| nCurType==CT_INDEX|| nCurType==CT_DELIMITER|| nCurType==CT_OTHER )
		{
			//Chinese char, index number,delimiter and other is treated as atom
            
			if(TrimStringSpaces(sChar))
			{
				strcat(m_sAtom[j],sChar);
				m_nAtomPOS[j]=nCurType;

				m_nAtomLength[j]= strlen(m_sAtom[j]);//Save its length
				j+=1;
				m_sAtom[j][0]=0;//init
			}
			
		}
		else if( i < nLenth && (nCurType == CT_SINGLE || nCurType == CT_NUM || nCurType == CT_LETTER))
		{

			int istart ;
			std::string strBuffer;;
			strBuffer.reserve(MAX_WORD_SIZE);
			strBuffer += sChar;

			while (i < nLenth)
			{
				istart = i;
				GetToken(sSentence,nLenth,i,sChar,nNextType);
				if (nNextType == nCurType)
					strBuffer += sChar;
				else
				{
					i = istart;
					break;
				}
			}

			int nAtomLength = strBuffer.length();
			//
			//
			//sometimes a number will be more than MAX_WORD_SIZE
			//or sentence consisted by delimiters, like the following
			//"-------------------------------------------------------------------------------"
			//
			if(nAtomLength > 0 && nAtomLength < MAX_WORD_SIZE)
			{
               strcpy(m_sAtom[j],strBuffer.c_str());
			   m_nAtomPOS[j]   = nCurType;
			   m_nAtomLength[j] = nAtomLength;

			   j+=1;
			   m_sAtom[j][0]=0;//init
			}
		}
		else
		{
			if(TrimStringSpaces(sChar))
			{
				strcat(m_sAtom[j],sChar);
				m_nAtomPOS[j]=nCurType;

				m_nAtomLength[j]= strlen(m_sAtom[j]);//Save its length
				j+=1;
				m_sAtom[j][0]=0;//init
			}
		}
	}
	m_nAtomCount = j;//The count of segmentation atoms
	return true;
}

