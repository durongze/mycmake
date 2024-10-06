#ifndef CMAKE_FILE_H
#define CMAKE_FILE_H

#include <fstream>
#include <string>

#include <strstream>
#include <map>
#include <vector>

#define CMAKE_FIILE_TO_STR(X)   #X
#define CMAKE_DIR_SEP           "/"

void SetCMakeWorkDir(const std::string& rootDir);

std::string GetCMakeWorkDir();

class CMakeVar {
public:
	CMakeVar(const char* varName);
	CMakeVar(const std::string& varName);
	~CMakeVar();
	std::string Name()const;
	std::string Value()const;
public:
	friend bool operator<(const CMakeVar& my, const CMakeVar& other) ;
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
	CMakeFile(const char* filename, const char* projname);
	CMakeFile(const std::string& filename, const std::string& projname);
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
	CMakeVar GetProjIncDirSet();
	CMakeVar GetProjLibDirSet();
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
	std::string GenPlatformCMakeVar(const std::string& platform, const std::string& varType);
	int writeLibList(const std::string& libList, const std::string& platform);
	int writeLibraryLists();
	int writeOptList(const std::string& optionList, const std::string& platform, int FileType);
	int writeOptionLists();
	int writeFilterList(std::map<std::string, std::map<std::string, int> >& allDirFilter);
	int writeSetList(const CMakeVar& cmakeVar, const std::string& cmakeValue);
	int writeSetList(const CMakeVar& cmakeVar, const std::map<std::string, int>& fileMap);
	int writeRootDir();
	int writeFileList(const CMakeVar& cmakeVar, const std::map<std::string, int>& fileMap);
	int writeFileList(const std::map<std::string, int>& dirMap);
	int writeStaticLib();
	int writeSharedLib();
	int writeApp();
	int writeInstall();
	std::string GetCMakeDirVarSuffix(int FileType);

	std::string GetExtName(int FileType);

	std::string GetCMakeVarName(int FileType);
private:
	std::string GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::map<std::string, int>& fileList);
	std::string GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::map<std::string, int> &dirList, const std::string& extName);
	std::string GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::string& subDir, const std::string& extName);
private:
	std::string GetCMakeVarRealPath(const std::string& cmakeVarPath);
	std::string GenerateRelatPath(const std::string& topDir, const std::string& subDir, const std::string& baseDir);
private:
	std::fstream m_fs;
    std::string  m_name;
	
	CMakeVar m_ProjName;
	
	std::map<CMakeVar, std::string> m_platformOpts;
	std::map<CMakeVar, std::string> m_platformLibs;
	std::map<std::string, int> m_dirMap;
	std::map<std::string, std::map<std::string, int> > m_allDirFilter;
};

#endif