#include "StdAfx.h"
#include "ExceptionDump.h"
#include <sys/timeb.h>
#ifdef	OPENSDK  
;
#else 
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#endif

#include <QFile>
#include "ConfigSetting.h"
#include "pathmanager.h"

#include "UpLoader.h"
#include "CRC32.h"
#ifdef	OPENSDK  

#else 
	#include "RuntimeInstance.h"
	#include "json.h"
#endif
#include "pathManage.h"

typedef LPTOP_LEVEL_EXCEPTION_FILTER(_stdcall *pSetUnhandledExceptionFilter)(
   LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
   );

//异常处理回调
LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS* ExceptionInfo);

/************************************************************************/

void DisableSetUnhandledExceptionFilter() {
#ifdef	OPENSDK  
	;  
#else 
	TRACE6("DisableSetUnhandledExceptionFilter\n"); 
#endif

   void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")), "SetUnhandledExceptionFilter");
   if (addr) {
#ifdef	OPENSDK  
	   ;  
#else 
	   TRACE6("DisableSetUnhandledExceptionFilter Addr not NULL\n"); 
#endif
      unsigned char code[16];
      int size = 0;
      code[size++] = 0x33;
      code[size++] = 0xC0;
      code[size++] = 0xC2;
      code[size++] = 0x04;
      code[size++] = 0x00;
      DWORD dwOldFlag, dwTempFlag;

      if (!WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL)) {

#ifdef	OPENSDK  
		  ;  
#else 
		  TRACE6("WriteProcessMemory with not change page Auth Failed!\n"); 
#endif
         try {
#ifdef	OPENSDK  
			 ;  
#else 
			 TRACE6("VirtualProtect PAGE_READWRITE\n"); 
#endif
            VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);

#ifdef	OPENSDK  
			;  
#else 
			TRACE6("WriteProcessMemory code\n"); 
#endif
            WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);

#ifdef	OPENSDK  
			;  
#else 
			TRACE6("VirtualProtect old flag\n"); 
#endif
            VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);

#ifdef	OPENSDK  
			;  
#else 
			TRACE6("VirtualProtect old flag! end\n"); 
#endif
         } catch (...) {

#ifdef	OPENSDK  
			 ;  
#else 
			 TRACE6("DisableSetUnhandledExceptionFilter exception!\n"); 
#endif
         }
      } else {
#ifdef	OPENSDK  
		  ;  
#else 
		  TRACE6("WriteProcessMemory with not change page Auth Successed!\n"); 
#endif
      }
   } else {
#ifdef	OPENSDK  
	   ;  
#else 
	   TRACE6("DisableSetUnhandledExceptionFilter Addr NULL\n"); 
#endif
   }

#ifdef	OPENSDK  
   ;  
#else 
   TRACE6("DisableSetUnhandledExceptionFilter return\n"); 
#endif
}

CExceptionDump::CExceptionDump(void) {
   m_pSystemFilter = NULL;

   Init();
}

CExceptionDump::~CExceptionDump(void) {
#ifdef _M_X64
#else
   UnInit();
#endif // x64

}

//初始化
void CExceptionDump::Init() {
#ifdef	OPENSDK  
	;  
#else 
	TRACE6("CExceptionDump::Init()\n"); 
#endif
   pSetUnhandledExceptionFilter lpSetUnhandledExceptionFilter = (pSetUnhandledExceptionFilter)GetProcAddress(LoadLibrary((_T("kernel32.dll"))), "SetUnhandledExceptionFilter");

   if (NULL == lpSetUnhandledExceptionFilter) {

#ifdef	OPENSDK  
	   ;  
#else 
	   TRACE6("CExceptionDump::Init() is NULL\n"); 
#endif
      return;
   }

   m_pSystemFilter = lpSetUnhandledExceptionFilter(UnhandledExceptionFilterEx);

   DisableSetUnhandledExceptionFilter();
}

//反初始化
void CExceptionDump::UnInit() {
   if (NULL == m_pSystemFilter) {
      return;
   }

   pSetUnhandledExceptionFilter lpSetUnhandledExceptionFilter = (pSetUnhandledExceptionFilter)GetProcAddress(LoadLibrary((_T("kernel32.dll"))), "SetUnhandledExceptionFilter");

   if (NULL == lpSetUnhandledExceptionFilter) {
      return;
   }

   lpSetUnhandledExceptionFilter(m_pSystemFilter);
}

void CrashReport();
void UploadCrashFile(wchar_t* wsFileName);
void UploadEvents(string);

