#ifndef _VH_PROXY_TEST_H_
#define _VH_PROXY_TEST_H_
#include <QString>
#include <QObject>

class QNetworkReply;
class QAuthenticator;
class QNetworkProxy;

class HttpRequest :public QObject{
   Q_OBJECT
public:
   HttpRequest(QObject* obj = NULL);
   ~HttpRequest();
   bool VHProxyTest(QString ip, unsigned short port, QString userName, QString password, QString url);
   bool VHGetRet(QString url, int, QByteArray &ba);
   bool VHPostRet(QString url, int, QByteArray &ba, QString &json);

private slots:
   void slot_authenticationRequired(QNetworkReply *, QAuthenticator *);
   void slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
private:
   QString m_ip;
   int m_port;
   QString m_user;
   QString m_pwd;
};

#endif
