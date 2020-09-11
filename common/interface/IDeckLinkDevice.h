
#ifndef __DECK_LINK_DEVICE_INTERFACE_H__
#define __DECK_LINK_DEVICE_INTERFACE_H__

#include "VH_ConstDeff.h"

#ifdef DECKLINKDEVICE_EXPORTS
#define DECKDEVICE_API __declspec(dllexport)
#else
#define DECKDEVICE_API __declspec(dllimport)
#endif

#define DECKLINK_CLASSNAME L"DeckLinkCapture"
#define DECKLINK_DEVICE_NAME    L"deviceName"
enum DeckLinkDeviceEventEnum
{
   DeckLinkDeviceAdd,
   DeckLinkDeviceRemove,
   DeckLinkDeviceImageFormatChanged,
   DeckLinkDeviceEnd
};
typedef void (*DeckLinkDeviceEventCallBack)(DeckLinkDeviceEventEnum,void *,void *);
class DeckLinkDeviceCallBackStruct
{
public:
   DeckLinkDeviceCallBackStruct(DeckLinkDeviceEventCallBack callback,void *param)
   {
      mCallback=callback;
      mParam=param;
   }
   void Notify(DeckLinkDeviceEventEnum e,void *p)
   {
      mCallback(e,p,mParam);
   }
private:
   DeckLinkDeviceEventCallBack mCallback;
   void *mParam;
};
class IVideoChangedNotify {
public:
   virtual void OnVideoChanged(unsigned long& videoWidth, unsigned long& videoHeight, long long & frameDuration, long long &timeScale) = 0;
};

//显示模式
typedef struct DecklinkDisplayMode_st
{
   unsigned long long mode;
   wchar_t name[128];
}DecklinkDisplayMode;
//像素格式
typedef struct DecklinkPixelFormat_st
{
   unsigned long long format;
   wchar_t name[128];
}DecklinkPixelFormat;

class DECKDEVICE_API IDeckLinkDevice {
public:
   virtual ~IDeckLinkDevice() {}
   virtual void EnableAudio(const bool& enableAudio) = 0;
   virtual void EnableVideo(const bool& enableVideo, IVideoChangedNotify* videoNotify) = 0;
   virtual bool GetNextAudioBuffer(void **buffer, unsigned int *numFrames, unsigned long long *timestamp) = 0;
   virtual bool GetNextVideoBuffer(void **buffer, unsigned long long *bufferSize, unsigned long long *timestamp) = 0;
   virtual bool StartCapture() = 0;
   virtual void StopCapture() = 0;
   virtual bool IsCapturing() = 0;
   virtual bool GetAudioParam(unsigned int& channels, unsigned int& samplesPerSec, unsigned int& bitsPerSample) = 0;
   virtual bool GetVideoParam(unsigned long& videoWidth, unsigned long& videoHeight, long long & frameDuration, long long &timeScale) = 0; // default pixfmt
   virtual const wchar_t* GetDeckLinkDeviceName() const = 0;
   virtual void AudioReInit()=0;
   virtual void AudioSync()=0;
   virtual bool GetDeviceInfo(UINT &w,UINT &h,int &frameInternal) = 0;
};
DECKDEVICE_API bool GetDeckLinkDeviceInfo(DeviceInfo,UINT &w,UINT &h,int &frameInternal);
DECKDEVICE_API void InitDeckLinkDeviceManager(const wchar_t* logPath = NULL);
DECKDEVICE_API void UnInitDeckLinkDeviceManager();
DECKDEVICE_API unsigned int  GetDeckLinkDeviceNum();
DECKDEVICE_API const wchar_t*  GetDeckLinkDeviceName(const unsigned int &);
DECKDEVICE_API IDeckLinkDevice* GetDeckLinkDevice(const wchar_t* deviceName,void *);
DECKDEVICE_API void  ReleaseDeckLinkDevice(IDeckLinkDevice *,void *);
DECKDEVICE_API void *RegistterDeckLinkDeviceEvent(DeckLinkDeviceEventCallBack,void *param);
DECKDEVICE_API void UnRegistterDeckLinkDeviceEvent(void *);
DECKDEVICE_API void DeckLinkEventNotify(DeckLinkDeviceEventEnum ,void *);
#endif 
