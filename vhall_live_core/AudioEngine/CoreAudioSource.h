#ifndef __CORE_AUDIO_SOURCE_INCLUDE__
#define __CORE_AUDIO_SOURCE_INCLUDE__

#include "IAudioSource.h"
#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <propsys.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <endpointvolume.h>

//#define DEBUG_CORE_AUDIO_SOURCE      1 
typedef  QWORD(*FUN_GetVideoTime)();

class CBlankAudioPlayback {
public:
   CBlankAudioPlayback(CTSTR lpDevice);
   ~CBlankAudioPlayback();
private:
   IMMDeviceEnumerator *mmEnumerator;
   IMMDevice           *mmDevice;
   IAudioClient        *mmClient;
   IAudioRenderClient  *mmRender;
};
//BlankAudioPlayback

//Core Audio
class __declspec(dllexport)CoreAudioSource : public IAudioSource {
public:
   CoreAudioSource(bool useInputDevice, FUN_GetVideoTime funGetVideoTime, UINT dstSampleRateHz/* = 44100*/, bool useQPC = false, bool useVideoTime = false, int globalAdjust = 0);
   ~CoreAudioSource();
   QWORD GetTimestampInMs(QWORD qpcTimestamp);
   bool Reinitialize();
   void FreeData();
protected:
   virtual bool GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp);
   virtual void ReleaseBuffer();


   void StartBlankSoundPlayback(CTSTR lpDevice);
   void StopBlankSoundPlayback();
public:
	bool Initialize();
   bool Initialize(bool bMic, CTSTR lpID);
   void Reset();
   virtual void StartCapture();
   virtual void StopCapture();
private:
   CTSTR GetDeviceName() const;
private:
   bool     mUseInputDevice;  // this device is input device

   IMMDeviceEnumerator *mmEnumerator = nullptr;//��ý���豸ö����
   IMMDevice           *mmDevice = nullptr;//�����ӿ�   �ڶ�ý���豸ö���� �� ��ȡ
   IAudioClient        *mmClient = nullptr; // �����ͻ���  ����ͨ�� �����ӿڻ�ȡ�� �������������Ƶ��������ʼ����������ȡ��������������Ĵ�С������/ֹͣ����������Ĳɼ���
   IAudioCaptureClient *mmCapture = nullptr;//�����ɼ��ͻ��˽ӿ�   ���ɼ�������������ݣ������ڲ����������п��ƣ�
   IAudioClock         *mmClock = nullptr;
   IAudioEndpointVolume* _ptrCaptureVolume = nullptr; /* �ɼ��������� */
   bool bIsMic;
   bool bFirstFrameReceived;
   bool deviceLost;
   QWORD reinitTimer;
   FUN_GetVideoTime mFunGetVideoTime;
   //QWORD fakeAudioTimer;

   //UINT32 numFramesRead;

   UINT32 mNumTimesInARowNewDataSeen;
   String mDeviceId;
   String mDeviceName;
   bool mUseVideoTime;
   int mGlobalAdjustTime;
   QWORD mLastVideoTime;
   QWORD mCurVideoTime;
   UINT mSampleWindowSize;
   List<float> mInputBuffer;

   UINT mInputBufferSize = 0;
   QWORD mFirstTimestamp;
   QWORD mLastQPCTimestamp;
   UINT32 mAngerThreshold;
   bool mUseQPC;

   CBlankAudioPlayback *mCurBlankPlaybackThingy = nullptr;
#ifdef DEBUG_CORE_AUDIO_SOURCE
   FILE* mPcmFile;
#endif
};

#endif

