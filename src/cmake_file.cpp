#include "cmake_file.h"

#include <algorithm>
#include <cctype>
#include <strstream>
#include <regex>

#include "vcxproj_path.h"
#include "my_fs.h"

CMakeVar::CMakeVar(const char* varName) :m_varName(varName){

}

CMakeVar::CMakeVar(const std::string& varName) :m_varName(varName) {

}

CMakeVar::~CMakeVar() {

}

std::string CMakeVar::Name()const
{
	std::string str;
	str += m_varName;
	return str;
}

std::string CMakeVar::Value()const
{
	std::string str;
	str += "${";
	str += m_varName;
	str += "}";
	return str;
}

bool operator<(const CMakeVar& my, const CMakeVar& other)
{
	return my.m_varName < other.m_varName;
}

CMakeFile::CMakeFile(const char* filename, const char* projname):m_name(filename), m_ProjName(projname) {

}

CMakeFile::CMakeFile(const std::string& filename, const std::string& projname) :m_name(filename), m_ProjName(projname) {

}

CMakeFile::~CMakeFile() {

}

int CMakeFile::create()
{
	m_fs.open(m_name, std::ios::out | std::ios::trunc);
	m_fs.close();
    writeRootDir();
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
		[](unsigned char c) { return std::tolower(c); });
	return str;
}

std::string CMakeFile::GetCMakeDirVarSuffix(int FileType)
{
	std::strstream fs;
	switch (FileType) {
	case FILE_TYPE_ASM:
		fs << "_ASMS";
		break;
	case FILE_TYPE_CPP:
		fs << "_CPPS";
		break;
	case FILE_TYPE_HPP:
		fs << "_HPPS";
		break;
	case FILE_TYPE_RES:
		fs << "_RES";
		break;
	case FILE_TYPE_DEF:
		fs << "_DEFAULT";
		break;
	default:
		fs << "_ERROR";
		break;
	}
	fs << std::ends;
	return fs.str();
}

std::string CMakeFile::GetExtName(int FileType)
{
	std::strstream fs;
	switch (FileType) {
	case FILE_TYPE_ASM:
		fs << ".asm";
		break;
	case FILE_TYPE_CPP:
		fs << ".c*";
		break;
	case FILE_TYPE_HPP:
		fs << ".h";
		break;
	case FILE_TYPE_RES:
		fs << ".rc";
		break;
	case FILE_TYPE_DEF:
		fs << ".*";
		break;
	default:
		fs << ".*";
		break;
	}
	fs << std::ends;
	return fs.str();
}

std::string CMakeFile::GetCMakeVarName(int FileType)
{
	std::strstream fs;
	switch (FileType) {
	case FILE_TYPE_ASM:
		fs << GetProjAsmList().Name();
		break;
	case FILE_TYPE_CPP:
		fs << GetProjSrcList().Name();
		break;
	case FILE_TYPE_HPP:
		fs << GetProjIncList().Name();
		break;
	case FILE_TYPE_RES:
		fs << GetProjResList().Name();
		break;
	case FILE_TYPE_DEF:
		fs << GetProjDefList().Name();
		break;
	default:
		fs << CMAKE_FIILE_TO_STR(FILE_TYPE_DEF);
		break;
	}
	fs << std::ends;
	return fs.str();
}

int CMakeFile::writeOptList(const std::string& optionList, const std::string &platform)
{
	std::string optCMakeList;
	std::string optCMakeVar;
	std::regex optVarRegex("\\|");
    optCMakeVar =  std::regex_replace(platform, optVarRegex, "_");
	optCMakeVar = std::regex_replace(optCMakeVar, std::regex("'"), "");
	std::regex optVarRegexAfter(".*==(\\S+)");
	optCMakeVar = GetProjName().Name() + "_" + ToUpperStr(std::regex_replace(optCMakeVar, optVarRegexAfter, "$1"));

	std::regex optListRegex(";");
	optCMakeList = std::regex_replace(optionList, optListRegex, "    ");

	
	optCMakeList = std::regex_replace(optCMakeList, std::regex("\\%\\(PreprocessorDefinitions\\)"), "    ");
	m_platformOpts[optCMakeVar] = optCMakeList;

	return 0;
}

