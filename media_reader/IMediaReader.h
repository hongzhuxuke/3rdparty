#ifndef __MEDIA_READER_INTERFACE_H_
#define __MEDIA_READER_INTERFACE_H_

#define MEDIAREADER_EXPORTS

#include "IMediaOutput.h"
#include <memory>
#ifdef MEDIAREADER_EXPORTS
#define MEDIAREADER_API __declspec(dllexport)
#else
#define MEDIAREADER_API __declspec(dllimport)
#endif

//class IReaderNotify {
//public:   
//   virtual void OnVideoFrame(const int type, ) = 0;
//};
//
typedef void(*IMediaEventCallBack)(int, void *);
class MEDIAREADER_API IMediaReader {
public:
   virtual ~IMediaReader() {};
   virtual void SetVolume(const unsigned int & volume) = 0;
   virtual std::shared_ptr<IMediaOutput> GetMediaOut() = 0;
   virtual int GetPlayerState() = 0;
   virtual void SetEventCallBack(IMediaEventCallBack cb, void *param) = 0;
   //新增加的接口
   virtual bool VhallPlay(char *) = 0;
   virtual void VhallPause() = 0;
   virtual void VhallResume() = 0;
   virtual void VhallStop() = 0;
   virtual void VhallSeek(const unsigned long long& seekTimeInMs) = 0;
   virtual const long long VhallGetMaxDuration() = 0;
   virtual const long long VhallGetCurrentDuration() = 0;
   virtual void VhallSetEventCallBack(IMediaEventCallBack cb, void *param) = 0;
   virtual void VhallSetVolumn(unsigned int) = 0;
   virtual int VhallGetVolumn() = 0;
   virtual void VhallVolumnMute() = 0;
};

MEDIAREADER_API  std::shared_ptr<IMediaReader> CreateMediaReader();

MEDIAREADER_API  void DestoryMediaReader(std::shared_ptr<IMediaReader>& media_reader);

#endif
