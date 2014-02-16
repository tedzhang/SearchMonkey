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
 * Filename:  SegGraph.h
 * Abstract:
 *           interface for the Word Segmentation Directed Graph.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-4-18
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
// SegGraph.h: interface for the CSegGraph class.
//
//////////////////////////////////////////////////////////////////////


#pragma once 


#include "DynamicArray.h"//Dynamic array

class CDictionary;

class CSegGraph  
{
public:
	
	//Generate the segmentation word net according the original sentence
	//sSentence: the sentence
	//dictCore: core dictionary
	bool GenerateWordNet(char *sSentence,CDictionary &dictCore);
	
	char m_sAtom[MAX_SENTENCE_LEN][WORD_MAXLENGTH];
	int  m_nAtomLength[MAX_SENTENCE_LEN];
	int  m_nAtomPOS[MAX_SENTENCE_LEN]; 
	int  m_nAtomCount;

	//segGraph: The segmentation word graph
	CDynamicArray m_segGraph;	
protected:
	//Segment the atomic members from the original sentence
	//sSentence: the sentence
	//pAtoms: the buffer for returned segmented atoms
	//        Such as a Chinese Char, digit, single byte, or delimiters
	bool AtomSegment(char *sSentence, int nLenth);
	
};


