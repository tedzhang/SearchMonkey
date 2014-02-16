/****************************************************************************
 *
 * Copyright (c) 2000, 2001 
 *     Software Research Lab.
 *     Institute of Computing Tech.
 *     Chinese Academy of Sciences
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Institute of Computing Tech. and the posession or use of this file requires 
 * a written license from the author.
 *
 * Abstract:
 *           N-Shortest Path Problem for graph in word segement
 *           
 * Author: Kevin Chang (zhanghp@software.ict.ac.cn)
 *
 * Notes:
 *
 ****************************************************************************/
// NShortPath.h: interface for the CNShortPath class.
//
//////////////////////////////////////////////////////////////////////

#pragma  once 

#include "DynamicArray.h"
#include "queue.h"
#include <stdio.h>


class CNShortPath  
{
public:
	CNShortPath(CDynamicArray *aCost);
	virtual ~CNShortPath();
	
public:
	int m_nResultCount;
	int Output(int **nResult,bool bBest,int *npCount);
	int ShortPath();
private:
	void GetPaths(int **nResult=0, bool bBest=false);
	CDynamicArray  *m_apCost;
    int             m_nVertex;		//The number of vertex in the graph
	CQueue			*m_pParent;		//The 2-dimension array for the nodes
	ELEMENT_TYPE	*m_pWeight;	//The weight of node
};


