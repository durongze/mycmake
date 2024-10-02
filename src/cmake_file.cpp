#include "cmake_file.h"

#include <algorithm>
#include <cctype>

CMakeFile::CMakeFile(const char* filename):m_name(filename), m_ProjName("CMAKE_PROJ_NAME") {

}

CMakeFile::CMakeFile(const std::string& filename) :m_name(filename), m_ProjName("CMAKE_PROJ_NAME") {

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

std::string ToUpperStr(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c) { return std::toupper(c); });
	return str;
}

int CMakeFile::write(const std::map<std::string, int>& fileMap, int FileType)
{
	std::map<std::string, int>::const_iterator iterCppMap;

	if (fileMap.size() == 0) {
		return 0;
	}

	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << "set (";
	switch (FileType) {
		case FILE_TYPE_ASM:
			m_fs << ToUpperStr(m_ProjName) << "_ASM    ";
			break;
		case FILE_TYPE_CPP:
			m_fs << ToUpperStr(m_ProjName) << "_CPP    ";
			break;
		case FILE_TYPE_HPP:
			m_fs << ToUpperStr(m_ProjName) << "_HPP    ";
			break;
		case FILE_TYPE_RES:
			m_fs << ToUpperStr(m_ProjName) << "_RES    ";
			break;
		default:
			m_fs << ToUpperStr(m_ProjName) << "_DEF    ";
			break;
	}
	m_fs << std::endl;
	for (iterCppMap = fileMap.cbegin(); iterCppMap != fileMap.cend(); iterCppMap++) {
		m_fs << "    " << (iterCppMap->first) << std::endl;
	}
	m_fs << ")" << std::endl;
	m_fs.close();
	return 0;
}