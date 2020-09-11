#include "StdAfx.h"
#include "UtilityTool.h"


std::wstring CUtilityTool::CToW(const char *apsztext, UINT auiCode)
{
    std::wstring str;

    int nLenA = (int)strlen(apsztext);
    int nLenW = MultiByteToWideChar(auiCode, 0, apsztext, nLenA, NULL, NULL);

    wchar_t* bufw = (wchar_t*)malloc(nLenW * sizeof(wchar_t) + sizeof(wchar_t));

    if(NULL == bufw)
    {
        ASSERT(false);
        return str;
    }

    memset(bufw, 0, nLenW * sizeof(wchar_t) + sizeof(wchar_t));
    MultiByteToWideChar(auiCode, 0, apsztext, nLenA, bufw, nLenW);

    str.append(bufw);

    free(bufw);

    bufw = NULL;

    return str;
}

std::string CUtilityTool::WToC(const wchar_t *apwztext, UINT auiCode)
{
    std::string str;
    int nLenW = (int)wcslen(apwztext);
    int nLenA = WideCharToMultiByte(auiCode, 0, apwztext, nLenW, NULL, NULL, NULL, NULL);

    char* buf = (char*)malloc(nLenA +1);

    if(NULL == buf)
    {
        ASSERT(false);
        return str;
    }
    memset(buf, 0, nLenA +1);
    WideCharToMultiByte(auiCode, 0, apwztext, nLenW, buf, nLenA, NULL, NULL);

    str.append(buf);
    free(buf);
    buf = NULL;

    return str;
}
