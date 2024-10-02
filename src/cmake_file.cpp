#include "cmake_file.h"

CMakeFile::CMakeFile(const char* filename):m_name(filename) {

}

CMakeFile::CMakeFile(const std::string& filename) :m_name(filename) {

}

CMakeFile::~CMakeFile() {

}

int CMakeFile::create()
{
	m_fs.open(m_name, std::ios::out | std::ios::trunc);
	m_fs.close();
	return 0;
}

int CMakeFile::write(const std::string& txt)
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << txt << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::write(const std::map<std::string, int>& cppMap)
{
	std::map<std::string, int>::const_iterator iterCppMap;
	m_fs.open(m_name, std::ios::out | std::ios::app);
	for (iterCppMap = cppMap.cbegin(); iterCppMap != cppMap.cend(); iterCppMap++) {
		m_fs << (iterCppMap->first) << std::endl;
	}
	m_fs.close();
	return 0;
}