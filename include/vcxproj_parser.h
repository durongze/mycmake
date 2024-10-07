#ifndef MY_VCX_PROJ_PARSER_H
#define MY_VCX_PROJ_PARSER_H

#include <string>
#include "tinyxml.h"
#include <map>
#include <string>
#include <memory>

class VcxItemGroupProjCfg {
public:
	VcxItemGroupProjCfg(const std::string& cfg, const std::string &platform);
	~VcxItemGroupProjCfg();
private:
	std::string m_cfg;
	std::string m_platform;
};
using VcxItemGroupProjCfgPtr = std::shared_ptr<VcxItemGroupProjCfg>;

class VcxItemGroupNone {
public:
	VcxItemGroupNone();
	~VcxItemGroupNone();
private:

};

class VcxItemGroupResourceCompile {
public:
	VcxItemGroupResourceCompile();
	~VcxItemGroupResourceCompile();
private:

};
using VcxItemGroupResourceCompilePtr = std::shared_ptr<VcxItemGroupResourceCompile>;

class VcxItemGroupNASM {
public:
	VcxItemGroupNASM();
	~VcxItemGroupNASM();
private:

};
using VcxItemGroupNASMPtr = std::shared_ptr<VcxItemGroupNASM>;

class VcxItemGroupClCompile {
public:
	VcxItemGroupClCompile();
	~VcxItemGroupClCompile();
private:

};
using VcxItemGroupClCompilePtr = std::shared_ptr<VcxItemGroupClCompile>;

class VcxItemGroupClInclude {
public:
	VcxItemGroupClInclude();
	~VcxItemGroupClInclude();
private:

};
using VcxItemGroupClIncludePtr = std::shared_ptr<VcxItemGroupClInclude>;

class VcxItemGroup{
public:
	VcxItemGroup();
	~VcxItemGroup();
public:
	int MatchProjectConfiguration(std::string ElementName, std::string AttrName);
	int MatchNASM(std::string Name, std::string AttrName);
	int MatchClCompile(std::string Name, std::string AttrName);
	int MatchClInclude(std::string Name, std::string AttrName);
	int MatchResourceCompile(std::string Name, std::string AttrName);
	int Load(TiXmlElement* Project);
private:
	VcxItemGroupProjCfgPtr m_cfg;
	std::map<std::string, int> m_projCfg;
	std::map<std::string, int> m_asmMap;
	std::map<std::string, int> m_cppMap;
	std::map<std::string, int> m_incMap;
	std::map<std::string, int> m_resMap;
};

using VcxItemGroupPtr = std::shared_ptr<VcxItemGroup>;

class VcxPropertyGroupCfg {
public:
	VcxPropertyGroupCfg();
	~VcxPropertyGroupCfg();
public:
	int Match(std::string Name, std::string AttrName);
	int Load(TiXmlElement* Project);
private:
	std::string m_cfgType;
};
using VcxPropertyGroupCfgPtr = std::shared_ptr<VcxPropertyGroupCfg>;

class VcxImportGroupSet {
public:
	VcxImportGroupSet();
	~VcxImportGroupSet();
public:
	int Match(std::string Name, std::string AttrName);
	int Load(TiXmlElement* Project);
private:
};
using VcxImportGroupSetPtr = std::shared_ptr<VcxImportGroupSet>;

class VcxPropertyGroupTarget {
public:
	VcxPropertyGroupTarget();
	~VcxPropertyGroupTarget();
public:
	int Match(std::string Name, std::string AttrName);
	int Load(TiXmlElement* Project);
private:
	std::string m_targetName;
};
using VcxPropertyGroupTargetPtr = std::shared_ptr<VcxPropertyGroupTarget>;

class VcxItemDefinitionClCompile
{
public:
	VcxItemDefinitionClCompile();
	~VcxItemDefinitionClCompile();
public:
	int MatchPreprocessorDefinitions(std::string ElementName, std::string AttrName);
	int Load(TiXmlElement* itemDefinitionGroup);
};
using VcxItemDefinitionClCompilePtr = std::shared_ptr<VcxItemDefinitionClCompile>;

class VcxItemDefinitionLib
{
public:
	VcxItemDefinitionLib();
	~VcxItemDefinitionLib();
public:
	int MatchAdditionalDependencies(std::string Name, std::string AttrName);
	int Load(TiXmlElement* itemDefinitionGroup);
};
using VcxItemDefinitionLibPtr = std::shared_ptr<VcxItemDefinitionLib>;

class VcxItemDefinitionNASM
{
public:
	VcxItemDefinitionNASM();
	~VcxItemDefinitionNASM();
public:
	int MatchPreprocessorDefinitions(std::string Name, std::string AttrName);
	int Load(TiXmlElement* itemDefinitionGroup);
};
using VcxItemDefinitionNASMPtr = std::shared_ptr<VcxItemDefinitionNASM>;

