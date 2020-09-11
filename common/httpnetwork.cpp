#include "httpnetwork.h"
#include "pathmanager.h"
#include "ConfigSetting.h"
#include "DebugTrace.h"
#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QByteArray>


HttpNetWork::HttpNetWork(QObject *parent)
   : QObject(parent)
{

}

HttpNetWork::~HttpNetWork()
{
   TRACE6("%s release network start \n", __FUNCTION__);
   QMap<QNetworkReply*, QNetworkAccessManager*>::iterator iter = mNetWorkManagerMap.begin();
   while (iter != mNetWorkManagerMap.end()) {
      disconnect(iter.value(), SIGNAL(finished(QNetworkReply *)), this, SLOT(Slot_OnHttpNetWorkFinished(QNetworkReply *)));
      disconnect(iter.value(), SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this, SLOT(slot_authenticationRequired(QNetworkReply *, QAuthenticator *)));
      disconnect(iter.value(), SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), this, SLOT(slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
      if (iter.key()->isRunning()) {
         TRACE6("%s QNetworkReply is running \n", __FUNCTION__);
         iter.key()->abort();
         iter.key()->close();
      }
      delete iter.value();
      iter++;
   }
   mNetWorkManagerMap.clear();
   TRACE6("%s release network end \n", __FUNCTION__);
}

void HttpNetWork::HttpNetworkGet(const QString requestUrl) {
   QNetworkRequest request;
   request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
   request.setUrl(QUrl(requestUrl));

   QNetworkProxy proxy;
   QString qsConfPath = CPathManager::GetConfigPath();
   int is_http_proxy = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_OPEN, 0);
   if (is_http_proxy) {
      QString host = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_HOST, "");
      int port = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_PORT, 80);
      mProxyUser = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_USERNAME, "");
      mProxyPwd = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_PASSWORD, "");

      proxy.setType(QNetworkProxy::HttpProxy);
      proxy.setHostName(host);
      proxy.setPort(port);
      proxy.setUser(mProxyUser);
      proxy.setPassword(mProxyPwd);
      QNetworkProxy::setApplicationProxy(proxy);
   } else {
      proxy.setType(QNetworkProxy::NoProxy);
      QNetworkProxy::setApplicationProxy(proxy);
   }

   QNetworkAccessManager *networkManager = new QNetworkAccessManager();
   if (networkManager != NULL) {
      networkManager->setProxy(proxy);
      connect(networkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(Slot_OnHttpNetWorkFinished(QNetworkReply *)));
      connect(networkManager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this, SLOT(slot_authenticationRequired(QNetworkReply *, QAuthenticator *)));
      connect(networkManager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), this, SLOT(slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
      QNetworkReply *replay = networkManager->get(request);
      mNetWorkManagerMap[replay] = networkManager;
   }
}

void HttpNetWork::HttpNetworkPost(const QString url, const QByteArray bodyByteArray) {
   QNetworkProxy proxy;
   QString qsConfPath = CPathManager::GetConfigPath();
   int is_http_proxy = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_OPEN, 0);
   if (is_http_proxy) {
      QString host = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_HOST, "");
      int port = ConfigSetting::ReadInt(qsConfPath, GROUP_DEFAULT, PROXY_PORT, 80);
      mProxyUser = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_USERNAME, "");
      mProxyPwd = ConfigSetting::ReadString(qsConfPath, GROUP_DEFAULT, PROXY_PASSWORD, "");

      proxy.setType(QNetworkProxy::HttpProxy);
      proxy.setHostName(host);
      proxy.setPort(port);
      proxy.setUser(mProxyUser);
      proxy.setPassword(mProxyPwd);
      QNetworkProxy::setApplicationProxy(proxy);
   } else {
      proxy.setType(QNetworkProxy::NoProxy);
      QNetworkProxy::setApplicationProxy(proxy);
   }

   QNetworkAccessManager *networkManager = new QNetworkAccessManager();
   if (networkManager != NULL) {
      networkManager->setProxy(proxy);
      connect(networkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(Slot_OnHttpNetWorkFinished(QNetworkReply *)));

      connect(networkManager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this, SLOT(slot_authenticationRequired(QNetworkReply *, QAuthenticator *)));
      connect(networkManager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)), this, SLOT(slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));

      QNetworkRequest request;
      request.setUrl(url);
      request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
      QNetworkReply* reply = networkManager->post(request, bodyByteArray);

      mNetWorkManagerMap[reply] = networkManager;
   }
}

void HttpNetWork::Slot_OnHttpNetWorkFinished(QNetworkReply *replay) {

   QByteArray array;
   int codeErr = 0;
   if (replay) {
      array = replay->readAll();
      codeErr = replay->error();
      QMap<QNetworkReply*, QNetworkAccessManager*>::iterator iter = mNetWorkManagerMap.find(replay);
      if (iter != mNetWorkManagerMap.end()) {
         iter.value()->deleteLater();
         iter.key()->deleteLater();
         mNetWorkManagerMap.erase(iter);
      }
   }
   emit HttpNetworkGetFinished(array, codeErr);
}

void HttpNetWork::slot_authenticationRequired(QNetworkReply *replay, QAuthenticator *auth) {
   if (auth && !auth->isNull()) {
      auth->setUser(mProxyUser);
      auth->setPassword(mProxyPwd);
   }
}

void HttpNetWork::slot_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth) {
   if (auth && !auth->isNull()) {
      auth->setUser(mProxyUser);
      auth->setPassword(mProxyPwd);
   }
}


