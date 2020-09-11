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
   //�����Ƶ����
   virtual bool GetNextAudioBuffer(void **buffer, unsigned int *numFrames, unsigned long long *timestamp) = 0;
   //�����Ƶ����
   virtual bool GetNextVideoBuffer(void **buffer,
      unsigned long long *bufferSize,
      unsigned long long *timestamp,
      VideoDeviceSetting &setting
   ) = 0;
   //�ͷ���Ƶ������
   virtual void ReleaseVideoBuffer()=0;
   //����������Ϣ
   virtual bool SetVideoConfig(VideoDeviceSetting setting)=0;
   //��ȡ��Ƶ������Ϣ
   virtual bool GetAudioConfig(
      UINT &mInputBitsPerSample,
      bool &bFloat,
      UINT &mInputSamplesPerSec,
      UINT &mInputChannels,
      UINT &mInputBlockSize
      )=0;

   
};





//��ʼ��DShow
DSHOWPLUGIN_API void InitDShowPlugin(FUNCFileOpen,
      FUNCFileClear,
      FUNCFileRead,
      FUNCFileWrite,
      FUNCFileClose,
   const wchar_t* logPath = NULL);
//
DSHOWPLUGIN_API void SetDShowcaptureLog(void *,void *);
//����DShow
DSHOWPLUGIN_API void UnInitDShowPlugin();
/* ���DSHOW VIDEO�豸 */
DSHOWPLUGIN_API bool GetDShowVideoFilter(IDShowVideoFilterDevice *&,
   DeviceInfo deviceInfo,VideoDeviceSetting &setting,
   DShowDeviceType deviceType,DShowDevicePinType pinType,
   HANDLE handle,
   HANDLE pFunc);
//�ͷ�DSHOW VIDEO�豸
DSHOWPLUGIN_API bool ReleaseDShowVideoFilter(HANDLE handle);

DSHOWPLUGIN_API bool IsDShowVideoFilterExist(HANDLE handle);

DSHOWPLUGIN_API void DShowVideoManagerEnterMutex();
DSHOWPLUGIN_API void DShowVideoManagerLeaveMutex();



//�����Ƶ�豸������Ϣ
DSHOWPLUGIN_API bool GetDShowVideoFrameInfoList
   (DeviceInfo deviceInfo,
   FrameInfoList *frameInfoList,
   FrameInfo *currentFrameInfo,
   DeinterlacingType &deinterType
);
//����DSHOW�豸������
DSHOWPLUGIN_API bool SetDeviceAttribute(DeviceInfo deviceInfo,
   UINT width,UINT height,int frameInternal,
   VideoFormat &format);
//�����豸��Ĭ������
DSHOWPLUGIN_API bool SetDeviceDefaultAttribute(
   DeviceInfo device,UINT width,UINT height,int frameInternal,
   DeinterlacingType type,VideoFormat format);
//����豸��Ĭ������
DSHOWPLUGIN_API bool GetDeviceDefaultAttribute
   (DeviceInfo device,UINT &width,UINT &height,
   int &frameInternal,DeinterlacingType &type,
   VideoFormat &format);
//����豸��
DSHOWPLUGIN_API int  GetDeviceDisplayNumber(DeviceInfo device);
//�����豸�б�
DSHOWPLUGIN_API void SyncDeviceList(DeviceList&);
#endif
