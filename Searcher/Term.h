#pragma  once 
// Author : Yong Zhang(yongzhang81@gmail.com)
// 2/9/2007


#include "../include/Utilities/SmallObjectPool.h"
#include <string>

class CTerm
{
public:
	CTerm(const std::string &termtext,const std::string &field)  
		:m_text(termtext),m_field(field)
	{
	}
	CTerm() { }

	void SetText(const std::string &termText){ m_text = termText;}
	const std::string&  GetText() const{ return m_text;}

	void SetField(const std::string &field)  { m_field = field;}
	const std::string& GetField() const{ return m_field;}

	bool operator<(const CTerm& term) const
	{
       if(GetField() == term.GetField())
		   return m_text < term.GetText();
	   return GetField() < term.GetField();
	}

private:
	std::string m_text;
	std::string m_field;
};


class CTermPool:public CStaticSmallObjectPool<CTerm*,4096>
{
public:
	CTermPool()
	{
		for(int i = 0; i < 4096;++i)
			m_pool[i] = new CTerm;
	}
	
};
