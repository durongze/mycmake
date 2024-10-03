#include "vcxproj_path.h"

#include "my_fs.h"

#include <iostream>

void DumpStrMap(std::map<std::string, int> &strMap, const std::string& fileExt, std::ostream &os)
{
	std::map<std::string, int>::iterator iterStrMap;
	os << "ExtName:" << fileExt << std::endl;
	for (iterStrMap = strMap.begin(); iterStrMap != strMap.end(); iterStrMap++)
	{
		os << "[" << iterStrMap->second << "]:" << iterStrMap->first << std::endl;
	}
}

int RecordAllDirByFiles(const std::map<std::string, int>& fileMap, std::map<std::string, int>& dirMap)
{
	std::map<std::string, int>::const_iterator iterFileMap;
	std::string dirName;
	int dirIdx = 0;
	int dirLen = 0;
	for (iterFileMap = fileMap.cbegin(); iterFileMap != fileMap.cend(); iterFileMap++, dirIdx++)
	{
		dirLen = iterFileMap->first.rfind(DIR_SEP);
		dirName = iterFileMap->first.substr(0, dirLen);
		dirMap[dirName] = dirIdx;
	}

	return 0;
}

int CompareFileNameByPath(const std::map<std::string, int> &fileMap, const std::string &filePath, const std::string &fileExt, std::ostream& os)
{
	std::vector<std::string> files;
	std::vector<std::string>::iterator iterFile;
	std::map<std::string, int> fileAll;
	std::map<std::string, int>::iterator iterFileAll;
	std::map<std::string, int>::const_iterator iterFileMap;
	int ret = 0;
	int fileIdx = 0;
	std::string dbgRootDir = "E:\\code\\my\\ffmpeg\\FFmpeg\\SMP\\";
	ret = ReadFileByDir(dbgRootDir + filePath, fileExt, files);

	for (iterFile = files.begin(); iterFile != files.end(); iterFile++) {
		fileAll[*iterFile] = fileIdx++;
	}

	for (iterFileAll = fileAll.begin(), iterFileMap = fileMap.cbegin() ; iterFileAll != fileAll.end() && iterFileMap != fileMap.cend(); )
	{
		std::string fileCur = iterFileAll->first.substr(dbgRootDir.length(), iterFileAll->first.length() - dbgRootDir.length());
		ret = strcmp(fileCur.c_str(), iterFileMap->first.c_str());
		if (ret > 0) {
			os << "FileMap[" << iterFileMap->second << "]:" << iterFileMap->first << std::endl;
			iterFileMap++;
		} else if (ret < 0) {
			os << "FileAll[" << iterFileAll->second << "]:" << iterFileAll->first << std::endl;
			iterFileAll++;
		}
		else {
			iterFileAll++;
			iterFileMap++;
		}
	}
	while (iterFileAll != fileAll.end()) {
		os << "FileAll[" << iterFileAll->second << "]:" << iterFileAll->first << std::endl;
		iterFileAll++;
	}
	while (iterFileMap != fileMap.end()) {
		iterFileMap++;
	}

	return 0;
}

int CheckFileList(const std::map<std::string, int>& fileMap, const std::string& fileExt)
{
	std::map<std::string, int> dirMap;
	std::map<std::string, int>::const_iterator iterdirMap;
	RecordAllDirByFiles(fileMap, dirMap);
	DumpStrMap(dirMap, fileExt, std::cout);
	for (iterdirMap = dirMap.cbegin(); iterdirMap != dirMap.cend(); iterdirMap++)
	{
		CompareFileNameByPath(fileMap, iterdirMap->first, fileExt);
	}

	return 0;
}