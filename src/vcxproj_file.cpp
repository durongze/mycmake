#include "vcxproj_file.h"
#include <filesystem>

std::string GetFileName(const std::string& file)
{
	std::string fileName;
	std::filesystem::path vcxprojPath(file);
	fileName = vcxprojPath.filename().string();
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

TiXmlElement* Find_FirstItemGroup(TiXmlElement* Project, const char* name)
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

TiXmlElement* Find_PropertyGroup(TiXmlElement* Project, const char* name )
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

TiXmlElement* Find_ImportGroup(TiXmlElement* Project, const char* name )
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

TiXmlElement* Find_ItemDefinitionGroup(CMakeFile& cmakeFile, TiXmlElement* Project, const char* name)
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
			Find_PreprocessorDefinitions(cmakeFile, ItemDefinitionGroup);
		}
	}
	return ItemDefinitionGroup;
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

TiXmlElement* Find_PreprocessorDefinitions(CMakeFile& cmakeFile, TiXmlElement* ItemDefinitionGroup, const char* name )
{
	std::string platform;
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
			GetFirstAttrValue(ItemDefinitionGroup, platform);
			// cmakeFile.write(ClCompileChild->GetText());
			cmakeFile.writeOptList(ClCompileChild->GetText(), platform);
		}
	}
	return ItemDefinitionGroup;
}

int MatchItemGroup_NASM(std::string Name, std::string AttrName)
{
	std::regex ClCompile;
	std::string Pattern("NASM");
	std::string AttrPattern("Include");
	ClCompile = Pattern;
	if (std::regex_search(Name, ClCompile)) {
		ClCompile = AttrPattern;
		return std::regex_search(AttrName, ClCompile);
	}
	return false;
}

