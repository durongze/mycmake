
#include <iostream>
#include <fstream>
#include <string>
#include <strstream>
#include <map>
#include <vector>
#include <regex>
#include <iostream>
#include <iomanip>
#include <filesystem>

#include "tinyxml.h"
#include "xml_adapter.h"
#include "my_sys.h"
#include "my_fs.h"
#include "cmake_file.h"
#include "vcxproj_file.h"
#include "vcxproj_path.h"

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

int AppMain(int argc, char* argv[])
{
    std::string fileVcxproj;
	std::string cmake_file("CMakeLists.txt");
	if (argc == 2) {
		fileVcxproj = GetFileName(std::string(argv[1]));
	} 
	else {
		Useage(argv[0]);
		return -1;
	}

    if (CheckFile(fileVcxproj))
    {
        Useage(argv[0]);
		std::cout << fileVcxproj << std::endl << "Input fileName:";
		std::cin >> fileVcxproj;
        return -2;
    }
	else {
		// SetVcxprojWorkDir(GetAbsPathByName(std::string(argv[1])));
		std::filesystem::path  abs_path = std::filesystem::absolute(std::string(argv[1]));
		SetVcxprojWorkDir(abs_path.parent_path().string());
	}

    return ParseProjMain(fileVcxproj, cmake_file);
}

int main( int argc, char* argv[] )
{
    return AppMain(argc, argv);
}
