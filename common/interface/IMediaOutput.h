#ifndef __MEDIA_OUTPUT_INTERFACE_H__
#define __MEDIA_OUTPUT_INTERFACE_H__

#define MEDIA_OUTPUT_CLASSNAME L"MediaOutputSource"
//#define DECKLINK_DEVICE_NAME    L"deviceName"


/*
音频、视频输出接口

*/

class IVideoNotify {
public:
   virtual void OnVideoChanged(const unsigned long& videoWidth, const unsigned long& videoHeight, const long long & frameDuration, const long long &timeScale) = 0;
   virtual bool OnVideoFrame() = 0;
};

typedef void(*IMediaOutputRefitCallBack)(void *);


class  IMediaOutput {
public:
   virtual ~IMediaOutput() {}
   virtual void SetVideoNotify(IVideoNotify* videoNotify) = 0;
   virtual bool GetNextAudioBuffer(void **buffer, unsigned int *numFrames, unsigned long long *timestamp) = 0;
   virtual bool GetNextVideoBuffer(void *buffer, unsigned long long bufferSize, unsigned long long *timestamp) = 0;
   virtual bool GetAudioParam(unsigned int& channels, unsigned int& samplesPerSec, unsigned int& bitsPerSample) = 0;
   virtual bool GetVideoParam(unsigned long& videoWidth, unsigned long& videoHeight, long long & frameDuration, long long &timeScale) = 0; // default pixfmt   
   virtual void ResetPlayFileAudioData() = 0;
   virtual void Refit(){};
   virtual void SetRefit(IMediaOutputRefitCallBack refitCallBack,void *param){};
   
};

#endif 



