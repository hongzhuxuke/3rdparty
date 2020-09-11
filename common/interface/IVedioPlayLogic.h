#ifndef __I_VEDIOPLAY_LOGIC__H_INCLUDE__
#define __I_VEDIOPLAY_LOGIC__H_INCLUDE__

#include "VH_IUnknown.h"
// {1EB29C5A-1617-4AD0-A6EB-9F26E8FACE71}
DEFINE_GUID(IID_IVedioPlayLogic,
            0x1eb29c5a, 0x1617, 0x4ad0, 0xa6, 0xeb, 0x9f, 0x26, 0xe8, 0xfa, 0xce, 0x71);

class IVedioPlayLogic : public VH_IUnknown {
public:
   virtual void STDMETHODCALLTYPE ReposVedioPlay(bool bShow) = 0;
   virtual void STDMETHODCALLTYPE ForceHide(bool bShow) = 0;
   virtual void STDMETHODCALLTYPE StopAdmin(bool bForceHideAdmin = false) = 0;
   virtual void STDMETHODCALLTYPE StopPlayFile() = 0;
   virtual void STDMETHODCALLTYPE ResetPlayUiSize(int width) = 0;
   virtual bool STDMETHODCALLTYPE IsPlayMediaFileUIShown() = 0;
};
#endif // __I_VEDIOPLAY_LOGIC__H_INCLUDE__ 
