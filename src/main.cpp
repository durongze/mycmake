
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

CMakeFile cmake_file("CMakeLists.txt");

inline wchar_t* AnsiToUnicode(const char* szStr)
{
	wchar_t* pResult = NULL;
	int nLen = 0;
#ifdef MB_X_WC
	nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
#else
    nLen = strlen(szStr) + 1;
	pResult = new wchar_t[nLen];
	setlocale(LC_ALL, "en_GB.UTF8");
	mbstowcs(pResult, szStr, nLen + 1);
#endif
	return pResult;
}

inline char* UnicodeToAnsi(const wchar_t* szStr)
{
	int nLen = 0;
	char* pResult = NULL;
#ifdef MB_X_WC
	nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
#else
	setlocale(LC_ALL, "en_GB.UTF8");
    nLen = wcslen(szStr) + 1;
	pResult = new char[nLen];
	wcstombs(pResult, szStr, nLen);
#endif
	return pResult;
}


int CheckFile(std::string fileName)
{
	std::string projFile = fileName + ".vcxproj";
	std::string cwdDir;
#ifdef __linux
	char cwd_buf[256] = {0};
	cwdDir += (const char*)getcwd(cwd_buf, sizeof(cwd_buf));
	cwdDir += ":";
#else
	wchar_t cwd_buf[256] = { 0 };
	cwdDir = (const char*)_wgetcwd(cwd_buf, sizeof(cwd_buf) / sizeof(cwd_buf[0]));
#endif

    FILE *fpPdf = fopen(projFile.c_str(), "rb");
    if (fpPdf == NULL) {
		std::cout << cwdDir << projFile << " error:" << strerror(errno) << std::endl;
        return -2;
    } else {
        fclose(fpPdf);
    }
	return 0;
}

void Useage(std::string exec)
{
    std::cout << exec << " <fileName> " << std::endl;
}

std::string GetFileName(const std::string& fileName)
{
    std::string extName = ".vcxproj";
    size_t lenFileName = fileName.length();
    size_t lenExtName = extName.length();
    return fileName.substr(0, lenFileName - lenExtName);
}

TiXmlElement* Find_NextProjectConfigurations(TiXmlElement* FirstProjectConfigurations, const char* section)
{
	if (FirstProjectConfigurations == NULL) return NULL;
	TiXmlElement* ProjectConfigurations = NULL; // Include=""
	ProjectConfigurations = FirstProjectConfigurations->NextSiblingElement();
	DumpXmlNode(std::cout, ProjectConfigurations);
	return ProjectConfigurations;
}

TiXmlElement* Find_FirstProjectConfigurations(TiXmlElement* ItemGroup, const char* section)
{
	if (ItemGroup == NULL) return NULL;
	TiXmlElement* ProjectConfigurations = NULL; // Include=""
	ProjectConfigurations = ItemGroup->FirstChildElement();
	DumpXmlNode(std::cout, ProjectConfigurations);
	return ProjectConfigurations;
}

TiXmlElement* Find_FirstItemGroup(TiXmlElement* Project, const char* name="ItemGroup")
{
	if (Project == NULL) return NULL;
	TiXmlElement* ItemGroup = NULL; // Label="ProjectConfigurations"
	ItemGroup = Project->FirstChildElement();
	DumpXmlNode(std::cout, ItemGroup);
	return ItemGroup;
}

void DumpProjectConfiguration(TiXmlElement* root)
{
	if (NULL != root) {
		TiXmlElement* ItemGroup = Find_FirstItemGroup(root, "");
		TiXmlElement* ProjectConfiguration = Find_FirstProjectConfigurations(ItemGroup, "");
		for (; ProjectConfiguration; ProjectConfiguration = Find_NextProjectConfigurations(ProjectConfiguration, "")) {
			DumpXmlNode(std::cout, ProjectConfiguration);
		}
	}
}

int MatchPropertyGroup(std::string Name, std::string AttrName)
{
	std::regex PropertyGroup;
	std::string Pattern("PropertyGroup");
	std::string AttrPattern("^Condition.*");
	PropertyGroup = AttrPattern;
	if (std::regex_search(AttrName, PropertyGroup)) {
		PropertyGroup = Pattern;
		return std::regex_search(Name, PropertyGroup);
	}
	return false;
}

