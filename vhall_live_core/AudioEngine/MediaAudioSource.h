#ifndef _MEDIA_AUDIO_SOURCE_INCLUDE__
#define _MEDIA_AUDIO_SOURCE_INCLUDE__

#include "IAudioSource.h"
class IMediaOutput;
class MediaAudioSource : public IAudioSource {
public:
   MediaAudioSource(IMediaOutput* deckLinkDevice, UINT dstSampleRateHz/* = 44100*/);
   ~MediaAudioSource();
   virtual void ClearAudioBuffer();
protected:
   virtual bool GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp);
   virtual void ReleaseBuffer();
   virtual CTSTR GetDeviceName() const;
public:
   virtual bool Initialize();
private:
   IMediaOutput* mMediaOutput;
   UINT  mInputChannels;
   UINT  mInputSamplesPerSec;
   UINT  mInputBitsPerSample;
   UINT  mInputBlockSize;
   bool  mIsMediaOutInit;

   List<BYTE> mSampleBuffer;
   List<BYTE> mOutputBuffer;
   UINT  mSampleFrameCountIn100MS;   // only return 100ms data at once
   UINT mSampleSegmentSizeIn100Ms;   //100ms data size
   QWORD mLastestAudioSampleTime;
   int mLogCount = 0;
};

#endif 
