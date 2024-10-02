#ifndef CMAKE_FILE_H
#define CMAKE_FILE_H

#include <fstream>
#include <string>

#include <strstream>
#include <map>

#define CMAKE_FIILE_TO_STR(X)   #X

class CMakeFile{
public:
	enum FileType {
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
private:
	std::fstream m_fs;
    std::string  m_name;
	std::string m_ProjName;
};

#endif