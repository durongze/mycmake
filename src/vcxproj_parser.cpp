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

int VcxItemGroup::Match(std::string Name, std::string AttrName)
{
	return false;
}

int VcxItemGroup::Load(TiXmlElement* itemGroup)
{
	if (itemGroup == NULL) return NULL;

	DumpXmlNode(std::cout, itemGroup);

	// Find_ItemGroup_NASM(cmakeFile, itemGroup);
	// Find_ItemGroup_ClCompile(cmakeFile, itemGroup);
	// Find_ItemGroup_ClInclude(cmakeFile, itemGroup); // error
	// Find_ItemGroup_ResourceCompile(cmakeFile, itemGroup); // error

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

VcxItemDefinitionLib::VcxItemDefinitionLib() {}
VcxItemDefinitionLib::~VcxItemDefinitionLib() {}

VcxItemDefinitionNASM::VcxItemDefinitionNASM() {}
VcxItemDefinitionNASM::~VcxItemDefinitionNASM() {}

VcxItemDefinitionPreBuildEvent::VcxItemDefinitionPreBuildEvent() {}
VcxItemDefinitionPreBuildEvent::~VcxItemDefinitionPreBuildEvent() {}

VcxItemDefinitionPostBuildEvent::VcxItemDefinitionPostBuildEvent() {}
VcxItemDefinitionPostBuildEvent::~VcxItemDefinitionPostBuildEvent() {}

VcxItemDefinitionCustomBuildStep::VcxItemDefinitionCustomBuildStep() {}
VcxItemDefinitionCustomBuildStep::~VcxItemDefinitionCustomBuildStep() {}

VcxItemDefinitionGroup::VcxItemDefinitionGroup() {}
VcxItemDefinitionGroup::~VcxItemDefinitionGroup() {}

int VcxItemDefinitionGroup::Load(TiXmlElement* itemDefinitionGroup)
{
	if (itemDefinitionGroup == NULL) return NULL;

	DumpXmlNode(std::cout, itemDefinitionGroup);

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