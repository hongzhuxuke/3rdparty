#include "VH_ConstDeff.h"
#include "Logging.h"
#include "BufferQueue.h"
#include "MediaReader.h"
#include <mutex>


#define MAX_BUFFER_NUM 100
#define SAMP_FORMAT "S16N"
#define CHROMA "RV32"
static int G_thread_count = 0;
MediaReader* gMediaReader = NULL;
std::mutex   gMediareaderMutex;
Logger *g_pLogger = NULL;

#define Log g_pLogger->logInfo
MediaReader::MediaReader() :
mVideoNotify(nullptr),
mVlc(nullptr),
mMediaListPlayer(nullptr),
mMediaList(nullptr),
mAudioVolume(100),
mAudioVolumeSet(false),
mAudioBufferQueue(0),
mVideoWidth(0),
mVideoHeight(0),
mVideoFrameSize(0),
mMediaLog(0),
mThreadHandle(NULL),
mIsRefit(false),
func_RefitCallBack(NULL),
mRefitParam(NULL),
mMediaPlayer(NULL),
mVlcLog(NULL)
{
   v_mtuex_init(&mMutex);
   v_mtuex_init(&mMutexAudio);
   v_mtuex_init(&eventCallBackMutex);
   v_mtuex_init(&mMediaPlayerMutex);
   v_mtuex_init(&mIsRefitMutex);

   mSampleRate = 44100;
   mChannels = 2;
   mBitPerSample = 16;


   mVlcIsInit = init();

   eventCallBack = NULL;
   eventCallBackParam = NULL;

   mpWaveOutPlay = new WaveOutPlay();
}
MediaReader::~MediaReader() {
   Log(TEXT("MediaReader::~MediaReader"));

   v_mtuex_destory(&mMediaPlayerMutex);
   v_mtuex_destory(&eventCallBackMutex);
   mVideoNotify = NULL;
   if (mMediaListPlayer) {
      libvlc_media_list_player_release(mMediaListPlayer);
      mMediaListPlayer = NULL;
   }
   if (mMediaPlayer) {
      //libvlc_media_player_release(mMediaPlayer);
      //MediaPlayerStop(mMediaPlayer);
      OutputDebugStringA("MediaPlayerStopThread STOP");
      libvlc_media_player_stop(mMediaPlayer);
      OutputDebugStringA("MediaPlayerStopThread Release");
      libvlc_media_player_release(mMediaPlayer);
      OutputDebugStringA("MediaPlayerStopThread Return");
      mMediaPlayer = NULL;
   }
   if (mVlc) {
      libvlc_release(mVlc);
      mVlc = NULL;
   }
   if (mVlcLog) {
      fclose(mVlcLog);
      mVlcLog = NULL;
   }

   if (mpWaveOutPlay) {
      mpWaveOutPlay->StopPlay();
      delete mpWaveOutPlay;
      mpWaveOutPlay = NULL;
   }

   if (mAudioBufferQueue) {
      delete mAudioBufferQueue;
      mAudioBufferQueue = NULL;
   }
   if(mVideoBuffer!=NULL) {
      delete mVideoBuffer;
      mVideoBuffer=NULL;
   }
   
   v_mtuex_destory(&mMutexAudio);
   v_mtuex_destory(&mMutex);
   v_mtuex_destory(&mIsRefitMutex);
}
void MediaReader::SetVideoNotify(IVideoNotify* videoNotify) {
   v_lock_mutex(&mMutex);
   mVideoNotify = videoNotify;
   v_unlock_mutex(&mMutex);
}
bool MediaReader::GetNextAudioBuffer(void **buffer, unsigned int *numFrames, unsigned long long *timestamp) {
   v_lock_mutex(&mMutexAudio);
   if (mbIsResume && mAudioBufferQueue && mAudioBufferQueue->GetUnitCount() >= 20) {
      mbEnableGetData = true;
      mbIsResume = false;
   }
   if (mAudioBufferQueue && mbEnableGetData) {
      DataUnit* updateBuffer = mAudioBufferQueue->GetDataUnit(false);
      if (updateBuffer) {
         *buffer = updateBuffer->unitBuffer;
         *numFrames = (unsigned int)(updateBuffer->dataSize / mChannels / (mBitPerSample / 8));
         *timestamp = updateBuffer->timestap;
         mAudioBufferQueue->FreeDataUnit(updateBuffer);
         v_unlock_mutex(&mMutexAudio);
         return true;
      } else {
         v_unlock_mutex(&mMutexAudio);
         return false;
      }
   }
   v_unlock_mutex(&mMutexAudio);
   return false;
}

