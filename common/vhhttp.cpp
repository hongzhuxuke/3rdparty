#include "vhhttp.h"
#include "DebugTrace.h"
#include "ConfigSetting.h"
#include "pathmanager.h"

#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QtNetwork/QNetworkRequest>
#include <QDebug>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QAuthenticator>

VHHttp::VHHttp(QObject *parent) : QObject(parent) {
    connect(&mManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(finished(QNetworkReply*)));
    connect(&mTimer,SIGNAL(timeout()),&mLoop,SLOT(quit()));
    connect(&mTimer,SIGNAL(timeout()),&mTimer,SLOT(stop()));
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(timeout()));
    connect(this,SIGNAL(SigNetworkFinished()),&mLoop,SLOT(quit()));

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
    QObject::connect(&mManager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this, SLOT(slot_authenticationRequired(QNetworkReply *, QAuthenticator *)));
    QObject::connect(&mManager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), this, SLOT(slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));

}

VHHttp::~VHHttp() {

}
void VHHttp::finished(QNetworkReply *r) {
   if(r) {
      if(r->error()==QNetworkReply::NoError){
         QByteArray ba=r->readAll();
         QJsonDocument doc=QJsonDocument::fromJson(ba);
         mRet=doc.object();
         r->deleteLater();
      }
      else {
         mRet = QJsonObject();
      }
   }

   emit this->SigNetworkFinished();
}
void VHHttp::timeout() {
   TRACE6("VHHttp::timeout\n");
}

QJsonObject VHHttp::Get(QString urlStr,int timeout) {
   
    QUrl url=QUrl(urlStr);
    QNetworkRequest req=QNetworkRequest(url);
    req.setRawHeader("HOST", url.host().toUtf8());
    req.setRawHeader("PORT", QString::number(url.port()).toUtf8());  
    
    mManager.get(req);
    if(timeout>0){
        mTimer.start(timeout);
    }
    mLoop.exec();

    QJsonDocument doc ;
    doc.setObject(mRet);

    TRACE6("Get[%s] [%s]\n",urlStr.toLocal8Bit().data(),doc.toJson(QJsonDocument::Compact));

    return mRet;
}

void VHHttp::slot_authenticationRequired(QNetworkReply *, QAuthenticator *auth) {
   if (auth && !auth->isNull()) {
      auth->setUser(m_user);
      auth->setPassword(m_pwd);
   }
}

void VHHttp::slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *auth) {
   if (auth && !auth->isNull()) {
      auth->setUser(m_user);
      auth->setPassword(m_pwd);
   }
}
