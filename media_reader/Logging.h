#ifndef LOGGING_H
#define LOGGING_H
#include <Windows.h>
#include <stdio.h>
#ifndef _MAX_PATH
#define _MAX_PATH 256
#endif

typedef enum _LogType {
   LOG_INFO=1,
   LOG_WARNING=2,
   LOG_ERROR=3,
   LOG_CRASH = 4
}LogType;
enum LogFileDir {
   SYSTEM,
   USER,
   CURRENT
};

class Logger {
public:
    Logger(const wchar_t* logFileName, LogFileDir dirType);
    virtual ~Logger();
    void logCrash(LPCSTR fmtStr, ...);
    void logError(LPCSTR fmtStr, ...);
    void logWarning(LPCSTR fmtStr, ...);
    void logInfo(LPCSTR fmtStr, ...);

    void logCrash(LPCWSTR fmtStr, ...);
    void logError(LPCWSTR fmtStr, ...);
    void logWarning(LPCWSTR fmtStr, ...);
    void logInfo(LPCWSTR fmtStr, ...);
    void logHex(void *buf,int len);
    void logStringA(char *buf,int len);
    void logStringW(wchar_t *buf,int len);

private:
    FILE *openLogFile();
    void log(LogType logType, LPCSTR fmtStr, va_list args);
    void log(LogType logType, LPCWSTR fmtStr, va_list args);
private:
    bool initSuccess;
    CRITICAL_SECTION logCs;
    WCHAR logFilePath[_MAX_PATH];
};

/* The main entry of an application should instantiate gLogger immediately. */
extern Logger *g_pLogger;
#define gLogger g_pLogger
#define LOG_OUTPUT_ERROR g_pLogger->logError
#define LOG_OUTPUT_INFO g_pLogger->logInfo
#define VAppInfo g_pLogger->logInfo
#define VAppDebug g_pLogger->logInfo
#define VAppWarn g_pLogger->logWarning
#define VAppError g_pLogger->logError
#define VAppCrash g_pLogger->logCrash

#define VHLogDebug(...) if(g_pLogger){g_pLogger->logInfo(__VA_ARGS__);}



#endif
