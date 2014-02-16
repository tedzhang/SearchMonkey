#pragma  once 
#include <hash_set>
#include <string.h>
#include <string>

//
//
//停止词过滤
//

class CStopwordFilter
{
public:
	//true : if it's a stop word
	//
	//判断词word是否为一个停止词
	static bool Accept(const char *word);

	//
	//加载停止词文件,停止词文件和核心词典文件在同一个目录下
	//文件名为"stopwords.txt"
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
