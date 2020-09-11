// Logging needs to be the first support, otherwise, debugging is difficult.
// We shouldn't need to use TCHAR anymore.  Nowadays, always use WCHAR.  TChar is difficult to use.
//
#include <stdio.h>
#include <share.h>
#include <ShlObj.h>
#include <crtdbg.h>
#include <wchar.h>
#include "Logging.h"

int  gDebugLevel = 0;
//extern int  gDebugLevel ;
Logger::Logger(const wchar_t* logFileName, LogFileDir dirType) {
   initSuccess = false;
   InitializeCriticalSection(&logCs);

   logFilePath[0] = L'\0';
   if (dirType == USER) {
      SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, logFilePath);
      //if (!SHGetSpecialFolderPathW(NULL, logFilePath, CSIDL_LOCAL_APPDATA, FALSE)) {
      /* if (!SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, logFilePath)) {
          OutputDebugStringW(L"Logger::Logger: SHGetSpecialFolderPathW failed.");
          return;
          }*/
      wcscat_s(logFilePath, _MAX_PATH, L"\\VHallHelper");
      if (!CreateDirectoryW(logFilePath, NULL)
          && GetLastError() != ERROR_ALREADY_EXISTS) {
         OutputDebugStringW(L"Logger::Logger: CreateDirectoryW failed.");
         return;
      }
      //wcscat_s(logFilePath, _MAX_PATH, L"\\");
      wcscat_s(logFilePath, _MAX_PATH, logFileName);
   } else if (dirType == SYSTEM) {
      if (!GetWindowsDirectoryW(logFilePath, _MAX_PATH)) {
         OutputDebugStringW(L"Logger::Logger: GetWindowsDirectory failed.");
         return;
      }
      //wcscat_s(logFilePath, _MAX_PATH, L"\\");
      wcscat_s(logFilePath, _MAX_PATH, logFileName);
   } else if (dirType == CURRENT) {
      WCHAR tmp[1024 * 10];
      tmp[0] = L'0';
      if (GetModuleFileNameW(NULL, tmp, sizeof(tmp)) > 0) {
         WCHAR* dst = NULL;
         WCHAR* p = tmp;
         while (*p) {
            if (*p == L'\\')
               dst = p;
            p = CharNextW(p);
         }

         if (dst)
            *dst = 0;
      }
      //return tmp;
      //wcscat_s(logFilePath, _MAX_PATH, logFileName);
      wcscat_s(logFilePath, _MAX_PATH, tmp);
      //wcscat_s(logFilePath, _MAX_PATH, L"\\");
      wcscat_s(logFilePath, _MAX_PATH, logFileName);
   } else {
      _ASSERT(FALSE);
   }
   initSuccess = true;
   DWORD dwSize = -1;
   DWORD dwSizeHigh = 0;

   HANDLE hFile = CreateFileW(logFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile != (HANDLE)-1 && hFile != NULL) {
      dwSize = ::GetFileSize(hFile, &dwSizeHigh);
      CloseHandle(hFile);
      if (dwSizeHigh != 0 || dwSize > (DWORD)1024 * 1024 * 10) {
         SYSTEMTIME	sysTime;
         WCHAR		szName[1024];
         GetLocalTime(&sysTime);
         wsprintfW(szName, L"%s.%02d%02d%02d%02d.bak", logFilePath, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);
         _wrename(logFilePath, szName);
      }
   }
}

FILE *Logger::openLogFile() {
   FILE *logFile = _wfsopen(logFilePath, L"a+", _SH_DENYNO);
   if (logFile == NULL) {
      OutputDebugStringW(L"Logger::openLogFile: _wfopen_s failed.\n");
      return NULL;
   }
   return logFile;
}

Logger::~Logger() {
   OutputDebugStringW(L"Logger::~Logger.\n");
   DeleteCriticalSection(&logCs);
}

