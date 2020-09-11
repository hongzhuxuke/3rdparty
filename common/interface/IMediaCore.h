#ifndef _INTERFACE_MEDIA_CORE_INCLUDE_H__
#define _INTERFACE_MEDIA_CORE_INCLUDE_H__

#ifdef MEDIACORE_EXPORTS
#define MEDIACORE_API __declspec(dllexport)
#else
#define MEDIACORE_API __declspec(dllimport)
#endif

//#define DEBUG_MEDIA_CORE 1  
#include <stdint.h>
#include "live_open_define.h"
#include "IAudioCapture.h"
#include "IGraphics.h"
#include "MediaDefs.h"
class MEDIACORE_API IMediaCore {
public:
   virtual ~IMediaCore(){};
   virtual bool InitCapture(IMediaCoreEvent* mediaCoreEvent,IGraphics *graphic) = 0;
   virtual void SetLogLevel(char *path,int) = 0;
   virtual void UninitCapture() = 0 ;
   virtual void StartRecord(const char *) = 0;
   virtual void StopRecord(/*const char **/) = 0;
   virtual void SuspendRecord() = 0;  //ÔÝÍ£Â¼ÖÆ
   virtual void RecoveryRecord() = 0; //»Ö¸´Â¼ÖÆ

   virtual bool StartRtmp(const char *) = 0;
   virtual bool StopRtmp(const char *) = 0;

   virtual int  RtmpCount() = 0;
   virtual bool GetStreamStatus(int index,StreamStatus *) = 0;

   virtual bool ResetPublishInfo(const char *currentUrl,const char *nextUrl) = 0;
   virtual int GetSumSpeed() = 0;
   virtual UINT64 GetSendVideoFrameCount(int index) = 0;

   virtual IDataReceiver *GetDataReceiver() = 0;

   virtual void SetAudioParam(int channels, int samplesPerSecond, int samplesBits) = 0;
   virtual void SetOpenNoiseCancelling(bool bNoiseReduction) = 0;
	virtual void SetAudioParamReduction(const int& samplesPerSecond, const int& samplesBits, const bool& bNoiseReduction) = 0;
	virtual bool IsSamplesBitsChanged(const int& samplesBits) = 0;
   virtual int GetSamplesBits() = 0;
	virtual bool IsSamplePerSecondChange(const int& samplesPerSecond) = 0;
   virtual void SetVideoParam(int gop,int fps,int bits,int w,int h) = 0;   
   virtual void SetVideoParamEx(int video_process_filters,bool is_adjust_bitrate,bool is_quality_limited,bool is_encoder_debug,bool is_saving_data_debug,int high_codec_open) = 0;
   virtual void SetProxy(bool ,std::string host, int port, std::string username, std::string password) = 0;
   virtual void SetSceneInfo(VideoSceneType sceneType) = 0;
   virtual void GetSceneInfo(VideoSceneType& sceneType) = 0;
   virtual void SetVersion(const char *) = 0;
   virtual void GetVideoParamInfo(wchar_t *) = 0;
	virtual void SetVolumeAmplificateSize(float size) = 0;
	virtual int GetHighCodec() = 0;
   virtual int LivePushAmf0Msg(const char* data,int len) = 0;

   virtual void StartVoiceTransition(bool start = false,int font = 15,int lan = 0) = 0;
   virtual void ResetVoiceTransition() = 0;
	virtual void GetMediaFileWidthAndHeight(const char* path, int &width, int& height) = 0;
};
MEDIACORE_API IMediaCore* CreateMediaCore(const wchar_t* logPath = NULL);
MEDIACORE_API  void DestoryMediaCore(IMediaCore**mediaCore);

#endif //_MEDIA_CAPTURE_INCLUDE_H__
