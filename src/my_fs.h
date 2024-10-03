#ifndef __MY_FS_H__
#define __MY_FS_H__

#include <stdio.h>
#include <string>
#include <vector>

#ifdef WIN32

#define DIR_SEP "\\"
#define DIR_SEP_REGEX "\\\\"
#define DIR_CUR_REGEX "\\."

#ifdef MY_MRP_IO
#define MRP_API __declspec(dllexport)
#else
#define MRP_API __declspec(dllimport)
#endif
#else

#define DIR_SEP "/"

#define MRP_API
#endif

std::string GetAbsPathByName(const std::string& fileName);
int ReadFileByDir(std::string dirName, std::string fileExt, std::vector<std::string>& files);

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

MRP_API int RenameMrpFile(const char* filename, FILE* fp_log, FILE* fp_bat);
MRP_API int RenameMrpFileByDir(const char* dirname);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif