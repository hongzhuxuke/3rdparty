#ifndef __INTERFACE_AUDIO_CAPTURE_INCLUDE__
#define __INTERFACE_AUDIO_CAPTURE_INCLUDE__

#ifdef VHALL_EXPORTS
#define VHALL_API __declspec(dllexport)
#else
#define VHALL_API __declspec(dllimport)
#endif

//#define AUDIOENGINE_API


#include "VH_ConstDeff.h"
#include "IDeckLinkDevice.h"
class IAudioMeter {
public:
   virtual void UpdateMeter(float fTopVal, float fTopMax, float fTopPeak);
};

class IMediaOutput;
class VHALL_API IAudioCapture {
public:
   virtual ~IAudioCapture(){};
   virtual bool InitPlaybackSource(wchar_t* deviceId) = 0;
   virtual bool AddRecodingSource(const TYPE_RECODING_SOURCE& sourceType,
                                  void **pAudioSource,
                                  const bool& isNoiseGate,
                                  void **param,
                                  const int& paramCnt) = 0;

   //设置麦克风设备
	virtual bool SetMicDeviceInfo(DeviceInfo, bool isNoise, int closeThreshold, int openThreshold , int iAudioSampleRate) = 0;
   virtual bool SetNoiseGate(bool isNoise, int closeThreshold, int openThreshold) = 0;
   
   virtual bool DelAudioSource(void *auduiSource) = 0;
   virtual void SetPlaybackVolume(const float& plackBackVol) = 0;
   virtual void SetMicVolume(const float& micVol) = 0;
   virtual void ClearRecodingSource() = 0;
   virtual void SetAudioListening(IDataReceiver* dataReceiver) = 0;
   virtual bool Start() = 0;
	//virtual bool Stop() = 0;
   virtual void SetPriority(int) = 0;
   virtual void SetInterval(int) = 0;
   virtual void Shutdown() = 0;
   virtual unsigned int GetSampleRateHz() const = 0;
   virtual unsigned int NumAudioChannels() const = 0;
   virtual unsigned int GetBitsPerSample() const = 0;
   virtual void SetSaveMicAudio(bool bSave) = 0;
   virtual void GetAudioMeter(float& audioMag, float& audioPeak, float& audioMax) = 0;
   virtual void GetMicOrignalAudiometer(float& audioMag, float& audioPeak, float& audioMax) = 0;
   
   virtual float MuteMic(bool)=0;
   virtual float MuteSpeaker(bool)=0;
   virtual float GetMicVolunm()=0;
   virtual float GetSpekerVolumn()=0;
   virtual bool GetCurrentMic(DeviceInfo &deviceInfo)=0;
   virtual void SetForceMono(bool)= 0;
   virtual bool GetForceMono() = 0;
	virtual void SetSampleRateHz(const int& iSampleRateHz) = 0;
};



VHALL_API IAudioCapture* CreateAudioCapture(IDataReceiver* dataReceiver, int iSampleRateHz);
VHALL_API void DestroyAudioCapture(IAudioCapture** audioCapture);



#endif 

