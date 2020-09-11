#include "PublishInfo.h"
#include "json.h"
#include "ConfigSetting.h"
#include "pathmanager.h"

#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>
using namespace VHJson;
using namespace std;

#if _MSC_VER >= 1600  
#pragma execution_character_set("utf-8")  
#endif  


PublishInfo::PublishInfo() :nProtocolType(0){
}


PublishInfo::~PublishInfo()
{
   mPubLineInfo.clear();
}

void PublishInfo::init(string gUserName, string gRtmpUrls, string gToken, string gStreamName, string gChanelId, bool bHideLogo) {
   mCurRtmpUrlIndex = 0;
   mUserName = gUserName;
   mRtmpUrlsStr = gRtmpUrls;
   mToken = gToken;
   mStreamName = gStreamName;
   mChanelId = gChanelId;
   m_bHideLogo = bHideLogo;
   parsePubLineStr(gRtmpUrls);
   
}
void PublishInfo::InitMultitcpPubLine(QString customUrlFile) {
   QFile f;
   f.setFileName(customUrlFile);
   if(!f.open(QIODevice::ReadOnly)) {
      return ;
   }
   
   QByteArray ba = f.readAll();
   f.close();

   ba=QString::fromUtf8(ba).toUtf8();
   qDebug()<<"ba"<<ba;

   QJsonDocument doc = QJsonDocument::fromJson(ba);
   QJsonArray array = doc.array();

   for(int i = 0 ;i < array.count() ; i++) {
      QJsonObject obj=array[i].toObject();
      QString displayName=obj["displayName"].toString();
      QString url=obj["url"].toString();
      PubLineInfo pl;
      pl.mID = mPubLineInfo.count();
      pl.mStrAlias = displayName.toUtf8().toStdString();
      pl.mStrDomain = url.toLocal8Bit().toStdString();
      mPubLineInfo.insert(i,pl);
   }
}

void PublishInfo::parsePubLineStr(string gPubLineList){
   VHJson::Reader reader;
   VHJson::Value value;
   if (!reader.parse(gPubLineList, value)) {
      return;
   }
   int j = 0;
   for (int i = 0; i < value.size(); i++){
      string alias = value[i]["alias"].asString();
      string url = value[i]["url"].asString();
      PubLineInfo pl;
      pl.mID = i;
      pl.mStrAlias = alias;
      pl.mStrDomain = url;
      //多TCP参数为0
      //pl.mIMultiConnBufSize = 0;
      //pl.mIMultiConnNum = 0 ;
      mPubLineInfo.insert(i, pl);
      QUrl qurl = QUrl(QString::fromStdString(url));
	  QString toolConfPath = CPathManager::GetAppDataPath() + QString::fromStdWString(VHALL_TOOL_CONFIG);
	  int hideLogo = ConfigSetting::ReadInt(toolConfPath, GROUP_DEFAULT, PROXY_NOLINE, 0);

      if (qurl.scheme()=="rtmp"&& 1!=hideLogo) {
         qurl.setScheme("http");
         qurl.setPort(443);

         PubLineInfo plHttp;
         plHttp.mID = value.size()+j;

		 QString t = QString::fromUtf8("网络代理线路-");
		 plHttp.mStrAlias = (t.toUtf8() + QString::fromUtf8(alias.c_str())).toUtf8();
         plHttp.mStrDomain = qurl.toString().toStdString();
         mPubLineInfo.insert(value.size() + j, plHttp);
         j++;
      }

      if (i==0){
         mCurRtmpUrl = pl.mStrDomain;
      }
   }
}