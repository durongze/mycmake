#include "my_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#ifdef USE_ICONV
#include <iconv.h>
#endif

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#include <iostream>
int ReadFileByDir(std::string dirName, std::string fileExt, std::vector<std::string>& files)
{
    struct _finddata_t s_file;
    std::string str = dirName + "\\*" + fileExt;
    bool withDir = true;
    intptr_t h_file = _findfirst(str.c_str(), &s_file);
    if (h_file != static_cast<intptr_t>(-1.0)) {
        do {
            if (withDir) {
                if (*dirName.rbegin() == '\\') {
                    files.push_back(dirName + s_file.name);
                }
                else {
                    files.push_back(dirName + "\\" + s_file.name);
                }
            }
            else {
                files.push_back(std::string(s_file.name));
            }
        } while (_findnext(h_file, &s_file) == 0);
    }
    _findclose(h_file);
    return 0;
}

std::string GetAbsPathByName(const std::string& fileName)
{
    char absolutePath[MAX_PATH];

    // 获取绝对路径
    int len = GetFullPathName((LPTSTR)fileName.c_str(), MAX_PATH, (LPTSTR)absolutePath, NULL);
    if (len > 0 && len < MAX_PATH) {
        std::cout << "Absolute path: " << absolutePath << std::endl;
    }
    else {
        std::cerr << "Failed to get absolute path." << std::endl;
    }
    return std::string(absolutePath);
}

#else
#include <dirent.h>

/*
 *struct dirent {
     ino_t          d_ino;       //Inode number
     off_t          d_off;       //Not an offset; see below
     unsigned short d_reclen;    // Length of this record
     unsigned char  d_type;      // Type of file; not supported by all filesystem types
     char           d_name[256];  // Null-terminated filename
  };
 *
 * */
int ReadFile(const char *fileName)
{
    int ret = 0;
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        return 1;
    }
    ret = fseek(fp, -1, SEEK_END);
    printf("fseek:%d\n", ret);
    long fsize = ftell(fp);
    char* ctx = (char*)malloc(fsize + 1);
    if (ctx != NULL) {
        rewind(fp);
        ret = fread(ctx, 1, fsize, fp);
        printf("fread:%d\n", ret);
        char* s = fgets(ctx, fsize, fp);
        printf("fgets:%s\n", s);
        free(ctx);
    }
    fclose(fp);
    return 0;
}

int ReadFileByDir(std::string dirName, std::string fileExt, std::vector<std::string>& files)
{
    int idx = 0;
    struct dirent* pdir = NULL;
    DIR* handle = opendir(dirName.c_str());
    
    do {
        pdir = readdir(handle);
        if (pdir == NULL) {
            break;
        }
        if (strstr(pdir->d_name, fileExt.c_str()) == NULL) {
            continue;
        }
        printf("===========[%d] %s ===========\n", ++idx, pdir->d_name);
        printf("ino:%lu,\n", pdir->d_ino);
        printf("off:%ld,\n", pdir->d_off);
        printf("reclen:%d,\n", pdir->d_reclen);
        printf("type:%d,\n", pdir->d_type);
        if (8 == pdir->d_type)
        {
            files.push_back(pdir->d_name);
        }
    } while (true);

    closedir(handle);
    return 0;
}

std::string GetAbsPathByName(std::string fileName)
{
    char absolutePath[PATH_MAX];

    char* resolvedPath = realpath(fileName.c_str(), absolutePath);
    if (resolvedPath != nullptr) {
        std::cout << "Absolute path: " << resolvedPath << std::endl;
    }
    else {
        std::cerr << "Failed to get absolute path." << std::endl;
    }

    return std::string(absolutePath);
}

#endif

void fixAppName(std::string& appName, char src_ch, char dst_ch)
{
    int i = 0;
    for (; i < appName.length(); ++i) {
        if (appName[i] == src_ch) {
            appName[i] = dst_ch;
        }
    }
}

void StripAppName(std::string& appName, char src_ch, char dst_ch)
{
    int i = 0;
    int j = i;
    while (appName.find(src_ch, 0) != -1) {
        for (i = 0; i < appName.length(); ++i) {
            if (appName[i] == src_ch) {
                for (j = i; j < appName.length(); ++j) {
                    appName[j] = appName[j + 1];
                }
            }
        }
    }
}

int RenameMrpFile(const char* filename, FILE* fp_log, FILE* fp_bat)
{
    std::string appName;
    char appname[512] = { 0 };
    appName = appname;
    StripAppName(appName, ' ', 'x');
    StripAppName(appName, '?', 'x');
    StripAppName(appName, ':', 'x');
    StripAppName(appName, '>', 'x');
    StripAppName(appName, '\t', 'x');

    if (rename(filename, appName.c_str()) != 0) {
        if (fp_bat != NULL) {
            fprintf(fp_bat, "move %s %s\n", filename, appName.c_str());
        }
        if (fp_log != NULL) {
            fprintf(fp_log, "rename %s to %s\n", filename, appname);
        }
        else {
            fprintf(stdout, "rename %s to %s\n", filename, appname);
        }
    }

    return 0;
}

int RenameMrpFileByDir(const char* dirname)
{
    char appname[512] = { 0 };
    if (dirname == NULL || strlen(dirname) == 0) {
        return -1;
    }

    std::string dirName = dirname;
    std::string extName = ".mrp";
    std::vector<std::string> files;
    std::vector<std::string>::iterator iter;

    ReadFileByDir(dirName, extName, files);

    FILE* fp = fopen("log.txt", "w+");
    FILE* fp_ok = fopen("log_ok.bat", "w+");
    for (iter = files.begin(); iter != files.end(); ++iter) {
        std::string fileName = dirName + DIR_SEP + *iter;
        RenameMrpFile(fileName.c_str(), fp, fp_ok);
    }
    fclose(fp_ok);
    fclose(fp);
    // iconv_main(argc, argv);

    return 0;
}

void hex_data2gb2312_str(const uint8_t* data, unsigned long len)
{
#ifdef USE_ICONV
    iconv_t cd = iconv_open("UTF-8", "GB2312");

    size_t inLen = 128;
    char* szSrc = (char*)malloc(inLen);
    memset(szSrc, 0, inLen);
    memcpy(szSrc, data, inLen);

    size_t outLen = 600;
    char* szDest = (char*)malloc(outLen);
    memset(szDest, 0, outLen);

    char* pszDest = szDest;
    char* pszSrc = szSrc;
    int ret = iconv(cd, &pszSrc, &len, &pszDest , &outLen); 	
    if (ret != 0) {
    printf("iconv ret:%d, errno:%d\n", ret, errno);
    perror("iconv failed:\n");
    }
    printf("szDest:%s\n", szDest);
    iconv_close(cd);
#endif	
    return;
}

std::string ToUpperStr(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return str;
}

std::string ToLowerStr(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return str;
}