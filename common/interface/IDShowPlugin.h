#ifndef _IDSHOWPLUGIN_H_
#define _IDSHOWPLUGIN_H_
#include "VH_ConstDeff.h"
#define DSHOWPLUGIN_API __declspec(dllexport)
#define DSHOW_CLASSNAME TEXT("DeviceCapture")
typedef void (*FUNCAudioDataCallBack)(unsigned char *,size_t,long long ,long long,HANDLE);
typedef bool (*FUNCFileOpen)(char *filename);
typedef bool (*FUNCFileClear)();
typedef bool (*FUNCFileRead)(DeviceInfo &deviceInfo,UINT &width,UINT &height,int &frameInternal,DeinterlacingType &type,VideoFormat &format);
typedef bool (*FUNCFileWrite)(DeviceInfo deviceInfo,UINT width,UINT height,int frameInternal,DeinterlacingType type,VideoFormat format);
typedef bool (*FUNCFileClose)();


class DSHOWPLUGIN_API IDShowVideoFilterDevice
{
public:
   virtual ~IDShowVideoFilterDevice(){};
   //获得音频数据
   virtual bool GetNextAudioBuffer(void **buffer, unsigned int *numFrames, unsigned long long *timestamp) = 0;
   //获得视频数据
   virtual bool GetNextVideoBuffer(void **buffer,
      unsigned long long *bufferSize,
      unsigned long long *timestamp,
      VideoDeviceSetting &setting
   ) = 0;
   //释放视频缓冲区
   virtual void ReleaseVideoBuffer()=0;
   //设置配置信息
   virtual bool SetVideoConfig(VideoDeviceSetting setting)=0;
   //获取音频配置信息
   virtual bool GetAudioConfig(
      UINT &mInputBitsPerSample,
      bool &bFloat,
      UINT &mInputSamplesPerSec,
      UINT &mInputChannels,
      UINT &mInputBlockSize
      )=0;

   
};





//初始化DShow
DSHOWPLUGIN_API void InitDShowPlugin(FUNCFileOpen,
      FUNCFileClear,
      FUNCFileRead,
      FUNCFileWrite,
      FUNCFileClose,
   const wchar_t* logPath = NULL);
//
DSHOWPLUGIN_API void SetDShowcaptureLog(void *,void *);
//析构DShow
DSHOWPLUGIN_API void UnInitDShowPlugin();
/* 获得DSHOW VIDEO设备 */
DSHOWPLUGIN_API bool GetDShowVideoFilter(IDShowVideoFilterDevice *&,
   DeviceInfo deviceInfo,VideoDeviceSetting &setting,
   DShowDeviceType deviceType,DShowDevicePinType pinType,
   HANDLE handle,
   HANDLE pFunc);
//释放DSHOW VIDEO设备
DSHOWPLUGIN_API bool ReleaseDShowVideoFilter(HANDLE handle);

DSHOWPLUGIN_API bool IsDShowVideoFilterExist(HANDLE handle);

DSHOWPLUGIN_API void DShowVideoManagerEnterMutex();
DSHOWPLUGIN_API void DShowVideoManagerLeaveMutex();



//获得视频设备配置信息
DSHOWPLUGIN_API bool GetDShowVideoFrameInfoList
   (DeviceInfo deviceInfo,
   FrameInfoList *frameInfoList,
   FrameInfo *currentFrameInfo,
   DeinterlacingType &deinterType
);
//设置DSHOW设备的属性
DSHOWPLUGIN_API bool SetDeviceAttribute(DeviceInfo deviceInfo,
   UINT width,UINT height,int frameInternal,
   VideoFormat &format);
//设置设备的默认属性
DSHOWPLUGIN_API bool SetDeviceDefaultAttribute(
   DeviceInfo device,UINT width,UINT height,int frameInternal,
   DeinterlacingType type,VideoFormat format);
//获得设备的默认属性
DSHOWPLUGIN_API bool GetDeviceDefaultAttribute
   (DeviceInfo device,UINT &width,UINT &height,
   int &frameInternal,DeinterlacingType &type,
   VideoFormat &format);
//获得设备号
DSHOWPLUGIN_API int  GetDeviceDisplayNumber(DeviceInfo device);
//更新设备列表
DSHOWPLUGIN_API void SyncDeviceList(DeviceList&);
#endif
