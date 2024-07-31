#ifndef XML_ADAPTER_H
#define XML_ADAPTER_H

#include "tinyxml.h"

void DumpXmlNode(std::ostream &os, TiXmlElement* name);
void DumpXmlAttribute(std::ostream& os, TiXmlAttribute* attr);

#endif