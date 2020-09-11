#include "stdafx.h"
#include "Msg_CommonToolKit.h"

STRU_HTTPCENTER_HTTP_RQ::STRU_HTTPCENTER_HTTP_RQ() {
   memset(this, 0, sizeof(STRU_HTTPCENTER_HTTP_RQ));
}

STRU_HTTPCENTER_HTTP_RQ& STRU_HTTPCENTER_HTTP_RQ::operator=(const STRU_HTTPCENTER_HTTP_RQ& aoSrc) {
   m_i64UserData = aoSrc.m_i64UserData;
   m_dwPluginId = aoSrc.m_dwPluginId;
   wcsncpy(m_wzRequestUrl, aoSrc.m_wzRequestUrl, DEF_MAX_HTTP_URL_LEN);
   return*this;
}

STRU_HTTPCENTER_HTTP_RS::STRU_HTTPCENTER_HTTP_RS() {
   memset(this, 0, sizeof(STRU_HTTPCENTER_HTTP_RS));
}

STRU_HTTPCENTER_HTTP_RS& STRU_HTTPCENTER_HTTP_RS::operator=(const STRU_HTTPCENTER_HTTP_RS& aoSrc) {
   memset(this, 0, sizeof(STRU_HTTPCENTER_HTTP_RS));
   m_i64UserData = aoSrc.m_i64UserData;
   m_dwPluginId = aoSrc.m_dwPluginId;
   wcsncpy(m_wzRequestUrl, aoSrc.m_wzRequestUrl, DEF_MAX_HTTP_URL_LEN);
   wcsncpy(m_wzUrlData, aoSrc.m_wzUrlData, DEF_MAX_HTTP_URL_LEN);
   m_bIsSuc = aoSrc.m_bIsSuc;
   memcpy(m_uid, aoSrc.m_uid, DEF_MAX_HTTP_URL_LEN);
   bHasVideo = aoSrc.bHasVideo;
   return *this;
}