TiXmlElement* Find_PropertyGroup(TiXmlElement* Project, const char* name="PropertyGroup")
{
	if (Project == NULL) return NULL;
	TiXmlElement* PropertyGroup = NULL; // Condition=""
	PropertyGroup = Project->FirstChildElement();
	for (; PropertyGroup; PropertyGroup = PropertyGroup->NextSiblingElement()) {
		if (PropertyGroup->FirstAttribute() == NULL) {
			continue;
		}
		// DumpXmlNode(std::cout, PropertyGroup);
		if (MatchPropertyGroup(PropertyGroup->Value(), PropertyGroup->FirstAttribute()->Name())) {
			DumpXmlNode(std::cout, PropertyGroup);
		}
	}
	return PropertyGroup;
}

int MatchImportGroup(std::string Name, std::string AttrName)
{
	std::regex ImportGroup;
	std::string Pattern("ImportGroup");
	std::string AttrPattern("^Condition.*");
	ImportGroup = AttrPattern;
	if (std::regex_search(AttrName, ImportGroup)) {
		ImportGroup = Pattern;
		return std::regex_search(Name, ImportGroup);
	}
	return false;
}

TiXmlElement* Find_ImportGroup(TiXmlElement* Project, const char* name = "ImportGroup")
{
	if (Project == NULL) return NULL;
	TiXmlElement* ImportGroup = NULL; // Condition=""
	ImportGroup = Project->FirstChildElement();
	for (; ImportGroup; ImportGroup = ImportGroup->NextSiblingElement()) {
		if (ImportGroup->FirstAttribute() == NULL) {
			continue;
		}
		// DumpXmlNode(std::cout, ImportGroup);
		if (MatchImportGroup(ImportGroup->Value(), ImportGroup->FirstAttribute()->Name())) {
			DumpXmlNode(std::cout, ImportGroup);
		}
	}
	return ImportGroup;
}

int MatchItemDefinitionGroup(std::string Name, std::string AttrName)
{
	std::regex ItemDefinitionGroup;
	std::string Pattern("ItemDefinitionGroup");
	std::string AttrPattern("^Condition.*");
	ItemDefinitionGroup = AttrPattern;
	if (std::regex_search(AttrName, ItemDefinitionGroup)) {
		ItemDefinitionGroup = Pattern;
		return std::regex_search(Name, ItemDefinitionGroup);
	}
	return false;
}

int MatchPreprocessorDefinitions(std::string Name, std::string AttrName)
{
	std::regex PreprocessorDefinitions;
	std::string Pattern("PreprocessorDefinitions");
	PreprocessorDefinitions = Pattern;
	if (std::regex_search(Name, PreprocessorDefinitions)) {
		return true;
	}
	return false;
}

TiXmlElement* Find_PreprocessorDefinitions(TiXmlElement* ItemDefinitionGroup, const char* name = "PreprocessorDefinitions")
{
	if (ItemDefinitionGroup == NULL) return NULL;
	TiXmlElement* ClCompile = NULL;
	TiXmlElement* ClCompileChild = NULL;
	ClCompile = ItemDefinitionGroup->FirstChildElement();
	ClCompileChild = ClCompile->FirstChildElement();
	for (; ClCompileChild; ClCompileChild = ClCompileChild->NextSiblingElement()) {
		if (ClCompileChild->FirstAttribute() == NULL) {
			;
		}
		if (MatchPreprocessorDefinitions(ClCompileChild->Value(), "")) {
			DumpXmlNode(std::cout, ClCompileChild);
			cmake_file.write(ClCompileChild->GetText());
		}
	}
	return ItemDefinitionGroup;
}