int CMakeFile::writeSetList(const CMakeVar& cmakeVar, const std::string& cmakeValue)
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << "set (";

	m_fs << cmakeVar.Name() << "    " << cmakeValue;

	m_fs << ")" << std::endl << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeSetList(const CMakeVar &cmakeVar, const std::map<std::string, int>& fileMap)
{
	std::map<std::string, int>::const_iterator iterCppMap;
	std::string str;

	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << "set (";

	m_fs << cmakeVar.Name();
	m_fs << std::endl;

	std::regex dir_sep_regex(DIR_SEP_REGEX);
	for (iterCppMap = fileMap.cbegin(); iterCppMap != fileMap.cend(); iterCppMap++) {


		str =  std::regex_replace(iterCppMap->first, dir_sep_regex, CMAKE_DIR_SEP);

		m_fs << "    " << str << std::endl;
	}
	
	m_fs << ")" << std::endl << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::write(const std::map<std::string, int>& fileMap, int FileType)
{
	std::map<std::string, int> dirMap;
	std::map<std::string, std::map<std::string, int> > dirFileFilter;
	std::map<std::string, std::map<std::string, int> >::const_iterator iterDirFileFilter;
	if (fileMap.size() == 0) {
		return 0;
	}

	if (true) {
		CheckFileList(fileMap, GetExtName(FileType), dirFileFilter);
		for (iterDirFileFilter = dirFileFilter.cbegin(); iterDirFileFilter != dirFileFilter.cend(); iterDirFileFilter++) {
			std::string cmake_dir_var = "FILTER_";
			std::regex dir_cur_regex(DIR_CUR_REGEX);
			std::regex dir_sep_regex(DIR_SEP_REGEX);
			cmake_dir_var = cmake_dir_var + iterDirFileFilter->first;
			cmake_dir_var = std::regex_replace(cmake_dir_var, dir_sep_regex, "_");
			cmake_dir_var = std::regex_replace(cmake_dir_var, dir_cur_regex, "");
			cmake_dir_var = cmake_dir_var + GetCMakeDirVarSuffix(FileType);
			cmake_dir_var = std::regex_replace(cmake_dir_var, std::regex("__"), "_");
			CMakeVar curDirCMakeVar = ToUpperStr(cmake_dir_var);
			writeSetList(curDirCMakeVar, iterDirFileFilter->second);
			
			m_allDirFilter[curDirCMakeVar] = FileType;

			m_dirMap[iterDirFileFilter->first] = iterDirFileFilter->second.size();
		}
	}
	else {
		writeSetList (GetCMakeVarName(FileType), fileMap);
	}

	return 0;
}

int CMakeFile::writeOptionLists()
{
	std::map<CMakeVar, std::string>::const_iterator  iterPlatformOpts;
	for (iterPlatformOpts = m_platformOpts.cbegin(); iterPlatformOpts != m_platformOpts.cend(); iterPlatformOpts++)
	{
		writeSetList(iterPlatformOpts->first, iterPlatformOpts->second);
	}
	return 0;
}

int CMakeFile::writeCMakeLists()
{
	writeOptionLists();
	writeFileList(m_dirMap);
	writeStaticLib();
	writeSharedLib();
	writeApp();
	writeInstall();
	return 0;
}

int CMakeFile::writeRootDir()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);

	m_fs << "cmake_minimum_required(VERSION 3.15)" << std::endl << std::endl;

	m_fs << "project(" << GetProjName().Name() << ")" << std::endl << std::endl;

	m_fs << "set(CMAKE_C_STANDARD 99) " << std::endl;
	m_fs << "set(CMAKE_C_STANDARD_REQUIRED ON) " << std::endl;
	m_fs << "set(CMAKE_CXX_STANDARD 11) " << std::endl;
	m_fs << "set(CMAKE_CXX_STANDARD_REQUIRED ON) " << std::endl;
	m_fs << "set(CMAKE_CXX_EXTENSIONS OFF) " << std::endl;
	m_fs << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON) " << std::endl  << std::endl;

	m_fs << "set_property(GLOBAL PROPERTY USE_FOLDERS ON) " << std::endl << std::endl;

	m_fs << "set(ENV{HomeDir}  ${CMAKE_CURRENT_LIST_DIR}/../out/${CMAKE_SYSTEM_NAME})" << std::endl << std::endl;

	m_fs << "set(" << GetProjBinDir().Name() << " ${CMAKE_CURRENT_BINARY_DIR})                  " << std::endl;
	m_fs << "set(" << GetProjTopDir().Name() << " ${CMAKE_CURRENT_SOURCE_DIR}) #CMAKE_SOURCE_DIR" << std::endl << std::endl;
	m_fs << "set(" << GetProjIncDirSet().Name() << " ${CMAKE_CURRENT_SOURCE_DIR}/inc)                  " << std::endl;
	m_fs << "set(" << GetProjLibDirSet().Name() << " ${CMAKE_CURRENT_SOURCE_DIR}/lib) #CMAKE_SOURCE_DIR" << std::endl << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeFileList(const std::map<std::string, int>& dirMap)
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << std::endl;
	m_fs << GenerateFileFuncLine(GetProjAsmList().Name(), GetProjTopDir().Value(), dirMap, ".asm*") << std::endl;
	m_fs << GenerateFileFuncLine(GetProjIncList().Name(), GetProjTopDir().Value(), dirMap, ".h*") << std::endl;
	m_fs << GenerateFileFuncLine(GetProjSrcList().Name(), GetProjTopDir().Value(), dirMap, ".c*") << std::endl;
	m_fs << GenerateFileFuncLine(GetProjAppList().Name(), GetProjTopDir().Value(), dirMap, ".c*") << std::endl;
	m_fs << "list        (REMOVE_ITEM    " << GetProjSrcList().Name() << "                    " << GetProjAppList().Value() << "    )" << std::endl;
	m_fs << "source_group(TREE         " << GetProjTopDir().Value() << "     FILES       " << GetProjSrcList().Value() << "    )" << std::endl << std::endl << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeStaticLib()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << std::endl;
	m_fs << "set                       (  " << GetProjStaticLib().Name() << "               " << ToLowerStr(GetProjStaticLib().Name()) << "          )" << std::endl;
	m_fs << "add_library               (" << GetProjStaticLib().Value() << "   STATIC     " << GetProjSrcList().Value() << "    )" << std::endl;
	m_fs << "target_compile_definitions(" << GetProjStaticLib().Value() << "   PRIVATE    HAVE_CONFIG_H          " << GetProjDefList().Value() << ")" << std::endl;
	m_fs << "target_include_directories(" << GetProjStaticLib().Value() << "   PUBLIC     " << GetProjIncDirSet().Value() << "                )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjStaticLib().Value() << "   PUBLIC     " << GetProjLibDirSet().Value() << "                 )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjStaticLib().Value() << "              ${dep_libs}               " << GetProjAppDepLib().Value() << "           )" << std::endl;
	m_fs << "target_compile_options    (" << GetProjStaticLib().Value() << "   PRIVATE     /Z7     /W4           )" << std::endl;
	m_fs << "set_target_properties     (" << GetProjStaticLib().Value() << "   PROPERTIES CLEAN_DIRECT_OUTPUT 1     OUTPUT_NAME " << ToLowerStr(GetProjStaticLib().Name()) << "  )" << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeSharedLib()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << std::endl;
	m_fs << "set                       (  " << GetProjSharedLib().Name() << "               " << ToLowerStr(GetProjSharedLib().Name()) << "          )" << std::endl;
	m_fs << "add_library               (" << GetProjSharedLib().Value() << "   SHARED     " << GetProjSrcList().Value() << "    )" << std::endl;
	m_fs << "target_compile_definitions(" << GetProjSharedLib().Value() << "   PRIVATE    HAVE_CONFIG_H          " << GetProjDefList().Value() << ")" << std::endl;
	m_fs << "target_include_directories(" << GetProjSharedLib().Value() << "   PUBLIC     " << GetProjIncDirSet().Value() << "                 )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjSharedLib().Value() << "   PUBLIC     " << GetProjLibDirSet().Value() << "                 )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjSharedLib().Value() << "              ${dep_libs}               " << GetProjAppDepLib().Value() << "           )" << std::endl;
	m_fs << "target_compile_options    (" << GetProjSharedLib().Value() << "   PRIVATE     /Z7     /W4           )" << std::endl;
	m_fs << "set_target_properties     (" << GetProjSharedLib().Value() << "   PROPERTIES CLEAN_DIRECT_OUTPUT 1     OUTPUT_NAME " << ToLowerStr(GetProjSharedLib().Name()) << "  )" << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeApp()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << std::endl;
	m_fs << "set                       (  " << GetProjAppName().Name() << "                " << ToLowerStr(GetProjAppName().Name()) << "        )" << std::endl;
	m_fs << "add_executable            (" << GetProjAppName().Value() << "               " << GetProjSrcList().Value() << "        " << GetProjAppList().Value() << "            )" << std::endl;
	m_fs << "target_compile_definitions(" << GetProjAppName().Value() << "   PRIVATE     __SIZE_T_DEFINED	       " << GetProjAppDefs().Value() << "               )" << std::endl;
	m_fs << "target_include_directories(" << GetProjAppName().Value() << "   PUBLIC      " << GetProjIncDirSet().Value() << "                   )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjAppName().Value() << "   PUBLIC      " << GetProjLibDirSet().Value() << "                   )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjAppName().Value() << "               " << GetProjStaticLib().Value() << "         )" << std::endl;
	m_fs.close();
	return 0;
}

