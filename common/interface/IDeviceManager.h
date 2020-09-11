#ifndef __I_DEVICE_MANAGER__H_INCLUDE__
#define __I_DEVICE_MANAGER__H_INCLUDE__

#include "VH_IUnknown.h"
#include "VH_ConstDeff.h"
#include <vector>
using namespace std;
// {EEE28E1A-7D7A-4D1D-9922-E72085A5591A}
DEFINE_GUID(IID_IDeviceManager,
            0xeee28e1a, 0x7d7a, 0x4d1d, 0x99, 0x22, 0xe7, 0x20, 0x85, 0xa5, 0x59, 0x1a);

class IDeviceManager : public VH_IUnknown {
public:
   //获得麦克风列表
   virtual void STDMETHODCALLTYPE GetMicDevices(DeviceList& deviceList) = 0;

   virtual void STDMETHODCALLTYPE GetSpeakerDevices(DeviceList& deviceList) = 0;

   virtual void STDMETHODCALLTYPE GetVedioDevices(DeviceList& deviceList) = 0;

   virtual void STDMETHODCALLTYPE SetMasterVolume(DeviceInfo, float fVolume) = 0;

   virtual void STDMETHODCALLTYPE GetMasterVolume(DeviceInfo, float* fVolume) = 0;

   virtual void STDMETHODCALLTYPE SetMute(LPWSTR wsDeviceId, bool bMute) = 0;

   virtual void STDMETHODCALLTYPE GetMute(LPWSTR wsDeviceId, bool* bMute) = 0;

   virtual void STDMETHODCALLTYPE SetEnhanceLevel(LPWSTR wsDeviceId, float fLevel) = 0;

   virtual void STDMETHODCALLTYPE GetEnhanceLevel(LPWSTR wsDeviceId, float *pfLevel, float *pfMin, float *pfMax, float *pfStep) = 0;

   virtual void STDMETHODCALLTYPE GetResolution(vector<FrameInfo>& mediaOutputList, vector<SIZE>& resolutions, UINT64& minFrameInterval, UINT64& maxFrameInterval, const wstring& deviceName, const wstring& deviceID) = 0;

   virtual void STDMETHODCALLTYPE OpenPropertyPages(HWND hWndOwner,DeviceInfo ) = 0;      //dwType 0代表Audio1代表Vedio
};
#endif // __I_DEVICE_MANAGER__H_INCLUDE__ 
