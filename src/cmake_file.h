#ifndef CMAKE_FILE_H
#define CMAKE_FILE_H

#include <fstream>
#include <string>

class CMakeFile{
public:
	CMakeFile(const char* filename);
	CMakeFile(const std::string& filename);
	~CMakeFile();
	int create();
	int write(const std::string& txt);
private:
	std::fstream m_fs;
    std::string  m_name;
};

#endif