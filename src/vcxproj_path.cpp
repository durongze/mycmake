#include "vcxproj_path.h"

#include "my_fs.h"

#include <iostream>

void DumpStrMap(std::map<std::string, int> &strMap, std::ostream &os)
{
	std::map<std::string, int>::iterator iterStrMap;
	for (iterStrMap = strMap.begin(); iterStrMap != strMap.end(); iterStrMap++)
	{
		os << "[" << iterStrMap->second << "]:" << std::endl;
	}
}

int RecordAllDirByFiles(std::map<std::string, int>& fileMap)
{
	std::map<std::string, int> dirMap;
	std::map<std::string, int>::iterator iterFileMap;
	std::string dirName;
	int dirIdx = 0;
	int dirLen = 0;
	for (iterFileMap = fileMap.begin(); iterFileMap != fileMap.end(); iterFileMap++, dirIdx++)
	{
		dirLen = iterFileMap->first.length() - iterFileMap->first.rfind(DIR_SEP);
		dirName = iterFileMap->first.substr(0, dirLen);
		dirMap[dirName] = dirIdx;
	}
	DumpStrMap(dirMap, std::cout);
	return 0;
}

int CompareFileNameByPath(std::map<std::string, int> &fileMap, std::string &filePath, std::string &fileExt)
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

int GenCMakeFileList(std::map<std::string, int>& fileMap)
{

	return 0;
}