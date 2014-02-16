#include <windows.h>
#include "FileUtil.h"
using namespace std;

char Good_Split_Char = '\\';

string FileUtil::splitFileName(string const& path)
{
	string gPath = (path);
	basic_string<char>::size_type sz = gPath.rfind(Good_Split_Char);
	if (sz == basic_string<char>::npos)
	{
		sz = -1;
	}
	return gPath.substr(sz+1);
}
vector<string>	FileUtil::listDir(string const& dir,string const& filterStr)
{
	vector<string> files;
	string filePath = (dir);
	if(filterStr.size() != 0){
		filePath += "\\";
		filePath += filterStr;
	}
	else{
		filePath += "\\*.*";
	}
	WIN32_FIND_DATA findData = {0};
	HANDLE hFind = FindFirstFile(filePath.c_str(),&findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return files;
	}
	if(strcmp(findData.cFileName,".") != 0 && strcmp(findData.cFileName,"..") != 0)
		files.push_back(findData.cFileName);
	//left file
	while(FindNextFile(hFind,&findData)){
		if(strcmp(findData.cFileName,".") != 0 && strcmp(findData.cFileName,"..") != 0)
			files.push_back(findData.cFileName);
	}
	FindClose(hFind);
	return files;
}