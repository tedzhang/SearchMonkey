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
 *           interface for the CSegment class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-4-23
 *
 * Notes:  N-Shortest paths Word segmentation
 *                
 ****************************************************************************/

#pragma  once 


#include "DynamicArray.h"
#include "Dictionary.h"
#include "SegGraph.h"//The segmentation graph


class CDictionary;

class  CSegment  
{
public:
	

	 bool GetLastWord(PWORD_RESULT pItem, char *sWordRet);
	 int  GetResultCount(PWORD_RESULT pItem);
	 bool OptimumSegmet(int nResultCount=1);
	
	//Word Segmentation based on optimum segmentation graph
	//After unknown word recognition
	bool Segment(char *sSentence,CDictionary &dictCore,int nResultCount=1);
	
	CSegment();
	~CSegment();

	PWORD_RESULT *	m_pWordSeg;
	int **			m_nSegRoute;//The segmentation route

	int m_nSegmentCount;
	//The segmentation result
	CDynamicArray m_graphOptimum;//The optimumized segmentation graph
	CSegGraph     m_graphSeg;//The segmentation graph
protected:
	bool IsYearTime(char *sNum);
	bool GenerateWord(int ** nSegRoute, int nIndex);
};


