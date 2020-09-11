#include "MediaPlayerImpl.h"
#include <mutex>
#include "Logging.h"

static std::shared_ptr<MediaPlayerImpl> gMediaReader;
Logger *g_pLogger = NULL;
std::mutex gMediareaderMutex;

MediaPlayerImpl::MediaPlayerImpl():
    mVlc(nullptr),
    mMediaListPlayer(nullptr),
    mMediaList(nullptr),
    mMediaPlayer(nullptr),
    mMediaLog(nullptr)
{
    char *args[] = {
        (char*)"--no-osd",
        (char*)"--disable-screensaver",
        (char*)"--ffmpeg-hw",
        (char*)"--no-video-title-show",
    };
    int ret = 0;
    mVlc = libvlc_new(4, args);
    if (mVlc == NULL) {
        g_pLogger->logError("MediaReader::init libvlc_new failed");
    }

}


MediaPlayerImpl::~MediaPlayerImpl()
{

}

int MediaPlayerImpl::VhallPlay(char *url, void* wnd) {
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    //释放上一个播放器
    if (mMediaPlayer) {
        libvlc_media_player_stop(mMediaPlayer);
        libvlc_media_player_release(mMediaPlayer);
        mMediaPlayer = nullptr;
    }
    int ret = -1;
    libvlc_media_t *media = libvlc_media_new_location(mVlc, url);
    if (media) {
        mMediaPlayer = libvlc_media_player_new_from_media(media);
        libvlc_media_release(media);
        if (mMediaPlayer) {
            ret = libvlc_media_player_play(mMediaPlayer);
            if (ret) {
                libvlc_media_player_set_hwnd(mMediaPlayer, wnd);
            }
        }
    }
    return ret;
}

void MediaPlayerImpl::VhallPause() {
    g_pLogger->logError("MediaReader::VhallPause");
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_media_player_set_pause(mMediaPlayer, 1);
    }
    g_pLogger->logError("MediaReader::VhallPause");
    return;
}

void MediaPlayerImpl::VhallResume() {
    g_pLogger->logError("MediaReader::VhallResume");
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_media_player_set_pause(mMediaPlayer, 0);
    }
    g_pLogger->logError("MediaReader::VhallResume");
    return;
}

void MediaPlayerImpl::VhallStop() {
    g_pLogger->logError("MediaReader::VhallStop");
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_media_player_stop(mMediaPlayer);
        libvlc_media_player_release(mMediaPlayer);
        mMediaPlayer = nullptr;
    }
    g_pLogger->logError("MediaReader::VhallStop");
    return ;
}

void MediaPlayerImpl::VhallSeek(const unsigned long long& seekTimeInMs) {
    g_pLogger->logError("MediaReader::VhallSeek");
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_media_player_set_time(mMediaPlayer, seekTimeInMs);
    }
    g_pLogger->logError("MediaReader::VhallSeek");
    return ;
}

int MediaPlayerImpl::GetPlayerState() {
    return 0;
}

const long long MediaPlayerImpl::VhallGetMaxDuration() {
    long long maxdulation = -1;
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_media_t *media = libvlc_media_player_get_media(mMediaPlayer);
        maxdulation = (long long)libvlc_media_get_duration(media);
        libvlc_media_release(media);
    }
    return maxdulation;
}

const long long MediaPlayerImpl::VhallGetCurrentDuration() {
    long long dulation = -1;
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        dulation = (long long)libvlc_media_player_get_time(mMediaPlayer);
    }
    return dulation;
}

void MediaPlayerImpl::VhallSetEventCallBack(IMediaEventCallBack cb, void *param) {
    return ;
}

void MediaPlayerImpl::VhallSetVolumn(unsigned int value) {
    g_pLogger->logError("MediaReader::VhallSetVolumn %d", value);
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_audio_set_volume(mMediaPlayer, value);
    }
    g_pLogger->logError("MediaReader::VhallSetVolumn %d", value);
    return ;
}

int MediaPlayerImpl::VhallGetVolumn() {
    int value = 0;
    g_pLogger->logError("MediaReader::VhallGetVolumn");
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        value = libvlc_audio_get_volume(mMediaPlayer);
    }
    g_pLogger->logError("MediaReader::VhallGetVolumn %d", value);
    return 0;
}

int MediaPlayerImpl::GetPlayDevices(std::list<playOutDeviceInfo>& devicesList) {
    g_pLogger->logError("MediaReader::GetPlayDevices");
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mVlc) {
        libvlc_audio_output_device_t* audioDevices = libvlc_audio_output_device_list_get(mVlc, NULL);
        libvlc_audio_output_device_t* current = audioDevices;
        do {
            if (current) {
                playOutDeviceInfo info;
                if (current->psz_description != nullptr) {
                    info.name = std::string(current->psz_description);
                }
                if (audioDevices->psz_device) {
                    info.dev_id = std::string(current->psz_device);
                }
                devicesList.push_back(info);
                current = current->p_next;
            }
        } while (!current);
        if (audioDevices) {
            libvlc_audio_output_device_list_release(audioDevices);
            audioDevices = nullptr;
        }
    }
    g_pLogger->logError("MediaReader::GetPlayDevices");
    return 0;
}

int MediaPlayerImpl::SetPlayDevice(const char* dev_id) {
    g_pLogger->logError("MediaReader::SetPlayDevice %s", dev_id);
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_audio_output_device_set(mMediaPlayer,NULL,dev_id);
    }
    g_pLogger->logError("MediaReader::SetPlayDevice");
    return 0;
}

int MediaPlayerImpl::SetPlayRate(float rate) {
    int value = 0;
    g_pLogger->logError("MediaReader::SetPlayRate");
    std::unique_lock<std::mutex> lock(mMediaPlayerMutex);
    if (mMediaPlayer) {
        libvlc_media_player_set_rate(mMediaPlayer, rate);
    }
    g_pLogger->logError("MediaReader::SetPlayRate");
    return 0;
}

MEDIAPLAYER_API  std::shared_ptr<IMediaPlayer> CreateMediaPlayer() {
    std::unique_lock<std::mutex> lck(gMediareaderMutex);
    if (g_pLogger == NULL) {
        g_pLogger = new Logger(L"MediaReader.log", USER);
    }
    if (gMediaReader == NULL) {
        gMediaReader.reset(new MediaPlayerImpl());
        g_pLogger->logInfo("CreateMediaReader create new media reader");
    }
    else {
        g_pLogger->logInfo("CreateMediaReader use exist media reader");
    }
    return gMediaReader;
}

MEDIAPLAYER_API void DestoryMediaPlayer(std::shared_ptr<IMediaPlayer>& media_reader) {
    std::unique_lock<std::mutex> lck(gMediareaderMutex);
    if (gMediaReader.get() == media_reader.get()) {
        gMediaReader.reset();
        media_reader.reset();
        g_pLogger->logInfo("DestoryMediaReader destory mediareader");
    }
    else {
        g_pLogger->logWarning("DestoryMediaReader invalid mediareader");
    }
    if (g_pLogger) {
        delete g_pLogger;
        g_pLogger = NULL;
    }
}