int CMakeFile::writeInstall()
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << std::endl << std::endl;
	m_fs << "SET(install_targets " << GetProjStaticLib().Value() << "    " << GetProjSharedLib().Value() << "  " << GetProjAppName().Value() << "  ) #" << std::endl;
	m_fs << "SET(install_hdr_dir include)#" << std::endl << std::endl;



	m_fs << "INSTALL(TARGETS       ${install_targets}" << std::endl;
	m_fs << "        RUNTIME       DESTINATION bin" << std::endl;
	m_fs << "        LIBRARY       DESTINATION lib" << std::endl;
	m_fs << "        ARCHIVE       DESTINATION lib" << std::endl;
	m_fs << "        PUBLIC_HEADER DESTINATION ${install_hdr_dir}" << std::endl;
	m_fs << ")" << std::endl;

	m_fs << "install(FILES      " << GetProjIncList().Value()  << "    DESTINATION    ${install_hdr_dir})" << std::endl;
    m_fs << "install(DIRECTORY  \"inc_sub_dir\"                        DESTINATION    ${install_hdr_dir}    FILES_MATCHING    PATTERN   \"*.h\"   )" << std::endl;
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
	str += "_BIN_DIR";
	return str;
}

CMakeVar  CMakeFile::GetProjTopDir() {
	std::string str;
	str += m_ProjName.Name();
	str += "_TOP_DIR";
	return str;
}

