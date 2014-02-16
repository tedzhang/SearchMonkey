#include "FieldInfos.h"
#include "BtStgCtrl.h"
#include "IndexOutput.h"
#include "IndexInput.h"

CFieldInfos* CFieldInfos::m_pthis = NULL;

void CFieldInfos::Write(CBtStgCtrl *pStorage)
{
	pStorage->beginLock();

	COutputBuffer buffer;

	FieldName2FieldInfoTable::iterator it = m_table.begin();
	for(;it != m_table.end();++it)
	{
		FieldInfo &fi = it->second;
		std::string strFieldName = it->first;

		BYTE isTokenize = fi.isTokenized?1:0;
		buffer.WriteVInt(fi.fieldNumber);
		buffer.WriteByte(isTokenize);
		buffer.WriteFloat(fi.fBoost);
		

		if(0 != pStorage->SetField(strFieldName.c_str(),buffer.GetBuffer(),buffer.Length()))
			return;
		buffer.Flush();
	}
	pStorage->endLock();
}

void CFieldInfos::Read(CBtStgCtrl *pStorage)
{
 
	pStorage->GetAllFields(getFieldCallback);

}

void  CFieldInfos::getFieldCallback(const char* fieldName, char *value)
{
	 CIndexInputBuffer buffer(value);
	 UINT nFieldNum = buffer.ReadVInt();
	 bool isTokenized = buffer.ReadByte()== 1?true:false;
	 float fBoost     = buffer.ReadFloat();

     m_pthis->Add(fieldName,nFieldNum,isTokenized,fBoost);
}