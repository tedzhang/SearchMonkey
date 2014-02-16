#pragma  once 
#include<vector>
#include "Global.h"

class COutputBuffer
{

private:
	
	BYTE m_buffer[MAX_INDEXDATA_SIZE];
	UINT   m_nBufferPointer;
	static const int ReserveBufferSize = 2000;
public:

	COutputBuffer():m_nBufferPointer(0)
	{ 
	}
	void  WriteByte(BYTE b)
	{
		m_buffer[m_nBufferPointer++] = b;
	}

	void  WriteBytes(BYTE b[], int length)
	{
		for(int i = 0; i < length;++i)
		{
			m_buffer[m_nBufferPointer++]= b[i];
		}
		m_nBufferPointer += length;
	}

	void  WriteInt(int i)
	{
		WriteByte((BYTE) (i >> 24));
		WriteByte((BYTE) (i >> 16));
		WriteByte((BYTE) (i >> 8));
		WriteByte((BYTE) i);
	}

	void  WriteVInt(UINT i)
	{
		while ((i & ~ 0x7F) != 0)
		{
			WriteByte((BYTE) ((i & 0x7f) | 0x80));
			i = (i>>7);
		}
		WriteByte((BYTE) i);
	}

	void  WriteString(const std::string &s)
	{
		int length = s.length();
		WriteVInt(length);
		WriteChars(s.c_str(), 0, length);
	}

	void WriteString(const char* str)
	{
		int length = strlen(str);
		WriteVInt(length);
		WriteChars(str,0,length);
	}
	void WriteFloat(float f)
	{
		char s[sizeof(float)+1];
		memcpy(s,&f,sizeof(float));
		WriteChars(s,0,sizeof(float));
	}

	void  WriteChars(const char* s, size_t start, size_t length)
	{
		size_t end = start + length;
		for (size_t i = start; i < end; i++)
		{
			char ch = s[i];
			WriteByte((BYTE)ch);
		}
	}

	void  Flush() 
	{
		m_nBufferPointer = 0;
	}

	void  Seek(long pos) 
	{
		m_nBufferPointer = pos;
	}

	UINT  Length() 
	{
		return m_nBufferPointer;
	}

	const char *GetBuffer()
	{
		return (const char*)m_buffer;
	}

};