#include "vhsqllite.h"
#include <QDebug>
#include <QDir>
bool VHSqlLite::createDataFile(const QString &strFileName)
{
    if(!QFile::exists(strFileName))
    {
        QDir fileDir = QFileInfo(strFileName).absoluteDir();
        QString strFileDir = QFileInfo(strFileName).absolutePath();
        if(!fileDir.exists())
        {
            fileDir.mkpath(strFileDir);
        }
        QFile dbFile(strFileName);
        if(!dbFile.open(QIODevice::WriteOnly))
        {
            dbFile.close();
            return false;
        }
        dbFile.close();
    }
    return true;
}
bool VHSqlLite::openDataBase(const QString& strFileName)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(strFileName);
    if(m_db.open())
    {
        return true;
    }
    return false;
}
void VHSqlLite::closeDataBase()
{
    if(m_db.isOpen()) {
        m_db.close();
    }
}
bool execWithNoReturn(QString sql) {
    return true;
}
QSqlQuery &VHSqlLite::GetQuery() {
    query=QSqlQuery(m_db);
    return query;
}
bool VHSqlLite::Init(QString dbName) {
    return openDataBase(dbName);
}
VHSqlLite::VHSqlLite(QObject *parent) : QObject(parent)
{

}

VHSqlLite::~VHSqlLite()
{
    closeDataBase();
}

