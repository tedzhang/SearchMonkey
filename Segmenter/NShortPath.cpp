

#include "NShortPath.h"
#include "Segment.h"
#include <memory.h>
#include <string.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNShortPath::CNShortPath( CDynamicArray *apCost )
{
	//Set the cost
	m_apCost=		apCost;				
	m_nVertex=		apCost->m_nCol+1;	

	if(m_nVertex < apCost->m_nRow+1)
	{
		//Get the vertex numbers
		m_nVertex=	apCost->m_nRow+1;	
	}
	m_pWeight= NULL;
	m_pParent= NULL;
	if( m_nVertex > 1 )
	{
		//not including the first node
		m_pParent=new CQueue[m_nVertex-1];
		m_pWeight=new ELEMENT_TYPE[m_nVertex-1];
	}
}

CNShortPath::~CNShortPath()
{
	if( m_pWeight != NULL )
	{
		delete [] m_pWeight;
	}
	if( m_pParent != NULL )
	{
		delete [] m_pParent;//not including the first node
	}
}



int CNShortPath::ShortPath()
{
	int nCurNode=1,nPreNode,nIndex;
	ELEMENT_TYPE eWeight;
	array_node_t* pEdgeList;
	int nZero=0;
	
	for(; nCurNode < m_nVertex; nCurNode++)
	{
		CQueue queWork;
		eWeight=m_apCost->GetElement(-1,nCurNode,0,&pEdgeList);//Get all the edges

		while(pEdgeList != NULL && pEdgeList->nCol == nCurNode)
		{
			nPreNode=	pEdgeList->nRow;
			eWeight=	pEdgeList->fValue;//Get the value of edges


			if(nPreNode > 0)//Push the weight and the pre node infomation
			{
				if(m_pWeight[nPreNode-1]==INFINITE_VALUE)
				{
					break;
				}
				queWork.Push(nPreNode, nZero, eWeight + m_pWeight[nPreNode-1]);
			}
			else
			{
				queWork.Push(nPreNode,nZero, eWeight);
				break;
			}

			pEdgeList=pEdgeList->pNext;
			
		}//end while
		

		m_pWeight[nCurNode-1]=INFINITE_VALUE;

		if( queWork.Pop(nPreNode,nIndex,eWeight))
		{
			//Set the current node weight and parent
			if(m_pWeight[nCurNode-1]==INFINITE_VALUE)
				m_pWeight[nCurNode-1]=eWeight;
			else if(m_pWeight[nCurNode-1]<eWeight)//Next queue
			{
				break;
			}
			m_pParent[nCurNode-1].Push(nPreNode,nIndex);
		}
	}//end for
	
	return 1;
}



//bBest=true: only get one best result and ignore others
//Added in 2002-1-24
void CNShortPath::GetPaths(int **nResult, bool bBest)
{
	//if(m_nResultCount >= MAX_SEGMENT_NUM)//Only need 10 result
	//	return ;

    CQueue queResult;
	int nZero=0;

	int nCurIndex,nParentNode,nParentIndex,nResultIndex=0;
    
    nResult[m_nResultCount][nResultIndex]=-1;//Init the result

	int nCurNode = m_nVertex-1;
	queResult.Push(nCurNode,nZero);
   
	nCurIndex=nZero;
    bool bFirstGet;
    while(!queResult.IsEmpty())
	{
		ELEMENT_TYPE eWeight;

		while( nCurNode > 0 )
		{
			//Get its parent and store them in nParentNode,nParentIndex
			if(m_pParent[nCurNode-1].Pop( nParentNode,nParentIndex,eWeight, false, true))
			{
			   nCurNode=nParentNode;
			   nCurIndex=nParentIndex;
			}
			if(nCurNode>0)
                queResult.Push(nCurNode,nCurIndex);
		}
		if(nCurNode==0)
		{ //Get a path and output
  		   nResult[m_nResultCount][nResultIndex++]=nCurNode;//Get the first node
		   bFirstGet=true;
		   nParentNode=nCurNode;
		   while(queResult.Pop( nCurNode, nCurIndex, eWeight,false,bFirstGet))
		   {
			   nResult[m_nResultCount][nResultIndex++]=nCurNode;
    	       bFirstGet=false;
			   nParentNode=nCurNode;
		   }
		   nResult[m_nResultCount][nResultIndex]=-1;//Set the end
		   m_nResultCount+=1;//The number of result add by 1
		   if(m_nResultCount>=MAX_SEGMENT_NUM)//Only need 10 result
				return ;
		   nResultIndex=0;
		   nResult[m_nResultCount][nResultIndex]=-1;//Init the result 

		   if(bBest)//Return the best result, ignore others
			   return ;
		}
		queResult.Pop( nCurNode, nCurIndex,eWeight,false,true);//Read the top node
        while(queResult.IsEmpty()==false&&(m_pParent[nCurNode-1].IsSingle()||m_pParent[nCurNode-1].IsEmpty(true)))
		{
	       queResult.Pop(nCurNode,nCurIndex,eWeight);//Get rid of it
		   queResult.Pop(nCurNode,nCurIndex,eWeight,false,true);//Read the top node
		}
        if(queResult.IsEmpty()==false&&m_pParent[nCurNode-1].IsEmpty(true)==false)
		{
			   m_pParent[nCurNode-1].Pop(nParentNode,nParentIndex,eWeight,false,false);
			   nCurNode=nParentNode;
			   nCurIndex=nParentIndex;
			   if(nCurNode>0)
			       queResult.Push(nCurNode,nCurIndex);
		}
	}
}


int CNShortPath::Output(int **nResult,bool bBest,int *npCount)
{
  //sResult is a string array
  m_nResultCount=0;//The 
  if(m_nVertex<2)
  {
	  nResult[0][0]=0;
	  nResult[0][1]=1;
	  *npCount=1;
	  return 1;
  }
  if( m_pWeight[m_nVertex-2] < INFINITE_VALUE )
  {
	  GetPaths( nResult, bBest);
	  *npCount=m_nResultCount;
	  if(nResult[0][0]!=-1 && bBest)//Get the best answer
		  return 1;
      if( m_nResultCount >= MAX_SEGMENT_NUM)//Only need 10 result
	 	  return 1;
  }
  return 1;
}
