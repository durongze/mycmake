#include "vcxproj_parser.h"

#include <fstream>
#include <iostream>
#include <regex>

#include "xml_adapter.h"

VcxItemGroupProjCfg::VcxItemGroupProjCfg(const std::string& cfg, const std::string& platform):m_cfg(cfg), m_platform(platform) {}
VcxItemGroupProjCfg::~VcxItemGroupProjCfg() {}

VcxItemGroupNone::VcxItemGroupNone() {}
VcxItemGroupNone::~VcxItemGroupNone() {}

VcxItemGroupResourceCompile::VcxItemGroupResourceCompile() {}
VcxItemGroupResourceCompile::~VcxItemGroupResourceCompile() {}

VcxItemGroupNASM::VcxItemGroupNASM() {}
VcxItemGroupNASM::~VcxItemGroupNASM() {}

VcxItemGroupClCompile::VcxItemGroupClCompile() {}
VcxItemGroupClCompile::~VcxItemGroupClCompile() {}

VcxItemGroupClInclude::VcxItemGroupClInclude() {}
VcxItemGroupClInclude::~VcxItemGroupClInclude() {}

VcxItemGroup::VcxItemGroup() {}
VcxItemGroup::~VcxItemGroup() {}

int VcxItemGroup::MatchProjectConfiguration(std::string ElementName, std::string AttrName)
{
	std::regex ClCompile;
	std::string Pattern("ProjectConfiguration");
	std::string AttrPattern("Include");
	ClCompile = Pattern;
	if (std::regex_search(ElementName, ClCompile)) {
		ClCompile = AttrPattern;
		return std::regex_search(AttrName, ClCompile);
	}
	return false;
}

int VcxItemGroup::MatchNASM(std::string ElementName, std::string AttrName)
{
	std::regex ClCompile;
	std::string Pattern("NASM");
	std::string AttrPattern("Include");
	ClCompile = Pattern;
	if (std::regex_search(ElementName, ClCompile)) {
		ClCompile = AttrPattern;
		return std::regex_search(AttrName, ClCompile);
	}
	return false;
}

int VcxItemGroup::MatchClCompile(std::string ElementName, std::string AttrName)
{
	std::regex ClCompile;
	std::string Pattern("ClCompile");
	std::string AttrPattern("Include");
	ClCompile = Pattern;
	if (std::regex_search(ElementName, ClCompile)) {
		ClCompile = AttrPattern;
		return std::regex_search(AttrName, ClCompile);
	}
	return false;
}

int VcxItemGroup::MatchClInclude(std::string ElementName, std::string AttrName)
{
	std::regex ClInclude;
	std::string Pattern("ClInclude");
	std::string AttrPattern("Include");
	ClInclude = Pattern;
	if (std::regex_search(ElementName, ClInclude)) {
		ClInclude = AttrPattern;
		return std::regex_search(AttrName, ClInclude);
	}
	return false;
}

int VcxItemGroup::MatchResourceCompile(std::string ElementName, std::string AttrName)
{
	std::regex ResourceCompile;
	std::string Pattern("ResourceCompile");
	std::string AttrPattern("Include");
	ResourceCompile = Pattern;
	if (std::regex_search(ElementName, ResourceCompile)) {
		ResourceCompile = AttrPattern;
		return std::regex_search(AttrName, ResourceCompile);
	}
	return false;
}

