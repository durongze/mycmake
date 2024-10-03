#include "cmake_file.h"

#include <algorithm>
#include <cctype>
#include <strstream>


#include "my_fs.h"

CMakeVar::CMakeVar(const char* varName) :m_varName(varName){

}

CMakeVar::CMakeVar(const std::string& varName) :m_varName(varName) {

}

CMakeVar::~CMakeVar() {

}

std::string CMakeVar::Name()
{
	std::string str;
	str += m_varName;
	return str;
}

std::string CMakeVar::Value()
{
	std::string str;
	str += "${";
	str += m_varName;
	str += "}";
	return str;
}

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

std::string ToLowerStr(std::string str)
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
			m_fs << GetProjAsmList().Name();
			break;
		case FILE_TYPE_CPP:
			m_fs << GetProjSrcList().Name();
			break;
		case FILE_TYPE_HPP:
			m_fs << GetProjIncList().Name();
			break;
		case FILE_TYPE_RES:
			m_fs << GetProjResList().Name();
			break;
		case FILE_TYPE_DEF:
			m_fs << GetProjDefList().Name();
			break;
		default:
			m_fs << CMAKE_FIILE_TO_STR(FILE_TYPE_DEF);
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

int CMakeFile::writeCMakeLists()
{
	writeRootDir();
	writeFileList();
	writeStaticLib();
	writeSharedLib();
	writeApp();
	return 0;
}

int CMakeFile::writeRootDir()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << "set(" << GetProjBinDir().Name() << " ${CMAKE_CURRENT_BINARY_DIR})                  " << std::endl;
	m_fs << "set(" << GetProjTopDir().Name() << " ${CMAKE_CURRENT_SOURCE_DIR}) #CMAKE_SOURCE_DIR" << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeFileList()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << GenerateFileFuncLine(GetProjAsmList().Name(), GetProjTopDir().Value(), "src", ".asm*");
	m_fs << GenerateFileFuncLine(GetProjIncList().Name(), GetProjTopDir().Value(), "src", ".h*");
	m_fs << GenerateFileFuncLine(GetProjSrcList().Name(), GetProjTopDir().Value(), "src", ".c*");
	m_fs << GenerateFileFuncLine(GetProjAppList().Name(), GetProjTopDir().Value(), "test", ".c*");
	m_fs << "list        (REMOVE_ITEM  " << GetProjSrcList().Name() << "                 " << GetProjAppList().Value() << "    )" << std::endl;
	m_fs << "source_group(TREE         " << GetProjTopDir().Value() << "     FILES       " << GetProjSrcList().Value() << "    )" << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeStaticLib()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << "set                       (  " << GetProjStaticLib().Name() << "               " << ToLowerStr(GetProjStaticLib().Name()) << "          )" << std::endl;
	m_fs << "add_library               (" << GetProjStaticLib().Value() << "   STATIC     " << GetProjSrcList().Value() << "    )" << std::endl;
	m_fs << "target_compile_definitions(" << GetProjStaticLib().Value() << "   PRIVATE    HAVE_CONFIG_H          " << GetProjDefList().Value() << ")" << std::endl;
	m_fs << "target_include_directories(" << GetProjStaticLib().Value() << "   PUBLIC     ./src/                 )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjStaticLib().Value() << "   PUBLIC     ./lib/                 )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjStaticLib().Value() << "              ${dep_libs}               " << GetProjAppDepLib().Value() << "           )" << std::endl;
	m_fs << "set_target_properties     (" << GetProjStaticLib().Value() << "   PROPERTIES CLEAN_DIRECT_OUTPUT 1     OUTPUT_NAME " << ToLowerStr(GetProjStaticLib().Name()) << "  )" << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeSharedLib()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << "set                       (  " << GetProjSharedLib().Name() << "               " << ToLowerStr(GetProjSharedLib().Name()) << "          )" << std::endl;
	m_fs << "add_library               (" << GetProjSharedLib().Value() << "   SHARED     " << GetProjSrcList().Value() << "    )" << std::endl;
	m_fs << "target_compile_definitions(" << GetProjSharedLib().Value() << "   PRIVATE    HAVE_CONFIG_H          " << GetProjDefList().Value() << ")" << std::endl;
	m_fs << "target_include_directories(" << GetProjSharedLib().Value() << "   PUBLIC     ./src/                 )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjSharedLib().Value() << "   PUBLIC     ./lib/                 )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjSharedLib().Value() << "              ${dep_libs}               " << GetProjAppDepLib().Value() << "           )" << std::endl;
	m_fs << "set_target_properties     (" << GetProjSharedLib().Value() << "   PROPERTIES CLEAN_DIRECT_OUTPUT 1     OUTPUT_NAME " << ToLowerStr(GetProjSharedLib().Name()) << "  )" << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeApp()
{
		m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << "set                       (  " << GetProjAppName().Name() << "                " << ToLowerStr(GetProjAppName().Name()) << "        )" << std::endl;
	m_fs << "add_executable            (" << GetProjAppName().Value() << "               " << GetProjSrcList().Value() << "        " << GetProjAppList().Value() << "            )" << std::endl;
	m_fs << "target_compile_definitions(" << GetProjAppName().Value() << "   PRIVATE     __SIZE_T_DEFINED	       " << GetProjAppDefs().Value() << "               )" << std::endl;
	m_fs << "target_include_directories(" << GetProjAppName().Value() << "   PUBLIC      ./src/                   )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjAppName().Value() << "   PUBLIC      ./lib/                   )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjAppName().Value() << "               " << GetProjStaticLib().Value() << "         )" << std::endl;
	m_fs.close();
	return 0;
}

