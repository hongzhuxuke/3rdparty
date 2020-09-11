#ifndef VHSQLLITE_H
#define VHSQLLITE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
class VHSqlLite : public QObject
{
    Q_OBJECT
public:
    explicit VHSqlLite(QObject *parent = 0);
    ~VHSqlLite();
    bool Init(QString dbName);
    QSqlQuery &GetQuery();
private:
    bool createDataFile(const QString &strFileName);
    bool openDataBase(const QString& strFileName);
    void closeDataBase();
private:
    QSqlDatabase m_db;
    QSqlQuery query;
};

#endif // VHSQLLITE_H