int VcxItemGroup::Load(TiXmlElement* itemGroup)
{
	TiXmlElement* itemGroupChild = NULL;
	if (itemGroup == NULL) return NULL;

	DumpXmlNode(std::cout, itemGroup);

	itemGroupChild = itemGroup->FirstChildElement();
	for (; itemGroupChild; itemGroupChild = itemGroupChild->NextSiblingElement()) {
		if (itemGroupChild->FirstAttribute() == NULL) {
			continue;
		}
		if (MatchProjectConfiguration(itemGroupChild->Value(), "Include")) {
			DumpXmlNode(std::cout, itemGroupChild);
			m_projCfg[itemGroupChild->FirstAttribute()->Value()] = m_projCfg.size();
		}
		if (MatchNASM(itemGroupChild->Value(), "Include")) {
			DumpXmlNode(std::cout, itemGroupChild);
			m_asmMap[itemGroupChild->FirstAttribute()->Value()] = m_asmMap.size();
		}
		if (MatchClCompile(itemGroupChild->Value(), "Include")) {
			DumpXmlNode(std::cout, itemGroupChild);
			m_cppMap[itemGroupChild->FirstAttribute()->Value()] = m_cppMap.size();
		}
		if (MatchClInclude(itemGroupChild->Value(), "Include")) {
			DumpXmlNode(std::cout, itemGroupChild);
			m_incMap[itemGroupChild->FirstAttribute()->Value()] = m_incMap.size();
		}
		if (MatchResourceCompile(itemGroupChild->Value(), "Include")) {
			DumpXmlNode(std::cout, itemGroupChild);
			m_resMap[itemGroupChild->FirstAttribute()->Value()] = m_incMap.size();
		}
	}

	return 0;
}

VcxPropertyGroupCfg::VcxPropertyGroupCfg() {}
VcxPropertyGroupCfg::~VcxPropertyGroupCfg() {}

int VcxPropertyGroupCfg::Load(TiXmlElement* propertyGroupCfg)
{
	if (propertyGroupCfg == NULL) return NULL;

	DumpXmlNode(std::cout, propertyGroupCfg);

	return 0;
}

VcxImportGroupSet::VcxImportGroupSet() {}
VcxImportGroupSet::~VcxImportGroupSet() {}

int VcxImportGroupSet::Load(TiXmlElement* importGroupSet)
{
	if (importGroupSet == NULL) return NULL;

	DumpXmlNode(std::cout, importGroupSet);

	return 0;
}

VcxPropertyGroupTarget::VcxPropertyGroupTarget() {}
VcxPropertyGroupTarget::~VcxPropertyGroupTarget() {}

int VcxPropertyGroupTarget::Load(TiXmlElement* propertyGroupTarget)
{
	if (propertyGroupTarget == NULL) return NULL;

	DumpXmlNode(std::cout, propertyGroupTarget);

	return 0;
}

VcxItemDefinitionClCompile::VcxItemDefinitionClCompile() {}
VcxItemDefinitionClCompile::~VcxItemDefinitionClCompile() {}

int VcxItemDefinitionClCompile::MatchPreprocessorDefinitions(std::string ElementName, std::string AttrName)
{
	std::regex PreprocessorDefinitions;
	std::string Pattern("PreprocessorDefinitions");
	PreprocessorDefinitions = Pattern;
	if (std::regex_search(ElementName, PreprocessorDefinitions)) {
		return true;
	}
	return false;
}

int VcxItemDefinitionClCompile::Load(TiXmlElement* ClCompile)
{
	TiXmlElement* ClCompileChild;
	if (ClCompile == NULL) {
		return NULL;
	}
	ClCompileChild = ClCompile->FirstChildElement();
	for (; ClCompileChild; ClCompileChild = ClCompileChild->NextSiblingElement()) {
		if (ClCompileChild->FirstAttribute() == NULL) {
			;
		}
		if (MatchPreprocessorDefinitions(ClCompileChild->Value(), "")) {
			DumpXmlNode(std::cout, ClCompileChild);
			// GetFirstAttrValue(ItemDefinitionGroup, platform);
			// cmakeFile.write(ClCompileChild->GetText());
			// cmakeFile.writeOptList(ClCompileChild->GetText(), platform, CMakeFile::FILE_TYPE_CPP);
		}
	}
	return 0;
}

VcxItemDefinitionLib::VcxItemDefinitionLib() {}
VcxItemDefinitionLib::~VcxItemDefinitionLib() {}

int VcxItemDefinitionLib::MatchAdditionalDependencies(std::string Name, std::string AttrName)
{
	std::regex AdditionalDependencies;
	std::string Pattern("AdditionalDependencies");
	AdditionalDependencies = Pattern;
	if (std::regex_search(Name, AdditionalDependencies)) {
		return true;
	}
	return false;
}

