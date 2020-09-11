#ifndef HTTPPROXYTEXT_H
#define HTTPPROXYTEXT_H
#include <QObject>

class HttpProxyInfo
{
public:
   HttpProxyInfo();
   ~HttpProxyInfo();

   static int GetProxyInfoFromBrowser(QString &ip,int &port,QString &userName, QString &userPwd);
   static bool IsEnableInternetProxy();
   static int GetProxyIP(QString &ip, int &port, QString &userName, QString &userPwd);
   static void SplitProxyIPInfo(const QString httpProxy, QString &ip,int &port);
   static void GetProxyUserInfo(const QString httpProxy, QString &userName, QString &userPwd);

};

#endif // HTTPPROXYTEXT_H
