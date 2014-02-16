#pragma	   once
#include <vector>
#include <string>

class FileUtil
{
public:
	//分割文件路径，成为文件名d:\test\test2.txt=>test2.txt
	static std::string splitFileName(std::string const& path);	
	/*得到指定文件夹下的所有文件，filterStr是通配符，比如*.*,*.xml之类*/
	static std::vector<std::string>	listDir(std::string const& dir,std::string const& filterStr = "*.*");
};