int VcxItemDefinitionLib::Load(TiXmlElement* Lib)
{
	TiXmlElement* LibChild;
	if (Lib == NULL) {
		return NULL;
	}
	LibChild = Lib->FirstChildElement();
	for (; LibChild; LibChild = LibChild->NextSiblingElement()) {
		if (LibChild->FirstAttribute() == NULL) {
			;
		}
		if (MatchAdditionalDependencies(LibChild->Value(), "")) {
			DumpXmlNode(std::cout, LibChild);
			// GetFirstAttrValue(ItemDefinitionGroup, platform);
			// cmakeFile.write(LibChild->GetText());
			// cmakeFile.writeLibList(LibChild->GetText(), platform);
		}
	}
	return 0;
}

VcxItemDefinitionNASM::VcxItemDefinitionNASM() {}
VcxItemDefinitionNASM::~VcxItemDefinitionNASM() {}

int VcxItemDefinitionNASM::MatchPreprocessorDefinitions(std::string Name, std::string AttrName)
{
	std::regex PreprocessorDefinitions;
	std::string Pattern("PreprocessorDefinitions");
	PreprocessorDefinitions = Pattern;
	if (std::regex_search(Name, PreprocessorDefinitions)) {
		return true;
	}
	return false;
}

int VcxItemDefinitionNASM::Load(TiXmlElement* NASM)
{
	TiXmlElement* NASMChild;
	if (NASM == NULL) {
		return NULL;
	}
	NASMChild = NASM->FirstChildElement();
	for (; NASMChild; NASMChild = NASMChild->NextSiblingElement()) {
		if (NASMChild->FirstAttribute() == NULL) {
			;
		}
		if (MatchPreprocessorDefinitions(NASMChild->Value(), "")) {
			DumpXmlNode(std::cout, NASMChild);
			// GetFirstAttrValue(ItemDefinitionGroup, platform);
			// cmakeFile.write(NASMChild->GetText());
			// cmakeFile.writeOptList(NASMChild->GetText(), platform, CMakeFile::FILE_TYPE_ASM);
		}
	}
	return 0;
}

VcxItemDefinitionPreBuildEvent::VcxItemDefinitionPreBuildEvent() {}
VcxItemDefinitionPreBuildEvent::~VcxItemDefinitionPreBuildEvent() {}

VcxItemDefinitionPostBuildEvent::VcxItemDefinitionPostBuildEvent() {}
VcxItemDefinitionPostBuildEvent::~VcxItemDefinitionPostBuildEvent() {}

VcxItemDefinitionCustomBuildStep::VcxItemDefinitionCustomBuildStep() {}
VcxItemDefinitionCustomBuildStep::~VcxItemDefinitionCustomBuildStep() {}

VcxItemDefinitionResourceCompile::VcxItemDefinitionResourceCompile() {}
VcxItemDefinitionResourceCompile::~VcxItemDefinitionResourceCompile() {}

int VcxItemDefinitionResourceCompile::MatchPreprocessorDefinitions(std::string Name, std::string AttrName)
{
	std::regex PreprocessorDefinitions;
	std::string Pattern("PreprocessorDefinitions");
	PreprocessorDefinitions = Pattern;
	if (std::regex_search(Name, PreprocessorDefinitions)) {
		return true;
	}
	return false;
}

int VcxItemDefinitionResourceCompile::Load(TiXmlElement* ResourceCompile)
{
	TiXmlElement* ResourceCompileChild;
	if (ResourceCompile == NULL) {
		return NULL;
	}
	ResourceCompileChild = ResourceCompile->FirstChildElement();
	for (; ResourceCompileChild; ResourceCompileChild = ResourceCompileChild->NextSiblingElement()) {
		if (ResourceCompileChild->FirstAttribute() == NULL) {
			;
		}
		if (MatchPreprocessorDefinitions(ResourceCompileChild->Value(), "")) {
			DumpXmlNode(std::cout, ResourceCompileChild);
			// GetFirstAttrValue(ItemDefinitionGroup, platform);
			// cmakeFile.write(ResourceCompileChild->GetText());
			// cmakeFile.writeOptList(ResourceCompileChild->GetText(), platform, CMakeFile::FILE_TYPE_RES);
		}
	}
	return 0;
}

