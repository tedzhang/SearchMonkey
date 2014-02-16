#pragma  once 
#include "Global.h"
#include "Field.h"
#include <vector>
#include "IndexExports.h"

class INDEXREXPORT_API CDocument
{
public:
	typedef std::vector<CField> FIELDS;
	int   GetFieldsCount() const{ return m_fields.size();}
	void  SetBoost(float boost) { m_boost = boost;}
	float GetBoost()const       { return m_boost;}
	void  Add(const CField &field){ m_fields.push_back(field);}

	const FIELDS& GetFields() const { return m_fields;}
	
	CField GetField(const std::string& name)const
	{
		for(int i = 0; i < m_fields.size();++i)
		{
			const CField & field = m_fields[i];
			if(field.GetName() == name)
				return field;
		}
		return CField();
	}
	std::string Get(const std::string &field)const
	{
		std::string result;
		result = GetField(field).GetValue();
		return result;
	}
private:
	FIELDS  m_fields;
	float   m_boost;
};