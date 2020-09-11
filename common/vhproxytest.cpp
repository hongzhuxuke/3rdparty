#include "vhproxytest.h"
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QAuthenticator>
#include <QEventLoop>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "pathmanager.h"
#include "ConfigSetting.h"

#include "DebugTrace.h"

bool HttpRequest::VHProxyTest(QString ip, unsigned short port, QString userName,
   QString password,QString url){
   TRACE6("VHProxyTest %s %d %s %s %s\n",
      ip.toLocal8Bit().data(),
      port,
      userName.toLocal8Bit().data(),
      password.toLocal8Bit().data(),
      url.toLocal8Bit().data()
   );
   QNetworkProxy proxy;
   proxy.setType(QNetworkProxy::HttpProxy);
   proxy.setHostName(ip);
   proxy.setPort(port);
   proxy.setUser(userName);
   proxy.setPassword(password);

   QNetworkAccessManager manager;
   QTimer timer ;
   
   manager.setProxy(proxy);
   QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
   QEventLoop loop;
   QObject::connect(&manager,SIGNAL(finished(QNetworkReply *)),&loop,SLOT(quit()));
   QObject::connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
   timer.start(1500);
   
   loop.exec();
   timer.stop();

   reply->deleteLater();

   if(!reply->isFinished()) {
      TRACE6("VHProxyTest %s %d %s %s %s Failed![Timeout]\n",
         ip.toLocal8Bit().data(),
         port,
         userName.toLocal8Bit().data(),
         password.toLocal8Bit().data(),
         url.toLocal8Bit().data()
         );
      return false;
   }

   if(reply->error()!=QNetworkReply::NoError) {
      TRACE6("VHProxyTest %s %d %s %s %s Error![%d]\n",
         ip.toLocal8Bit().data(),
         port,
         userName.toLocal8Bit().data(),
         password.toLocal8Bit().data(),
         url.toLocal8Bit().data(),
         reply->error()
         );
      return false;
   }
   
   QByteArray ba = reply->readAll();
   QJsonDocument doc = QJsonDocument::fromJson(ba);
   QJsonObject obj = doc.object();
   int code = obj["code"].toInt();
   if(code!=200) {
      TRACE6("VHProxyTest %s %d %s %s %s Failed![%s]\n",
         ip.toLocal8Bit().data(),
         port,
         userName.toLocal8Bit().data(),
         password.toLocal8Bit().data(),
         url.toLocal8Bit().data(),
         ba.data()
         );

      return false;
   }

   QList<QByteArray> rawHeaderList = reply->rawHeaderList();
   qDebug()<<"rawHeaderList"<<rawHeaderList;
   
   TRACE6("VHProxyTest %s %d %s %s %s Successed![%s]\n",
      ip.toLocal8Bit().data(),
      port,
      userName.toLocal8Bit().data(),
      password.toLocal8Bit().data(),
      url.toLocal8Bit().data(),
      ba.data()
      );

   return true;
}
bool VHGet(QString url, int timeout, QByteArray &ba) {

   TRACE6("VHGet %s %d\n",url.toUtf8().data(),timeout);
   QNetworkAccessManager manager;
   QTimer timer ;
   
   QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
   QEventLoop loop;
   QObject::connect(&manager,SIGNAL(finished(QNetworkReply *)),&loop,SLOT(quit()));
   QObject::connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));

   if(timeout > 0) {
      timer.start(timeout);
   }
   
   loop.exec();
   timer.stop();

   reply->deleteLater();

   if(!reply->isFinished()) {
      TRACE6("VHGet %s TimeOut\n",url.toUtf8().data());
      return false;
   }

   if(reply->error()!=QNetworkReply::NoError) {      
      TRACE6("VHGet %s Error %d\n",url.toUtf8().data(),QNetworkReply::NoError);
      return false;
   }

   ba = reply->readAll();
   return true;
}
bool HttpRequest::VHGetRet(QString url, int timeout, QByteArray &ba) {
   QString qsConfPath = CPathManager::GetConfigPath();
   QNetworkProxy proxy;
   int is_http_proxy = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_OPEN, 0);
   if (is_http_proxy) {
      m_ip = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_HOST, "");
      m_port = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_PORT, 80);
      m_user = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_USERNAME, "");
      m_pwd = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_PASSWORD, "");
      proxy.setHostName(m_ip);
      proxy.setPort(m_port);
      proxy.setType(QNetworkProxy::HttpProxy);
   } else {
      proxy.setType(QNetworkProxy::NoProxy);
      QNetworkProxy::setApplicationProxy(proxy);
   }

   TRACE6("VHGetRet url:%s ,timeout = %d\n",url.toLocal8Bit().data(),timeout);
   QNetworkAccessManager manager;
   QTimer timer ;
   manager.setProxy(proxy);
   QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
   QEventLoop loop;
   QObject::connect(&manager,SIGNAL(finished(QNetworkReply *)),&loop,SLOT(quit()));
   QObject::connect(&manager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this, SLOT(slot_authenticationRequired(QNetworkReply *, QAuthenticator *)));
   QObject::connect(&manager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), this, SLOT(slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
   QObject::connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));

   if(timeout > 0) {
      timer.start(timeout);
   }
   
   loop.exec();
   timer.stop();

   reply->deleteLater();

   if(!reply->isFinished()) {
      int errCode = reply->error();
      qDebug()<<"reply is not Finished!";
      return false;
   }
   int errCode = reply->error();
   if (errCode != QNetworkReply::NoError) {
      qDebug()<<"reply is error!";
      TRACE6("VHGetRet errCode £º%d\n",errCode);
      return false;
   }
   
   ba = reply->readAll();
   return true;

}

bool HttpRequest::VHPostRet(QString url, int timeout, QByteArray &ba, QString &json) {
   TRACE6("VHGetRet url:%s ,timeout = %d\n", url.toLocal8Bit().data(), timeout);
   QNetworkAccessManager manager;
   QTimer timer;

   QNetworkReply *reply = manager.post(QNetworkRequest(QUrl(url)), QByteArray(json.toStdString().c_str()));
   QEventLoop loop;
   QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));
   QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

   if (timeout > 0) {
      timer.start(timeout);
   }

   loop.exec();
   timer.stop();

   reply->deleteLater();

   if (!reply->isFinished()) {
      qDebug() << "reply is not Finished!";
      return false;
   }

   if (reply->error() != QNetworkReply::NoError) {
      qDebug() << "reply is error!";
      return false;
   }

   ba = reply->readAll();

   return true;
}

HttpRequest::HttpRequest(QObject* obj):QObject(obj) {

}

HttpRequest::~HttpRequest() {

}

void HttpRequest::slot_authenticationRequired(QNetworkReply *, QAuthenticator *auth) {
   if (auth && !auth->isNull()) {
      auth->setUser(m_user);
      auth->setPassword(m_pwd);
   }
}

void HttpRequest::slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *auth) {
   if (auth && !auth->isNull()) {
      auth->setUser(m_user);
      auth->setPassword(m_pwd);
   }
}