VcxItemDefinitionGroup::VcxItemDefinitionGroup() {}
VcxItemDefinitionGroup::~VcxItemDefinitionGroup() {}

int VcxItemDefinitionGroup::MatchClCompile(TiXmlElement* itemDefinitionGroupChild, TiXmlElement*& ItemDefinitionGroupClCompile)
{
	std::string platform;
	if (itemDefinitionGroupChild == NULL) return NULL;
	TiXmlElement* ClCompile = NULL;
	TiXmlElement* ClCompileChild = NULL;
	ClCompile = itemDefinitionGroupChild;
	for (; ClCompile; ClCompile = ClCompile->NextSiblingElement()) {
		if (ClCompile->Value() != NULL && std::string(ClCompile->Value()) == std::string("ClCompile")) {
			ItemDefinitionGroupClCompile = ClCompile;
			return true;
		}
	}
	return false;
}

int VcxItemDefinitionGroup::MatchLib(TiXmlElement* itemDefinitionGroupChild, TiXmlElement*& ItemDefinitionGroupLib)
{
	std::string platform;
	if (itemDefinitionGroupChild == NULL) return NULL;
	TiXmlElement* Lib = NULL;
	TiXmlElement* LibChild = NULL;
	Lib = itemDefinitionGroupChild;
	for (; Lib; Lib = Lib->NextSiblingElement()) {
		if (Lib->Value() != NULL) {
			if (std::string(Lib->Value()) == std::string("Lib") || std::string(Lib->Value()) == std::string("Link")) {
				ItemDefinitionGroupLib = Lib;
				return true;
			}
		}
	}
	return false;
}

int VcxItemDefinitionGroup::MatchNASM(TiXmlElement* itemDefinitionGroupChild, TiXmlElement*& ItemDefinitionGroupNASM)
{
	std::string platform;
	if (itemDefinitionGroupChild == NULL) return NULL;
	TiXmlElement* NASM = NULL;
	TiXmlElement* NASMChild = NULL;
	NASM = itemDefinitionGroupChild;
	for (; NASM; NASM = NASM->NextSiblingElement()) {
		if (NASM->Value() != NULL && std::string(NASM->Value()) == std::string("NASM")) {
			ItemDefinitionGroupNASM = NASM;
			return true;
		}
	}
	return false;
}

int VcxItemDefinitionGroup::MatchResourceCompile(TiXmlElement* itemDefinitionGroupChild, TiXmlElement*& ItemDefinitionGroupResourceCompile)
{
	std::string platform;
	if (itemDefinitionGroupChild == NULL) return NULL;
	TiXmlElement* ResourceCompile = NULL;
	TiXmlElement* ResourceCompileChild = NULL;
	ResourceCompile = itemDefinitionGroupChild;
	for (; ResourceCompile; ResourceCompile = ResourceCompile->NextSiblingElement()) {
		if (ResourceCompile->Value() != NULL && std::string(ResourceCompile->Value()) == std::string("NASM")) {
			ItemDefinitionGroupResourceCompile = ResourceCompile;
			return true;
		}
	}
	return false;
}


