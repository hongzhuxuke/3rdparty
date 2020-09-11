#ifndef MESSAGE_HTTPCENTER_DEFINE_H
#define MESSAGE_HTTPCENTER_DEFINE_H

#include "VH_ConstDeff.h"

enum MSG_HTTPCENTER_DEF {
   ///////////////////////////////////�����¼�///////////////////////////////////
   MSG_HTTPCENTER_HTTP_RQ = DEF_RECV_COMMONTOOLKIT_BEGIN,		// HTTP����
   MSG_HTTPCENTER_HTTP_RS,                                     // HTTPӦ��
   MSG_COMMONDATA_DATA_INIT,                                   // ���ݳ�ʼ�����
   MSG_HTTPCEMYER_HTTP_LOG_RQ,                                 // ��־�ϱ�����
};

// ����URL��������
struct STRU_HTTPCENTER_HTTP_RQ {
   DWORD m_dwPluginId;
   INT64	m_i64UserData;
   WCHAR m_wzRequestUrl[DEF_MAX_HTTP_URL_LEN + 1];            //����URL��ַ
public:
   STRU_HTTPCENTER_HTTP_RQ();
   STRU_HTTPCENTER_HTTP_RQ& operator = (const STRU_HTTPCENTER_HTTP_RQ& aoSrc);
};

// ����URL����Ӧ��
struct STRU_HTTPCENTER_HTTP_RS {
   DWORD m_dwPluginId;
   INT64	m_i64UserData;
   WCHAR m_wzRequestUrl[DEF_MAX_HTTP_URL_LEN + 1];            //����URL��ַ
   WCHAR m_wzUrlData[DEF_MAX_HTTP_URL_LEN + 1];               //URL��������
   BOOL m_bIsSuc;
   CHAR m_uid[DEF_MAX_HTTP_URL_LEN];
   bool  bHasVideo;
public:
   STRU_HTTPCENTER_HTTP_RS();
   STRU_HTTPCENTER_HTTP_RS& operator = (const STRU_HTTPCENTER_HTTP_RS& aoSrc);
};

#endif //MESSAGE_HTTPCENTER_DEFINE_H