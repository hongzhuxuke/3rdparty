#ifndef __MEDIA_PLAYER_INTERFACE_H_
#define __MEDIA_PLAYER_INTERFACE_H_
#include <memory>
#include <list>

#define MEDIAPLAYER_EXPORTS
#ifdef MEDIAPLAYER_EXPORTS
#define MEDIAPLAYER_API __declspec(dllexport)
#else
#define MEDIAPLAYER_API __declspec(dllimport)
#endif

typedef void(*IMediaEventCallBack)(int, void *);

class playOutDeviceInfo {
public:
    std::string name;
    std::string dev_id;
};

class MEDIAPLAYER_API IMediaPlayer{
public:
    virtual ~IMediaPlayer() {};
    virtual int GetPlayerState() = 0;
    //新增加的接口
    virtual int VhallPlay(char *, void* wnd) = 0;
    virtual void VhallPause() = 0;
    virtual void VhallResume() = 0;
    virtual void VhallStop() = 0;
    virtual void VhallSeek(const unsigned long long& seekTimeInMs) = 0;
    virtual const long long VhallGetMaxDuration() = 0;
    virtual const long long VhallGetCurrentDuration() = 0;
    virtual void VhallSetEventCallBack(IMediaEventCallBack cb, void *param) = 0;
    virtual void VhallSetVolumn(unsigned int) = 0;
    virtual int VhallGetVolumn() = 0;
    virtual int GetPlayDevices(std::list<playOutDeviceInfo>& devicesList) = 0;
    virtual int SetPlayDevice(const char* dev_id) = 0;
    virtual int SetPlayRate(float) = 0;
};

MEDIAPLAYER_API  std::shared_ptr<IMediaPlayer> CreateMediaPlayer();
MEDIAPLAYER_API  void DestoryMediaPlayer(std::shared_ptr<IMediaPlayer>& media_player);

#endif