class VcxItemDefinitionPreBuildEvent
{
public:
	VcxItemDefinitionPreBuildEvent();
	~VcxItemDefinitionPreBuildEvent();
};
using VcxItemDefinitionPreBuildEventPtr = std::shared_ptr<VcxItemDefinitionPreBuildEvent>;

class VcxItemDefinitionPostBuildEvent
{
public:
	VcxItemDefinitionPostBuildEvent();
	~VcxItemDefinitionPostBuildEvent();
};

class VcxItemDefinitionCustomBuildStep
{
public:
	VcxItemDefinitionCustomBuildStep();
	~VcxItemDefinitionCustomBuildStep();
};

class VcxItemDefinitionResourceCompile
{
public:
	VcxItemDefinitionResourceCompile();
	~VcxItemDefinitionResourceCompile();
public:
	int MatchPreprocessorDefinitions(std::string Name, std::string AttrName);
	int Load(TiXmlElement* itemDefinitionGroup);
};
using VcxItemDefinitionResourceCompilePtr = std::shared_ptr<VcxItemDefinitionResourceCompile>;

class VcxItemDefinitionGroup {
public:
	VcxItemDefinitionGroup();
	~VcxItemDefinitionGroup();
public:
	int MatchClCompile(TiXmlElement* itemDefinitionGroup, TiXmlElement*& ItemDefinitionGroupClCompile);
	int MatchLib(TiXmlElement* itemDefinitionGroup, TiXmlElement*& ItemDefinitionGroupLib);
	int MatchNASM(TiXmlElement* itemDefinitionGroup, TiXmlElement*& ItemDefinitionGroupNASM);
	int MatchResourceCompile(TiXmlElement* itemDefinitionGroup, TiXmlElement*& ItemDefinitionGroupResourceCompile);

	int Load(TiXmlElement* Project);
private:
	std::map <std::string, VcxItemDefinitionClCompilePtr> m_clCompile;
	std::map <std::string, VcxItemDefinitionLibPtr> m_lib;
	std::map <std::string, VcxItemDefinitionNASMPtr> m_nasm;
	std::map <std::string, VcxItemDefinitionResourceCompilePtr> m_resCompile;
};
using VcxItemDefinitionGroupPtr = std::shared_ptr<VcxItemDefinitionGroup>;

class VcxImportGroupTargetClCompile {
public:
	VcxImportGroupTargetClCompile();
	~VcxImportGroupTargetClCompile();
private:
};
using VcxImportGroupTargetClCompilePtr = std::shared_ptr<VcxImportGroupTargetClCompile>;

class VcxImportGroupTargetClInclude {
public:
	VcxImportGroupTargetClInclude();
	~VcxImportGroupTargetClInclude();
private:
};
using VcxImportGroupTargetClIncludePtr = std::shared_ptr<VcxImportGroupTargetClInclude>;

class VcxImportGroupTargetNone {
public:
	VcxImportGroupTargetNone();
	~VcxImportGroupTargetNone();
private:
};

class VcxImportGroupTargetNASM {
public:
	VcxImportGroupTargetNASM();
	~VcxImportGroupTargetNASM();
private:
};
using VcxImportGroupTargetNASMPtr = std::shared_ptr<VcxImportGroupTargetNASM>;

class VcxImportGroupTargetText {
public:
	VcxImportGroupTargetText();
	~VcxImportGroupTargetText();
private:
};

class VcxImportGroupTargetResourceCompile {
public:
	VcxImportGroupTargetResourceCompile();
	~VcxImportGroupTargetResourceCompile();
private:
};
using VcxImportGroupTargetResourceCompilePtr = std::shared_ptr<VcxImportGroupTargetResourceCompile>;

class VcxImportGroupTarget {
public:
	VcxImportGroupTarget();
	~VcxImportGroupTarget();
public:
	int Match(std::string Name, std::string AttrName);
	int Load(TiXmlElement* Project);
private:
};
using VcxImportGroupTargetPtr = std::shared_ptr<VcxImportGroupTarget>;

class VcxProject {
public:
	VcxProject();
	~VcxProject();
public:
	int Load(TiXmlElement* root);
private:
	int MatchItemGroup(TiXmlElement* projChild, std::string AttrName);
	int MatchPropertyGroup(TiXmlElement* projChild, std::string AttrName);
	int MatchImportGroup(TiXmlElement* projChild, std::string AttrName);
	int MatchItemDefinitionGroup(TiXmlElement* projChild, std::string AttrName);
private:
	std::map <std::string, VcxItemGroupPtr> m_itemGroup;
	std::map < std::string, VcxPropertyGroupCfgPtr> m_propertyGroupCfg;
	std::map < std::string, VcxImportGroupSetPtr> m_importGroupSet;
	std::map < std::string, VcxItemDefinitionGroupPtr> m_itemDefinitionGroup;
};


class VcxProjParser{
public :
	VcxProjParser();
	~VcxProjParser();
public:
	int Load(const std::string& fileName);
private:
	VcxProject m_vcxProj;
};

#endif