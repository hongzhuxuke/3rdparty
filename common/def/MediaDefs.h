#ifndef _MEDIA_DEFS_INCLUDE_H__
#define _MEDIA_DEFS_INCLUDE_H__

#include "VH_ConstDeff.h"
#include <stdint.h>
#include "live_open_define.h"

struct AudioStats {
   unsigned long startTimeInMs;
   unsigned long captureTailTimeInMs;
   unsigned long lastTimesToEncodeInMs;
   unsigned long lastTimesWillSend;
   unsigned long long dataLenIsEncoded;
   unsigned long realtimeWalkclock;
};

struct VideoStats {
   unsigned long startTimeInMs;
   unsigned long captureTailTimeInMs;
   unsigned long lastTimesToEncodeInMs;
   //video buffer
   unsigned long lastTimesWillSend;
   unsigned long frameNumIsEncoded;
   unsigned long realtimeWalkclock;
};

struct VideoInfo {
   unsigned int fps;
   unsigned int gop;
   unsigned int width;
   unsigned int height;
   unsigned int bitrate;
   bool high_profile_oepn;
   bool isUse444;  //default 420p
   bool enable;
   VideoSceneType sceneType;
};

struct AudioInfo {
   unsigned long sample;
   unsigned int channels;
   unsigned int bitRate;
   bool enable;
};
struct LogFrameStatus
{
   //丢帧总数
   unsigned long dropFrameCount;
   //发送总帧数
   unsigned long totalFrameCount;
   //编码总帧数
   unsigned long encodeFrameCount;
   //时间戳
   unsigned long long os_times;
};

#ifndef MAX_BUFF
#define   MAX_BUFF 260
#endif

struct RtmpInfo {
   char rtmpURL[MAX_BUFF];
   char streamname[MAX_BUFF];
   char strUser[MAX_BUFF];
   char strPass[MAX_BUFF];
   char token[MAX_BUFF];
   int  iMultiConnNum;
   int  iMultiConnBufSize;
   bool m_bDispatch = false;
   struct Dispatch_Param m_dispatchParam;
};
struct FileInfo {
   wchar_t filePath[MAX_BUFF];
};
struct NetworkConnectParam {
   char serverIP[20];
   char streamID[128];
   char serverUrl[512];
};


typedef struct OBSOutPutInfo_st
{
   SIZE m_baseSize;
   SIZE m_outputSize;
   unsigned int videoBits;
   bool m_isAuto;
}OBSOutPutInfo;

struct EventCtx {
   MediaCoreEvent eventType;
   char* desc;
};

class IMediaCoreEvent {
public:
   virtual void OnNetworkEvent(const MediaCoreEvent& eventType,void *data=NULL) = 0;
   virtual void OnMediaCoreLog(const char *) = 0;   
   virtual void OnMediaReportLog(const char *) = 0;
   virtual void OnMeidaTransition(const wchar_t *, int length) = 0;
};

#endif //_MEDIA_CAPTURE_INCLUDE_H__