bool MediaReader::GetNextVideoBuffer(void *buffer, unsigned long long bufferSize, unsigned long long *timestamp) {
   v_lock_mutex(&mMutex);
   if(bufferSize>0&&this->mVideoWidth>0&&this->mVideoHeight>0) {
      unsigned long long len=this->mVideoWidth*mVideoHeight*4;
      if(len==bufferSize) {
         memcpy(buffer,mVideoBuffer,len);
      }
   }
   v_unlock_mutex(&mMutex);
   return false;
}
bool MediaReader::GetAudioParam(unsigned int& channels, unsigned int& samplesPerSec, unsigned int& bitsPerSample) {
   channels = mChannels;
   samplesPerSec = mSampleRate;
   bitsPerSample = mBitPerSample;
   return true;
}
bool MediaReader::GetVideoParam(unsigned long& videoWidth, unsigned long& videoHeightt, long long & frameDuration, long long &timeScale) {
   videoHeightt = mVideoHeight;
   videoWidth = mVideoWidth;
   return true;
}
int MediaReader::GetPlayerState() {
   if (mThreadHandle) {
      DWORD exitCode = -1;
      GetExitCodeThread(mThreadHandle, &exitCode);
      if (exitCode == STILL_ACTIVE) {
         return -2;
      }
      else {
         mThreadHandle = NULL;
      }
   }

   int state = -1;
   if (mMediaPlayer) {
      state = (int)libvlc_media_player_get_state(mMediaPlayer);
   }
   return state;
}
void MediaReader::SetVolume(const unsigned int & volume) {
   mAudioVolume = volume;
   if (g_pLogger) {
      g_pLogger->logInfo("%s current volume:%d", __FUNCTION__, volume);
   }
}
unsigned int MediaReader::GetVolume() {
   if (mMediaPlayer) {
      return 	libvlc_audio_get_volume(mMediaPlayer);
   }

   return 0xFFFFFFFF;
}
void MediaReader::SetEventCallBack(IMediaEventCallBack cb, void *param) {

   v_lock_mutex(&eventCallBackMutex);
   this->eventCallBack = cb;
   this->eventCallBackParam = param;
   v_unlock_mutex(&eventCallBackMutex);

}
void MediaReader::UIEventCallBack(int type) {
   v_lock_mutex(&eventCallBackMutex);
   if (this->eventCallBack) {
      this->eventCallBack(type, this->eventCallBackParam);
   }
   v_unlock_mutex(&eventCallBackMutex);
}
void MediaReader::Refit()
{   
   v_lock_mutex(&mIsRefitMutex);
   if(this->func_RefitCallBack==NULL)
   {
      this->mIsRefit=true;
   }
   else
   {
      this->func_RefitCallBack(this->mRefitParam);
   }
   v_unlock_mutex(&mIsRefitMutex);
}
void MediaReader::SetRefit(IMediaOutputRefitCallBack refitCallBack,void *param)
{
   v_lock_mutex(&mIsRefitMutex);
   this->func_RefitCallBack=refitCallBack;
   this->mRefitParam=param;
   if(this->mIsRefit&&refitCallBack)
   {
      this->func_RefitCallBack(this->mRefitParam);
   }
   v_unlock_mutex(&mIsRefitMutex);
}
IMediaOutput* MediaReader::GetMediaOut() {
   return this;
}

bool MediaReader::init() {
   char *args[] = {
      "--no-osd",
      "--disable-screensaver",
      "--ffmpeg-hw",
      "--no-video-title-show",
   };
   int ret = 0;

   mVlc = libvlc_new(4, args);
   if (mVlc == NULL) {
      g_pLogger->logError("MediaReader::init libvlc_new failed");
      return false;
   }

   return true;
}

