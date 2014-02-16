#pragma  once 

#include <string>
#include "IndexExports.h"

class INDEXREXPORT_API CField
{
public:
	typedef enum { YES,NO,COMPRESS} StoreType;
	typedef enum { NOT,TOKENIZED,UN_TOKENIZED} IndexType;

	CField(const std::string &strName,const std::string &strfieldsData,float fboost= 1.0f)
		:name(strName),fieldsData(strfieldsData),boost(fboost)
	{
	}
	CField() : isCompressed(false),isStored(false),isIndexed(false),isTokenized(false)
	{

	}


	CField(const std::string &strName,const std::string &value,
		   StoreType stType,IndexType idxType):name(strName),fieldsData(value),boost(1.0f)
	{
		if (stType == StoreType::YES)
		{
			isStored = true;
			isCompressed = false;
		}
		else if (stType == StoreType::COMPRESS)
		{
			isStored = true;
			isCompressed = true;
		}
		else if (stType == StoreType::NO)
		{
			isStored = false;
			isCompressed = false;
		}
		else
		{
			assert(0);
		}

		if (idxType == IndexType::NOT)
		{
			isIndexed = false;
			isTokenized = false;
		}
		else if (idxType == IndexType::TOKENIZED)
		{
			isIndexed = true;
			isTokenized = true;
		}
		else if (idxType == IndexType::UN_TOKENIZED)
		{
			isIndexed = true;
			isTokenized = false;
		}
		else
		{
			assert(0);
		}
	}

	void  SetBoost(float fboost){ boost = fboost;}
	float GetBoost()      const          { return boost;}
	const std::string& GetName() const          { return name; }
	const std::string& GetValue()const          { return fieldsData;}
	bool       IsIndexed() const         { return isIndexed;}
	bool       IsCompressed() const      { return isCompressed;}
	bool       IsTokenized()  const      { return isTokenized;}
	bool       IsStored()     const      { return isStored;  }

private:
	 bool  isStored ;
	 bool  isIndexed;
	 bool  isTokenized ;
	 bool  isCompressed;

	 float boost ;
	 std::string name;
	 std::string fieldsData ;


};