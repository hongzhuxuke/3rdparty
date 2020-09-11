#include "Utility.h"

#pragma comment (lib,"winmm.lib") 
#pragma comment (lib,"psapi.lib")

extern LARGE_INTEGER clockFreq;
__declspec(thread) LONGLONG lastQPCTime = 0;
QWORD GetQPCTimeNS() {
   LARGE_INTEGER currentTime;
   QueryPerformanceCounter(&currentTime);

   if (currentTime.QuadPart < lastQPCTime)
      Log(TEXT("GetQPCTimeNS: WTF, clock went backwards! %I64d < %I64d"), currentTime.QuadPart, lastQPCTime);

   lastQPCTime = currentTime.QuadPart;

   double timeVal = double(currentTime.QuadPart);
   timeVal *= 1000000000.0;
   timeVal /= double(clockFreq.QuadPart);

   return QWORD(timeVal);
}

QWORD GetQPCTime100NS() {
   LARGE_INTEGER currentTime;
   QueryPerformanceCounter(&currentTime);

   if (currentTime.QuadPart < lastQPCTime)
      Log(TEXT("GetQPCTime100NS: WTF, clock went backwards! %I64d < %I64d"), currentTime.QuadPart, lastQPCTime);

   lastQPCTime = currentTime.QuadPart;

   double timeVal = double(currentTime.QuadPart);
   timeVal *= 10000000.0;
   timeVal /= double(clockFreq.QuadPart);

   return QWORD(timeVal);
}

QWORD GetQPCTimeMS() {
   LARGE_INTEGER currentTime;
   QueryPerformanceCounter(&currentTime);

   if (currentTime.QuadPart < lastQPCTime)
      Log(TEXT("GetQPCTimeMS: WTF, clock went backwards! %I64d < %I64d"), currentTime.QuadPart, lastQPCTime);

   lastQPCTime = currentTime.QuadPart;

   QWORD timeVal = currentTime.QuadPart;
   timeVal *= 1000;
   timeVal /= clockFreq.QuadPart;

   return timeVal;
}

void MixAudio(float *bufferDest, float *bufferSrc, UINT totalFloats, bool bForceMono) {
   UINT floatsLeft = totalFloats;
   float *destTemp = bufferDest;
   float *srcTemp = bufferSrc;

   if ((UPARAM(destTemp) & 0xF) == 0 && (UPARAM(srcTemp) & 0xF) == 0) {
      UINT alignedFloats = floatsLeft & 0xFFFFFFFC;

      if (bForceMono) {
         __m128 halfVal = _mm_set_ps1(0.5f);
         for (UINT i = 0; i < alignedFloats; i += 4) {
            float *micInput = srcTemp + i;
            __m128 val = _mm_load_ps(micInput);
            __m128 shufVal = _mm_shuffle_ps(val, val, _MM_SHUFFLE(2, 3, 0, 1));

            _mm_store_ps(micInput, _mm_mul_ps(_mm_add_ps(val, shufVal), halfVal));
         }
      }

      //__m128 maxVal = _mm_set_ps1(1.0f);
      //__m128 minVal = _mm_set_ps1(-1.0f);

      //for (UINT i = 0; i < alignedFloats; i += 4) {
      //   float *pos = destTemp + i;

      //   __m128 mix;
      //   mix = _mm_add_ps(_mm_load_ps(pos), _mm_load_ps(srcTemp + i));
      //   mix = _mm_min_ps(mix, maxVal);
      //   mix = _mm_max_ps(mix, minVal);

      //   _mm_store_ps(pos, mix);
      //}

      floatsLeft &= 0x3;
      destTemp += alignedFloats;
      srcTemp += alignedFloats;
   }

   if (floatsLeft) {
      if (bForceMono) {
         for (UINT i = 0; i < floatsLeft; i += 2) {
            srcTemp[i] += srcTemp[i + 1];
            srcTemp[i] *= 0.5f;
            srcTemp[i + 1] = srcTemp[i];
         }
      }

      //for (UINT i = 0; i < floatsLeft; i++) {
      //   float val = destTemp[i] + srcTemp[i];

      //   if (val < -1.0f)     val = -1.0f;
      //   else if (val > 1.0f) val = 1.0f;

      //   destTemp[i] = val;
      //}
   }
}



int OBSMessageBox(HWND hwnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT flags) {

   return MessageBox(hwnd, lpText, lpCaption, flags);
}