///////////////////////////////////////////////////////////////////
//static 
void MediaReader::vlcLog(void *data, int level, const libvlc_log_t *ctx,
                         const char *fmt, va_list args) {
   MediaReader *_this = reinterpret_cast<MediaReader *>(data);
   v_lock_mutex(&_this->mMutex);
   size_t len;
   char *buffer = NULL;
   len = _vscprintf(fmt, args) + 1;  /* _vscprintf doesn't count terminating NUL */

   buffer = (char *)malloc(len * sizeof(char)* 2);
   vsprintf_s(buffer, len, fmt, args);

   //g_pLogger->logInfo("%s", buffer);

   free(buffer);
   v_unlock_mutex(&_this->mMutex);
}


void MediaReader::vlcEvent(const libvlc_event_t *e, void *data) {
   MediaReader *_this = reinterpret_cast<MediaReader *>(data);
   g_pLogger->logInfo("MediaReader::vlcEvent type = %d", e->type);
   if (e->type == libvlc_MediaPlayerEndReached) {
      //_this->UIEventCallBack(e->type);
   } else if (e->type == libvlc_MediaPlayerPlaying) {
   } else if (e->type == libvlc_MediaPlayerPositionChanged) {
      //  libvlc_event_detach(libvlc_media_player_event_manager(_this->mMediaPlayer), libvlc_MediaPlayerPositionChanged, vlcEvent, _this);
      // libvlc_video_set_adjust_int(_this->mMediaPlayer, libvlc_adjust_Enable, 0);
   }

}

//Ϊlibvlc�ṩ��Ƶ��ʾ��buffer
void *MediaReader::videoLock(void *data, void **pixelData) {
   std::unique_lock<std::mutex> lock(gMediareaderMutex);
   if (gMediaReader == NULL) {
      return NULL;
   }
   void *ptr = reinterpret_cast<MediaReader *>(data)->videoLock(pixelData);
   return ptr;
}

//�ûظ�libvlc�ṩ��Ƶ��ʾ��buffer
void MediaReader::videoUnlock(void *data, void *id, void *const *pixelData) {
   std::unique_lock<std::mutex> lock(gMediareaderMutex);
   if (gMediaReader == NULL) {
      return;
   }
   reinterpret_cast<MediaReader *>(data)->videoUnlock(id, pixelData);
}

void MediaReader::display(void *data, void *id) {
   //g_pLogger->logInfo("MediaReader::display ");
}

unsigned MediaReader::VideoSetupFormatCallback(
   void **opaque,
   char *chroma,
   unsigned *width,
   unsigned *height,
   unsigned *pitches,
   unsigned *lines) {
   if (gMediaReader != *opaque) {
      return 0;
   }
   return reinterpret_cast<MediaReader *>(*opaque)->VideoFormatCallback(chroma, width, height, pitches, lines);
}

void MediaReader::VideoSetupFormatCleanup(void *opaque) {

   if (gMediaReader != opaque) {
      return;
   }
   reinterpret_cast<MediaReader *>(opaque)->VideoFormatCleanup();
}

int MediaReader::audioSetupCallbackProxy(void **opaque, char *format, unsigned *rate, unsigned *channels) {
   if (gMediaReader != *opaque) {
      return 0;
   }
   return reinterpret_cast<MediaReader *>(*opaque)->AudioSetupCallback(format, rate, channels);
}

void MediaReader::audioCleanupCallbackProxy(void *opaque) {
   if (gMediaReader != opaque) {
      return;
   }

   reinterpret_cast<MediaReader *>(opaque)->AudioCleanupCallback();
}

void MediaReader::audioPlayCallbackProxy(void *opaque, const void *samples, unsigned count, int64_t pts) {
   if (gMediaReader != opaque) {
      return;
   }

   reinterpret_cast<MediaReader *>(opaque)->AudioPlayCallback(samples, count, pts);
}

