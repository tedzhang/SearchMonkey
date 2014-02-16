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
 * Filename: UnknowWord.cpp
 * Abstract:
 *           interface for the CUnknowWord class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-4-23
 *
 * Notes:  Unknown words recognition based on Role-tagging
 *                
 ****************************************************************************/

#pragma  once 

#include "span.h"  //CSpan class
#include "SegGraph.h"//CSpan class

class  CUnknowWord  
{
public:
	//	Judge whether the name is a given name
	bool IsGivenName(char *sName);
	bool Configure(char *sConfigFile,enum TAG_TYPE type=TT_PERSON);
	bool Recognition(PWORD_RESULT pWordSegResult, CDynamicArray &graphOptimum,CSegGraph &graphSeg,CDictionary &dictCore);

	//Unknown dictionary
	CDictionary m_dict;
	
private:
	//Role tagging
	CSpan m_roleTag;
	//The POS of such a category
	int m_nPOS;
};


