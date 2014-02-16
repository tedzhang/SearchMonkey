#include "ResultList.h"
#include "Global.h"


//create a new one
CResultList* CResultList::Interset(CResultList &list2)
{
	
	CTermDoc *pThisHead =  getFirst();
	CTermDoc *plist2Head = list2.getFirst();
	CResultList *pResult = NULL;
	
    while(pThisHead  && plist2Head)
	{
          if(pThisHead->GetDocID() < plist2Head->GetDocID())
			  pThisHead = getNext();
		  else if(pThisHead->GetDocID() > plist2Head->GetDocID())
			  plist2Head = list2.getNext();
		  else
		  {
             if(!pResult)
				 pResult = new CResultList;
			 CTermDoc* pHitDoc = new CTermDoc(pThisHead->GetDocID(),pThisHead->GetScore()+plist2Head->GetScore());
			 pResult->addNodeTail(pHitDoc);
			 pThisHead = getNext();
			 plist2Head =list2.getNext();
		  }
	}
	return pResult;
}

CResultList* CResultList::Merge( CResultList &list2)
{
	CTermDoc *pThisHead =  getFirst();
	CTermDoc *plist2Head = list2.getFirst();
	CResultList *pResult = NULL;
	if(pThisHead ||plist2Head)
		pResult = new CResultList;

	while(pThisHead  && plist2Head )
	{
		if(pThisHead->GetDocID() < plist2Head->GetDocID())
		{
			pResult->addNodeTail(pThisHead->Clone());
			pThisHead = getNext();
		}
		else if(pThisHead->GetDocID() > plist2Head->GetDocID())
		{
			pResult->addNodeTail(plist2Head->Clone());
			plist2Head = list2.getNext();
		}
		else
		{
			CTermDoc *pHitDoc = new CTermDoc(pThisHead->GetDocID(),pThisHead->GetScore()+ plist2Head->GetScore());
			pResult->addNodeTail(pHitDoc);
			pThisHead = getNext();
			plist2Head =list2.getNext();
		}
	}
	CResultList *pRemainList = NULL;
	if(pThisHead||plist2Head)
	{
		pRemainList = pThisHead ? this: &list2;
	}
	if(pResult && pRemainList)	
	{
		CTermDoc *pDoc = NULL;
		while(pDoc =pRemainList->getCurrent() )
		{
			pResult->addNodeTail(pDoc->Clone());
			pRemainList->getNext();
		}
		
	}
	return pResult;

}

//
//
// Return a list contains elements in list1 but not in list2
CResultList* CResultList::Difference(CResultList &list2)
{
	CTermDoc *pThisHead =  getFirst();
	CTermDoc *plist2Head = list2.getFirst();
	CResultList *pResult = NULL;
	if(pThisHead)
		pResult = new CResultList;

    while(pThisHead && plist2Head) 
	{
		if(pThisHead->GetDocID() < plist2Head->GetDocID())
		{
			pResult->addNodeTail(pThisHead->Clone());
			pThisHead = getNext();
		}
		else if(pThisHead->GetDocID() > plist2Head->GetDocID())
		{
			plist2Head = list2.getNext();
		}
		else
		{
			pThisHead->SetScore(pThisHead->GetScore()- plist2Head->GetScore());
			pThisHead = getNext();
			plist2Head = list2.getNext();
		}
	}
	if(pThisHead)
	{
		CTermDoc *pHitDoc = NULL;
		while(pHitDoc = getCurrent())
		{
           pResult->addNodeTail(pHitDoc->Clone());
		   getCurrent();
		}
		
	}
	return pResult;
}


