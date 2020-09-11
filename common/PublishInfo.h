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
   string mStrAlias;//��ʾ����
   string mStrDomain;//����
   string mPingInfo;
   //��������Ϊ��TCP���ݣ�����Ϊ0����ΪRTMP������Ϊ��TCP
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

   //��������Ϊ��TCP���ݣ�����Ϊ0����ΪRTMP������Ϊ��TCP
   int  mIMultiConnNum = 0;
   int  mIMultiConnBufSize = 0;
   QString mRoomToken; //����΢�𻥶���Ҫ��token
   QString mPluginUrl; //�ĵ����url
	QString mMsgToken;
   QString mUserId;//�������û�ID�����   
   QString mWebinarType;//flash���ݲ������Ƿ�Ϊ��α�� ismix 
   QString mAccesstoken;//flash���ݲ���
   QString mRole;
   QString mScheduler;
   QString mWebinarName;
};