CMakeVar  CMakeFile::GetProjIncDirSet() {
	std::string str;
	str += m_ProjName.Name();
	str += "_INC_DIRS";
	return str;
}

CMakeVar  CMakeFile::GetProjLibDirSet() {
	std::string str;
	str += m_ProjName.Name();
	str += "_LIB_DIRS";
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

std::string CMakeFile::GenerateFileFuncLine(const std::string &varName, const std::string &topDir, const std::map<std::string, int> &subDirList, const std::string &extName)
{
	std::strstream fileFuncLine;
	std::map<std::string, int>::const_iterator iterDir;
	fileFuncLine << "file        (GLOB           " << varName << "         RELATIVE   " << topDir << "  ";
	for (iterDir = subDirList.cbegin(); iterDir != subDirList.cend(); iterDir++) {
		fileFuncLine << topDir << DIR_SEP << iterDir->first << DIR_SEP << "*" << (extName.at(0) == '.' ? extName : std::string(".") + extName) << "   ";
	}
	fileFuncLine << "      )" << std::endl << std::ends;

	std::regex dir_sep_regex(DIR_SEP_REGEX);
	return  std::regex_replace(fileFuncLine.str(), dir_sep_regex, CMAKE_DIR_SEP);
}

std::string CMakeFile::GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::string& subDir, const std::string& extName)
{
	std::strstream fileFuncLine;
	std::vector<std::string>::const_iterator iterDir;
	fileFuncLine << "file        (GLOB           " << varName << "         RELATIVE   " << topDir << "  ";

		fileFuncLine << topDir << DIR_SEP << subDir << DIR_SEP << "*" << (extName.at(0) == '.' ? extName : std::string(".") + extName) << "   ";

	fileFuncLine << "      )" << std::endl << std::ends;

	std::regex dir_sep_regex(DIR_SEP_REGEX);
	return  std::regex_replace(fileFuncLine.str(), dir_sep_regex, CMAKE_DIR_SEP);
}