int VcxItemDefinitionGroup::Load(TiXmlElement* itemDefinitionGroup)
{
	TiXmlElement* itemDefinitionGroupChild = NULL;
	TiXmlElement* itemDefinitionGroupChildDefs = NULL;
	if (itemDefinitionGroup == NULL) return NULL;

	DumpXmlNode(std::cout, itemDefinitionGroup);

	itemDefinitionGroupChild = itemDefinitionGroup->FirstChildElement();

	for (; itemDefinitionGroupChild; itemDefinitionGroupChild = itemDefinitionGroupChild->NextSiblingElement()) 
	{
		if (MatchClCompile(itemDefinitionGroupChild, itemDefinitionGroupChildDefs)) {
			DumpXmlNode(std::cout, itemDefinitionGroupChild);
			m_clCompile[itemDefinitionGroupChild->Value()] = std::make_shared<VcxItemDefinitionClCompile>();
			m_clCompile[itemDefinitionGroupChild->Value()]->Load(itemDefinitionGroupChildDefs);
		}
		if (MatchLib(itemDefinitionGroupChild, itemDefinitionGroupChildDefs)) {
			DumpXmlNode(std::cout, itemDefinitionGroupChild);
			m_lib[itemDefinitionGroupChild->Value()] = std::make_shared<VcxItemDefinitionLib>();
			m_lib[itemDefinitionGroupChild->Value()]->Load(itemDefinitionGroupChildDefs);
		}
		if (MatchNASM(itemDefinitionGroupChild, itemDefinitionGroupChildDefs)) {
			DumpXmlNode(std::cout, itemDefinitionGroupChild);
			m_nasm[itemDefinitionGroupChild->Value()] = std::make_shared<VcxItemDefinitionNASM>();
			m_nasm[itemDefinitionGroupChild->Value()]->Load(itemDefinitionGroupChildDefs);
		}
		if (MatchResourceCompile(itemDefinitionGroupChild, itemDefinitionGroupChildDefs)) {
			DumpXmlNode(std::cout, itemDefinitionGroupChild);
			m_resCompile[itemDefinitionGroupChild->Value()] = std::make_shared<VcxItemDefinitionResourceCompile>();
			m_resCompile[itemDefinitionGroupChild->Value()]->Load(itemDefinitionGroupChildDefs);
		}
	}

	return 0;
}

VcxImportGroupTargetClCompile::VcxImportGroupTargetClCompile() {}
VcxImportGroupTargetClCompile::~VcxImportGroupTargetClCompile() {}

VcxImportGroupTargetClInclude::VcxImportGroupTargetClInclude() {}
VcxImportGroupTargetClInclude::~VcxImportGroupTargetClInclude() {}

VcxImportGroupTargetNone::VcxImportGroupTargetNone() {}
VcxImportGroupTargetNone::~VcxImportGroupTargetNone() {}

VcxImportGroupTargetNASM::VcxImportGroupTargetNASM() {}
VcxImportGroupTargetNASM::~VcxImportGroupTargetNASM() {}

VcxImportGroupTargetText::VcxImportGroupTargetText() {}
VcxImportGroupTargetText::~VcxImportGroupTargetText() {}

VcxImportGroupTargetResourceCompile::VcxImportGroupTargetResourceCompile() {}
VcxImportGroupTargetResourceCompile::~VcxImportGroupTargetResourceCompile() {}

VcxImportGroupTarget::VcxImportGroupTarget() {}
VcxImportGroupTarget::~VcxImportGroupTarget() {}

int VcxImportGroupTarget::Load(TiXmlElement* importGroupTarget)
{
	if (importGroupTarget == NULL) return NULL;

	DumpXmlNode(std::cout, importGroupTarget);

	return 0;
}

VcxProject::VcxProject() {}
VcxProject::~VcxProject() {}

int VcxProject::MatchItemGroup(TiXmlElement *projChild, std::string AttrName)
{
	std::regex ItemGroup;
	std::string ElementPattern("ItemGroup");
	std::string AttrPattern("");

	ItemGroup = ElementPattern;
	if (std::regex_search(projChild->Value(), ItemGroup)) {
		ItemGroup = AttrPattern;
		return true;
	}
	return false;
}

int VcxProject::MatchPropertyGroup(TiXmlElement* projChild, std::string AttrName)
{
	std::regex PropertyGroup;
	std::string ElementPattern("PropertyGroup");
	std::string AttrPattern("^Condition.*");
	
	PropertyGroup = ElementPattern;
	if (!std::regex_search(projChild->Value(), PropertyGroup)) {
		return false;
	}
	if (AttrName.length() == 0) {
		return true;
	}
	PropertyGroup = AttrPattern;

	TiXmlAttribute* attr;
	attr = projChild->FirstAttribute();
	do {
		if (attr->Value() && std::regex_search(attr->Value(), PropertyGroup)) {
			return true;
		}
	} while ((attr != projChild->LastAttribute()) && (attr = attr->Next()));
	return false;

}