void Logger::log(LogType logType, LPCSTR fmtStr, va_list args) {
   size_t len;
   char *buffer;
   SYSTEMTIME curTime;
   char timeStamp[64];

   if (!initSuccess) {
      return;
   }
   if (gDebugLevel > logType) {
      return;
   }
   FILE *logFile = openLogFile();
   if (!logFile) {
      return;
   }
   len = _vscprintf(fmtStr, args) + 1;  /* _vscprintf doesn't count terminating NUL */
   buffer = (char *)malloc(len * sizeof(char));
   vsprintf_s(buffer, len, fmtStr, args);
   GetLocalTime(&curTime);
   sprintf_s(timeStamp, sizeof(timeStamp), "%4d-%02d-%02d %02d:%02d:%02d",
             curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour,
             curTime.wMinute, curTime.wSecond);
   EnterCriticalSection(&logCs);
   switch (logType) {
   case LOG_INFO:
      fprintf(logFile, "%s %s", timeStamp, "[INFO] ");
#ifdef _DEBUG
      fprintf(stdout, "%s %s", timeStamp, "[INFO] ");
#endif
      break;
   case LOG_WARNING:
      fprintf(logFile, "%s %s", timeStamp, "[WARNING] ");
#ifdef _DEBUG
      fprintf(stdout, "%s %s", timeStamp, "[WARNING] ");
#endif
      break;
   case LOG_ERROR:
      fprintf(logFile, "%s %s", timeStamp, "[ERROR] ");
#ifdef _DEBUG
      fprintf(stdout, "%s %s", timeStamp, "[ERROR] ");
#endif
      break;
   default:
      break;
   }
   fprintf(logFile, "%s\n", buffer);
#ifdef _DEBUG
   fprintf(stdout, "%s\n", buffer);
#endif
   LeaveCriticalSection(&logCs);
   fflush(logFile);
#ifdef _DEBUG
   fflush(stdout);
#endif
   free(buffer);
   fclose(logFile);
}

void Logger::log(LogType logType, LPCWSTR fmtStr, va_list args) {
   size_t len;
   WCHAR *buffer;
   SYSTEMTIME curTime;
   WCHAR timeStamp[128];

   if (!initSuccess) {
      return;
   }
   if (gDebugLevel > logType) {
      return;
   }
   FILE *logFile = openLogFile();
   if (!logFile) {
      return;
   }
   len = _vscwprintf(fmtStr, args) + 1;  /* _vscprintf doesn't count terminating NUL */
   buffer = (WCHAR *)malloc(len * sizeof(WCHAR));

   vswprintf_s(buffer, len, fmtStr, args);
   GetLocalTime(&curTime);
   swprintf_s(timeStamp, 128, L"%4d-%02d-%02d %02d:%02d:%02d",
              curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour,
              curTime.wMinute, curTime.wSecond);
   EnterCriticalSection(&logCs);
   switch (logType) {
   case LOG_INFO:
      fwprintf(logFile, L"%ws %ws", timeStamp, L"[INFO] ");
#ifdef _DEBUG
      fwprintf(stdout, L"%s %s", timeStamp, L"[INFO] ");
#endif
      break;
   case LOG_WARNING:
      fwprintf(logFile, L"%s %s", timeStamp, L"[WARNING] ");
#ifdef _DEBUG
      fwprintf(stdout, L"%s %s", timeStamp, L"[WARNING] ");
#endif
      break;
   case LOG_ERROR:
      fwprintf(logFile, L"%s %s", timeStamp, L"[ERROR] ");
#ifdef _DEBUG
      fwprintf(stdout, L"%s %s", timeStamp, L"[ERROR] ");
#endif
      break;
   default:
      break;
   }
   fwprintf(logFile, L"%ws\n", buffer);
#ifdef _DEBUG
   fwprintf(stdout, L"%ws\n", buffer);
   OutputDebugString(timeStamp);
   OutputDebugString(TEXT(" "));
   OutputDebugString(buffer);
   OutputDebugString(TEXT("\n"));

#endif
   LeaveCriticalSection(&logCs);
   fflush(logFile);
#ifdef _DEBUG
   fflush(stdout);
#endif
   free(buffer);
   fclose(logFile);
}
void Logger::logCrash(LPCSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_ERROR, fmtStr, args);
#if defined(_DEBUG) && defined(_WIN32)
   __debugbreak();