void *MediaReader::videoLock(void **pixelData) {
   
   v_lock_mutex(&mMutex);
   *pixelData=mVideoBuffer;
   return mVideoBuffer;
}
uint64_t os_gettime_ns(void);

void MediaReader::videoUnlock(void *id, void *const *pixelData) {
   if (!gMediaReader) {
      return;
   }

   if (mVideoNotify)
         mVideoNotify->OnVideoFrame();
   v_unlock_mutex(&mMutex);
}

unsigned int MediaReader::VideoFormatCallback(char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines) {
   v_lock_mutex(&mMutex);
   g_pLogger->logInfo("MediaReader::VideoFormatCallback will reinit video queue");
   memcpy(chroma, CHROMA, sizeof(CHROMA)-1);
   *pitches = *width * 4;
   *lines = *height;
   mVideoWidth = *width;
   mVideoHeight = *height;
   mVideoFrameSize = (*pitches)*(*lines);

   if(mVideoBuffer!=NULL) {
      delete []mVideoBuffer;
      mVideoBuffer = NULL;
   }
   
   if(mVideoWidth > 0 && mVideoHeight > 0) {
      mVideoBuffer = new unsigned char [mVideoWidth * mVideoHeight * 4];
   }

   if (mVideoNotify)
      mVideoNotify->OnVideoChanged(mVideoWidth, mVideoHeight, 0, 0);
   v_unlock_mutex(&mMutex);
   return 1;
}
void MediaReader::VideoFormatCleanup() {
   g_pLogger->logInfo("MediaReader::VideoFormatCleanup will destory video queue");
}

int MediaReader::AudioSetupCallback(char *format, unsigned *rate, unsigned *channels) {
   v_lock_mutex(&mMutexAudio);
   memcpy(format, SAMP_FORMAT, sizeof(SAMP_FORMAT));
   *channels = mChannels;
   *rate = mSampleRate;
   if (mAudioBufferQueue) {
      delete mAudioBufferQueue;
      mAudioBufferQueue = NULL;
   }

   if(mAudioBufferQueue == NULL){
      mAudioBufferQueue = new BufferQueue(mSampleRate*(mBitPerSample / 8)*mChannels, MAX_BUFFER_NUM);
   }
   
   mStartTimeInMs = 0;
   v_unlock_mutex(&mMutexAudio);
   mpWaveOutPlay->InitFormat(mSampleRate, mChannels, mBitPerSample);
   return 0;
}
void MediaReader::AudioCleanupCallback() {
   g_pLogger->logInfo("MediaReader::AudioCleanupCallback will destory audio queue");
   v_lock_mutex(&mMutexAudio);
   if (mAudioBufferQueue) {
      delete mAudioBufferQueue;
      mAudioBufferQueue = NULL;
   }
   mStartTimeInMs = 0;
   v_unlock_mutex(&mMutexAudio);
}

//�ı�������С�㷨
void RaiseVolume(char* buf, UINT32 size, UINT32 uRepeat, double vol) {
   if (!size) {
      return;
   }
   for (int i = 0; i < size;) {
      signed long minData = -0x8000; //�����8bit����������-0x80
      signed long maxData = 0x7FFF;//�����8bit����������0xFF

      signed short wData = buf[i + 1];
      wData = MAKEWORD(buf[i], buf[i + 1]);
      signed long dwData = wData;

      for (int j = 0; j < uRepeat; j++) {
         dwData = dwData * vol;//1.25;
         if (dwData < -0x8000) {
            dwData = -0x8000;
         } else if (dwData > 0x7FFF) {
            dwData = 0x7FFF;
         }
      }
      wData = LOWORD(dwData);
      buf[i] = LOBYTE(wData);
      buf[i + 1] = HIBYTE(wData);
      i += 2;
   }
}
//�ı�������С
void samplesRaise(const void *samples, long size, float vol) {
   vol /= 100;
   RaiseVolume((char *)samples, size, 1, vol);

}
static bool have_clockfreq = false;
static LARGE_INTEGER clock_freq;
static uint32_t winver = 0;

