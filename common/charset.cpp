//#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//#include <assert.h>
#include <string>
//#include <atlbase.h>
using namespace std;
#include "charset.h"

BOOL WcharToChar(const WCHAR *wStr,
			char **mbStr);

string	Utf8Tostring(const char *utf8Str)
{
	string	sRet	;

	if(!utf8Str || strlen(utf8Str)==0)
		return "";

    sRet = utf8Str;
	return sRet;
}
wstring	Utf8ToWstring(const string&utf8Str )
{
   wstring	sRet	;
   WCHAR*	pwszBuf = NULL;
   char *  ansiBuf = NULL;

   if(utf8Str.length()==0)
      return L"";

   //USES_CONVERSION;
   Utf8ToWchar(utf8Str.c_str(),&pwszBuf);
   if(pwszBuf){
     sRet = pwszBuf;
     free((pwszBuf));
     }

   return sRet;
}
string   WstringToUtf8(const WCHAR* wstr)
{
   string sUtf8TMp;
   wstring wsName = wstr;
   char* utf8 = NULL;               
   WcharToUtf8(wsName.c_str(), &utf8);
   if(utf8){
      sUtf8TMp = utf8;             
      free(utf8);
   }
   return sUtf8TMp;
}
string	stringToUtf8(string str)
{
	string	sRet	;
	WCHAR*	wStr	= NULL;
	char*	utf8Str	= NULL;

	if(str.empty())
		return str;
	CharToWchar(str.data(),&wStr);
	WcharToUtf8(wStr,&utf8Str);

	sRet = utf8Str;
	
	free(wStr);
	free(utf8Str);

	return sRet;
}

string	charToUnicode(const char* str)
{
	WCHAR*	wStr	= NULL;
	string	sTmp	;
	char *	pBuf	= NULL;
	string	sRet	;

	try
	{

		CharToWchar(str,&wStr);

		pBuf = new char[(wcslen(wStr)+4)*sizeof(WCHAR)];
		memset(pBuf,0,(wcslen(wStr)+4)*sizeof(WCHAR));
		pBuf[0] = (char)0xFF;
		pBuf[1] = (char)0xFE;
		wcscpy((wchar_t *)(pBuf+2),wStr);

		sRet.append((wcslen(wStr)+4)*sizeof(WCHAR),0);
		memcpy((char *)sRet.data(),pBuf,(wcslen(wStr)+4)*sizeof(WCHAR));

		free(wStr);
		delete [] pBuf;
		pBuf = NULL;
	}
	catch(...)
	{
		;
	}

	return sRet;
}

BOOL
WcharToUtf8(const WCHAR *wStr,
            char **utf8Str)
{
   size_t byteCount = 0;
   if (!wStr) {
      *utf8Str = NULL;
      return FALSE;
   }

   if (wcslen(wStr) == 0) {
	    *utf8Str = NULL;
      return FALSE;
   }

   /* Get the buffer size. */
   byteCount = WideCharToMultiByte(CP_UTF8, 0, wStr, -1,
                                    NULL, 0, NULL, NULL);
   if (byteCount == 0) {
    //  printf("WcharToUtf8: Error getting buffer size: %d\n", GetLastError());
	   *utf8Str = NULL;
      return FALSE;
   } 

   *utf8Str = (char *)malloc(1 + byteCount);
//   assert(*utf8Str);

   byteCount = WideCharToMultiByte(CP_UTF8, 0, wStr, -1,
                                   *utf8Str, (int)byteCount, NULL, NULL);
   if (byteCount == 0) {
    //  fprintf(stderr, "WcharToUtf8: WideCharToMultiByte failed: %d\n", 
     //         GetLastError());
      free(*utf8Str);
      *utf8Str = NULL;
      return FALSE;
   }
   (*utf8Str)[byteCount] = '\0';
   return TRUE;
}


BOOL
Utf8ToWchar(const char *utf8Str,
            WCHAR **wStr)
{
   size_t strLen;
   size_t WcharLen;
   if (!utf8Str) {
      *wStr = NULL;
      return FALSE;
   }
   strLen = strlen(utf8Str);
   if (strLen == 0) {
	   *wStr = NULL;
      return FALSE;
   }

   /* Get the buffer size. */
   WcharLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
   if (WcharLen == 0) {
   //   fprintf(stderr, "Utf8ToWchar: Error getting buffer size: %d\n", 
    //          GetLastError());
	  *wStr = NULL;
      return FALSE;
   }

   *wStr = (WCHAR *)malloc(sizeof(WCHAR) * (WcharLen + 1));
  
   if(*wStr == NULL) {
	   ;
	   //throws exception here
   }
//   assert(*wStr);

   WcharLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1,
                                  *wStr,(int) WcharLen);
   if (WcharLen == 0) {
    //  fprintf(stderr, "Utf8ToWchar MultiByteToWideChar error: %d\n", 
    //          GetLastError());
	  free(*wStr);
      *wStr = NULL;
      return FALSE;
   }
   (*wStr)[WcharLen] = L'\0';
   return TRUE;
}