TiXmlElement* Find_ItemDefinitionGroup(TiXmlElement* Project, const char* name = "ItemDefinitionGroup")
{
	if (Project == NULL) return NULL;
	TiXmlElement* ItemDefinitionGroup = NULL; // Condition=""
	ItemDefinitionGroup = Project->FirstChildElement();
	for (; ItemDefinitionGroup; ItemDefinitionGroup = ItemDefinitionGroup->NextSiblingElement()) {
		if (ItemDefinitionGroup->FirstAttribute() == NULL) {
			continue;
		}
		// DumpXmlNode(std::cout, ItemDefinitionGroup);
		if (MatchItemDefinitionGroup(ItemDefinitionGroup->Value(), ItemDefinitionGroup->FirstAttribute()->Name())) {
			DumpXmlNode(std::cout, ItemDefinitionGroup);
			Find_PreprocessorDefinitions(ItemDefinitionGroup);
		}
	}
	return ItemDefinitionGroup;
}

int MatchItemGroup_ClCompile(std::string Name, std::string AttrName)
{
	std::regex ClCompile;
	std::string Pattern("ClCompile");
	std::string AttrPattern("Include");
	ClCompile = Pattern;
	if (std::regex_search(Name, ClCompile)) {
		ClCompile = AttrPattern;
		return std::regex_search(AttrName, ClCompile);
	}
	return false;
}

TiXmlElement* Find_ItemGroup_ClCompile(TiXmlElement* ItemGroup, const char* name = "ClCompile")
{
	if (ItemGroup == NULL) return NULL;
	TiXmlElement* ClCompile = NULL; 
	ClCompile = ItemGroup->FirstChildElement();
	for (; ClCompile; ClCompile = ClCompile->NextSiblingElement()) {
		if (ClCompile->FirstAttribute() == NULL) {
			continue;
		}
		if (MatchItemGroup_ClCompile(ClCompile->Value(), "Include")) {
			DumpXmlNode(std::cout, ClCompile);
			cmake_file.write(ClCompile->FirstAttribute()->Value());
		}
	}
}

int MatchItemGroup(std::string Name, std::string AttrName)
{
	std::regex ItemGroup;
	std::string Pattern("ItemGroup");
	std::string AttrPattern("");
	ItemGroup = AttrPattern;
	ItemGroup = Pattern;
	if (std::regex_search(Name, ItemGroup)) {
		return true;
	}
	return false;
}

TiXmlElement* Find_ItemGroup(TiXmlElement* Project, const char* name = "ItemGroup")
{
	if (Project == NULL) return NULL;
	TiXmlElement* ItemGroup = NULL; // Condition=""
	ItemGroup = Project->FirstChildElement();
	for (; ItemGroup; ItemGroup = ItemGroup->NextSiblingElement()) {
		if (ItemGroup->FirstAttribute() == NULL) {
			;
		}

		// DumpXmlNode(std::cout, ItemGroup);
		if (MatchItemGroup(ItemGroup->Value(), "")) {
			// DumpXmlNode(std::cout, ItemGroup);
			Find_ItemGroup_ClCompile(ItemGroup);
		}
	}
	return ItemGroup;
}


void DumpAllGroup(TiXmlElement* root)
{
	if (NULL != root) {
		cmake_file.create();
		Find_PropertyGroup(root);
		Find_ImportGroup(root);
		Find_ItemDefinitionGroup(root);
		Find_ItemGroup(root);
	}
}

int ParseProjMain(std::string &fileName)
{
	bool ret;
	std::string extName = ".vcxproj";
	TiXmlDocument doc;
	TiXmlElement* root;
	fileName += extName;
	ret = doc.LoadFile(fileName.c_str(), TIXML_ENCODING_UTF8);
	if (!ret) {
		std::cout << fileName << std::endl;
		return -1;
	}
	root = doc.RootElement();
	// DumpProjectConfiguration(root);
	DumpAllGroup(root);
	return 0;
}

int AppMain(int argc, char* argv[])
{
    std::string fileName;
	if (argc == 2) {
		fileName = GetFileName(std::string(argv[1]));
	} 
	else {
		Useage(argv[0]);
		return -1;
	}

    if (CheckFile(fileName))
    {
        Useage(argv[0]);
		std::cout << fileName << std::endl << "Input fileName:";
		std::cin >> fileName;
        return -2;
    }
    return ParseProjMain(fileName);
}

int main( int argc, char* argv[] )
{
    return AppMain(argc, argv);
}
