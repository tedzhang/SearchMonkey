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
 * Filename: Span.h: 
 * Abstract:
 *          interface for the CSpan class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-4-23
 *
 * Notes:    Tagging with Hidden Markov Model
 *                
 ****************************************************************************/


#pragma  once 


#include "ContextStat.h"
#include "Dictionary.h"
#include <math.h>


#define MAX_WORDS_PER_SENTENCE 120
#define MAX_UNKNOWN_PER_SENTENCE 200
#define MAX_POS_PER_WORD 20
#define LITTLE_FREQUENCY 6

enum TAG_TYPE{
		TT_NORMAL,
		TT_PERSON,
		TT_PLACE,
		TT_TRANS
};


class CSpan  
{

public:
	CSpan();
	~CSpan(){ }

public:
	bool TransRecognize(CDictionary &dictCore,CDictionary &transDict);
	bool PlaceRecognize(CDictionary &dictCore,CDictionary &placeDict);
	bool PersonRecognize(CDictionary &personDict);
	bool POSTagging(PWORD_RESULT pWordItems,CDictionary &dictCore,CDictionary &dictUnknown);
	
	
	//POS tagging with Hidden Markov Model
	void SetTagType(enum TAG_TYPE nType=TT_NORMAL);
	//Set the tag type
	bool LoadContext(char *sFilename);
	int m_nUnknownIndex;

	//The number of unknown word
	int m_nUnknownWords[MAX_UNKNOWN_PER_SENTENCE][2];

	//The start and ending possition of unknown position
	ELEMENT_TYPE m_dWordsPossibility[MAX_UNKNOWN_PER_SENTENCE];
	
	//The possibility of unknown words
	CContextStat m_context;//context
protected:
	ELEMENT_TYPE  ComputePossibility(int nStartPos,int nLength,CDictionary &dict);
	int GetFrom(PWORD_RESULT pWordItems,int nIndex,CDictionary &dictCore,CDictionary &dictUnknown);
	//Get words from the word items, start from nIndex, Function for unknown words recognition
	bool GuessPOS(int nIndex,int *pSubIndex);
	bool GetBestPOS();
	bool Reset(bool bContinue=true);
	bool UnknownMatch();
	bool SplitPersonPOS(CDictionary &unlistDict);
	bool Disamb();
private:
	TAG_TYPE m_tagType;//The type of tagging
	int m_nStartPos;
	
	int m_nBestTag[MAX_WORDS_PER_SENTENCE];

	//Record the Best Tag
	char m_sWords[MAX_WORDS_PER_SENTENCE][MAX_WORD_SIZE];
	int m_nWordPosition[MAX_WORDS_PER_SENTENCE];
	int m_nTags[MAX_WORDS_PER_SENTENCE][MAX_POS_PER_WORD];
	char m_nBestPrev[MAX_WORDS_PER_SENTENCE][MAX_POS_PER_WORD];
	char m_nCurLength;
	double m_dFrequency[MAX_WORDS_PER_SENTENCE][MAX_POS_PER_WORD];
};

//Set the tag type
inline void CSpan::SetTagType(enum TAG_TYPE  nType)
{
	m_tagType=nType;
}



inline bool CSpan::Disamb()
{
	int i,j,k,nMinCandidate;
	double dMinFee=0.0,dTmp;
	for(i=1;i<m_nCurLength;i++)//For every word
	{
		for(j=0;m_nTags[i][j]>=0;j++)//For every word
		{
			nMinCandidate=MAX_POS_PER_WORD+1;
			for(k=0;m_nTags[i-1][k]>=0;k++)
			{
				dTmp=-log(m_context.GetContextPossibility(0,m_nTags[i-1][k],m_nTags[i][j]));
				dTmp+=m_dFrequency[i-1][k];//Add the fees
				if(nMinCandidate>10||dTmp<dMinFee)//Get the minimum fee
				{
					nMinCandidate=k;
					dMinFee=dTmp;
				}
			}
			m_nBestPrev[i][j]=nMinCandidate;//The best previous for j
			m_dFrequency[i][j]=m_dFrequency[i][j]+dMinFee;
		}
	}

	return true;
}

inline bool CSpan::GetBestPOS()
{
	Disamb();
	for(int i=m_nCurLength-1,j=0;i>0 && j >= 0;i--)
	{
		if(m_sWords[i][0])
		{//Not virtual ending
			m_nBestTag[i]=m_nTags[i][j];//Record the best POS and its possibility
		}
		j=m_nBestPrev[i][j];
	}
	int nEnd=m_nCurLength;//Set the end of POS tagging
	if(m_sWords[m_nCurLength-1][0]==0)
		nEnd=m_nCurLength-1;
	m_nBestTag[nEnd]=-1;
	return true;
}