static inline uint64_t get_clockfreq(void)
{
	if (!have_clockfreq)
		QueryPerformanceFrequency(&clock_freq);
	return clock_freq.QuadPart;
}

uint64_t os_gettime_ns(void)
{
	LARGE_INTEGER current_time;
	double time_val;

	QueryPerformanceCounter(&current_time);
	time_val = (double)current_time.QuadPart;
	time_val *= 1000000000.0;
	time_val /= (double)get_clockfreq();

	return (uint64_t)time_val;
}
void MediaReader::AudioPlayCallback(const void *samples, unsigned count, int64_t pts) {
   // g_pLogger->logInfo("MediaReader::AudioPlayCallback count:%d pts:%lld", count, pts);

   v_lock_mutex(&mMutexAudio);
   pts /= 1000;

   int64_t sysos=os_gettime_ns();
   sysos /= 1000000;

   //if (mStartTimeInMs == 0) {
   //    mStartTimeInMs = sysos - pts;
   //    mStartTimeInMs += pts;
   //}
   //else {
   //    float timestamp = (float)count * 1000 / (float)mSampleRate;
   //    mCountTimeStamp += timestamp;
   //    unsigned long long uLLtimeStamp = mCountTimeStamp;
   //    mStartTimeInMs += uLLtimeStamp;
   //}

   if (mStartTimeInMs == 0) {
       mStartTimeInMs = sysos - pts;
   }
   
   long size = count *mChannels * (mBitPerSample / 8);
   samplesRaise(samples, size, this->mAudioVolume);
   DataUnit* newAudio = mAudioBufferQueue->MallocDataUnit();
   if (newAudio) {
      newAudio->dataSize = size;
      newAudio->timestap = mStartTimeInMs + pts;
		if (NULL == newAudio->unitBuffer){
			newAudio->unitBuffer = (unsigned char*)malloc(size+1);
			memset(newAudio->unitBuffer, 0, size + 1);
		}

      memcpy(newAudio->unitBuffer, samples, size);
      mAudioBufferQueue->PutDataUnit(newAudio);
      if (mpWaveOutPlay) {
         mpWaveOutPlay->PlayAudio(newAudio->unitBuffer, size);
      }
   }
   else {
       g_pLogger->logInfo("MediaReader::AudioPlayCallback MallocDataUnit failed");
   }
   v_unlock_mutex(&mMutexAudio);
}


DWORD __stdcall MediaPlayerStopThread(LPVOID lpUnused) {
   libvlc_media_player_t *player = (libvlc_media_player_t*)lpUnused;
   if (!player) {
      return 0;
   }
   
   OutputDebugStringA("MediaPlayerStopThread STOP");
   libvlc_media_player_stop(player);
   OutputDebugStringA("MediaPlayerStopThread Release");
   libvlc_media_player_release(player);
   OutputDebugStringA("MediaPlayerStopThread Return");
   return 0;
}

void MediaReader::MediaPlayerStop(libvlc_media_player_t *mPlayer) {
   if (mPlayer) { 
      OutputDebugStringA("MediaPlayerStopThread STOP");
      libvlc_media_player_stop(mPlayer);
      OutputDebugStringA("MediaPlayerStopThread Release");
      libvlc_media_player_release(mPlayer);
      OutputDebugStringA("MediaPlayerStopThread Return");
   }
   //mThreadHandle = CreateThread(NULL, 0, MediaPlayerStopThread, mPlayer, 0, NULL);
}