BOOL
CharToWchar(const char *utf8Str,
            WCHAR **wStr)
{
   size_t strLen;
   size_t WcharLen;
   if (!utf8Str) {
      *wStr = NULL;
      return FALSE;
   }
   strLen = strlen(utf8Str);
   if (strLen == 0) {
	   *wStr = NULL;
      return FALSE;
   }

   /* Get the buffer size. */
   WcharLen = MultiByteToWideChar(CP_ACP, 0, utf8Str, -1, NULL, 0);
   if (WcharLen == 0) {
      fprintf(stderr, "Utf8ToWchar: Error getting buffer size: %d\n", 
              GetLastError());
	   *wStr = NULL;
      return FALSE;
   }

   *wStr = (WCHAR *)malloc(sizeof(WCHAR) * (WcharLen + 1));
//   assert(*wStr);

   WcharLen = MultiByteToWideChar(CP_ACP, 0, utf8Str, -1,
                                  *wStr, (int)WcharLen);
   if (WcharLen == 0) {
      fprintf(stderr, "Utf8ToWchar MultiByteToWideChar error: %d\n", 
              GetLastError());
	   free(*wStr);
      *wStr = NULL;
      return FALSE;
   }
   (*wStr)[WcharLen] = L'\0';
   return TRUE;
}


BOOL
WcharToChar(const WCHAR *wStr,
			char **mbStr)
{
	size_t mbstrLen;
	size_t WcharLen;
	BOOL   bUsedDefault = FALSE;
	if (!wStr) {
		*mbStr = NULL;
		return FALSE;
	}
	WcharLen = wcslen(wStr);
	if (WcharLen == 0) {
		*mbStr = NULL;
		return FALSE;
	}

	/* Get the buffer size. */
	mbstrLen = WideCharToMultiByte(CP_ACP, 0, wStr, -1, NULL, 0,NULL,&bUsedDefault);
	if (mbstrLen == 0) {
		fprintf(stderr, "WcharToChar: Error getting buffer size: %d\n", 
			GetLastError());
		*mbStr = NULL;
		return FALSE;
	}

	*mbStr = (char *)malloc(sizeof(char) * (mbstrLen + 1));
	//   assert(*wStr);

	mbstrLen = WideCharToMultiByte(CP_ACP, 0, wStr, -1,
		*mbStr, (int)mbstrLen,NULL,&bUsedDefault);
	if (mbstrLen == 0) {
		fprintf(stderr, "WcharToChar WideCharToMultiByte error: %d\n", 
			GetLastError());
		free(*mbStr);
		*mbStr = NULL;
		return FALSE;
	}
	(*mbStr)[mbstrLen] = '\0';
	return TRUE;
}


wstring	stringToWstring2(string str)
{   
   wstring ws;
   WCHAR* pWcha = NULL;
   CharToWchar(str.c_str(),&pWcha);;
   if(pWcha) 
   {
      ws = pWcha;
      free(pWcha);
	}
	return ws;
}
string	wstringToString(wstring str)
{
	char* pCha = NULL;
	string s;
	WcharToChar(str.c_str(),&pCha);;
	if(pCha) 
	{
		s = pCha;
		free(pCha);
	}
	return s;
}

void	SplitString2(const string& str, const string& sKey, vector<string>& vecResult, BOOL bUrl)
{
   string::size_type	npos = 0;
   string::size_type	nPrev = 0;
   //	string				sLine	;

   while (true)
   {
      nPrev = npos;
      npos = str.find(sKey, npos);

      if (nPrev != 0 && bUrl == TRUE)
      {
         //if it is url, not check the url's key
         npos = string::npos;
      }

      if (npos != string::npos)
      {
         if (npos != nPrev)
         {
            //				sLine = str.substr(nPrev,npos - nPrev);
            vecResult.push_back(str.substr(nPrev, npos - nPrev));
         }
         npos += sKey.size();
         if (npos == nPrev)
            break;
      }
      else
      {
         if (nPrev<str.size())
            vecResult.push_back(str.substr(nPrev));
         break;
      }
   }
}