#ifndef VCX_PROJ_PATH_H
#define VCX_PROJ_PATH_H

#include <string>
#include <map>
#include <vector>

void DumpStrMap(std::map<std::string, int>& strMap, std::ostream& os);

int RecordAllDirByFiles(std::map<std::string, int> &fileMap);

int CompareFileNameByPath(std::map<std::string, int> &fileMap, std::string &filePath, std::string &fileExt);

int GenCMakeFileList(std::map<std::string, int>& fileMap);

#endif