#ifndef XML_ADAPTER_H
#define XML_ADAPTER_H

#include "tinyxml.h"

#define DumpXmlNode(os, xmlName) MyDumpXmlNode(os, xmlName, __FUNCTION__)

void GetFirstAttrValue(TiXmlElement* name, std::string& platform);
void MyDumpXmlNode(std::ostream &os, TiXmlElement* name, const std::string func);
void MyDumpXmlAttribute(std::ostream& os, TiXmlAttribute* attr);

#endif