#include "pathmanager.h"
#include <QStandardPaths>
#include "ConfigSetting.h"
#include <QDebug>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir> 

CPathManager::CPathManager()
{

}

CPathManager::~CPathManager()
{

}

QString CPathManager::GetAppDataPath()
{
   QString strAppDataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

	//int iPos = strAppDataPath.lastIndexOf("/");
	//strAppDataPath = strAppDataPath.mid(0, iPos);
	strAppDataPath.append("\\VHallHelper\\");
	return strAppDataPath;
}

QString CPathManager::GetConfigPath()
{
	QString strAppDataPath = GetAppDataPath() + QString::fromStdWString(CONFIGPATH);
	return strAppDataPath;
}

QString CPathManager::GetToolConfigPath() {
    QString strAppDataPath = GetAppDataPath() + QString::fromStdWString(VHALL_TOOL_CONFIG);
    return strAppDataPath;
}

QString CPathManager::GetAudiodevicePath()
{
	QString strAppDataPath = GetAppDataPath() + QString::fromStdWString(CONFIGPATH_DEVICE);

	return strAppDataPath;
}

QJsonObject CPathManager::GetJsonObjectFromString(const QString jsonString)
{
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
	if (jsonDocument.isNull()){
		//qDebug() << "===> please check the string " << jsonString.toLocal8Bit().data();
	}
	QJsonObject jsonObject = jsonDocument.object();
	return jsonObject;
}

QJsonArray CPathManager::GetJsonArrayFromString(const QString jsonString)
{
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
	if (jsonDocument.isNull()) {
		//qDebug() << "===> please check the string " << jsonString.toLocal8Bit().data();
	}
	QJsonArray jsonObject = jsonDocument.array();
	return jsonObject;
}

QString CPathManager::GetStringFromJsonObject(const QJsonObject& jsonObject)
{
	return QString(QJsonDocument(jsonObject).toJson());
}

QString CPathManager::GetObjStrValue(const QJsonObject& obj, const QString& strNames)
{
	QString strValue = "";
	if (obj[strNames].isString())
	{
		strValue = obj[strNames].toString();
	}
	else
	{
		strValue = QString::number(obj[strNames].toInt());
	}
	return strValue;
}

bool CPathManager::isDirExist(const QString& strPath)
{
	QDir dir(strPath);
	if (dir.exists())
	{
		return true;
	}
	else
	{
		bool ok = dir.mkpath(strPath);//创建多级目录
		return ok;
	}
}

QString CPathManager::GetLogoImagePath() {
    QString logoImagePath = QCoreApplication::applicationDirPath() + "/login_logo.png";
    QString path = logoImagePath.replace("\\","/");
    return path;
}

QString CPathManager::GetSplashImagePath() {
    QString logoImagePath = QCoreApplication::applicationDirPath() + "/splash.png";
    QString path = logoImagePath.replace("\\", "/");
    return path;
}

QString CPathManager::GetTitleLogoImagePath() {
    QString logoImagePath = QCoreApplication::applicationDirPath() + "/vhallTitleLogo.png";
    QString path = logoImagePath.replace("\\", "/");
    return path;
}

QString CPathManager::GetVersionBackImagePath() {
    QString logoImagePath = QCoreApplication::applicationDirPath() + "/versionBack.png";
    QString path = logoImagePath.replace("\\", "/");
    return path;
}

QString CPathManager::GetVhprocessbarbackImagePath() {
    QString logoImagePath = QCoreApplication::applicationDirPath() + "/vhprocessbarback.png";
    QString path = logoImagePath.replace("\\", "/");
    return path;
}

QString CPathManager::GetNetIP(QString webCode)
{
	QString web = webCode.replace(" ", "");
	web = web.replace("\r", "");
	web = web.replace("\n", "");
	QStringList list = web.split("<CENTER>");
	if (list.size() < 2)
	{
		qDebug() << list.at(0);
		return "";
	}
	QString tar = list[1];
	QStringList ipTar = tar.split("</h2></CENTER>");
	if (ipTar.size() < 2)
	{
		return "";
	}
	QStringList ipAll = ipTar.at(0).split("<h2>");
	if (ipAll.size() < 2)
	{
		return "";
	}
	QStringList ip = ipAll.at(1).split(",");
	if (ip.size() < 2)
	{
		return "";
	}
	return  ip.at(0);
}
