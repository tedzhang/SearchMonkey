#pragma  once 
#include <map>
#include <string>
#include "Global.h"
#include "Document.h"
#include "IndexExports.h"

struct FieldInfo
{
	std::string fieldName ;
	UINT        fieldNumber;
	bool        isTokenized;
	float       fBoost;

	FieldInfo(const std::string& name,UINT nNum,bool Tokenize,float fBoost=1.0)
		:fieldName(name),fieldNumber(nNum),isTokenized(Tokenize)
		 ,fBoost(fBoost)
	{
	}

};


class CBtStgCtrl;

class INDEXREXPORT_API CFieldInfos
{
	
private:
	
	//key is field name
	typedef std::map<std::string,FieldInfo> FieldName2FieldInfoTable;
	FieldName2FieldInfoTable m_table;

public:
	CFieldInfos(){ m_pthis = this;}

	void Add(const CDocument &doc)
	{
		const CDocument::FIELDS & fields = doc.GetFields();
		for(int i = 0; i < doc.GetFieldsCount();++i)
		{
			const CField &field = fields[i];
			Add(field.GetName(),field.IsTokenized());
		}
	}
	void Add(const std::string & name,bool isTokenized)
	{

		FieldName2FieldInfoTable::iterator it = m_table.find(name);
		if(it == m_table.end())
		{
			m_table.insert(std::make_pair(name,FieldInfo(name,m_table.size(),isTokenized)));
		}
		else
		{
			FieldInfo &fi = it->second;
			if(fi.isTokenized != isTokenized)
			{
				fi.isTokenized = true;
			}
		}
	}

	int GetFieldNumber(const std::string &name)const
	{
		FieldName2FieldInfoTable::const_iterator it = m_table.find(name);
		if(it != m_table.end())
		  return it->second.fieldNumber;
		return -1;
	}

	std::string GetFieldName(UINT nFieldNumber) const
	{
       FieldName2FieldInfoTable::const_iterator it = m_table.begin();
	   for(;it != m_table.end();++it)
	   {
		   if(it->second.fieldNumber == nFieldNumber)
			   return it->first;
	   }
	   return std::string();
	}

	float GetFieldBoost(const std::string &name) const
	{
		FieldName2FieldInfoTable::const_iterator it = m_table.find(name);
		if(it != m_table.end())
			return it->second.fBoost;
		return 1.0f;
	}

	int GetFieldCount() const
	{
		return m_table.size();
	}

	void Add(const std::string & str, UINT nFieldNum,bool tokenized,float boost)
	{
		m_table.insert(std::make_pair(str,FieldInfo(str,nFieldNum,tokenized,boost)));
	}

	

	//
	// –Ú¡–ªØ
	void Write(CBtStgCtrl *pStorage);
	void Read(CBtStgCtrl *pStorage);
	static void  getFieldCallback(const char* fieldName, char *value);
	static CFieldInfos *m_pthis;
	


};