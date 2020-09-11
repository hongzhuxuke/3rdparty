#include "cbasedlg.h"
#include <QKeyEvent>
#include <QJsonDocument> 


#include "pathmanager.h"

CBaseDlg::CBaseDlg(QWidget *parent)
: QDialog(parent)
{

}

CBaseDlg::~CBaseDlg()
{

}

void CBaseDlg::keyPressEvent(QKeyEvent *event) {
   if (event) {
      switch (event->key()) {
      case Qt::Key_Escape:
         break;
      default:
         QDialog::keyPressEvent(event);
      }
   }
}


CAliveDlg::CAliveDlg(QWidget *parent)
	:CBaseDlg(parent)
{

}

CAliveDlg::~CAliveDlg()
{

}

QString CAliveDlg::AnalysisOnOffLine(const QString& param, VhallAudienceUserInfo* Info)
{
	QString strEvent = "";
	//eyJ1c2VyX2lkIjoiMTEwOTcxMCIsImFjY291bnRfaWQiOjAsInVzZXJfbmFtZSI6IjExMSIsImF2YXRhciI6IiIsInJvb20iOiI4NzI0MjUzOTMiLCJhcHBfbmFtZSI6InZoYWxsIiwiZXZlbnQiOiJvbmxpbmUiLCJkYXRhIjp7InR5cGUiOiJvbmxpbmUiLCJ1c2VyX2lkIjoiMTEwOTcxMCIsInJvbGUiOiJndWVzdCIsInNvY2tldF9pZCI6IlVub2JMUTh6TEFJU0VxdU9BQVR3IiwiYWNjb3VudF9pZCI6MCwiY29uY3VycmVudF91c2VyIjoyLCJhdHRlbmRfY291bnQiOjMsImlzX2dhZyI6MCwidHJhY2tzTnVtIjo4MCwidXNlcl9jb25uZWN0aW9uX251bSI6IjEifSwidGltZSI6IjIwMTgtMDUtMTEgMTE6MTg6NDcifQ==
	QString trValue = QByteArray::fromBase64(param.toUtf8());
	QJsonDocument doc = QJsonDocument::fromJson(trValue.toUtf8());
	QJsonObject obj = doc.object();

	QString strRoom = CPathManager::GetObjStrValue(obj, "room");
	if (strRoom.compare(GetStreamName()) == 0)
	{
		Info->userName = CPathManager::GetObjStrValue(obj, "user_name");
		Info->userId = CPathManager::GetObjStrValue(obj, "user_id");
		strEvent = CPathManager::GetObjStrValue(obj, "event");
		QJsonObject ObjData = obj["data"].toObject();

		Info->role = CPathManager::GetObjStrValue(ObjData, "role");
		if ("0" == CPathManager::GetObjStrValue(ObjData, "is_gag"))
		{
			Info->gagType = VhallShowType_Allow;
		}
		else
		{
			Info->gagType = VhallShowType_Prohibit;
		}
		Info->miUserCount = CPathManager::GetObjStrValue(ObjData, "concurrent_user").toInt();
		
	}
	return strEvent;
}


void CAliveDlg::AnalysisLoginInfo(const QString& strInfo)
{
   QByteArray dataArray(strInfo.toStdString().c_str());
	QJsonDocument doc = QJsonDocument::fromJson(dataArray);
	QJsonObject obj = doc.object();

	if (obj.isEmpty()) {
		return;
	}

	mstrAvatar = obj["avatar"].toString();
	mstrAvatar_httpurl = QString("http:%1").arg(mstrAvatar);
	mstrNick_name = obj["nick_name"].toString();
	mstrUser_id = obj["join_uid"].toString();
	mstrRole = obj["role"].toString();

	qDebug() << "VhallRightExtraWidgetLogic::DealInitUserInfo" << obj;
}