#pragma once

#include <string>

class CUtilityTool
{
public:

    // ���ַ�ת��Ϊ���ַ�
    static std::wstring CToW(const char *apsztext, UINT auiCode = CP_UTF8);

    // ���ַ�ת��Ϊ���ַ�
    static std::string WToC(const wchar_t *apwztext, UINT auiCode = CP_UTF8);

};