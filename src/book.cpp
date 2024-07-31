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

void DumpXmlDeclaration(TiXmlDeclaration *decl)
{
	if (NULL != decl)
	{
		std::cout << decl->Version() << std::endl;
		std::cout << decl->Standalone() << std::endl;
		std::cout << decl->Encoding() << std::endl;
	}
}

TiXmlElement *FindChapter(TiXmlElement* body, const char *chapter)
{
	if (body == NULL) return NULL;
	TiXmlElement* chapterXml = NULL;
	chapterXml = body->FirstChildElement();
	DumpXmlNode(std::cout, chapterXml);
	return chapterXml;
}

TiXmlElement *FindLastChapter(TiXmlElement* body, const char *chapter)
{
	if (body == NULL) return NULL;
	TiXmlElement* chapterXml = NULL;
	chapterXml = body->FirstChildElement();
	for (; chapterXml != NULL && chapterXml->NextSiblingElement() != NULL; chapterXml = (TiXmlElement*)chapterXml->NextSiblingElement()) {}
	DumpXmlNode(std::cout, chapterXml);
	return chapterXml;
}

TiXmlElement *AppendChapter(TiXmlElement* body, const char *chapter, const char *pageno)
{
	TiXmlElement *n = new TiXmlElement("div");
	n->SetAttribute("class", "kindle-cn-toc-level-1");
	TiXmlElement *a = new TiXmlElement("a");
	a->SetAttribute("href", pageno);
	TiXmlText newText(chapter);
	a->LinkEndChild(&newText);
	n->InsertEndChild(*a);
	body->InsertEndChild(*n);
	return FindLastChapter(body, NULL);
}

TiXmlElement *FindSection(TiXmlElement* chapter, const char *section)
{
	if (chapter == NULL) return NULL;
	TiXmlElement* sectionXml = NULL;
	sectionXml = chapter->FirstChildElement();
	sectionXml = sectionXml->NextSiblingElement();
	DumpXmlNode(std::cout, sectionXml);
	return sectionXml;
}

TiXmlElement *FindLastSection(TiXmlElement* chapter, const char *section)
{
	if (chapter == NULL) return NULL;
	TiXmlElement* sectionXml = NULL;
	sectionXml = chapter->FirstChildElement();
	int idx = 0;
	for (; sectionXml != NULL && sectionXml->NextSiblingElement() != NULL; sectionXml = sectionXml->NextSiblingElement()) {
		idx++;
	}
	if (idx == 0) {
		return NULL;
	} else {
		DumpXmlNode(std::cout, sectionXml);
		return sectionXml;
	}
}

TiXmlElement *AppendSection(TiXmlElement* chapter, const char *section, const char *pageno)
{
	TiXmlElement *n = new TiXmlElement("div");
	n->SetAttribute("class", "kindle-cn-toc-level-2");
	TiXmlElement *a = new TiXmlElement("a");
	a->SetAttribute("href", pageno);
	TiXmlText newText(section);
	a->LinkEndChild(&newText);
	n->InsertEndChild(*a);
	chapter->InsertEndChild(*n);
	return FindLastSection(chapter, NULL);
}

TiXmlElement *FindLesson(TiXmlElement* section, const char *lesson)
{
	if (section == NULL) return NULL;
	TiXmlElement* lessonXml = NULL;
	lessonXml = section->FirstChildElement();
	lessonXml = lessonXml->NextSiblingElement();
	DumpXmlNode(std::cout, lessonXml);
	return lessonXml;
}

TiXmlElement *FindLastLesson(TiXmlElement* section, const char *lesson)
{
	if (section == NULL) return NULL;
	TiXmlElement* lessonXml = NULL;
	lessonXml = (TiXmlElement*)section->FirstChildElement();
	int idx = 0;
	for (; lessonXml != NULL && lessonXml->NextSiblingElement() != NULL; lessonXml = lessonXml->NextSiblingElement()) {
		idx++;
	}
	if (idx == 0) {
		return NULL;
	} else {
		DumpXmlNode(std::cout, lessonXml);
		return lessonXml;
	}
}

TiXmlElement *AppendLesson(TiXmlElement* section, const char *lesson, const char *pageno)
{
	TiXmlElement *n = new TiXmlElement("div");
	n->SetAttribute("class", "kindle-cn-toc-level-3");
	TiXmlElement *a = new TiXmlElement("a");
	a->SetAttribute("href", pageno);
	TiXmlText newText(lesson);
	a->LinkEndChild(&newText);
	n->InsertEndChild(*a);
	section->InsertEndChild(*n);
	return FindLastSection(section, NULL);
}