bool MediaReader::VhallPlay(char *file,bool audioFile) {
   int nRet = 0;
   v_lock_mutex(&mMediaPlayerMutex);
   //�ͷ���һ��������
   if (mMediaPlayer) {
      libvlc_media_player_stop(mMediaPlayer);
      OutputDebugStringA("MediaPlayerStopThread Release");
      libvlc_media_player_release(mMediaPlayer);
      OutputDebugStringA("MediaPlayerStopThread Return");
      mMediaPlayer = NULL;
   }
   if (this->GetPlayerState() == -2) {
      v_unlock_mutex(&mMediaPlayerMutex);
      return false;
   }
   mbAudioFile = audioFile;
   libvlc_media_t *m = libvlc_media_new_path(mVlc, file);
   if (m) {
      libvlc_media_player_t *mp = libvlc_media_player_new_from_media(m);
      if (mp) {
         AudioCleanupCallback();
         //��Ƶ�ص�,��Ƶ�ص������ڴ�й¶
         libvlc_audio_set_format_callbacks(mp, audioSetupCallbackProxy, audioCleanupCallbackProxy);
         libvlc_audio_set_callbacks(mp, audioPlayCallbackProxy, nullptr, nullptr, nullptr, nullptr, this);
         //��Ƶ�ص�
         libvlc_video_set_callbacks(mp, videoLock, videoUnlock, display, this);
         libvlc_video_set_format_callbacks(mp, VideoSetupFormatCallback, VideoSetupFormatCleanup);
         nRet = libvlc_media_player_play(mp);
         mMediaPlayer = mp;
      }
      libvlc_media_release(m);
   }
   v_unlock_mutex(&mMediaPlayerMutex);
   if (mpWaveOutPlay) {
      mpWaveOutPlay->StopPlay();
   }
   return nRet == 0 ? true : false;
}

void MediaReader::VhallPause() {
   v_lock_mutex(&mMediaPlayerMutex);
   if (mMediaPlayer) {
      libvlc_media_player_pause(mMediaPlayer);
   }
   v_unlock_mutex(&mMediaPlayerMutex);

   v_lock_mutex(&mMutexAudio);
   if (mAudioBufferQueue) {
      delete mAudioBufferQueue;
      mAudioBufferQueue = NULL;
   }
   if (mAudioBufferQueue == NULL) {
      mAudioBufferQueue = new BufferQueue(mSampleRate*(mBitPerSample / 8)*mChannels, MAX_BUFFER_NUM);
   }
   v_unlock_mutex(&mMutexAudio);

   mbIsResume = true;
   mbEnableGetData = false;
   mStartTimeInMs = 0;
   if (mpWaveOutPlay) {
      mpWaveOutPlay->WaveOutPlayReset();
   }
}

void MediaReader::VhallResume() {
   v_lock_mutex(&mMediaPlayerMutex);

   if (mMediaPlayer) {
      libvlc_media_player_set_pause(mMediaPlayer, 0);
   }
   v_unlock_mutex(&mMediaPlayerMutex);
   if (mpWaveOutPlay) {
      mpWaveOutPlay->WaveOutPlayReset();
   }
}

int MediaReader::SetPlayOutAudio(bool enable) {
   int nRet = -1;
   if (mpWaveOutPlay) {
      //if ((GetPlayerState() == PLAYUI_PLAYSTATE_PALYERING || GetPlayerState() == PLAYUI_PLAYSTATE_OPENING || GetPlayerState() == PLAYUI_PLAYSTATE_BUFFERING) && enable) {
      //   mpWaveOutPlay->ResetInitFormat(mSampleRate, mChannels, mBitPerSample);
      //}
      mpWaveOutPlay->SetEnablePlayOut(enable);
      nRet = 0;
   }
   return nRet;
}

void MediaReader::VhallStop() {
   v_lock_mutex(&mMediaPlayerMutex);
   if (mMediaPlayer) {
      libvlc_media_player_stop(mMediaPlayer);
      OutputDebugStringA("MediaPlayerStopThread Release");
      libvlc_media_player_release(mMediaPlayer);
      OutputDebugStringA("MediaPlayerStopThread Return");
      mMediaPlayer = NULL;
   }
   v_unlock_mutex(&mMediaPlayerMutex);
   mbIsResume = false;
   mbEnableGetData = true;
   if (mpWaveOutPlay) {
      mpWaveOutPlay->StopPlay();
   }
}

