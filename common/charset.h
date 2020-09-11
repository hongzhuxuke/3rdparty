#ifndef	_CHARSET_H_
#define	_CHARSET_H_
#include <vector>

string	stringToUtf8(string str);
string	charToUnicode(const char *str);
string	Utf8Tostring(const char *utf8Str);
wstring	Utf8ToWstring(const string&utf8Str);
string   WstringToUtf8(const WCHAR* wstr);
BOOL	CharToWchar(const char *utf8Str,WCHAR **wStr);
BOOL	WcharToUtf8(const WCHAR *wStr, char **utf8Str);
BOOL	Utf8ToWchar(const char *utf8Str, WCHAR **wStr);

wstring	stringToWstring2(string str);
string	wstringToString(wstring str);
void	SplitString2(const string& str, const string& sKey, vector<string>& vecResult, BOOL bUrl);

//´Ó
#ifndef UNICODE
#define STC(s) s
#else
#define STC(s) stringToWstring2(s)
#endif

#ifndef UNICODE
#define WTC(s) s
#else
#define WTC(s) wstringToString(s)
#endif

#endif//_CHARSET_H_