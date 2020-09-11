#pragma once
#include "IMediaPlayer.h"
#include "vlc/vlc.h"
#include <mutex>

class MediaPlayerImpl final :public IMediaPlayer, public std::enable_shared_from_this<MediaPlayerImpl>
{
public:
    virtual ~MediaPlayerImpl();
    virtual int VhallPlay(char *url, void* wnd);
    virtual void VhallPause();
    virtual void VhallResume();
    virtual void VhallStop();
    virtual void VhallSeek(const unsigned long long& seekTimeInMs);
    virtual int GetPlayerState();
    virtual const long long VhallGetMaxDuration();
    virtual const long long VhallGetCurrentDuration();
    virtual void VhallSetEventCallBack(IMediaEventCallBack cb, void *param);
    virtual void VhallSetVolumn(unsigned int);
    virtual int VhallGetVolumn();
    virtual int GetPlayDevices(std::list<playOutDeviceInfo>& devicesList);
    virtual int SetPlayDevice(const char* dev_id);
    virtual int SetPlayRate(float);
protected:
    MediaPlayerImpl();
    friend MEDIAPLAYER_API std::shared_ptr<IMediaPlayer> CreateMediaPlayer();
private:
    //libvlc
    libvlc_instance_t *mVlc;
    libvlc_media_list_player_t *mMediaListPlayer;
    libvlc_media_list_t *mMediaList;
    libvlc_media_player_t *mMediaPlayer;
    libvlc_log_t* mMediaLog;

    std::mutex mMediaPlayerMutex;
};