void MediaReader::VhallSeek(const unsigned long long& seekTimeInMs) {
   if (mpWaveOutPlay) {
      mpWaveOutPlay->WaveOutPlayReset();
   }
   v_lock_mutex(&mMediaPlayerMutex);
   if (mMediaPlayer) {
      libvlc_media_player_set_time(mMediaPlayer, seekTimeInMs);
   }
   v_unlock_mutex(&mMediaPlayerMutex);
   if (!mbAudioFile) {
       mbIsResume = true;
       mbEnableGetData = false;
   }
   v_lock_mutex(&mMutexAudio);
   if (mAudioBufferQueue) {
      delete mAudioBufferQueue;
      mAudioBufferQueue = NULL;
   }
   if (mAudioBufferQueue == NULL) {
      mAudioBufferQueue = new BufferQueue(mSampleRate*(mBitPerSample / 8)*mChannels, MAX_BUFFER_NUM);
   }
   //mStartTimeInMs = 0;
   v_unlock_mutex(&mMutexAudio);
}

const long long MediaReader::VhallGetMaxDulation() {
   long long maxdulation = -1;
   v_lock_mutex(&mMediaPlayerMutex);
   if (mMediaPlayer) {
      libvlc_media_t *media = libvlc_media_player_get_media(mMediaPlayer);
      maxdulation = (long long)libvlc_media_get_duration(media);
      libvlc_media_release(media);
   }
   v_unlock_mutex(&mMediaPlayerMutex);
   return maxdulation;
}

const long long MediaReader::VhallGetCurrentDulation() {
   long long dulation = -1;
   v_lock_mutex(&mMediaPlayerMutex);
   if (mMediaPlayer) {
      dulation = (long long)libvlc_media_player_get_time(mMediaPlayer);
   }
   v_unlock_mutex(&mMediaPlayerMutex);
   return dulation;
}

void MediaReader::VhallSetEventCallBack(IMediaEventCallBack cb, void *param) {
   v_lock_mutex(&eventCallBackMutex);
   this->eventCallBack = cb;
   this->eventCallBackParam = param;
   v_unlock_mutex(&eventCallBackMutex);
}

void MediaReader::VhallSetVolumn(unsigned int value) {
   v_lock_mutex(&mMediaPlayerMutex);
   if (mpWaveOutPlay) {
      mpWaveOutPlay->SetPlayVolume(value);
   }
   if (mMediaPlayer) {
      libvlc_audio_set_volume(mMediaPlayer, value);
      mAudioVolume = value;
   }
   v_unlock_mutex(&mMediaPlayerMutex);
}

int MediaReader::VhallGetVolumn() {
   return this->mAudioVolume;
}
void MediaReader::VhallVolumnMute() {

}

void MediaReader::ResetPlayFileAudioData() {
    //v_lock_mutex(&mMutexAudio);
    //mStartTimeInMs = 0;
    //v_unlock_mutex(&mMutexAudio);
    long long curTime = VhallGetCurrentDulation();
    VhallSeek(curTime);
}

MEDIAREADER_API  IMediaReader* CreateMediaReader() {
   std::unique_lock<std::mutex> lock(gMediareaderMutex);
   if (g_pLogger == NULL) {
      SYSTEMTIME loSystemTime;
      GetLocalTime(&loSystemTime);
      wchar_t lwzLogFileName[255] = { 0 };
      wsprintf(lwzLogFileName, L"%s%s_%4d_%02d_%02d_%02d_%02d%s", VH_LOG_DIR, L"MediaReader", loSystemTime.wYear, loSystemTime.wMonth, loSystemTime.wDay, loSystemTime.wHour, loSystemTime.wMinute, L".log");
      g_pLogger = new Logger(lwzLogFileName, USER);
   }
   if (gMediaReader == NULL) {
      gMediaReader = new MediaReader();
      g_pLogger->logInfo("CreateMediaReader create new media reader");

   } else {
      g_pLogger->logInfo("CreateMediaReader use exist media reader");
   }
   return gMediaReader;
}

MEDIAREADER_API void DestoryMediaReader(IMediaReader** mediaReader) {
   std::unique_lock<std::mutex> lock(gMediareaderMutex);
   if (gMediaReader == *mediaReader) {
      delete gMediaReader;      
      gMediaReader = NULL;
      *mediaReader = NULL;
   } 
   if (g_pLogger) {
      delete g_pLogger;
      g_pLogger=NULL;
   } 
}

