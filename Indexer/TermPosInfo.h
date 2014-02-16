#pragma  once 

#include "Global.h"

class CPosInfo
{
public:
	CPosInfo(USHORT nWordPos)
		:m_nWordPos(nWordPos),m_pNext(NULL)
	{
		
	};
	CPosInfo():m_nWordPos(0),m_pNext(NULL)
	{
		
	}
	~CPosInfo()
	{
	};
	void SetWordIndex(USHORT nWordPos)
	{
		m_nWordPos = nWordPos;
	}
	USHORT GetWordPos()
	{
		return m_nWordPos;
	}
    CPosInfo      * m_pNext;
	
private:
	USHORT	        m_nWordPos;	//分词属性, 表示分词编号
	
};