CMakeVar  CMakeFile::GetProjName() {
	std::string str;
	str += m_ProjName.Name();
	return str;
}

CMakeVar  CMakeFile::GetProjBinDir() {
	std::string str;
	str += m_ProjName.Name();
	str += "_BIN_SRC_DIR";
	return str;
}

CMakeVar  CMakeFile::GetProjTopDir() {
	std::string str;
	str += m_ProjName.Name();
	str += "_TOP_SRC_DIR";
	return str;
}

CMakeVar  CMakeFile::GetProjAsmList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_ASM_LIST";
	return str;
}

CMakeVar  CMakeFile::GetProjHdrList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_HDR_LIST";
	return str;
}

CMakeVar  CMakeFile::GetProjSrcList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_SRC_LIST";
	return str;
}

CMakeVar  CMakeFile::GetProjAppList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_APP_LIST";
	return str;
}

CMakeVar CMakeFile::GetProjIncList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_INC_LIST";
	return str;
}

CMakeVar CMakeFile::GetProjLibList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_LIB_LIST";
	return str;
}

CMakeVar  CMakeFile::GetProjResList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_RES_LIST";
	return str;
}

CMakeVar  CMakeFile::GetProjDefList() {
	std::string str;
	str += m_ProjName.Name();
	str += "_DEF_LIST";
	return str;
}

CMakeVar  CMakeFile::GetProjStaticLib() {
	std::string str;
	str += m_ProjName.Name();
	str += "_STATIC";
	return str;
}

CMakeVar  CMakeFile::GetProjSharedLib() {
	std::string str;
	str += m_ProjName.Name();
	str += "_SHARED";
	return str;
}

CMakeVar  CMakeFile::GetProjAppName() {
	std::string str;
	str += m_ProjName.Name();
	str += "_APP";
	return str;
}

CMakeVar  CMakeFile::GetProjAppDepLib() {
	std::string str;
	str += m_ProjName.Name();
	str += "_DEP_LIBS";
	return str;
}

CMakeVar  CMakeFile::GetProjAppDefs() {
	std::string str;
	str += m_ProjName.Name();
	str += "_APP_DEFS";
	return str;
}

std::string CMakeFile::GenerateFileFuncLine(const std::string &varName, const std::string &topDir, const std::vector<std::string> &subDirList, const std::string &extName)
{
	std::strstream fileFuncLine;
	std::vector<std::string>::const_iterator iterDir;
	fileFuncLine << "file        (GLOB           " << varName << "         RELATIVE   " << topDir << "  ";
	for (iterDir = subDirList.cbegin(); iterDir != subDirList.cend(); iterDir++) {
		fileFuncLine << topDir << DIR_SEP << *iterDir << DIR_SEP << "*" << (extName.at(0) == '.' ? extName : std::string(".") + extName);
	}
	fileFuncLine  << "      )" << std::endl;
	return fileFuncLine.str();
}

std::string CMakeFile::GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::string& subDir, const std::string& extName)
{
	std::strstream fileFuncLine;
	std::vector<std::string>::const_iterator iterDir;
	fileFuncLine << "file        (GLOB           " << varName << "         RELATIVE   " << topDir << "  ";

		fileFuncLine << topDir << DIR_SEP << subDir << DIR_SEP << "*" << (extName.at(0) == '.' ? extName : std::string(".") + extName);

	fileFuncLine << "      )" << std::endl;
	return fileFuncLine.str();
}