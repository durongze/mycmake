#include "vcxproj_path.h"

#include "my_fs.h"
#include <string>
#include <map>
#include <vector>

int CompareFileNameByPath(std::map<std::string, int> fileMap, std::string filePath, std::string fileExt)
{
	std::vector<std::string> files;
	std::vector<std::string>::iterator iterFile;
	std::map<std::string, int> fileAll;
	std::map<std::string, int>::iterator iterFileAll;
	std::map<std::string, int>::iterator iterFileMap;
	int ret = 0;
	int fileIdx = 0;
	ret = ReadFileByDir(filePath, fileExt, files);

	for (iterFile = files.begin(); iterFile != files.end(); iterFile++, fileIdx++) {
		fileAll[*iterFile] = fileIdx;
	}

	for (iterFileAll = fileAll.begin(), iterFileMap = fileMap.begin() ; iterFileAll != fileAll.end() && iterFileMap != fileMap.end(); )
	{
		ret = strcmp(iterFileAll->first.c_str(), iterFileMap->first.c_str());
		if (ret > 0) {
			iterFileMap++;
		} else if (ret < 0) {
			iterFileAll++;
		}
		else {
			iterFileAll++;
			iterFileMap++;
		}
	}
	while (iterFileAll != fileAll.end()) {
		iterFileAll++;
	}
	while (iterFileMap != fileMap.end()) {
		iterFileMap++;
	}

	return 0;
}