#ifndef __IGRAHICS_INCLUDE__
#define __IGRAHICS_INCLUDE__
#include "VH_ConstDeff.h"


#ifdef  VHALL_EXPORT
#define VHALL_EXPORT     __declspec(dllimport)
#else
#define VHALL_EXPORT     __declspec(dllexport)
#endif

#define OP_TYPE_FULL_SCREEN   0
#define OP_TYPE_MOVE_TOP      1
#define OP_TYPE_MOVE_BOTTOM   2
#define OP_TYPE_MODIFY        3
#define OP_TYPE_DELETE        4

typedef bool(*IGraphicDeleteSourceHook)(void *, void *,SOURCE_TYPE);

typedef long (*IPOSTCRMessageFunc)(DWORD,void *,DWORD);
class IDataReceiver;
class IMediaCore;

class __declspec(dllexport) IGraphics {
public:
   IGraphics() {};
   virtual ~IGraphics() {};
   virtual void SetDataReceiver(IDataReceiver *)=0;
   virtual bool AddPersistentSource(const wchar_t* path, const RECT& rect, const int & index) = 0;
   virtual bool AddNewSource(SOURCE_TYPE captureType, void* sourceCtx, void **pSourceItem = NULL) = 0;
   virtual void ClearSource(void *sourceItem = NULL) = 0;
   virtual void ClearSourceByType(SOURCE_TYPE sourceType) = 0;

   virtual void ProcessSource(const int & opType) = 0;
   virtual bool isSelectAreaSource() = 0;
   virtual bool IsHasWindowsSource() = 0;
   virtual bool IsHasNoPersistentSource() = 0;
   virtual bool IsHasMonitorSource() = 0;
   virtual int  GetCurrentItemType()=0;
   virtual bool IsModifiable() = 0;
   virtual bool Preview(const SIZE &,const SIZE &,bool) = 0;
   /*  virtual bool Start() = 0;*/
   virtual void Resize(const RECT& client, bool bRedrawRenderFrame) = 0;
   virtual void StopPreview(bool isRestart) = 0;
   // virtual void Shutdown() = 0;
   virtual bool Destory() = 0;
   virtual  SIZE GetResolution() = 0;
   virtual  SIZE GetBaseSize() = 0;
   virtual  int GetFps() = 0;
   virtual void GetSceneInfo(VideoSceneType& sceneType) = 0;
   virtual void OnMouseEvent(const UINT& mouseEvent, const POINTS& mousePos) = 0;
   //data 
   virtual void SetSyncWithEncoder(const bool & isWillsync) = 0;
   virtual unsigned char** LockCurrentFramePic(unsigned long long &t,unsigned long long currentTime = 0) = 0;
   virtual void UnlockCurrentFramePic() = 0;
   virtual void SetSourceDeleteHook(IGraphicDeleteSourceHook funcDeleteSourceHook, void *param) = 0;
   virtual void SourceRefit(void *item) = 0;
   virtual void SetFps(int )=0;
   virtual bool SetSourceVisible(wchar_t *sourceName,bool isVisible,bool isWait)=0;
   virtual bool WaitSetSourceVisible()=0;
   virtual void UpdataSourceElement() = 0;
   virtual unsigned char * OBSMemoryCreate(int size)=0;
   virtual void OBSMemoryFree(void *)=0;
   virtual void *GetMediaSource()=0;
   virtual void SetPostMsgHook(IPOSTCRMessageFunc funcPostMsg)=0;
   virtual int GetGraphicsDeviceInfoCount()=0;
   virtual bool GetGraphicsDeviceInfo(DeviceInfo &,DataSourcePosType &posType,int count)=0;
   virtual bool GetGraphicsDeviceInfoExist(DeviceInfo &deviceInfo,bool&)=0;
   virtual bool ModifyDeviceSource(DeviceInfo&srcDevice,DeviceInfo&desDevice,DataSourcePosType posType)=0;
   virtual bool ReloadDevice(DeviceInfo &deviceInfo)=0;
   virtual bool IsHasSource(SOURCE_TYPE ) =0;
   virtual void DeviceRecheck(DeviceList &)=0;
   virtual void SetCreateTextTextureMemory(FuncCreateTextureByText,FuncFreeMemory)=0;
   virtual void SetCreateTextureFromFileHook(FuncCheckFileTexture)=0;
   virtual void ModifyAreaShared(int left,int top,int right,int bottom) = 0;
   virtual void SetMediaCore(IMediaCore *) = 0;
   virtual HWND GetDeviceRenderHwnd(void *) = 0;
   virtual void DoHideLogo(bool bHide) = 0;
   virtual void ReinitMedia() = 0;
};



__declspec(dllexport) IGraphics* CreateGraphics(HWND msgWnd, HWND renderFrame, SIZE baseSize,int scaleType, const wchar_t *logPath = NULL);
__declspec(dllexport) void DestoryGraphics(IGraphics** graphics);

#endif

