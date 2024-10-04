#ifndef VCX_PROJ_PATH_H
#define VCX_PROJ_PATH_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

void SetVcxprojWorkDir(const std::string& rootDir);

void DumpStrMap(std::map<std::string, int>& strMap, const std::string& fileExt, std::ostream& os);

int RecordAllDirByFiles(const std::map<std::string, int> &fileMap, std::map<std::string, int>& dirMap, std::map<std::string, std::map<std::string, int> >& dirFile);

int CompareFileNameByPath(const std::map<std::string, int> &fileMap, const std::string &filePath, const std::string &fileExt, std::map<std::string, int>& fileFilter, std::ostream& os = std::cout);

std::string ProjConfigToCMakeVarStr(const std::string& projCfg, const std::string& cmakeSuffix);

int CheckFileList(const std::map<std::string, int>& fileMap, const std::string& fileExt, const std::string& cmakeSuffix, std::map<std::string, int> &dirMap, std::map<std::string, std::map<std::string, int> > & dirFileFilter);

#endif