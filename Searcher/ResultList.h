#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "TermDoc.h"
#include "../include/Utilities/SingleLink.h"
#include "Searcher_Export.h"

//
//
//有序链表的交，或，异操作
//
//链表里面每个节点的数据是按docID排好序的
//

class SEARCHER_API CResultList:public CSingleLink<CTermDoc>
{
public:
	//
	// this 和list2求交，返回this（有序）
	CResultList* Interset( CResultList &list2);
	//
	// this和list2求或，返回this（有序）
	CResultList* Merge( CResultList  &list2);

	//
	//this和list2求异，即this - list2
	//如：this  1,3,5
	//list2： 3
	//返回结果为1，5，返回this
	CResultList* Difference(CResultList &list2);
};

//
// Utility function for clear the result list 
//
inline  void Delete_ResultList(CResultList *&p)
{
	while(p->rmFirstNode() == 0)
		;

	delete p;
}