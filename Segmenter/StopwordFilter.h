#pragma  once 
#include <hash_set>
#include <string.h>
#include <string>

//
//
//ֹͣ�ʹ���
//

class CStopwordFilter
{
public:
	//true : if it's a stop word
	//
	//�жϴ�word�Ƿ�Ϊһ��ֹͣ��
	static bool Accept(const char *word);

	//
	//����ֹͣ���ļ�,ֹͣ���ļ��ͺ��Ĵʵ��ļ���ͬһ��Ŀ¼��
	//�ļ���Ϊ"stopwords.txt"
	static bool Load(const char* stopwordfile);

private:

	struct strless
	{
		bool operator()(const char *str1,const char *str2)const
		{
			return strcmp(str1,str2) < 0;
		}
	};
	typedef stdext::hash_set<const char*,stdext::hash_compare<const char*,strless> > STOPWORDSLIST;
	static STOPWORDSLIST m_list;
};

inline bool CStopwordFilter::Accept(const char *word)
{
	bool ret = false;
	if(m_list.find(word) != m_list.end())
		ret = true;
	return ret;
}
