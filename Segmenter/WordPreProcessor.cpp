
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
	//word是由空白字符组成的,去掉所有空白后词为空
	//这时候返回false
	 if(!trimspaces(word,nLen))
		 return false;
	
	 //if it's an english word
	 //if is ,do stem
	 char ch = word[0];
	 //
	 //判断是否为英文单词
	 //只需简单判断是否第1个字母为英文字母
     if(('a'<= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z' ))
	 {
		 //do not convert "AND","NOT","OR" to lower case
		 //
		 //只有在搜索部分使用的时候,我们不需要转换"AND","NOT","OR"
		 //因为这些词是搜索语法的关键词,但是在建立索引的时候,这些词会被作为停止词处理
		 //掉,不需要存储到索引里面
		 //所以keepLogicalWord只是在搜索部分使用的时候才会为true

		 if(keeplogicalword)
		 {
			 if(strcmp(word,"AND") == 0 || strcmp(word,"NOT") == 0 || strcmp(word,"OR")== 0)
				 return true;
		 }
		 nLen = strlen(word);
		 //
		 //英文单词转换为小写
		 LowerString(word,nLen);
		 stemword(word,nLen);
	 }

	 //
	 //如果是停止词,返回false
	 if(m_filter.Accept(word))
		 return false;

	 return true;


}