TiXmlElement* Find_ItemGroup_NASM(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name)
{
	if (ItemGroup == NULL) return NULL;
	TiXmlElement* ret = NULL;
	TiXmlElement* NASM = NULL;
	std::map<std::string, int> asmMap;
	int asmIdx = 0;

	NASM = ItemGroup->FirstChildElement();
	for (; NASM; NASM = NASM->NextSiblingElement()) {
		if (NASM->FirstAttribute() == NULL) {
			continue;
		}
		if (MatchItemGroup_NASM(NASM->Value(), "Include")) {
			DumpXmlNode(std::cout, NASM);
			asmMap[NASM->FirstAttribute()->Value()] = asmIdx++;
			ret = NASM;
		}
	}

	cmakeFile.write(asmMap, CMakeFile::FILE_TYPE_ASM);
	return ret;
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

TiXmlElement* Find_ItemGroup_ClCompile(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name )
{
	if (ItemGroup == NULL) return NULL;
	TiXmlElement* ret = NULL;
	TiXmlElement* ClCompile = NULL;
	std::map<std::string, int> cppMap;
	int cppIdx = 0;
	ClCompile = ItemGroup->FirstChildElement();
	for (; ClCompile; ClCompile = ClCompile->NextSiblingElement()) {
		if (ClCompile->FirstAttribute() == NULL) {
			continue;
		}
		if (MatchItemGroup_ClCompile(ClCompile->Value(), "Include")) {
			DumpXmlNode(std::cout, ClCompile);
			cppMap[ClCompile->FirstAttribute()->Value()] = cppIdx++;
			ret = ClCompile;
		}
	}

	cmakeFile.write(cppMap, CMakeFile::FILE_TYPE_CPP);
	return ret;
}

int MatchItemGroup_ClInclude(std::string Name, std::string AttrName)
{
	std::regex ClInclude;
	std::string Pattern("ClInclude");
	std::string AttrPattern("Include");
	ClInclude = Pattern;
	if (std::regex_search(Name, ClInclude)) {
		ClInclude = AttrPattern;
		return std::regex_search(AttrName, ClInclude);
	}
	return false;
}

TiXmlElement* Find_ItemGroup_ClInclude(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name)
{
	if (ItemGroup == NULL) return NULL;
	TiXmlElement* ret = NULL;
	TiXmlElement* ClInclude = NULL;
	std::map<std::string, int> incMap;
	int incIdx = 0;
	ClInclude = ItemGroup->FirstChildElement();
	for (; ClInclude; ClInclude = ClInclude->NextSiblingElement()) {
		if (ClInclude->FirstAttribute() == NULL) {
			continue;
		}
		if (MatchItemGroup_ClInclude(ClInclude->Value(), "Include")) {
			DumpXmlNode(std::cout, ClInclude);
			incMap[ClInclude->FirstAttribute()->Value()] = incIdx++;
			ret = ClInclude;
		}
	}
	cmakeFile.write(incMap, CMakeFile::FILE_TYPE_HPP);
	return ret;
}

int MatchItemGroup_ResourceCompile(std::string Name, std::string AttrName)
{
	std::regex ResourceCompile;
	std::string Pattern("ResourceCompile");
	std::string AttrPattern("Include");
	ResourceCompile = Pattern;
	if (std::regex_search(Name, ResourceCompile)) {
		ResourceCompile = AttrPattern;
		return std::regex_search(AttrName, ResourceCompile);
	}
	return false;
}

TiXmlElement* Find_ItemGroup_ResourceCompile(CMakeFile& cmakeFile, TiXmlElement* ItemGroup, const char* name)
{
	if (ItemGroup == NULL) return NULL;
	TiXmlElement* ret = NULL;
	TiXmlElement* ResourceCompile = NULL;
	std::map<std::string, int> resMap;
	int resIdx = 0;
	ResourceCompile = ItemGroup->FirstChildElement();
	for (; ResourceCompile; ResourceCompile = ResourceCompile->NextSiblingElement()) {
		if (ResourceCompile->FirstAttribute() == NULL) {
			continue;
		}
		if (MatchItemGroup_ResourceCompile(ResourceCompile->Value(), "Include")) {
			DumpXmlNode(std::cout, ResourceCompile);
			resMap[ResourceCompile->FirstAttribute()->Value()] = resIdx++;
			ret = ResourceCompile;
		}
	}
	cmakeFile.write(resMap, CMakeFile::FILE_TYPE_RES);
	return ret;
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

TiXmlElement* Find_ItemGroup(CMakeFile& cmakeFile, TiXmlElement* Project, const char* name )
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
			Find_ItemGroup_NASM(cmakeFile, ItemGroup);
			Find_ItemGroup_ClCompile(cmakeFile, ItemGroup);
			Find_ItemGroup_ClInclude(cmakeFile, ItemGroup); // error
			Find_ItemGroup_ResourceCompile(cmakeFile, ItemGroup); // error
		}
	}
	return ItemGroup;
}


void DumpAllGroup(CMakeFile& cmakeFile, TiXmlElement* root)
{
	if (NULL != root) {
		cmakeFile.create();
		Find_PropertyGroup(root);
		Find_ImportGroup(root);
		Find_ItemDefinitionGroup(cmakeFile, root);
		Find_ItemGroup(cmakeFile, root);
		cmakeFile.writeCMakeLists();
	}
}



int ParseProjMain(const std::string& fileVcxproj, const std::string& fileCmake)
{
	bool ret;
	std::string extName = ".vcxproj";
	TiXmlDocument doc;
	TiXmlElement* root;
	std::string projVcxproj = fileVcxproj + extName;
	ret = doc.LoadFile(projVcxproj.c_str(), TIXML_ENCODING_UTF8);
	if (!ret) {
		std::cout << projVcxproj << std::endl;
		return -1;
	}

	root = doc.RootElement();
	// DumpProjectConfiguration(root);
	DumpAllGroup(CMakeFile(fileCmake, fileVcxproj), root);
	return 0;
}
