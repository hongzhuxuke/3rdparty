#include "ConfigSetting.h"
#include <QMessageBox>
#include <QSettings>

bool ConfigSetting::writeValue(const QString& path, const QString& group, const QString& user_key, const QVariant &value) {
   if (path.isEmpty() || user_key.isEmpty()) {
      return false;
   } else {
      QSettings config(path, QSettings::IniFormat);

      config.beginGroup(group);
      config.setValue(user_key, value);
      config.endGroup();

      return true;
   }
}

int ConfigSetting::ReadInt(const QString& path, const QString& group, const QString& user_key, const int& defaultValue) {
   int retVal = 0;
   QSettings config(path, QSettings::IniFormat);
   QString fullKey = group + QString("/") + user_key;

   if (config.contains(fullKey)) {
      QVariant retVar = config.value(group + QString("/") + user_key);
      retVal = retVar.toInt();
   } else {
      retVal = defaultValue;
   }
   return retVal;
}
QString ConfigSetting::ReadString(const QString& path, const QString& group, const QString& user_key, const QString& defaultValue) {
   QString retVal;
   QSettings config(path, QSettings::IniFormat);
   QString fullKey = group + QString("/") + user_key;
   if (config.contains(fullKey)) {
      QVariant retVar = config.value(group + QString("/") + user_key);
      retVal = retVar.toString();
   } else {
      retVal = defaultValue;
   }
   return retVal;
}
