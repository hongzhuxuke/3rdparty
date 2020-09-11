#ifndef CRASHREPORT_H
#define CRASHREPORT_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class CUpLoader : public QWidget {
   Q_OBJECT
public:
   CUpLoader(QWidget *parent = 0);
   ~CUpLoader();

   void UpLoadFile(QString Url, wchar_t* wzFileName);

   void UpLoadInfo(QString Url, char* wzInfo);

public slots :
   void replyFinished(QNetworkReply*);
   void upLoadError(QNetworkReply::NetworkError errorCode);
   void OnUploadProgress(qint64 bytesSent, qint64 bytesTotal);

public:
   QNetworkAccessManager* m_pUploadManager = NULL;
};

#endif // CRASHREPORT_H
