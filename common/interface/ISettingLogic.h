#ifndef __I_SETTING_LOGIC__H_INCLUDE__
#define __I_SETTING_LOGIC__H_INCLUDE__

#include "VH_IUnknown.h"
// {BC6D89D9-D291-4EFB-B507-B35F612ED05D}
DEFINE_GUID(IID_ISettingLogic,
            0xbc6d89d9, 0xd291, 0x4efb, 0xb5, 0x7, 0xb3, 0x5f, 0x61, 0x2e, 0xd0, 0x5d);

class ISettingLogic : public VH_IUnknown {
public:
   virtual bool STDMETHODCALLTYPE IsCameraShow() = 0;   
   virtual void STDMETHODCALLTYPE ApplySettings(int) = 0;
   virtual void STDMETHODCALLTYPE InitAudioSetting() = 0;
   virtual void STDMETHODCALLTYPE InitCameraSetting() = 0;
   virtual bool STDMETHODCALLTYPE GetIsServerPlayback() = 0;
   virtual void STDMETHODCALLTYPE ActiveSettingUI() = 0;   
   virtual void STDMETHODCALLTYPE GetDesktopCameraList(void **) = 0;
   virtual void* STDMETHODCALLTYPE LockVideo(QString deviceID = QString(),int index = -1) = 0;
   virtual void STDMETHODCALLTYPE ResetLockVideo() = 0;
   virtual void STDMETHODCALLTYPE UnlockVideo(void *) = 0;
   virtual bool STDMETHODCALLTYPE IsDesktopCameraListShow() = 0;
   virtual void STDMETHODCALLTYPE DeviceAdd(void* apData, DWORD adwLen) = 0;   
   virtual void STDMETHODCALLTYPE DealCameraSelect(void* apData, DWORD adwLen) = 0;   
   virtual void STDMETHODCALLTYPE InitSysSetting() = 0;
   virtual void STDMETHODCALLTYPE SetDesktopShareState(bool) = 0;
   virtual void STDMETHODCALLTYPE SetCutRecordDisplay(const int iCutRecord) = 0;


};
#endif // __I_SETTING_LOGIC__H_INCLUDE__ 
