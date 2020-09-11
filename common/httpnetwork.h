#ifndef HTTPNETWORK_H
#define HTTPNETWORK_H

#include <QObject>
#include <QNetworkProxy>

class QNetworkReply;
class QAuthenticator;
class QNetworkAccessManager;

class HttpNetWork : public QObject {
   Q_OBJECT

public:
   HttpNetWork(QObject *parent);
   ~HttpNetWork();

   void HttpNetworkGet(const QString url);
   void HttpNetworkPost(const QString url,const QByteArray array);

private slots:
   void Slot_OnHttpNetWorkFinished(QNetworkReply *);
   void slot_authenticationRequired(QNetworkReply *replay, QAuthenticator *auth);
   void slot_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth);

signals:
   void HttpNetworkGetFinished(QByteArray data, int code);
private:
   QMap<QNetworkReply*, QNetworkAccessManager*> mNetWorkManagerMap;
   QString mProxyUser;
   QString mProxyPwd;
};

#endif // HTTPNETWORK_H