TiXmlElement *AppendChapterByBody(TiXmlElement* body, const char *chapter, const char *pageno)
{
	if (body == NULL) {
		return NULL;
	}
	return AppendChapter(body, chapter, pageno);
}

TiXmlElement *AppendSectionByBody(TiXmlElement* body, const char *section, const char *pageno)
{
	if (body == NULL) {
		return NULL;
	}
	TiXmlElement *chapter = FindLastChapter(body, NULL);
	if (chapter == NULL) {
		return AppendSection(body, section, pageno);
	}
	return AppendSection(chapter, section, pageno);
}

TiXmlElement *AppendLessonByBody(TiXmlElement* body, const char *lesson, const char *pageno)
{
	if (body == NULL) {
		return NULL;
	}
	TiXmlElement *chapter = FindLastChapter(body, NULL);
	if (chapter == NULL) {
		return AppendSection(body, lesson, pageno);
	}
	TiXmlElement *section = FindLastSection(chapter, NULL);
	if (section == NULL) {
		return AppendSection(chapter, lesson, pageno);
	}
	return AppendSection(section, lesson, pageno);
}

class XmlCfg
{
public:
	XmlCfg()
	{
	}
	bool MatchChapter(std::string chapter)
	{
        std::string pattern(MY_TEXT("^\\s*Unit.*|^\\s*Chapter\\s*\\d*|^\\s*第.*章|^\\s*\\d{1,}[^\\.\\d].*|^\\s*\\d{1,}\\.[^\\d].*"));
        m_chapter=(pattern);
		return std::regex_search(chapter, m_chapter);
	}

	int MatchSection(std::string section)
	{
        std::string pattern(MY_TEXT("^\\s*第.*节|^\\s*\\d{1,}\\.\\d{1,3}[^\\.\\d].*|^\\s*\\d{1,}\\.\\d{1,}\\.\\D.*"));
        m_section = (pattern);
        return std::regex_search(section, m_section);
	}

	int MatchLesson(std::string lesson)
	{
        std::string pattern("^\\s*lesson.*|^\\s*\\d{1,}\\.\\d{1,}\\.\\d{1,}[^\\.\\d].*|^\\s*\\d{1,}\\.\\d{1,}\\.\\d{1,}\\.\\D.*");
		m_lesson = (pattern);
        return std::regex_search(lesson, m_lesson);
	}
	int MatchType(std::string title) 
	{
		std::cout << "title:" << title << std::endl;
		if (MatchChapter(title)) {
			return 1;
		}
		if (MatchSection(title)) {
			return 2;
		}
		if (MatchLesson(title)) {
			return 3;
		}
		return 3;
	}
private:
	std::regex m_chapter;
	std::regex m_section;
	std::regex m_lesson;
};

XmlCfg cfg;

int ParseLine(const std::string& lineSrc, std::vector<std::string>& allCol)
{
    char oper = ' ';
    std::string line = lineSrc;
    line.erase(lineSrc.find_last_not_of(oper) + 1, lineSrc.length());

	for (int idxCol = 0; idxCol < line.length(); ++idxCol)
	{
		int idxTab = line.find(oper, idxCol);
		std::string element;
		if (idxTab == -1) {
			element = line.substr(idxCol, line.length() - idxCol);
			allCol.push_back(element);
			break;
		}
		else {
			element = line.substr(idxCol, idxTab - idxCol);
			idxCol = idxTab;
			if (element.length() > 0) {
				allCol.push_back(element);
			}
		}
	}
	return 0;
}

int ProcTxtToMap(const std::string &file, std::map<int, std::vector<std::string> >& allData)
{
	std::fstream s(file, std::fstream::in | std::fstream::out);
	int idxRow = 0;
	for (std::string line; std::getline(s, line); ++idxRow) {
		std::vector<std::string> allCol;
		if (259 == idxRow) {
			std::cout << "idxRow:" << idxRow << std::endl;
		}
		ParseLine(line, allCol);
		if (allCol.size() != 0) {
			allData[idxRow] = allCol;
		}
		else {
			--idxRow;
		}
	}
	return 0;
}

int WriteMapToTxt(const std::string &file, std::map<int, std::vector<std::string> >& allData)
{
    std::fstream s(file, std::fstream::out | std::fstream::trunc);
    for (auto iterLine = allData.begin(); iterLine != allData.end(); ++iterLine) {
        for (auto iterWord = iterLine->second.begin(); iterLine->second.end() != iterWord; ++iterWord) {
            s << *iterWord << " ";
        }
        s << std::endl;
    }
    return 0;
}

