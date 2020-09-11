#ifndef VHHTTP_H
#define VHHTTP_H

#include <QObject>
#include <QJsonObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QEventLoop>
#include <QTimer>

class QAuthenticator;
class QNetworkProxy;

class VHHttp : public QObject
{
    Q_OBJECT
public:
    explicit VHHttp(QObject *parent = 0);
    ~VHHttp();
    QJsonObject Get(QString url,int timeout = 0);

signals:
    void SigNetworkFinished();
public slots:
    void finished(QNetworkReply *);
    void timeout();

private slots:
    void slot_authenticationRequired(QNetworkReply *, QAuthenticator *);
    void slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);

private:
    QNetworkAccessManager mManager;
    QTimer mTimer;
    QEventLoop mLoop;
    QJsonObject mRet;


    QString m_ip;
    int m_port;
    QString m_user;
    QString m_pwd;
};

#endif // VHHTTP_H
