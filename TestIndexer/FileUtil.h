#pragma	   once
#include <vector>
#include <string>

class FileUtil
{
public:
	//�ָ��ļ�·������Ϊ�ļ���d:\test\test2.txt=>test2.txt
	static std::string splitFileName(std::string const& path);	
	/*�õ�ָ���ļ����µ������ļ���filterStr��ͨ���������*.*,*.xml֮��*/
	static std::vector<std::string>	listDir(std::string const& dir,std::string const& filterStr = "*.*");
};