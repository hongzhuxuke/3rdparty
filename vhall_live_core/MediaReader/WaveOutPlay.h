#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <list>
#include <atomic>
#include <vector>
#include "concurrentqueue.h"

#define LOOP_SIZE 1

class Logger;

class AudioData {
public:
   AudioData(unsigned char* _data, long _len);
   ~AudioData();

   unsigned char* mData;
   long mDataLen;
};

#define MAX_CACHE_AUDIO_SIZE 20    //vlc��Ƶ���ݻ���ǰ����Ƶ����200ms���ң�������Ҫ�Ȼ�����Ƶ���ݣ������������Ƶ��ͬ������
#define MAX_WAVE_PLAY_HEAD 4       //�ಥ�Ż�����
#define MAX_CLEAR_AUDIO_DATA_SIZE 100

class WaveOutPlay
{
public:
   WaveOutPlay(const wchar_t* logPath = NULL);
   ~WaveOutPlay();

   void InitFormat(int sampleRate, int channel, int bitPerSample);
   void ResetInitFormat(int sampleRate, int channel, int bitPerSample);
   void PlayAudio(unsigned char* data, long dataLen);
   void StopPlay();
   void SetEnablePlayOut(bool enable);
   void WaveOutPlayReset();
   void SetPlayVolume(int vol);
   bool IsHasWaveHandle();
   void Process(DWORD dw1, DWORD dw2);
   void CheckAudioHead(DWORD_PTR dw1, DWORD_PTR dw2);

private:
   void ClearAudioData();
   AudioData *GetPlayAudio();
private:
   UINT mTimeEventId = 0;
   HWAVEOUT       mhWaveOut = NULL;
   WAVEFORMATEX   mWaveFormate;
   HANDLE          mWaitPlayCallBack = NULL;
   HANDLE          mPlayOutThread = NULL;
   WAVEHDR         mWaveHeader[4];

   std::atomic_int mnPlayIndex = 0;
   std::atomic_bool mbDirectPlay = false;
   std::atomic_bool mbIsOpenPlayOutFunc = false;
   std::atomic_bool bIsCacheMaxPlayAudio = false;

   unsigned int   mChannels = 0;
   unsigned int   mSampleRate = 0;
   unsigned int   mBitPerSample = 0; //bits per sample 
   Logger *mpWaveLogger = NULL;
   //git���ص�ַhttps://github.com/cameron314/concurrentqueue
   moodycamel::ConcurrentQueue<AudioData*> mLockFreePlayQueue;//ԭ�Ӳ������С�
   moodycamel::ConcurrentQueue<AudioData*> mLockFreeClearQueue;//ԭ�Ӳ������С�
};

