#include "stdafx.h"
#include "UpLoader.h"
#include <QFile>
#include <QEventLoop>
#include <QTimer>

CUpLoader::CUpLoader(QWidget *parent /*= 0*/)
: QWidget(parent) {
}

CUpLoader::~CUpLoader() {

}

void CUpLoader::upLoadError(QNetworkReply::NetworkError errorCode) {
#ifdef	OPENSDK
	;
#else
   TRACE6("%s code:%d\n", __FUNCTION__, errorCode);
#endif
   return;
}

void CUpLoader::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal) {
#ifdef	OPENSDK
	;
#else
   TRACE6("%s bytesSent %ld bytesTotal:%ld\n", __FUNCTION__, bytesSent, bytesTotal);
#endif
   return;
}

void CUpLoader::replyFinished(QNetworkReply*) {
#ifdef	OPENSDK
	;
#else
   TRACE6("%s \n", __FUNCTION__);
#endif
   return;
}

void CUpLoader::UpLoadFile(QString Url, wchar_t* wzFileName) {
#ifdef	OPENSDK
	;
#else
   QNetworkAccessManager* pUploadManager = new QNetworkAccessManager;
   QFile file(QString::fromWCharArray(wzFileName));
   file.open(QIODevice::ReadOnly);

   TRACE6("%s file :%s\n", __FUNCTION__, QString::fromWCharArray(wzFileName).toStdString().c_str());

   int file_len = file.size();
   QDataStream in(&file);
   char* pbuffer = new char[file_len];

   if (!pbuffer) {

      TRACE6("Crash buffer create fail!");

      return;
   }

   in.readRawData(pbuffer, file_len);
   file.close();

   QNetworkRequest request(Url);
   request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
   QByteArray arr = QByteArray(pbuffer, file_len);
   QEventLoop loop;
   QNetworkReply* _reply = pUploadManager->post(request, arr);
   connect(pUploadManager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
   connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
   connect(_reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(OnUploadProgress(qint64, qint64)));
   loop.exec();

   if (_reply) {
      //响应返回值
      QString qStrResult = _reply->readAll();
      _reply->deleteLater();
   }
   pUploadManager->deleteLater();

   TRACE6("Crash send end!");
#endif
}

void CUpLoader::UpLoadInfo(QString Url, char* wzInfo) {
#ifdef	OPENSDK
	;
#else
   QNetworkAccessManager* pUploadManager = new QNetworkAccessManager;
   int len = strlen(wzInfo);

   QNetworkRequest request(Url);
   request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
   QByteArray arr = QByteArray(wzInfo, len);
   QEventLoop loop;
   QNetworkReply* _reply = pUploadManager->post(request, arr);
   connect(pUploadManager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
   connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
   connect(_reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(OnUploadProgress(qint64, qint64)));

   QTimer timer;
   QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
   timer.start(2000);
   loop.exec();

   if (_reply) {
      //响应返回值
      QString qStrResult = _reply->readAll();
      _reply->deleteLater();
   }
   pUploadManager->deleteLater();
#endif
}
