#ifndef CMAKE_FILE_H
#define CMAKE_FILE_H

#include <fstream>
#include <string>

#include <strstream>
#include <map>
#include <vector>

#define CMAKE_FIILE_TO_STR(X)   #X

class CMakeVar {
public:
	CMakeVar(const char* varName);
	CMakeVar(const std::string& varName);
	~CMakeVar();
	std::string Name()const;
	std::string Value()const;
private:
	std::string m_varName;
};

class CMakeFile{
public:
	enum FileType {
		FILE_TYPE_NONE,
		FILE_TYPE_DEF,
		FILE_TYPE_ASM,
		FILE_TYPE_CPP,
		FILE_TYPE_HPP,
		FILE_TYPE_RES
	};
public:
	CMakeFile(const char* filename);
	CMakeFile(const std::string& filename);
	~CMakeFile();
	int create();
	int write(const std::string& txt);
	int write(const std::map<std::string, int>& fileMap,  int fileType = 0);
	int write();
	int CMakeFile::writeCMakeLists();
private:
	CMakeVar GetProjName();
	CMakeVar GetProjBinDir();
	CMakeVar GetProjTopDir();
	CMakeVar GetProjAsmList();
	CMakeVar GetProjHdrList();
	CMakeVar GetProjSrcList();
	CMakeVar GetProjAppList();
	CMakeVar GetProjIncList();
	CMakeVar GetProjLibList();
	CMakeVar GetProjResList();
	CMakeVar GetProjDefList();
	CMakeVar GetProjStaticLib();
	CMakeVar GetProjSharedLib();
	CMakeVar GetProjAppName();
	CMakeVar GetProjAppDepLib();
	CMakeVar GetProjAppDefs();
public:
	int writeSetList(const CMakeVar& cmakeVar, const std::map<std::string, int>& fileMap);
	int writeRootDir();
	int writeFileList();
	int writeStaticLib();
	int writeSharedLib();
	int writeApp();
	
	std::string CMakeFile::GetCMakeDirVarSuffix(int FileType);

	std::string CMakeFile::GetExtName(int FileType);

	std::string CMakeFile::GetCMakeVarName(int FileType);
private:
	std::string GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::vector<std::string> &dirList, const std::string& extName);
	std::string GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::string& subDir, const std::string& extName);
private:
	std::fstream m_fs;
    std::string  m_name;
	CMakeVar m_ProjName;
};

#endif