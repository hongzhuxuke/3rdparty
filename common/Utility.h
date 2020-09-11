#ifndef __UTILITY_INCLUDE_H__
#define __UTILITY_INCLUDE_H__
#define WINVER         0x0600
#define _WIN32_WINDOWS 0x0600
#define _WIN32_WINNT   0x0600
#define NTDDI_VERSION  NTDDI_VISTASP1

#define WIN32_LEAN_AND_MEAN
#define ISOLATION_AWARE_ENABLED 1
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <vector>
#include <xmmintrin.h>
#include <emmintrin.h>

#define USE_TRACE 1
#include "Utility/XT.h"


//for audio vol
#define VOL_MIN -96
#define VOL_MAX 0

#define VOLN_METERED  0x302

#define VOLN_ADJUSTING  0x300
#define VOLN_FINALVALUE 0x301
#define VOLN_TOGGLEMUTE 0x302

#define VOLN_MUTELEVEL 0.05f

/* 60 db dynamic range values for volume control scale*/
#define VOL_ALPHA .001f
#define VOL_BETA 6.908f



#define SafeReleaseLogRef(var) if(var) {ULONG chi = var->Release(); OSDebugOut(TEXT("releasing %s, %d refs were left\r\n"), L#var, chi); var = NULL;}
#define SafeRelease(var) if(var) {var->Release(); var = NULL;}

//big endian conversion functions
#define QWORD_BE(val) (((val>>56)&0xFF) | (((val>>48)&0xFF)<<8) | (((val>>40)&0xFF)<<16) | (((val>>32)&0xFF)<<24) | \
   (((val >> 24) & 0xFF) << 32) | (((val >> 16) & 0xFF) << 40) | (((val >> 8) & 0xFF) << 48) | ((val & 0xFF) << 56))
#define DWORD_BE(val) (((val>>24)&0xFF) | (((val>>16)&0xFF)<<8) | (((val>>8)&0xFF)<<16) | ((val&0xFF)<<24))
#define WORD_BE(val)  (((val>>8)&0xFF) | ((val&0xFF)<<8))

__forceinline QWORD fastHtonll(QWORD qw) { return QWORD_BE(qw); }
__forceinline DWORD fastHtonl(DWORD dw) { return DWORD_BE(dw); }
__forceinline  WORD fastHtons(WORD  w) { return  WORD_BE(w); }

//arghh I hate defines like this
#define RUNONCE static bool bRunOnce = false; if(!bRunOnce && (bRunOnce = true))

inline BOOL CloseDouble(double f1, double f2, double precision = 0.001) {
   return fabs(f1 - f2) <= precision;
}

QWORD GetQWDif(QWORD val1, QWORD val2);

QWORD GetQPCTimeNS();
QWORD GetQPCTime100NS();
QWORD GetQPCTimeMS();
void MixAudio(float *bufferDest, float *bufferSrc, UINT totalFloats, bool bForceMono);



#endif