#ifndef _VHALLSTREAMREMUX_H_
#define _VHALLSTREAMREMUX_H_
#include <QObject>
#include <QString>
#include <QSet>
#include <QThread>
#include <QMap>
#include <QTimer>
#include <QMutex>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonObject>

class HttpNetWork;

struct MixParam {
   QJsonObject obj;
   bool cancelMix;
};

class VhallStreamRemix : public QObject {
   Q_OBJECT

public:
   explicit VhallStreamRemix(QObject *parent = 0);
   ~VhallStreamRemix();
   void SetStreamid(QString streamId) { mStreamId = streamId; }
   void CancelRemix();
   void RemixStream(QString &desktopId, QSet<QString> & cameras, QSet<QString> &mic);

   void StopMixStream(const QString& streamID);
   void Init(int);

public slots:
   void slot_OnHttpPostNetWorkfinished(QByteArray, int);
   void slot_OnMixStream();
signals:
   void SigRemixEnd(bool);
   void SigFadout(QString);
   void SigTimeExpired();
private:
   void PostBody(QJsonObject &, bool bCancelMix = false);
   void InsertUserLayOut(QJsonArray &input_stream_list, int &startLayout, const QList<QString> &users,bool audioType = false);

private:
   QString mStreamId;
   int m_appid = 1253248467;
   HttpNetWork* mHttpNetwork = NULL;   
   int mLastEventID = 0;
   QMutex mixParamMutex;
   QList<MixParam> mMixStreamParamList;
   QTimer mMixStreamTimer;
};
#endif