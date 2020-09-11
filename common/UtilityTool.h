#pragma once

#include <string>

class CUtilityTool
{
public:

    // ¶à×Ö·û×ª»»Îª¿í×Ö·û
    static std::wstring CToW(const char *apsztext, UINT auiCode = CP_UTF8);

    // ¿í×Ö·û×ª»»Îª¶à×Ö·û
    static std::string WToC(const wchar_t *apwztext, UINT auiCode = CP_UTF8);

};