#include "cmake_file.h"

#include <algorithm>
#include <cctype>
#include <strstream>
#include <regex>
#include <filesystem>

#include "vcxproj_path.h"
#include "my_fs.h"

static std::string cmakeRootDir;

void SetCMakeWorkDir(const std::string& rootDir)
{
	cmakeRootDir = rootDir + DIR_SEP;
	std::cout << "cmakeRootDir:" << cmakeRootDir << std::endl;
}

std::string GetCMakeWorkDir()
{
	return cmakeRootDir;
}

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

CMakeFile::CMakeFile(const char* filename, const char* projname):m_name(filename), m_ProjName(ToUpperStr(std::string(projname))) {

}

CMakeFile::CMakeFile(const std::string& filename, const std::string& projname) :m_name(filename), m_ProjName(ToUpperStr(projname)) {

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

std::string CMakeFile::GenPlatformCMakeVar(const std::string& platform, const std::string &varType)
{

	std::string optCMakeVar;
	std::regex optVarRegex("\\|");
	optCMakeVar = std::regex_replace(platform, optVarRegex, "_");
	optCMakeVar = std::regex_replace(optCMakeVar, std::regex("'"), "");
	std::regex optVarRegexAfter(".*==(\\S+)");
	optCMakeVar = GetProjName().Name() + "_" + varType + "_" + ToUpperStr(std::regex_replace(optCMakeVar, optVarRegexAfter, "$1"));
	return optCMakeVar;
}

int CMakeFile::writeLibList(const std::string& libList, const std::string& platform)
{
	std::string libMakeList;
	std::string libCMakeVar;
	libCMakeVar = GenPlatformCMakeVar(platform, std::string("LIBS"));

	std::regex optListRegex(";");
	libMakeList = std::regex_replace(libList, optListRegex, "    ");

	
	libMakeList = std::regex_replace(libMakeList, std::regex("\\%\\(AdditionalDependencies\\)"), "    ");
	m_platformLibs[libCMakeVar] = libMakeList;
	return 0;
}

int CMakeFile::writeOptList(const std::string& optionList, const std::string &platform, int FileType)
{
	std::string optCMakeList;
	std::string optCMakeVar;
	
	optCMakeVar = GenPlatformCMakeVar(platform, "DEFS") + GetCMakeDirVarSuffix(FileType);
	
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
	std::string curDirCMakeVarSuffix;

	if (fileMap.size() == 0) {
		return 0;
	}

	if (true) {
		curDirCMakeVarSuffix = GetCMakeDirVarSuffix(FileType);
		CheckFileList(fileMap, GetExtName(FileType), curDirCMakeVarSuffix, m_dirMap, m_allDirFilter);
	}
	else {
		writeSetList (GetCMakeVarName(FileType), fileMap);
	}

	return 0;
}

int CMakeFile::writeLibraryLists()
{
	std::map<CMakeVar, std::string>::const_iterator  iterPlatformLibs;
	for (iterPlatformLibs = m_platformLibs.cbegin(); iterPlatformLibs != m_platformLibs.cend(); iterPlatformLibs++)
	{
		writeSetList(iterPlatformLibs->first, iterPlatformLibs->second);
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

int CMakeFile::writeFilterList(std::map<std::string, std::map<std::string, int> > &allDirFilter)
{
	std::map<std::string, std::map<std::string, int> >::iterator iterDirFileFilter;
	for (iterDirFileFilter = allDirFilter.begin(); iterDirFileFilter != allDirFilter.end(); iterDirFileFilter++) {

		// writeSetList(iterDirFileFilter->first, iterDirFileFilter->second);
		writeFileList(iterDirFileFilter->first, iterDirFileFilter->second);
	}
	return 0;
}


int CMakeFile::writeCMakeLists()
{
	writeOptionLists();
	writeLibraryLists();
	writeFilterList(m_allDirFilter);
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

int CMakeFile::writeFileList(const CMakeVar& cmakeVar, const std::map<std::string, int>& fileMap)
{
	m_fs.open(m_name, std::ios::out | std::ios::app);
	m_fs << std::endl;
	m_fs << GenerateFileFuncLine(cmakeVar.Name(), GetProjTopDir().Value(), fileMap) << std::endl;
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
	m_fs << "target_compile_definitions(" << GetProjStaticLib().Value() << "   PRIVATE    " << GetProjDefList().Value() << "          HAVE_CONFIG_H" << ")" << std::endl;
	m_fs << "target_include_directories(" << GetProjStaticLib().Value() << "   PUBLIC     " << GetProjIncDirSet().Value() << "                )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjStaticLib().Value() << "   PUBLIC     " << GetProjLibDirSet().Value() << "                )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjStaticLib().Value() << "              " << GetProjAppDepLib().Value() << "        ${dep_libs}  " << ")" << std::endl;
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
	m_fs << "target_compile_definitions(" << GetProjSharedLib().Value() << "   PRIVATE    " << GetProjDefList().Value() << "    HAVE_CONFIG_H      " << ")" << std::endl;
	m_fs << "target_include_directories(" << GetProjSharedLib().Value() << "   PUBLIC     " << GetProjIncDirSet().Value() << "                 )" << std::endl;
	m_fs << "target_link_directories   (" << GetProjSharedLib().Value() << "   PUBLIC     " << GetProjLibDirSet().Value() << "                 )" << std::endl;
	m_fs << "target_link_libraries     (" << GetProjSharedLib().Value() << "              " << GetProjAppDepLib().Value() << "   ${dep_libs}        " << ")" << std::endl;
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
	m_fs << "target_compile_definitions(" << GetProjAppName().Value() << "   PRIVATE     " << GetProjAppDefs().Value() << "     __SIZE_T_DEFINED	" << ")" << std::endl;
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

std::string CMakeFile::GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::map<std::string, int>& fileList)
{
	std::string relatPath;
	std::strstream fileFuncLine;
	std::map<std::string, int>::const_iterator iterDir;
	fileFuncLine << "file        (GLOB           " << varName << "         RELATIVE   " << topDir << "  " << std::endl;
	for (iterDir = fileList.cbegin(); iterDir != fileList.cend(); iterDir++) {
		// fileFuncLine << topDir << DIR_SEP << iterDir->first << std::endl;
		relatPath = GenerateRelatPath(topDir, iterDir->first, GetCMakeWorkDir());
		fileFuncLine << "        " << relatPath << std::endl;
	}
	fileFuncLine << "    )" << std::endl << std::ends;

	std::regex dir_sep_regex(DIR_SEP_REGEX);
	return  std::regex_replace(fileFuncLine.str(), dir_sep_regex, CMAKE_DIR_SEP);
}

std::string FixRelatPath(const std::string& relatPath)
{
	std::strstream fileFuncLine;
	if (relatPath.length() > 0) {
		fileFuncLine << "        " << relatPath << (relatPath.at(relatPath.length() - 1) == DIR_SEP_C ? "" : std::string(DIR_SEP));
	}
	fileFuncLine << std::ends;
    return fileFuncLine.str();
}

std::string CMakeFile::GenerateFileFuncLine(const std::string &varName, const std::string &topDir, const std::map<std::string, int> &subDirList, const std::string &extName)
{
	std::string relatPath;
	std::strstream fileFuncLine;
	std::map<std::string, int>::const_iterator iterDir;
	fileFuncLine << "file        (GLOB           " << varName << "         RELATIVE   " << topDir << "  " << std::endl;
	for (iterDir = subDirList.cbegin(); iterDir != subDirList.cend(); iterDir++) {
		// fileFuncLine << topDir << DIR_SEP << iterDir->first << DIR_SEP;
		relatPath = GenerateRelatPath(topDir, iterDir->first, GetCMakeWorkDir());
		relatPath = FixRelatPath(relatPath);
		fileFuncLine << relatPath;
		fileFuncLine << "*" << (extName.at(0) == '.' ? extName : std::string(".") + extName) << "   ";
	}
	fileFuncLine << "    )" << std::endl << std::ends;

	std::regex dir_sep_regex(DIR_SEP_REGEX);
	return  std::regex_replace(fileFuncLine.str(), dir_sep_regex, CMAKE_DIR_SEP);
}

std::string CMakeFile::GenerateFileFuncLine(const std::string& varName, const std::string& topDir, const std::string& subDir, const std::string& extName)
{
	std::string relatPath;
	std::strstream fileFuncLine;
	std::vector<std::string>::const_iterator iterDir;
	fileFuncLine << "file        (GLOB           " << varName << "         RELATIVE   " << topDir << "  " << std::endl;

	// fileFuncLine << topDir << DIR_SEP << subDir << DIR_SEP;
	relatPath = GenerateRelatPath(topDir, subDir, GetCMakeWorkDir());
	relatPath = FixRelatPath(relatPath);
	fileFuncLine << relatPath;
	fileFuncLine << "*" << (extName.at(0) == '.' ? extName : std::string(".") + extName) << "   ";

	fileFuncLine << "    )" << std::endl << std::ends;

	std::regex dir_sep_regex(DIR_SEP_REGEX);
	return  std::regex_replace(fileFuncLine.str(), dir_sep_regex, CMAKE_DIR_SEP);
}

std::string CMakeFile::GetCMakeVarRealPath(const std::string& cmakeVarPath)
{
	std::string topCMakePath;
	topCMakePath = GetProjTopDir().Value();
	std::string srcCMakePath;
	srcCMakePath = cmakeVarPath;
	if (0 == strncmp(topCMakePath.c_str(), srcCMakePath.c_str(), strlen(topCMakePath.c_str()))) {
		std::string topPathVal = GetVcxprojWorkDir();
		// std::regex dir_sep_regex(GetProjTopDir().Value());
		// srcPath = std::regex_replace(srcPath.string(), dir_sep_regex, topPathVal);
		return srcCMakePath.replace(0, strlen(topCMakePath.c_str()), topPathVal.c_str());
	}
	return cmakeVarPath;
}

std::string CMakeFile::GenerateRelatPath(const std::string& topDir, const std::string& subFile, const std::string& baseDir)
{
	std::filesystem::path srcPath;
	std::filesystem::path basePath;
	std::strstream fileFuncLine;

	fileFuncLine << topDir << DIR_SEP << subFile << std::ends;
	srcPath = GetCMakeVarRealPath(fileFuncLine.str());
	basePath = GetCMakeVarRealPath(baseDir);
	if (srcPath.string().at(0) != '$') {
		srcPath = std::filesystem::relative(srcPath, basePath);
	}
	return srcPath.string();
}