int VcxProject::MatchImportGroup(TiXmlElement* projChild, std::string AttrName)
{
	std::regex ImportGroup;
	std::string ElementPattern("ImportGroup");
	std::string AttrPattern("^Condition.*");

	ImportGroup = ElementPattern;
	if (!std::regex_search(projChild->Value(), ImportGroup)) {
		return false;
	}
	if (AttrName.length() == 0) {
		return true;
	}
	ImportGroup = AttrPattern;

	TiXmlAttribute* attr;
	attr = projChild->FirstAttribute();
	do {
		if (attr->Value() && std::regex_search(attr->Value(), ImportGroup)) {
			return true;
		}
	} while ((attr != projChild->LastAttribute()) && (attr = attr->Next()));
	return false;
}

int VcxProject::MatchItemDefinitionGroup(TiXmlElement* projChild, std::string AttrName)
{
	std::regex ItemDefinitionGroup;
	std::string ElementPattern("ItemDefinitionGroup");
	std::string AttrPattern("^Condition.*");

	ItemDefinitionGroup = ElementPattern;
	if (!std::regex_search(projChild->Value(), ItemDefinitionGroup)) {
		return false;
	}
	if (AttrName.length() == 0) {
		return true;
	}
	ItemDefinitionGroup = AttrPattern;

	TiXmlAttribute* attr;
	attr = projChild->FirstAttribute();
	do {
		if (attr->Value() && std::regex_search(attr->Value(), ItemDefinitionGroup)) {
			return true;
		}
	} while ((attr != projChild->LastAttribute()) && (attr = attr->Next()));
	return false;
}

int VcxProject::Load(TiXmlElement* Project)
{
	if (Project == NULL) return NULL;
	TiXmlElement* ProjectChild = NULL; // Condition=""
	ProjectChild = Project->FirstChildElement();
	for (; ProjectChild; ProjectChild = ProjectChild->NextSiblingElement()) {
		if (ProjectChild->FirstAttribute() == NULL) {
			;
		}
		if (MatchItemGroup(ProjectChild, "")) {
			m_itemGroup[ProjectChild->Value()] = std::make_shared<VcxItemGroup>();
			m_itemGroup[ProjectChild->Value()]->Load(ProjectChild);
		} 
		else if (MatchPropertyGroup(ProjectChild, "")) {
			m_propertyGroupCfg[ProjectChild->Value()] = std::make_shared<VcxPropertyGroupCfg>();
			m_propertyGroupCfg[ProjectChild->Value()]->Load(ProjectChild);
		}
		else if (MatchImportGroup(ProjectChild, "")) {
			m_importGroupSet[ProjectChild->Value()] = std::make_shared<VcxImportGroupSet>();
			m_importGroupSet[ProjectChild->Value()]->Load(ProjectChild);
		}
		else if (MatchItemDefinitionGroup(ProjectChild, "")) {
			m_itemDefinitionGroup[ProjectChild->Value()] = std::make_shared<VcxItemDefinitionGroup>();
			m_itemDefinitionGroup[ProjectChild->Value()]->Load(ProjectChild);
		}
	}
	return 0;
}

VcxProjParser::VcxProjParser() {}
VcxProjParser::~VcxProjParser() {}

int VcxProjParser::Load(const std::string& projVcxproj)
{
	bool ret;
	TiXmlDocument doc;
	TiXmlElement* root;
	ret = doc.LoadFile(projVcxproj.c_str(), TIXML_ENCODING_UTF8);
	if (!ret) {
		std::cout << projVcxproj << std::endl;
		return -1;
	}

	root = doc.RootElement();
	m_vcxProj.Load(root);
	return 0;
}