#endif
   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);
   TerminateProcess(GetCurrentProcess(), 0xFFFFFFFF);
   return;
}
void Logger::logError(LPCSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_ERROR, fmtStr, args);
   return;
}

void Logger::logWarning(LPCSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_WARNING, fmtStr, args);
   return;
}

void Logger::logInfo(LPCSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_INFO, fmtStr, args);
   return;
}
void Logger::logStringA(char *buf,int len) {
   SYSTEMTIME curTime;
   char timeStamp[64];

   if (!initSuccess) {
      return;
   }

   FILE *logFile = openLogFile();
   if (!logFile) {
      return;
   }

   GetLocalTime(&curTime);
   sprintf_s(timeStamp, sizeof(timeStamp), "%4d-%02d-%02d %02d:%02d:%02d",
             curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour,
             curTime.wMinute, curTime.wSecond);
   
   EnterCriticalSection(&logCs);
   fprintf(logFile, "%s", timeStamp);
   fprintf(logFile, " %s\n", buf);
   LeaveCriticalSection(&logCs);
   fflush(logFile);
   fclose(logFile);
}
void Logger::logStringW(wchar_t *buf,int len){
   SYSTEMTIME curTime;
   wchar_t timeStamp[64];

   if (!initSuccess) {
      return;
   }

   FILE *logFile = openLogFile();
   if (!logFile) {
      return;
   }
   
   GetLocalTime(&curTime);
   swprintf_s(timeStamp, sizeof(timeStamp), L"%4d-%02d-%02d %02d:%02d:%02d",
             curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour,
             curTime.wMinute, curTime.wSecond);
   
   EnterCriticalSection(&logCs);

   fwprintf(logFile, L"%s", timeStamp);
   fwprintf(logFile, L"%s\n", buf);
   
   LeaveCriticalSection(&logCs);
   fflush(logFile);
   fclose(logFile);

}

void Logger::logHex(void *buf,int len){
   SYSTEMTIME curTime;
   wchar_t timeStamp[64];

   if (!initSuccess) {
      return;
   }

   FILE *logFile = openLogFile();
   if (!logFile) {
      return;
   }
   
   GetLocalTime(&curTime);
   swprintf_s(timeStamp, sizeof(timeStamp), L"%4d-%02d-%02d %02d:%02d:%02d",
             curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour,
             curTime.wMinute, curTime.wSecond);
   
   EnterCriticalSection(&logCs);

   fwprintf(logFile, L"%s\n", timeStamp);
   unsigned char *m = (unsigned char *)buf;
   for(int i=0;i<len;i++) {
      fwprintf(logFile, L"%02X " , m[i]);
      if(i%16==15||i==len-1) {
         fwprintf(logFile, L"\n");
      }
   }
   
   LeaveCriticalSection(&logCs);
   fflush(logFile);
   fclose(logFile);
}

void Logger::logCrash(LPCWSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_CRASH, fmtStr, args);
#if defined(_DEBUG) && defined(_WIN32)
   __debugbreak();
#endif
   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);
   TerminateProcess(GetCurrentProcess(), 0xFFFFFFFF);
}
void Logger::logError(LPCWSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_CRASH, fmtStr, args);
   return;
}

void Logger::logWarning(LPCWSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_WARNING, fmtStr, args);
   return;
}

void Logger::logInfo(LPCWSTR fmtStr, ...) {
   va_list args;
   va_start(args, fmtStr);
   log(LOG_INFO, fmtStr, args);
	va_end(args);
   return;
}
