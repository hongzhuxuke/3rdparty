#ifndef MESSAGE_HTTPCENTER_DEFINE_H
#define MESSAGE_HTTPCENTER_DEFINE_H

#include "VH_ConstDeff.h"

enum MSG_HTTPCENTER_DEF {
   ///////////////////////////////////接收事件///////////////////////////////////
   MSG_HTTPCENTER_HTTP_RQ = DEF_RECV_COMMONTOOLKIT_BEGIN,		// HTTP请求
   MSG_HTTPCENTER_HTTP_RS,                                     // HTTP应答
   MSG_COMMONDATA_DATA_INIT,                                   // 数据初始化完成
   MSG_HTTPCEMYER_HTTP_LOG_RQ,                                 // 日志上报请求
};

// 访问URL数据请求
struct STRU_HTTPCENTER_HTTP_RQ {
   DWORD m_dwPluginId;
   INT64	m_i64UserData;
   WCHAR m_wzRequestUrl[DEF_MAX_HTTP_URL_LEN + 1];            //请求URL地址
public:
   STRU_HTTPCENTER_HTTP_RQ();
   STRU_HTTPCENTER_HTTP_RQ& operator = (const STRU_HTTPCENTER_HTTP_RQ& aoSrc);
};

// 访问URL数据应答
struct STRU_HTTPCENTER_HTTP_RS {
   DWORD m_dwPluginId;
   INT64	m_i64UserData;
   WCHAR m_wzRequestUrl[DEF_MAX_HTTP_URL_LEN + 1];            //请求URL地址
   WCHAR m_wzUrlData[DEF_MAX_HTTP_URL_LEN + 1];               //URL请求数据
   BOOL m_bIsSuc;
   CHAR m_uid[DEF_MAX_HTTP_URL_LEN];
   bool  bHasVideo;
public:
   STRU_HTTPCENTER_HTTP_RS();
   STRU_HTTPCENTER_HTTP_RS& operator = (const STRU_HTTPCENTER_HTTP_RS& aoSrc);
};

#endif //MESSAGE_HTTPCENTER_DEFINE_H