int MergeLastWord(std::vector<std::string> & line)
{
	int idx = 0;
	std::string tail = *line.rbegin();
	for (auto iterWord = line.rbegin(); iterWord != line.rend(); ++iterWord, ++idx) {
		if (1 == idx) {
			*iterWord += tail;
			line.pop_back();
			break;
		}
	}
	return 0;
}

int SplitPageNoFromTxt(const std::string &file)
{
    std::string txtFile = file + ".txt";
    std::map<int, std::vector<std::string> > allData;
    ProcTxtToMap(txtFile, allData);
    for (auto iterLine = allData.begin(); iterLine != allData.end(); ++iterLine) {
		if (iterLine->first == 259) {
			std::cout << "line:" << iterLine->first << std::endl;
		}
        MergeLastWord(iterLine->second);
		auto iterWord = iterLine->second.rbegin();
        if (iterLine->second.rend() == iterWord) {
            continue;
        }
        for (auto iterCh = iterWord->crbegin(); iterCh != iterWord->crend(); ++iterCh) {
            if ((*iterCh) < '0' || (*iterCh) > '9') {
                iterWord->insert(iterCh.base(), 8, MY_TEXT(' '));
                break;
            }
        }
    }
    WriteMapToTxt(txtFile + ".txt", allData);
    return 0;
}

int InitXmlDoc(const char* docName)
{
    TiXmlDocument doc;
    doc.LoadFile(docName, TIXML_ENCODING_LEGACY);
    TiXmlElement* root = doc.RootElement();
    if (NULL != root) {
        doc.Clear();
    }
    TiXmlElement html("html");
    TiXmlElement head("head");
    TiXmlElement body("body");
    html.InsertEndChild(head);
    html.InsertEndChild(body);
    doc.InsertEndChild(html);
    doc.SaveFile(docName);
    return 0;
}

int GenXmlDoc(const char* docName, int type, const char *title, const char *pageno)
{
	TiXmlDocument doc;
	doc.LoadFile(docName, TIXML_ENCODING_LEGACY);

	if (docName == NULL || title == NULL || pageno == NULL) {
		return 0;
	}
	/*<?xml version="1.0" standalone="yes"?>,
	*/
	TiXmlNode * node = doc.FirstChild();
	if (NULL != node) {
		TiXmlDeclaration *decl = node->ToDeclaration();
		DumpXmlDeclaration(decl);
	}

	TiXmlElement* root = doc.RootElement();
	TiXmlElement* body = NULL;
	if (NULL != root) {
		TiXmlElement* head = root->FirstChildElement();
		DumpXmlNode(std::cout, head);
		if (head) {
			body = head->NextSiblingElement();
			DumpXmlNode(std::cout, body);
		}
	}
	if (type == 1) {
		TiXmlElement *chpater = AppendChapterByBody(body, title, pageno);
	} else if (type == 2) {
		TiXmlElement *section = AppendSectionByBody(body, title, pageno);
	} else {
		TiXmlElement *lesson = AppendLessonByBody(body, title, pageno);
	}
	doc.SaveFile(docName);
	return 0;
}

int IsPageNo(const std::string &pageNo)
{
	for (std::string::const_iterator iter = pageNo.cbegin(); iter != pageNo.cend(); ++iter) {
		if (*iter < '0' || *iter > '9') {
			return false;
		}
	}
    return true;
}

int XmlMain(std::string fname, int pageoffset)
{
    InitXmlDoc((fname + ".xml").c_str());
	int idx = 0;
	int type = 0;
	std::string pageno = "1";
	std::string title;
	std::string file = (fname + ".txt");
	std::map<int, std::vector<std::string> > allData;
	ProcTxtToMap(file.c_str(), allData);
	std::map<int, std::vector<std::string> >::iterator lineIter;
	std::vector<std::string>::iterator wordIter;
	for (lineIter = allData.begin(); allData.end() != lineIter; lineIter++)
	{
		for (idx = 0, title = "", wordIter = lineIter->second.begin(); lineIter->second.end() != wordIter; wordIter++) {
			if (lineIter->second.size() == ++idx && IsPageNo(*wordIter)) {
				pageno = *wordIter;
				break;
			}
			title += *wordIter;
		}
		pageno = std::to_string(atoi(pageno.c_str()) + pageoffset);
		type = cfg.MatchType(title);
		GenXmlDoc((fname + ".xml").c_str(), type, title.c_str(), pageno.c_str());
	}
	return 0;
}
