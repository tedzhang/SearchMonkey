
#include "WordPreProcessor.h"
#include "Global.h"

inline bool  trimspaces(char *s,int len)
{
	char word[MAX_WORD_SIZE];
	word[0] = 0;

	unsigned char ch;

	int j  = 0;

	for(int i = 0; i < len;++i)
	{
		ch = s[i];
		if(ch == 32) 
			continue;
		if(ch== 161 &&s[i+1] == 161) //chinese white space
		{
           i++;continue;
		}
		word[j++] = ch;
	}
	if(j == 0)
	{
		s[0] = 0;//all are blank
		return false;
	}
	else
	{
		word[j] = 0;
		strncpy(s,word,MAX_WORD_SIZE);
	}
	return true;

}

inline void LowerString(char *s,int len)
{
	for(int i = 0; i < len;++i)
		s[i] = tolower(s[i]);
}

CWordPreProcessor::CWordPreProcessor()
{
	
}

bool CWordPreProcessor::LoadStopwordsList(const char* path)
{
	return m_filter.Load(path);
}
//
//
//return true,if we should accept it

bool CWordPreProcessor::Process(char* word,int nLen,bool keeplogicalword)
{
     //remove blank in the left and right
	 //
	 
	//
	//word���ɿհ��ַ���ɵ�,ȥ�����пհ׺��Ϊ��
	//��ʱ�򷵻�false
	 if(!trimspaces(word,nLen))
		 return false;
	
	 //if it's an english word
	 //if is ,do stem
	 char ch = word[0];
	 //
	 //�ж��Ƿ�ΪӢ�ĵ���
	 //ֻ����ж��Ƿ��1����ĸΪӢ����ĸ
     if(('a'<= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z' ))
	 {
		 //do not convert "AND","NOT","OR" to lower case
		 //
		 //ֻ������������ʹ�õ�ʱ��,���ǲ���Ҫת��"AND","NOT","OR"
		 //��Ϊ��Щ���������﷨�Ĺؼ���,�����ڽ���������ʱ��,��Щ�ʻᱻ��Ϊֹͣ�ʴ���
		 //��,����Ҫ�洢����������
		 //����keepLogicalWordֻ������������ʹ�õ�ʱ��Ż�Ϊtrue

		 if(keeplogicalword)
		 {
			 if(strcmp(word,"AND") == 0 || strcmp(word,"NOT") == 0 || strcmp(word,"OR")== 0)
				 return true;
		 }
		 nLen = strlen(word);
		 //
		 //Ӣ�ĵ���ת��ΪСд
		 LowerString(word,nLen);
		 stemword(word,nLen);
	 }

	 //
	 //�����ֹͣ��,����false
	 if(m_filter.Accept(word))
		 return false;

	 return true;


}
