#pragma once
#include <string>  
using namespace std;
#include <QList>
#include <QJsonArray>
#include <QMutex>
#include <QJsonParseError>
#include <QVariant>
#include "json.h"

struct PubLineInfo
{
   int mID;
   string mStrAlias;//显示名称
   string mStrDomain;//域名
   string mPingInfo;
   //以下内容为多TCP内容，若均为0，则为RTMP，否则为多TCP
   //int  mIMultiConnNum = 0;
   //int  mIMultiConnBufSize = 0;
}; 

class PublishInfo
{
public:
   PublishInfo();
   ~PublishInfo();
public:
   void init(string gUserName, string gToken, string gStreamName, string gChanelId, string gRtmpUrl, bool bHideLogo);
   void parsePubLineStr(string gPubLineList);
   void InitMultitcpPubLine(QString customUrlFile);
public:
   string mUserName;
   string mToken;
   string mStreamName;
   string mChanelId;
   string mRtmpUrlsStr;
   string mCurRtmpUrl;
   int mCurRtmpUrlIndex;
   int nProtocolType;
   bool m_bHideLogo = false;
   QList<PubLineInfo> mPubLineInfo;

   //以下内容为多TCP内容，若均为0，则为RTMP，否则为多TCP
   int  mIMultiConnNum = 0;
   int  mIMultiConnBufSize = 0;
   QString mRoomToken; //加入微吼互动需要的token
   QString mPluginUrl; //文档插件url
	QString mMsgToken;
   QString mUserId;//流名与用户ID的组合   
   QString mWebinarType;//flash传递参数，是否为多嘉宾活动 ismix 
   QString mAccesstoken;//flash传递参数
   QString mRole;
   QString mScheduler;
   QString mWebinarName;
};

