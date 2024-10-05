#include "vcxproj_path.h"

#include "my_fs.h"

#include <iostream>
#include <regex>

std::string vsProjRootDir;

void SetVcxprojWorkDir(const std::string& rootDir)
{
	vsProjRootDir = rootDir + DIR_SEP;
	std::cout << "vsProjRootDir:" << vsProjRootDir << std::endl;
}

std::string GetVcxprojWorkDir()
{
	return vsProjRootDir;
}

void DumpStrMap(std::map<std::string, int> &strMap, const std::string& fileExt, std::ostream &os)
{
	std::map<std::string, int>::iterator iterStrMap;
	os << "ExtName:" << fileExt << std::endl;
	for (iterStrMap = strMap.begin(); iterStrMap != strMap.end(); iterStrMap++)
	{
		os << "[" << iterStrMap->second << "]:" << iterStrMap->first << std::endl;
	}
}

int RecordAllDirByFiles(const std::map<std::string, int>& fileMap, std::map<std::string, int>& dirMap, std::map<std::string, std::map<std::string, int> >& dirFile)
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
		dirFile[dirName][iterFileMap->first] = iterFileMap->second;
	}

	return 0;
}

int CompareFileNameByPath(const std::map<std::string, int> &fileMap, const std::string &filePath, const std::string &fileExt, std::map<std::string, int>& fileFilter, std::ostream& os)
{
	std::vector<std::string> files;
	std::vector<std::string>::iterator iterFile;
	std::map<std::string, int> fileAll;
	std::map<std::string, int>::iterator iterFileAll;
	std::map<std::string, int>::const_iterator iterFileMap;
	int ret = 0;
	int fileIdx = 0;
	int fileAllNo = 0;
	int fileMapNo = 0;

	std::string projRootDir = GetVcxprojWorkDir();
	ret = ReadFileByDir(projRootDir + filePath, fileExt, files);

	os << __FUNCTION__ << "  DIR:" << filePath << std::endl;
	for (iterFile = files.begin(); iterFile != files.end(); iterFile++) {
		fileAll[*iterFile] = fileIdx++;
	}

	for (iterFileAll = fileAll.begin(), iterFileMap = fileMap.cbegin() ; iterFileAll != fileAll.end() && iterFileMap != fileMap.cend(); )
	{
		std::string fileCur = iterFileAll->first.substr(projRootDir.length(), iterFileAll->first.length() - projRootDir.length());
		ret = strcmp(fileCur.c_str(), iterFileMap->first.c_str());
		if (ret > 0) {
			os << fileMapNo << " FileMap[" << iterFileMap->second << "]:" << iterFileMap->first << std::endl;
			iterFileMap++;
			fileMapNo++;
		} else if (ret < 0) {
			fileFilter[fileCur] = iterFileAll->second;
			os << fileAllNo << " FileAll[" << iterFileAll->second << "]:" << fileCur << std::endl;
			iterFileAll++;
			fileAllNo++;
		}
		else {
			iterFileAll++;
			iterFileMap++;
		}
	}
	while (iterFileAll != fileAll.end()) {
		std::string fileCur = iterFileAll->first.substr(projRootDir.length(), iterFileAll->first.length() - projRootDir.length());
		fileFilter[fileCur] = iterFileAll->second;
		os << "FileAll[" << iterFileAll->second << "]:" << iterFileAll->first << std::endl;
		iterFileAll++;
	}
	while (iterFileMap != fileMap.end()) {
		iterFileMap++;
	}

	return 0;
}

std::string ProjConfigToCMakeVarStr(const std::string& projCfg, const std::string& cmakeSuffix)
{
	std::string cmake_dir_var = "FILTER_";
	std::regex dir_cur_regex(DIR_CUR_REGEX);
	std::regex dir_sep_regex(DIR_SEP_REGEX);
	cmake_dir_var = cmake_dir_var + projCfg;
	cmake_dir_var = std::regex_replace(cmake_dir_var, dir_sep_regex, "_");
	cmake_dir_var = std::regex_replace(cmake_dir_var, dir_cur_regex, "");
	cmake_dir_var = cmake_dir_var + cmakeSuffix;
	cmake_dir_var = std::regex_replace(cmake_dir_var, std::regex("__"), "_");
	return ToUpperStr(cmake_dir_var);
}

int CheckFileList(const std::map<std::string, int>& fileMap, const std::string& fileExt, const std::string& cmakeVarSuffix, std::map<std::string, int> &dirMap, std::map<std::string, std::map<std::string, int> > &dirFileFilter)
{
	std::map<std::string, std::map<std::string, int> > dirFile;
	std::map<std::string, int>::const_iterator iterdirMap;
	RecordAllDirByFiles(fileMap, dirMap, dirFile);
	DumpStrMap(dirMap, fileExt, std::cout);
	for (iterdirMap = dirMap.cbegin(); iterdirMap != dirMap.cend(); iterdirMap++)
	{
		std::string curDirCMakeVar = ProjConfigToCMakeVarStr(iterdirMap->first, cmakeVarSuffix); 
		CompareFileNameByPath(dirFile[iterdirMap->first], iterdirMap->first, fileExt, dirFileFilter[curDirCMakeVar]);
	}

	return 0;
}