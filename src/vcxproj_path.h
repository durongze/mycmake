#ifndef VCX_PROJ_PATH_H
#define VCX_PROJ_PATH_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

void DumpStrMap(std::map<std::string, int>& strMap, const std::string& fileExt, std::ostream& os);

int RecordAllDirByFiles(const std::map<std::string, int> &fileMap, std::map<std::string, int>& dirMap);

int CompareFileNameByPath(const std::map<std::string, int> &fileMap, const std::string &filePath, const std::string &fileExt, std::ostream& os = std::cout);

int CheckFileList(const std::map<std::string, int>& fileMap, const std::string& fileExt);

#endif