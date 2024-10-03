#include <iostream>

#include "tinyxml.h"
#include "xml_adapter.h"
#include "my_sys.h"

void GetFirstAttrValue(TiXmlElement* name, std::string &platform)
{
	TiXmlAttribute* attr;
	if (NULL != name)
	{
		attr = name->FirstAttribute();
		if (attr) {
			platform = attr->Value();
		}
	}
}

void DumpXmlNode(std::ostream& os, TiXmlElement* name)
{
	TiXmlAttribute* attr;
	if (NULL != name)
	{
		if (NULL != name->Value()) {
			os << "Value:" << name->Value() << " ";
		}
		if (NULL != name->GetText()) {
			os << "Text:" << name->GetText() << " ";
		}
		os << std::endl;
		attr = name->FirstAttribute();
		do {
			DumpXmlAttribute(os, attr);
		} while ((attr != name->LastAttribute()) && (attr = attr->Next()));
		os << std::endl;
		TiXmlElement *aXml = name->FirstChildElement();
		// DumpXmlNode(os, aXml);
	}
}

void DumpXmlAttribute(std::ostream& os, TiXmlAttribute* attr)
{
	if (attr) {
		os << "\tAttr:" << attr->Name() << "=" << attr->Value() << std::endl;
	}
}