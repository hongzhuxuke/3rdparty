#include "HttpProxyGet.h"
#include <windows.h>
#include <atlbase.h>
#include <WinCred.h>



HttpProxyInfo::HttpProxyInfo()
{

}

HttpProxyInfo::~HttpProxyInfo()
{

}

int HttpProxyInfo::GetProxyInfoFromBrowser(QString &ip, int &port, QString &userName, QString &userPwd) {
   if (IsEnableInternetProxy()) {
      GetProxyIP(ip,port,userName,userPwd);
   }
   return 0;
}

int HttpProxyInfo::GetProxyIP(QString &ip, int &port, QString &userName, QString &userPwd) {
   HKEY hOpen;
   char key[255] = "\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
   char name[30] = "ProxyEnable";
   char buf[255] = { 0 };
   DWORD size;

   int nRet = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ, &hOpen);
   if (ERROR_SUCCESS == nRet) {
      RegQueryValueEx(hOpen, L"ProxyServer", NULL, NULL, (BYTE*)buf, &size);
   } else {
      RegCloseKey(hOpen);
      return GetLastError();
   }

   QString proxyInfo;
   for (int i = 0; i < size; i++) {
      char da = buf[i];
      if (da != 0) {
         proxyInfo.append(da);
      }
   }

   //提取HTTP代理ip：port,更加ip：port 提取用户名和密码
   //http = 172.16.11.191:8081; https = 172.16.11.191:8081; ftp = 172.16.11.191:8081
   QStringList proxylist = proxyInfo.split(";");
   for (int i = 0; i < proxylist.size(); i++) {
      QString httpProxy = proxylist.at(i);
      if (httpProxy.contains("http")) {
         //http = 172.16.11.191:8081
         httpProxy = httpProxy.replace("http=", "");
         SplitProxyIPInfo(httpProxy,ip,port);
         GetProxyUserInfo(httpProxy,userName,userPwd);
         break;
      }
      else {
         SplitProxyIPInfo(httpProxy, ip, port);
         GetProxyUserInfo(httpProxy, userName, userPwd);
      }
   }
   return 0;
}

void HttpProxyInfo::GetProxyUserInfo(const QString httpProxy, QString &userName, QString &userPwd) {
   DWORD dwCount = 0;
   PCREDENTIAL * pCredArray = NULL;
   if (CredEnumerate(NULL, 0, &dwCount, &pCredArray)) {
      for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++) {
         PCREDENTIAL pCredential = pCredArray[dwIndex];
         std::wstring target = pCredential->TargetName;
         if (target == httpProxy.toStdWString()) {
            std::wstring usr = pCredential->UserName;
            std::wstring pwd = (LPCWSTR)pCredential->CredentialBlob;
            userName = QString::fromStdWString(usr);
            userPwd = QString::fromStdWString(pwd);
            break;
         }
      }
      CredFree(pCredArray);
   }
}

void HttpProxyInfo::SplitProxyIPInfo(const QString httpProxy, QString &ip, int &port) {
   //172.16.11.191:8081
   QStringList ipInfo = httpProxy.split(":");
   for (int index = 0; index < ipInfo.size(); index++) {
      if (index == 0) {
         ip = ipInfo[index];
      } else if (index == 1) {
         port = ipInfo[index].toInt();
      }  
   }
}

bool HttpProxyInfo::IsEnableInternetProxy() {
   bool bEnableProxy = false;
   HKEY hOpen;
   char key[255] = "\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
   char name[30] = "ProxyServer";
   char buf[255] = { 0 };
   DWORD size;

   int nRet = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ, &hOpen);
   if (ERROR_SUCCESS == nRet) {
      RegQueryValueEx(hOpen, L"ProxyEnable", NULL, NULL, (BYTE*)buf, &size);
   } else {
      bEnableProxy = false;
   }
   if (size != 0 && buf[0] == 1) {
      bEnableProxy = true;
   } 
   RegCloseKey(hOpen);
   return bEnableProxy;
}
