#ifndef __MEDIA_READER_INCLUDE_H_
#define __MEDIA_READER_INCLUDE_H_
#include "IMediaReader.h"
#include "BufferQueue.h"
#include "vlc/vlc.h"
#include <string>
#include <vector>

enum  MediaEntryType {
   MediaEntry_File = 0,
   MediaEntry_Url
};

class MediaEntry {
public:
   std::string mMediaFile;  //must utf8
   MediaEntryType mMediaEntryType;
   void* mMediaItem; // libvlc_media_t
};

class MediaReader final :public IMediaOutput, public IMediaReader, public std::enable_shared_from_this<MediaReader> {
public:
   virtual ~MediaReader();
   virtual void SetVideoNotify(IVideoNotify* videoNotify);
   virtual bool GetNextAudioBuffer(void **buffer, unsigned int *numFrames, unsigned long long *timestamp);
   virtual bool GetNextVideoBuffer(void *buffer, unsigned long long bufferSize, unsigned long long *timestamp);
   virtual bool GetAudioParam(unsigned int& channels, unsigned int& samplesPerSec, unsigned int& bitsPerSample);
   virtual bool GetVideoParam(unsigned long& videoWidth, unsigned long& videoHeightt, long long & frameDuration, long long &timeScale); // default pixfmt
   virtual int  GetPlayerState();   
   void UIEventCallBack(int);
public:
   virtual void SetVolume(const unsigned int & volume);
   virtual unsigned int GetVolume();   
   virtual void SetEventCallBack(IMediaEventCallBack cb,void *param);
   //------------------------------新增加的接口------------------------------
   virtual bool VhallPlay(char *);
   virtual void VhallPause();
   virtual void VhallResume();
   virtual void VhallStop();   
   virtual void VhallSeek(const unsigned long long& seekTimeInMs);
   virtual const long long VhallGetMaxDuration();
   virtual const long long VhallGetCurrentDuration();
   virtual void VhallSetEventCallBack(IMediaEventCallBack cb,void *param);
   virtual void VhallSetVolumn(unsigned int);
   virtual int VhallGetVolumn();
   virtual void VhallVolumnMute();
   //------------------------------------------------------------------------
   virtual std::shared_ptr<IMediaOutput> GetMediaOut();
   virtual void Refit();
   virtual void SetRefit(IMediaOutputRefitCallBack refitCallBack,void *param);
protected:
   MediaReader();
   friend MEDIAREADER_API std::shared_ptr<IMediaReader> CreateMediaReader();
private:
   bool init();
   void SyncVideoFrameTime();
private:
   bool                       mVlcIsInit;
   bool                       mEnableAudio;
   bool                       mEnableVideo;
   bool                       mIsInit;
   bool                       mHasVideo;
   bool  mIsRefit;
   v_mutex_t mIsRefitMutex;
   IMediaOutputRefitCallBack func_RefitCallBack;
   void *mRefitParam;
   bool mIsActive;  //output yum pcm
   std::vector<MediaEntry*> mMediaConfigs;
   unsigned int mAudioVolume;
   bool mAudioVolumeSet;
   bool mIsOutputDevice;
   std::string mOutputDevice; //utf8
   IVideoNotify* mVideoNotify;
   BufferQueue* mAudioBufferQueue;
   unsigned char *mVideoBuffer=NULL;   
   v_mutex_t   mMutex;
   v_mutex_t   mMutexAudio;
  
   uint64_t   mLastAudioPacketTime;
   uint64_t   mCurrentVideoFrameTime;

   unsigned long mVideoWidth;
   unsigned long mVideoHeight;
   long mVideoFrameSize;

   unsigned int            mChannels;
   unsigned int            mSampleRate;
   unsigned int            mBitPerSample; //bits per sample 
   FILE* mVlcLog;

   //libvlc
   libvlc_instance_t *mVlc;
   libvlc_media_list_player_t *mMediaListPlayer;
   libvlc_media_list_t *mMediaList;
   libvlc_media_player_t *mMediaPlayer;
   v_mutex_t mMediaPlayerMutex;
   libvlc_log_t* mMediaLog;
   friend void vlcEvent(const libvlc_event_t *e, void *data);
   IMediaEventCallBack eventCallBack;
   void *eventCallBackParam;   
   v_mutex_t   eventCallBackMutex;
   HANDLE mThreadHandle;
private:
   //libvlc
   
   void MediaPlayerStop(libvlc_media_player_t *mPlayer);
   static void vlcLog(void *data, int level, const libvlc_log_t *ctx,
                      const char *fmt, va_list args);
   static void vlcEvent(const libvlc_event_t *e, void *data);
   static void *videoLock(void *data, void **pixelData);
   static void videoUnlock(void *data, void *id, void *const *pixelData);
   static void display(void *data, void *id);
   static unsigned VideoSetupFormatCallback(
      void **data,
      char *chroma,
      unsigned *width,
      unsigned *height,
      unsigned *pitches,
      unsigned *lines);
   static void VideoSetupFormatCleanup(void *data);
   static int audioSetupCallbackProxy(void **data, char *format, unsigned *rate, unsigned *channels);
   static void audioCleanupCallbackProxy(void *data);
   static void audioPlayCallbackProxy(void *data, const void *samples, unsigned count, int64_t pts);

   void *videoLock(void **pixelData);
   void videoUnlock(void *id, void *const *pixelData);
   unsigned int VideoFormatCallback(char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines);
   void VideoFormatCleanup();

   int AudioSetupCallback(char *format, unsigned *rate, unsigned *channels);
   void AudioCleanupCallback();
   void AudioPlayCallback(const void *samples, unsigned count, int64_t pts);

};

#endif
