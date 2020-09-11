#ifndef __I_OBSCONTROL_LOGIC__H_INCLUDE__
#define __I_OBSCONTROL_LOGIC__H_INCLUDE__
#include "VH_ConstDeff.h"
#include "VH_IUnknown.h"
#include "MediaDefs.h"
#include <strmif.h>
#include <dvdmedia.h>
#include <vector>
#include <QDateTime>
// {46D179F1-B38D-4702-8DA5-31F5227C218B}
DEFINE_GUID(IID_IOBSControlLogic,
            0x46d179f1, 0xb38d, 0x4702, 0x8d, 0xa5, 0x31, 0xf5, 0x22, 0x7c, 0x21, 0x8b);

class IOBSControlLogic : public VH_IUnknown {
public:
   virtual void STDMETHODCALLTYPE GetStreamStatus(std::vector<StreamStatus> *) = 0;

   virtual bool STDMETHODCALLTYPE MediaPlay(char* fileName, bool audioFile) = 0;

   virtual bool STDMETHODCALLTYPE SetEnablePlayOutMediaAudio(bool enable) = 0;

   virtual void STDMETHODCALLTYPE MediaStop() = 0;

   virtual void STDMETHODCALLTYPE MediaPause() = 0;

   virtual void STDMETHODCALLTYPE MediaResume() = 0;

   virtual void STDMETHODCALLTYPE MediaSeek(const unsigned long long& seekTime) = 0;

   virtual int STDMETHODCALLTYPE GetPlayerState() = 0;

   virtual void STDMETHODCALLTYPE SetVolume(const unsigned int & volume) = 0;

   virtual const long STDMETHODCALLTYPE VhallGetMaxDulation() = 0;

   virtual const long STDMETHODCALLTYPE VhallGetCurrentDulation() = 0;

   virtual void STDMETHODCALLTYPE OnMouseEvent(const UINT& mouseEvent, const POINTS& mousePos) = 0;

   virtual void STDMETHODCALLTYPE Resize(const RECT& client, bool bRedrawRenderFrame) = 0;

   virtual bool STDMETHODCALLTYPE IsHasNoPersistentSource() = 0;

   virtual bool STDMETHODCALLTYPE IsCanModify() = 0;

   virtual SOURCE_TYPE STDMETHODCALLTYPE GetCurrentItemType() = 0;

   virtual bool STDMETHODCALLTYPE IsHasMonitorSource() = 0;

   virtual void STDMETHODCALLTYPE GetCurrentFramePicSize(int &,int &)=0;

   virtual void STDMETHODCALLTYPE GetBaseSize(int &,int &)=0;

   virtual unsigned char** STDMETHODCALLTYPE LockCurrentFramePic(unsigned long long &t) = 0;

   virtual void STDMETHODCALLTYPE UnlockCurrentFramePic() =0;

   virtual bool STDMETHODCALLTYPE SetSourceVisible(wchar_t *sourceName,bool,bool)=0;      

   virtual bool STDMETHODCALLTYPE WaitSetSourceVisible()=0;

   virtual unsigned char * STDMETHODCALLTYPE MemoryCreate(int)=0;

   virtual void  STDMETHODCALLTYPE MemoryFree(void *)=0;

   virtual void  STDMETHODCALLTYPE Reset(const bool reRecord) = 0;
   
   virtual bool STDMETHODCALLTYPE IsHasPlaybackAudioDevice()=0;

   virtual bool STDMETHODCALLTYPE SetRecordPath(bool,wchar_t *)=0;
   //virtual bool STDMETHODCALLTYPE SetRecordFileNum(const int& iNum)=0;
   virtual void STDMETHODCALLTYPE GetAudioMeter(float& audioMag, float& audioPeak, float& audioMax) = 0;

   virtual void STDMETHODCALLTYPE GetMicAudioMeter(float& audioMag, float& audioPeak, float& audioMax) = 0;

   virtual void STDMETHODCALLTYPE SetSaveMicAudio(bool bSave) = 0;
   //
   virtual int STDMETHODCALLTYPE GetGraphicsDeviceInfoCount() = 0;
   
   virtual bool STDMETHODCALLTYPE GetGraphicsDeviceInfo(DeviceInfo &,DataSourcePosType &posType,int count)=0;

   virtual bool STDMETHODCALLTYPE GetGraphicsDeviceInfoExist(DeviceInfo &,bool &isFullScreen)=0;

   virtual bool STDMETHODCALLTYPE ModifyDeviceSource(DeviceInfo&srcDevice,DeviceInfo&desDevice,DataSourcePosType posType)=0;

   virtual bool STDMETHODCALLTYPE ReloadDevice(DeviceInfo&srcDevice)=0;


   virtual float STDMETHODCALLTYPE MuteMic(bool)=0;

   virtual float STDMETHODCALLTYPE MuteSpeaker(bool)=0;

   virtual void STDMETHODCALLTYPE SetMicVolunm(float)=0;

   virtual void STDMETHODCALLTYPE SetSpekerVolumn(float)=0;

   virtual float STDMETHODCALLTYPE GetMicVolunm()=0;

   virtual float STDMETHODCALLTYPE GetSpekerVolumn()=0;

   virtual bool STDMETHODCALLTYPE GetCurrentMic(DeviceInfo &)=0;

   //virtual void STDMETHODCALLTYPE SetSocketClient(FuncNotifyService)=0;

   virtual bool STDMETHODCALLTYPE IsHasSource(SOURCE_TYPE type)=0;

   virtual void STDMETHODCALLTYPE DeviceRecheck()=0;

   virtual void STDMETHODCALLTYPE EnterSharedDesktop() = 0;

   virtual void STDMETHODCALLTYPE LeaveSharedDesktop() = 0;

   virtual void STDMETHODCALLTYPE SetForceMono(bool) = 0;

   virtual bool STDMETHODCALLTYPE GetForceMono() = 0;

   virtual bool STDMETHODCALLTYPE ResetPublishInfo(const char *currentUrl,const char *nextUrl) = 0;

   virtual int STDMETHODCALLTYPE GetSumSpeed() = 0;

   virtual UINT64 STDMETHODCALLTYPE GetSendVideoFrameCount() = 0;

   virtual void STDMETHODCALLTYPE ModifyAreaShared(int,int,int,int) = 0;

   virtual void STDMETHODCALLTYPE ClearAllSource(bool)=0;

   virtual void STDMETHODCALLTYPE DealAddCameraSync(void* apData, DWORD adwLen) = 0;

   virtual void STDMETHODCALLTYPE DoHideLogo(bool) = 0;

   virtual void STDMETHODCALLTYPE SetAudioCapture(bool) = 0;

	virtual QDateTime STDMETHODCALLTYPE GetStartStreamTime() = 0;

   virtual void STDMETHODCALLTYPE LivePushAmf0Msg(const char* data,int length) = 0;
   virtual bool STDMETHODCALLTYPE IsStartStream() = 0;
   //virtual QString STDMETHODCALLTYPE CommitString() = 0;
   virtual void STDMETHODCALLTYPE StopRecord(bool bCoercion = false) = 0;
   virtual void STDMETHODCALLTYPE InitCapture() = 0;
	virtual void GetMediaFileWidthAndHeight(const char* path, int &width, int& height) = 0;
};
#endif // __I_OBSCONTROL_LOGIC__H_INCLUDE__ 
