#ifndef CPATHMANAGER_H
#define CPATHMANAGER_H
#include <QString>
#include <QRect>
#include <QJsonObject>
#include <QJsonArray> 

class CPathManager {

public:
	CPathManager();
	~CPathManager();
	static QString GetAppDataPath();
	static QString GetConfigPath();
    static QString GetToolConfigPath();
	static QString GetAudiodevicePath();
	static QJsonObject GetJsonObjectFromString(const QString jsonString);
	static QJsonArray GetJsonArrayFromString(const QString jsonString);
	static QString GetStringFromJsonObject(const QJsonObject& jsonObject);
	static QString GetObjStrValue(const QJsonObject& obj, const QString& strNames);
	static bool isDirExist(const QString& strPath);
    //��ȡ΢��logoͼƬ·���ӿ�
    static QString GetLogoImagePath();
    static QString GetSplashImagePath();
    static QString GetTitleLogoImagePath();
    static QString GetVersionBackImagePath();
    static QString GetVhprocessbarbackImagePath();

private:
	static QString GetNetIP(QString webCode);
};


#endif // CPATHMANAGER_H