//异常处理
LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS* ExceptionInfo) {
#ifdef	OPENSDK  
	;  
#else 
	TRACE6("UnhandledExceptionFilterEx\n"); 
#endif

   //崩溃上报
   CrashReport();

   //记录Dump
   QString curTime = QString::number(QDateTime::currentDateTime().toTime_t());
   wchar_t lwzFileName[128 + 1] = { 0 };
   QString msConfPath = CPathManager::GetConfigPath();
   QString logIdBase = ConfigSetting::ReadString(msConfPath, GROUP_DEFAULT, LOG_ID_BASE, "");
   swprintf_s(lwzFileName, 128, L"%s_%s_%s.dmp", GetAppDataPath().c_str(), curTime.toStdWString().c_str(), logIdBase.toStdWString().c_str());
   //QString currentDateTime = QString::number(QDateTime::currentDateTime().toTime_t());
   //QString lastUploadTime = ConfigSetting::ReadString(msConfPath, GROUP_DEFAULT, LAST_DUMP_UPLOAD_TIME, "");
   //if (!lastUploadTime.isEmpty() && (lastUploadTime.compare(currentDateTime) == 0 || (currentDateTime.toULongLong() - lastUploadTime.toULongLong() <= 2))) {
   //    ConfigSetting::writeValue(msConfPath, GROUP_DEFAULT, LAST_DUMP_UPLOAD_TIME, currentDateTime);
   //    #ifdef	OPENSDK  ;  #else TRACE6("UnhandledExceptionFilterEx curTime:%s lastTime:%s\n", currentDateTime.toStdString().c_str(), lastUploadTime.toStdString().c_str());
   //    //return EXCEPTION_EXECUTE_HANDLER;
   //}
   //ConfigSetting::writeValue(msConfPath, GROUP_DEFAULT, LAST_DUMP_UPLOAD_TIME, currentDateTime);

   //创建dump文件
   HANDLE hDumpFile = CreateFile(lwzFileName, GENERIC_WRITE, 0, NULL,
                                 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

   if (NULL == hDumpFile) {
      return EXCEPTION_EXECUTE_HANDLER;
   }

   //异常信息
   MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
   loExceptionInfo.ExceptionPointers = ExceptionInfo;
   loExceptionInfo.ThreadId = GetCurrentThreadId();
   loExceptionInfo.ClientPointers = TRUE;

   //写入dump
   MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
                     MiniDumpNormal, &loExceptionInfo, NULL, NULL);

#ifdef OPENSDK
   ;
#else
   string sEvents = CSingletonRuntimeInstance::Instance().GetEvents();

   //上报用户事件
   UploadEvents(sEvents);
#endif
   //关闭文件句柄
   CloseHandle(hDumpFile);



   //HTTP 上传文件
   UploadCrashFile(lwzFileName);

   //通知windows异常已经处理完毕，退出程序，有助于keeper重启程序
   return EXCEPTION_EXECUTE_HANDLER;
}

void CrashReport() {
#ifdef	OPENSDK
	;
#else
   QString qsUrl = MONITOR_URL;
   QString token = "{\"v\":\"%1\",\"p\":\"%2\"}";
   token =
      token.arg(QString::fromStdWString(gCurrentVersion.c_str()))
      .arg(QString::fromStdString(gCurStreamID.c_str()));

   qsUrl = qsUrl.arg(QString::fromStdString("14004"))
      .arg("0")
      .arg(gCurStreamID.c_str())
      .arg(QString(QString::fromStdString(token.toStdString()).toLatin1().toBase64()));

   QNetworkAccessManager* pNetAccessMgr = new QNetworkAccessManager;
   if (pNetAccessMgr != NULL) {
      QEventLoop loop;
      QObject::connect(pNetAccessMgr, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()), Qt::DirectConnection);
      QNetworkReply *reply = pNetAccessMgr->get(QNetworkRequest(QUrl(qsUrl)));
      loop.exec();
      if (reply) {
         //响应返回值
         QString qStrResult = reply->readAll();
         reply->deleteLater();
      }
   }
#endif
}

void UploadCrashFile(wchar_t* wsFileName) {
   SYSTEMTIME	sysTime;
   WCHAR		szTime[1024] = { 0 };
   GetLocalTime(&sysTime);
   wsprintfW(szTime, L"%02d-%02d-%02d %02d:%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);

   QString szToken = QString("%1 %2").arg(QString::fromStdString(gCurStreamID))
      .arg(QString::fromStdWString(L"helper"));


   u_int32_t crcToken = crc32(szToken.toUtf8(), szToken.length());

   QString msConfPath = CPathManager::GetConfigPath();
   QString dumpSerUrl = ConfigSetting::ReadString(CPathManager::GetToolConfigPath(), GROUP_DEFAULT, DUMP_SERVER_URL, "http://helpdump.t.vhall.com/upload");
   QString qUrl = dumpSerUrl+QString("?streamID=%1&version=%2&filename=%3&crashTime=%4&MengZhu=0&token=%5")
      .arg(QString::fromStdString(gCurStreamID))
      .arg(QString::fromStdWString(gCurrentVersion))
      .arg(QString::fromWCharArray(wsFileName))
      .arg(QString::fromWCharArray(szTime))
      .arg(QString("%1").arg(crcToken));
#ifdef	OPENSDK
   ;
#else
   CUpLoader loDmpFileUp;
   loDmpFileUp.UpLoadFile(qUrl, wsFileName);
#endif
}

void UploadEvents(string sEventID) {
#ifdef	OPENSDK
	;
#else
   VHJson::Value valueeventid;

   VHJson::Value jsonRoot;
   VHJson::Value jsonItem;

   valueeventid["eventid"] = sEventID;

   jsonItem["version"] = QString::fromStdWString(gCurrentVersion).toStdString();
   jsonItem["event"].append(valueeventid);
   jsonRoot["helper"] = jsonItem;

   string sInfo = jsonRoot.toStyledString();

   CUpLoader oUpLoader;
   oUpLoader.UpLoadInfo("http://datacollect.vhall.com:7880/helperevent", (char*)sInfo.c_str());
#endif
}
