#pragma  once 

#include "Global.h"

class CIndexInputBuffer 
{
public:
	CIndexInputBuffer(char *pBuffer)
	{
		m_pBuffer = pBuffer;
		m_nBufferPointer = 0;
	}
	char *m_pBuffer;
	UINT  m_nBufferPointer;


	BYTE  ReadByte()
	{
         return m_pBuffer[m_nBufferPointer++];
	}
	
	void  ReadBytes(BYTE b[], int offset, int len)
	{
		 for(int i = 0; i < len;++i)
          b[i] = m_pBuffer[m_nBufferPointer+offset];
		 m_nBufferPointer += offset;

	}
    
	int ReadInt()
	{
		return ((ReadByte() & 0xFF) << 24) | ((ReadByte() & 0xFF) << 16) | ((ReadByte() & 0xFF) << 8) | (ReadByte() & 0xFF);
	}

    UINT ReadVInt()
	{
		BYTE b = ReadByte();
		UINT i = b & 0x7F;
		for (int shift = 7; (b & 0x80) != 0; shift += 7)
		{
			b = ReadByte();
			i |= (b & 0x7F) << shift;
		}
		return i;
	}
    
	std::string ReadString()
	{
		int length = ReadVInt();
		std::string result;
		for(int i = 0; i < length;++i)
			result += ReadByte();

		return result;
	}
	
    void  ReadChars(char *buffer, int start, int length)
	{
		int end = start + length;
		for (int i = start; i < end; i++)
		{
			BYTE b = ReadByte();
			buffer[i] = b;
		}
	}

	float  ReadFloat()
	{
		float f;
		char s[sizeof(float)+1];
		ReadChars(s,0,sizeof(float));
		memcpy(&f,s,sizeof(float));
		return f;
	}

    long  Length() 
	{
		return m_nBufferPointer;
	}

	void  Seek(long pos) 
	{
		m_nBufferPointer = pos;
	}
	void Reset()
	{
		m_nBufferPointer = 0;
	}

};