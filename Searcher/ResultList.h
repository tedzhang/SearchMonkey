#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007

#include "TermDoc.h"
#include "../include/Utilities/SingleLink.h"
#include "Searcher_Export.h"

//
//
//��������Ľ����������
//
//��������ÿ���ڵ�������ǰ�docID�ź����
//

class SEARCHER_API CResultList:public CSingleLink<CTermDoc>
{
public:
	//
	// this ��list2�󽻣�����this������
	CResultList* Interset( CResultList &list2);
	//
	// this��list2��򣬷���this������
	CResultList* Merge( CResultList  &list2);

	//
	//this��list2���죬��this - list2
	//�磺this  1,3,5
	//list2�� 3
	//���ؽ��Ϊ1��5������this
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