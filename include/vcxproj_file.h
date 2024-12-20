#ifndef VCX_PROJ_FILE_H
#define VCX_PROJ_FILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <strstream>
#include <map>
#include <vector>
#include <regex>
#include <iostream>
#include <iomanip>

#include "tinyxml.h"
#include "xml_adapter.h"
#include "my_sys.h"
#include "cmake_file.h"

std::string GetFileName(const std::string& fileName);

TiXmlElement* Find_NextProjectConfigurations(TiXmlElement* FirstProjectConfigurations, const char* section);

TiXmlElement* Find_FirstProjectConfigurations(TiXmlElement* ItemGroup, const char* section);

TiXmlElement* Find_FirstItemGroup(TiXmlElement* Project, const char* name = "ItemGroup");

void DumpProjectConfiguration(TiXmlElement* root);

int MatchPropertyGroup(std::string Name, std::string AttrName);
TiXmlElement* Find_PropertyGroup(TiXmlElement* Project, const char* name = "PropertyGroup");

int MatchImportGroup(std::string Name, std::string AttrName);
TiXmlElement* Find_ImportGroup(TiXmlElement* Project, const char* name = "ImportGroup");

int MatchItemDefinitionGroup(std::string Name, std::string AttrName);
TiXmlElement* Find_ItemDefinitionGroup(CMakeFile& cmakeFile, TiXmlElement* Project, const char* name = "ItemDefinitionGroup");

int MatchAdditionalDependencies(std::string Name, std::string AttrName);
TiXmlElement* Find_AdditionalDependencies(CMakeFile& cmakeFile, TiXmlElement* ItemDefinitionGroup, const char* name = "AdditionalDependencies");

int MatchPreprocessorDefinitions(std::string Name, std::string AttrName);
TiXmlElement* Find_ClCompilePreprocessorDefinitions(CMakeFile& cmakeFile, TiXmlElement* ItemDefinitionGroup, const char* name = "PreprocessorDefinitions");
TiXmlElement* Find_NASMPreprocessorDefinitions(CMakeFile& cmakeFile, TiXmlElement* ItemDefinitionGroup, const char* name = "PreprocessorDefinitions");
TiXmlElement* Find_ResourceCompilePreprocessorDefinitions(CMakeFile& cmakeFile, TiXmlElement* ItemDefinitionGroup, const char* name = "PreprocessorDefinitions");

int MatchItemGroup_NASM(std::string Name, std::string AttrName);
TiXmlElement* Find_ItemGroup_NASM(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name = "NASM");

int MatchItemGroup_ClCompile(std::string Name, std::string AttrName);
TiXmlElement* Find_ItemGroup_ClCompile(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name = "ClCompile");

int MatchItemGroup_ClInclude(std::string Name, std::string AttrName);
TiXmlElement* Find_ItemGroup_ClInclude(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name = "ClInclude");

int MatchItemGroup_ResourceCompile(std::string Name, std::string AttrName);
TiXmlElement* Find_ItemGroup_ResourceCompile(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name = "ResourceCompile");

int MatchItemGroup(std::string Name, std::string AttrName);
TiXmlElement* Find_ItemGroup(CMakeFile& cmakeFile, TiXmlElement* Project, const char* name = "ItemGroup");


void DumpAllGroup(CMakeFile& cmakeFile, TiXmlElement* root);

int ParseProjMain(const std::string& fileVcxproj, const std::string& fileCmake);


#endif