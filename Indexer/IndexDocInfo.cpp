#include "IndexDocInfo.h"

void CIndexDocInfo::AddUntokenizedInfo(int fieldID,const std::string & value)
{
	FieldID2UnTokenizedMap::iterator it = m_UnTokenizedInfo.find(fieldID);
	if(it == m_UnTokenizedInfo.end())
		m_UnTokenizedInfo.insert(std::make_pair(fieldID,value));
}