/********************************************************************************
 Copyright (C) 2012 Hugh Bailey <obs.jim@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 ********************************************************************************/
#include "OBSApi.h"

#include <dshow.h>
#include <Amaudio.h>
#include <Dvdmedia.h>
#include "VH_ConstDeff.h"

#include "IDShowPlugin.h"
#include "DeviceSource.h"
BOOL SaveBitmapToFile(HBITMAP   hBitmap, wchar_t * szfilename);

void SaveHDC(wchar_t *filename, HDC hDC, int w, int h) {


   HDC hDCMem = ::CreateCompatibleDC(hDC);//��������DC   

   HBITMAP hBitMap = ::CreateCompatibleBitmap(hDC, w, h);//��������λͼ   
   HBITMAP hOldMap = (HBITMAP)::SelectObject(hDCMem, hBitMap);//��λͼѡ��DC�������淵��ֵ   

   ::BitBlt(hDCMem, 0, 0, w, h, hDC, 0, 0, SRCCOPY);//����ĻDC��ͼ���Ƶ��ڴ�DC��   

   SaveBitmapToFile(hBitMap, filename);

   ::SelectObject(hDCMem, hOldMap);//ѡ���ϴεķ���ֵ   

   //�ͷ�   
   ::DeleteObject(hBitMap);
   ::DeleteDC(hDCMem);

}


struct ResSize {
   UINT cx;
   UINT cy;
};

enum {
   COLORSPACE_AUTO,
   COLORSPACE_709,
   COLORSPACE_601
};

#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
   EXTERN_C const GUID DECLSPEC_SELECTANY name \
   = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#define STDCALL                 __stdcall
DWORD STDCALL PackPlanarThread(ConvertData *data);

#define NEAR_SILENT  3000
#define NEAR_SILENTf 3000.0

static DeinterlacerConfig deinterlacerConfigs[DEINTERLACING_TYPE_LAST] = {
   { DEINTERLACING_NONE, FIELD_ORDER_NONE, DEINTERLACING_PROCESSOR_CPU },
   { DEINTERLACING_DISCARD, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_CPU },
   { DEINTERLACING_RETRO, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_CPU | DEINTERLACING_PROCESSOR_GPU, true },
   { DEINTERLACING_BLEND, FIELD_ORDER_NONE, DEINTERLACING_PROCESSOR_GPU },
   { DEINTERLACING_BLEND2x, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_GPU, true },
   { DEINTERLACING_LINEAR, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_GPU },
   { DEINTERLACING_LINEAR2x, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_GPU, true },
   { DEINTERLACING_YADIF, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_GPU },
   { DEINTERLACING_YADIF2x, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_GPU, true },
   { DEINTERLACING__DEBUG, FIELD_ORDER_TFF | FIELD_ORDER_BFF, DEINTERLACING_PROCESSOR_GPU },
};
const int inputPriority[] =
{
   1,
   6,
   7,
   7,

   12,
   12,

   -1,
   -1,

   13,
   13,
   13,
   13,

   5,
   -1,

   10,
   10,
   10,

   9
};

BOOL SaveBitmapToFile(HBITMAP   hBitmap, wchar_t * szfilename) {
   HDC     hDC;
   //��ǰ�ֱ�����ÿ������ռ�ֽ���            
   int     iBits;
   //λͼ��ÿ������ռ�ֽ���            
   WORD     wBitCount;
   //�����ɫ���С��     λͼ�������ֽڴ�С     ��λͼ�ļ���С     ��     д���ļ��ֽ���                
   DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
   //λͼ���Խṹ                
   BITMAP     Bitmap;
   //λͼ�ļ�ͷ�ṹ            
   BITMAPFILEHEADER     bmfHdr;
   //λͼ��Ϣͷ�ṹ                
   BITMAPINFOHEADER     bi;
   //ָ��λͼ��Ϣͷ�ṹ                    
   LPBITMAPINFOHEADER     lpbi;
   //�����ļ��������ڴ�������ɫ����                
   HANDLE     fh, hDib, hPal, hOldPal = NULL;

   //����λͼ�ļ�ÿ��������ռ�ֽ���                
   hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
   iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
   DeleteDC(hDC);
   if (iBits <= 1)
      wBitCount = 1;
   else  if (iBits <= 4)
      wBitCount = 4;
   else if (iBits <= 8)
      wBitCount = 8;
   else
      wBitCount = 24;

   GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
   bi.biSize = sizeof(BITMAPINFOHEADER);
   bi.biWidth = Bitmap.bmWidth;
   bi.biHeight = Bitmap.bmHeight;
   bi.biPlanes = 1;
   bi.biBitCount = wBitCount;
   bi.biCompression = BI_RGB;
   bi.biSizeImage = 0;
   bi.biXPelsPerMeter = 0;
   bi.biYPelsPerMeter = 0;
   bi.biClrImportant = 0;
   bi.biClrUsed = 0;

   dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

   //Ϊλͼ���ݷ����ڴ�                
   hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
   lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
   *lpbi = bi;

   //     �����ɫ��                    
   hPal = GetStockObject(DEFAULT_PALETTE);
   if (hPal) {
      hDC = ::GetDC(NULL);
      hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
      RealizePalette(hDC);
   }

   //     ��ȡ�õ�ɫ�����µ�����ֵ                
   GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
             (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
             (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

   //�ָ���ɫ��                    
   if (hOldPal) {
      ::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
      RealizePalette(hDC);
      ::ReleaseDC(NULL, hDC);
   }

   //����λͼ�ļ�                    
   fh = CreateFile(szfilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

   if (fh == INVALID_HANDLE_VALUE)         return     FALSE;

   //     ����λͼ�ļ�ͷ                
   bmfHdr.bfType = 0x4D42;     //     "BM"                
   dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
   bmfHdr.bfSize = dwDIBSize;
   bmfHdr.bfReserved1 = 0;
   bmfHdr.bfReserved2 = 0;
   bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
   //     д��λͼ�ļ�ͷ                
   WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
   //     д��λͼ�ļ���������                
   WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
   //���                    
   GlobalUnlock(hDib);
   GlobalFree(hDib);
   CloseHandle(fh);

   return     TRUE;

}

bool DeviceSource::Init(XElement *data) {
   hSampleMutex = OSCreateMutex();
   if (!hSampleMutex) {
      AppWarning(TEXT("DShowPlugin: could not create sample mutex"));
      return false;
   }

   m_bDeviceExist = true;
   m_device = NULL;
   int numThreads = MAX(OSGetTotalCores() - 2, 1);
   hConvertThreads = (HANDLE*)Allocate(sizeof(HANDLE)*numThreads);
   convertData = (ConvertData*)Allocate(sizeof(ConvertData)*numThreads);

   zero(hConvertThreads, sizeof(HANDLE)*numThreads);
   zero(convertData, sizeof(ConvertData)*numThreads);

   this->mElementData = data;
   UpdateSettings();

   Log(TEXT("Using directshow input"));

   m_imageBufferSize = 0;

   return true;
}
DeviceSource::DeviceSource() :tipTexture(NULL)
, m_LastSyncTime(0)
, m_deviceNameTexture(NULL)
, m_device(NULL) {
   tipTexture = CreateTextureFromFile(L"cameraLoading.png", TRUE);
   Log(L"tipTexture %p\n", tipTexture);
   m_disconnectTexture = CreateTextureFromFile(L"disconnected.png", TRUE);
   Log(L"m_disconnectTexture %p\n", m_disconnectTexture);
   m_failedTexture = CreateTextureFromFile(L"cameraLoadingFailed.png", TRUE);
   Log(L"m_failedTexture %p\n", m_failedTexture);
   m_blackBackgroundTexture = CreateTexture(100, 100, GS_BGRA, NULL, FALSE, FALSE);
   Log(L"m_blackBackgroundTexture %p\n", m_blackBackgroundTexture);
}
DeviceSource::~DeviceSource() {
   Stop();
   UnloadFilters();

   if (hConvertThreads)
      Free(hConvertThreads);

   if (convertData)
      Free(convertData);

   if (hSampleMutex)
      OSCloseMutex(hSampleMutex);

   ReleaseDShowVideoFilter(this);
   if (tipTexture) {
      delete tipTexture;
      tipTexture = NULL;
   }
   if (m_disconnectTexture) {
      delete m_disconnectTexture;
      m_disconnectTexture = NULL;
   }
   if (m_failedTexture) {
      delete m_failedTexture;
      m_failedTexture = NULL;
   }
   if (m_blackBackgroundTexture) {
      delete m_blackBackgroundTexture;
      m_blackBackgroundTexture = NULL;
   }
   if (m_deviceNameTexture) {
      delete m_deviceNameTexture;
      m_deviceNameTexture = NULL;
   }
   if (m_renderView != NULL) {
      OBSGraphics_DestoryRenderTargetView(m_renderView);
      m_renderView = NULL;
   }
}

#define SHADER_PATH TEXT("shaders/")

String DeviceSource::ChooseShader() {

   if (m_setting.colorType == DeviceOutputType_RGB)
	//if(m_setting.colorType <= DeviceOutputType_ARGB32 /*|| m_setting.colorType >= DeviceOutputType_Y41P*/)
      return String();

   String strShader;
   strShader << SHADER_PATH;

   if (m_setting.colorType == DeviceOutputType_I420)
      strShader << TEXT("YUVToRGB.pShader");
   else if (m_setting.colorType == DeviceOutputType_YV12)
      strShader << TEXT("YVUToRGB.pShader");
   else if (m_setting.colorType == DeviceOutputType_YVYU)
      strShader << TEXT("YVXUToRGB.pShader");
   else if (m_setting.colorType == DeviceOutputType_YUY2)
      strShader << TEXT("YUXVToRGB.pShader");
   else if (m_setting.colorType == DeviceOutputType_UYVY)
      strShader << TEXT("UYVToRGB.pShader");
   else if (m_setting.colorType == DeviceOutputType_HDYC)
      strShader << TEXT("HDYCToRGB.pShader");
   else
      strShader << TEXT("RGB.pShader");

   return strShader;
}

String DeviceSource::ChooseDeinterlacingShader() {
   String shader;
   shader << SHADER_PATH << TEXT("Deinterlace_");
#ifdef _DEBUG
#define DEBUG__ _DEBUG
#undef _DEBUG
#endif
#define SELECT(x) case DEINTERLACING_##x: shader << String(TEXT(#x)).MakeLower(); break;
   switch (deinterlacer.type) {
      SELECT(RETRO)
         SELECT(BLEND)
         SELECT(BLEND2x)
         SELECT(LINEAR)
         SELECT(LINEAR2x)
         SELECT(YADIF)
         SELECT(YADIF2x)
         SELECT(_DEBUG)
   }
   return shader << TEXT(".pShader");
#undef SELECT
#ifdef DEBUG__
#define _DEBUG DEBUG__
#undef DEBUG__
#endif

}
const float yuv709Mat[16] = { 0.182586f, 0.614231f, 0.062007f, 0.062745f,
-0.100644f, -0.338572f, 0.439216f, 0.501961f,
0.439216f, -0.398942f, -0.040274f, 0.501961f,
0.000000f, 0.000000f, 0.000000f, 1.000000f };

const float yuvMat[16] = { 0.256788f, 0.504129f, 0.097906f, 0.062745f,
-0.148223f, -0.290993f, 0.439216f, 0.501961f,
0.439216f, -0.367788f, -0.071427f, 0.501961f,
0.000000f, 0.000000f, 0.000000f, 1.000000f };

const float yuvToRGB601[2][16] =
{
   {
      1.164384f, 0.000000f, 1.596027f, -0.874202f,
      1.164384f, -0.391762f, -0.812968f, 0.531668f,
      1.164384f, 2.017232f, 0.000000f, -1.085631f,
      0.000000f, 0.000000f, 0.000000f, 1.000000f
   },
   {
      1.000000f, 0.000000f, 1.407520f, -0.706520f,
      1.000000f, -0.345491f, -0.716948f, 0.533303f,
      1.000000f, 1.778976f, 0.000000f, -0.892976f,
      0.000000f, 0.000000f, 0.000000f, 1.000000f
   }
};

const float yuvToRGB709[2][16] = {
   {
      1.164384f, 0.000000f, 1.792741f, -0.972945f,
      1.164384f, -0.213249f, -0.532909f, 0.301483f,
      1.164384f, 2.112402f, 0.000000f, -1.133402f,
      0.000000f, 0.000000f, 0.000000f, 1.000000f
   },
   {
      1.000000f, 0.000000f, 1.581000f, -0.793600f,
      1.000000f, -0.188062f, -0.469967f, 0.330305f,
      1.000000f, 1.862906f, 0.000000f, -0.935106f,
      0.000000f, 0.000000f, 0.000000f, 1.000000f
   }
};

void DeviceSource::SetDeviceExist(bool ok) {
   if (m_bDeviceExist == ok) {
      return;
   }

   if (!m_bDeviceExist) {
      Reload();
   }

   m_bDeviceExist = ok;
}
HWND gTest = NULL;
//�ڴ�й©
bool DeviceSource::LoadFilters() {
   if (bCapturing || bFiltersLoaded)
      return false;
   m_LastSyncTime = 0;
   m_bIsHasNoData = false;
   String          strDeviceDisplayName;
   String          strDeviceName;
   String          strDeviceID;

   INT            frameInterval;
   bool bSucceeded = false;

   bool bAddedVideoCapture = false;
   HRESULT err;

   m_renderHWND = (HWND)mElementData->GetInt(L"renderHwnd", NULL);
   //����˴������¼��ش��ڣ��ᵼ�·ŵ�����ʱͼ���봰�ڱ�������ȷ��
   OBSAPI_EnterSceneMutex();
   if (m_renderView != NULL) {
      OBSGraphics_DestoryRenderTargetView(m_renderView);
      m_renderView = NULL;
   }

   m_renderView = OBSGraphics_CreateRenderTargetView(m_renderHWND, 0, 0);
   OBSAPI_LeaveSceneMutex();

   deinterlacer.isReady = true;
   bUseThreadedConversion = OBSAPI_UseMultithreadedOptimizations() && (OSGetTotalCores() > 1);
   strDeviceDisplayName = mElementData->GetString(TEXT("displayName"));
   strDeviceName = mElementData->GetString(TEXT("deviceName"));
   strDeviceID = mElementData->GetString(TEXT("deviceID"));

   //deinterlacer.type = data->GetInt(TEXT("deinterlacingType"), 0);
   //deinterlacer.type = data->GetInt(TEXT("deinterlacingType"), 2);

   //��������û������
   deinterlacer.fieldOrder = mElementData->GetInt(TEXT("deinterlacingFieldOrder"), 0);
   deinterlacer.processor = mElementData->GetInt(TEXT("deinterlacingProcessor"), 0);
   deinterlacer.doublesFramerate = mElementData->GetInt(TEXT("deinterlacingDoublesFramerate"), 0) != 0;
   if (strDeviceDisplayName.Length()) {
      wcscpy(deviceInfo.m_sDeviceDisPlayName, strDeviceDisplayName.Array());
      if (m_deviceNameTexture) {
         delete m_deviceNameTexture;
         m_deviceNameTexture = NULL;
      }

      m_deviceNameTexture = CreateTextureFromText(deviceInfo.m_sDeviceDisPlayName);
   }

   if (strDeviceName.Length()) {
      wcscpy(deviceInfo.m_sDeviceName, strDeviceName.Array());
   }

   if (strDeviceID.Length()) {
      wcscpy(deviceInfo.m_sDeviceID, strDeviceID.Array());
   }

   deviceInfo.m_sDeviceType = TYPE_DSHOW_VIDEO;

   DShowVideoManagerEnterMutex();
   ReleaseDShowVideoFilter(this);

   //m_setting.type=(DeinterlacingType);
   //m_setting ���ڻ�ȡɫ�ʿռ�
   if (!GetDShowVideoFilter(m_device,
      deviceInfo,
      m_setting,
      DShowDeviceType_Video,
      DShowDevicePinType_Video,
      this, NULL)) {
      Log(L"[GetDShowVideoFilter] Failed! [%s]", deviceInfo.m_sDeviceDisPlayName);
      DShowVideoManagerLeaveMutex();
      return false;
   }

   DShowVideoManagerLeaveMutex();
   FrameInfo currFrameInfo;
   if (!GetDShowVideoFrameInfoList(deviceInfo, NULL, &currFrameInfo, m_setting.type)) {
      Log(L"[GetDShowVideoFrameInfoList] Failed! [%s]", deviceInfo.m_sDeviceDisPlayName);
      return false;
   }
   deinterlacer.type = m_setting.type;

   //�ֱ���
   renderCX = newCX = currFrameInfo.maxCX;
   renderCY = newCY = currFrameInfo.maxCY;

   //֡��
   frameInterval = currFrameInfo.maxFrameInterval;

   bFirstFrame = true;
   bSucceeded = true;

cleanFinish:

   if (!bSucceeded) {
      bCapturing = false;


      if (colorConvertShader) {
         delete colorConvertShader;
         colorConvertShader = NULL;
      }

      if (lpImageBuffer) {
         Free(lpImageBuffer);
         lpImageBuffer = NULL;
      }

      bReadyToDraw = true;
   } else
      bReadyToDraw = false;

   // Updated check to ensure that the source actually turns red instead of
   // screwing up the size when SetFormat fails.
   if (renderCX <= 0 || renderCX >= 8192) { newCX = renderCX = 32; imageCX = renderCX; }
   if (renderCY <= 0 || renderCY >= 8192) { newCY = renderCY = 32; imageCY = renderCY; }

   if (colorConvertShader == NULL) {
      String strShader;
      strShader = ChooseShader();
      if (strShader.IsValid())
         colorConvertShader = CreatePixelShaderFromFile(strShader);
      ChangeSize(true, true);
   }

   ChangeSize(bSucceeded, true);

   return bSucceeded;
}

void DeviceSource::UnloadFilters() {
   if (m_deviceSourceTexture) {
      delete m_deviceSourceTexture;
      m_deviceSourceTexture = NULL;
   }
   if (previousTexture) {
      delete previousTexture;
      previousTexture = NULL;
   }

   KillThreads();

   if (bFiltersLoaded) {
      bFiltersLoaded = false;
   }
   if (colorConvertShader) {
      delete colorConvertShader;
      colorConvertShader = NULL;
   }

   if (lpImageBuffer) {
      Free(lpImageBuffer);
      lpImageBuffer = NULL;
   }


}

void DeviceSource::Start() {
   if (bCapturing)
      return;
   if (drawShader) {
      delete drawShader;
      drawShader = NULL;
   }
   drawShader = CreatePixelShaderFromFile(TEXT("shaders\\DrawTexture_ColorAdjust.pShader"));
   bCapturing = true;
}

void DeviceSource::Stop() {
   if (drawShader) {
      delete drawShader;
      drawShader = NULL;
   }
   if (!bCapturing)
      return;
   bCapturing = false;
}

void DeviceSource::BeginScene() {

   if (m_blackBackgroundTexture) {
      BYTE *lpData = NULL;
      UINT pitch = 0;
      if (m_blackBackgroundTexture->Map(lpData, pitch)) {
         if (lpData) {
            for (int i = 0; i < m_blackBackgroundTexture->Height(); i++) {
               for (int j = 0; j < pitch; j += 4) {
                  *(unsigned int *)&lpData[i*pitch + j] = 0xFF000000;
               }
            }
         }
         m_blackBackgroundTexture->Unmap();
      }
   }
   Start();
}

void DeviceSource::EndScene() {
   Stop();
}

void DeviceSource::GlobalSourceLeaveScene() {
   if (!enteredSceneCount)
      return;
   if (--enteredSceneCount)
      return;
}

void DeviceSource::GlobalSourceEnterScene() {
   if (enteredSceneCount++)
      return;
}
void DeviceSource::KillThreads() {
   int numThreads = MAX(OSGetTotalCores() - 2, 1);
   for (int i = 0; i < numThreads&&hConvertThreads; i++) {
      if (hConvertThreads[i]) {
         convertData[i].bKillThread = true;
         SetEvent(convertData[i].hSignalConvert);

         OSTerminateThread(hConvertThreads[i], 10000);
         hConvertThreads[i] = NULL;
      }

      convertData[i].bKillThread = false;

      if (convertData[i].hSignalConvert) {
         CloseHandle(convertData[i].hSignalConvert);
         convertData[i].hSignalConvert = NULL;
      }

      if (convertData[i].hSignalComplete) {
         CloseHandle(convertData[i].hSignalComplete);
         convertData[i].hSignalComplete = NULL;
      }
   }
}

void DeviceSource::ChangeSize(bool bSucceeded, bool bForce) {
   if (!bForce && renderCX == newCX && renderCY == newCY)
      return;

   if (newCX == 0 || newCY == 0) {
      return;
   }
   renderCX = newCX;
   renderCY = newCY;

   switch (m_setting.colorType) {

      //yuv420   
   case DeviceOutputType_I420:
   case DeviceOutputType_YV12:
      lineSize = renderCX; //per plane
      break;
      //yuv422   
   case DeviceOutputType_YVYU:
   case DeviceOutputType_YUY2:
   case DeviceOutputType_UYVY:
   case DeviceOutputType_HDYC:
      lineSize = (renderCX * 2);
      break;
	  //RGB
   case DeviceOutputType_RGB://case DeviceOutputType_RGB:
	   lineSize = renderCX * 4;
	   break;
   default:break;
   }

   linePitch = lineSize;
   lineShift = 0;
   imageCX = renderCX;
   imageCY = renderCY;

   deinterlacer.imageCX = renderCX;
   deinterlacer.imageCY = renderCY;

   if (deinterlacer.doublesFramerate)
      deinterlacer.imageCX *= 2;
   //ȥ��������
   switch (deinterlacer.type) {
   case DEINTERLACING_DISCARD:
      deinterlacer.imageCY = renderCY / 2;
      linePitch = lineSize * 2;
      renderCY /= 2;
      break;

   case DEINTERLACING_RETRO:
      deinterlacer.imageCY = renderCY / 2;
      if (deinterlacer.processor != DEINTERLACING_PROCESSOR_GPU) {
         lineSize *= 2;
         linePitch = lineSize;
         renderCY /= 2;
         renderCX *= 2;
      }
      break;

   case DEINTERLACING__DEBUG:
      deinterlacer.imageCX *= 2;
      deinterlacer.imageCY *= 2;
   case DEINTERLACING_BLEND2x:
      //case DEINTERLACING_MEAN2x:
   case DEINTERLACING_YADIF:
   case DEINTERLACING_YADIF2x:
      deinterlacer.needsPreviousFrame = true;
      break;
   }

   if (deinterlacer.type != DEINTERLACING_NONE
       && deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU) {

      deinterlacer.vertexShader.reset
         (CreateVertexShaderFromFile(TEXT("shaders/DrawTexture.vShader")));


      deinterlacer.pixelShader = CreatePixelShaderFromFileAsync
         (ChooseDeinterlacingShader());


      deinterlacer.isReady = false;

   }

   KillThreads();

   int numThreads = MAX(OSGetTotalCores() - 2, 1);
   for (int i = 0; i < numThreads; i++) {
      convertData[i].width = lineSize;
      convertData[i].height = renderCY;
      convertData[i].sample = NULL;
      convertData[i].hSignalConvert = CreateEvent(NULL, FALSE, FALSE, NULL);
      convertData[i].hSignalComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
      convertData[i].mLinePitch = linePitch;
      convertData[i].mLineShift = lineShift;

      if (i == 0)
         convertData[i].startY = 0;
      else
         convertData[i].startY = convertData[i - 1].endY;

      if (i == (numThreads - 1))
         convertData[i].endY = renderCY;
      else
         convertData[i].endY = ((renderCY / numThreads)*(i + 1)) & 0xFFFFFFFE;
   }

   if (m_setting.colorType == DeviceOutputType_YV12 || m_setting.colorType == DeviceOutputType_I420) {
      for (int i = 0; i < numThreads; i++)
         hConvertThreads[i] = OSCreateThread((XTHREAD)PackPlanarThread, convertData + i);
   }

   if (m_deviceSourceTexture) {
      delete m_deviceSourceTexture;
      m_deviceSourceTexture = NULL;
   }
   if (previousTexture) {
      delete previousTexture;
      previousTexture = NULL;
   }

   //-----------------------------------------------------
   // create the texture regardless, will just show up as red to indicate failure
   // ��������ռ�
   BYTE *textureData = (BYTE*)Allocate(renderCX*renderCY * 4);
   for (int i = 0; i < renderCX*renderCY; i++) {
      *(unsigned int *)&textureData[i * 4] = 0x00000000;
   }

   //---------------RGB------------------
   // DSHOW ��RGB ʵ������BGR
   if (m_setting.colorType == DeviceOutputType_RGB)
      //you may be confused, 
      //but when directshow outputs RGB, it's actually outputting BGR
   {
      m_deviceSourceTexture = CreateTexture(renderCX, renderCY,
                                            //RGB ��ʽ����Ҫ����BGR
                                            GS_BGR,
                                            textureData, FALSE, FALSE);

      if (bSucceeded && deinterlacer.needsPreviousFrame)
         previousTexture = CreateTexture(renderCX, renderCY, GS_BGR, textureData, FALSE, FALSE);
      if (bSucceeded && deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU)
         deinterlacer.texture.reset(
         CreateRenderTarget(deinterlacer.imageCX,
         deinterlacer.imageCY, GS_BGRA, FALSE));
   }
   //else if (m_setting.colorType == DeviceOutputType_RGB32|| m_setting.colorType == DeviceOutputType_ARGB32)
   //{
	  // Log(L"[DeviceSource::ChangeSize] Failed! DeviceOutputType��[%d]", m_setting.colorType);
	  // m_deviceSourceTexture = CreateTexture(renderCX, renderCY,
		 //  //RGB ��ʽ����Ҫ����BGR
		 //  GS_BGRA,
		 //  textureData, FALSE, FALSE);

	  // if (bSucceeded && deinterlacer.needsPreviousFrame)
		 //  previousTexture = CreateTexture(renderCX, renderCY, GS_BGRA, textureData, FALSE, FALSE);
	  // if (bSucceeded && deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU)
		 //  deinterlacer.texture.reset(
			//   CreateRenderTarget(deinterlacer.imageCX,
			//	   deinterlacer.imageCY, GS_BGRA, FALSE));
   //}
   //else if (m_setting.colorType >= DeviceOutputType_Y41P)
   //{
	  // Log(L"[DeviceSource::ChangeSize] Failed! DeviceOutputType��[%d]", m_setting.colorType);
   //}
   //---------------YUV-------------------
   else
      //��YUVƽ���Ϲ�����ʹ��RGB�������
      //if we're working with planar YUV,
      // we can just use regular RGB textures instead
   {
      //��������
      m_deviceSourceTexture = CreateTexture(renderCX, renderCY,
                                            GS_RGB,
                                            textureData, FALSE, FALSE);

      if (bSucceeded && deinterlacer.needsPreviousFrame)
         previousTexture = CreateTexture
         (renderCX, renderCY, GS_RGB, textureData, FALSE, FALSE);

      if (bSucceeded && deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU)
         deinterlacer.texture.reset(
         CreateRenderTarget(deinterlacer.imageCX,
         deinterlacer.imageCY, GS_BGRA, FALSE));
   }

   //����ɹ�����ʹ�ö��߳�ת��
   if (bSucceeded && bUseThreadedConversion) {
      if (m_setting.colorType == DeviceOutputType_I420
          || m_setting.colorType == DeviceOutputType_YV12) {

         LPBYTE lpData;
         if (m_deviceSourceTexture->Map(lpData, texturePitch))
            m_deviceSourceTexture->Unmap();
         else
            texturePitch = renderCX * 4;

         if (!lpImageBuffer || m_imageBufferSize != texturePitch * renderCY) {
            lpImageBuffer = (LPBYTE)Allocate(texturePitch*renderCY);
         }

         m_imageBufferSize = texturePitch*renderCY;
      }
   }

   Free(textureData);

   bFiltersLoaded = bSucceeded;
}

static DWORD STDCALL PackPlanarThread(ConvertData *data) {
   do {
      WaitForSingleObject(data->hSignalConvert, INFINITE);
      if (data->bKillThread) break;

      PackPlanar(data->output, data->input, data->width, data->height,
                 data->pitch,
                 data->startY, data->endY, data->mLinePitch, data->mLineShift);
      data->sample->Release();

      SetEvent(data->hSignalComplete);
   } while (!data->bKillThread);

   return 0;
}
void HDCRender(DeviceColorType type, HWND desHwnd, LPBYTE bytes, int cx, int cy) {
   return;
   char dbg[256] = { 0 };
   HDC hdc = GetDC(desHwnd);
   if (!hdc) {
      return;
   }

   HDC mdc = CreateCompatibleDC(hdc);
   if (!mdc) {
      DeleteDC(hdc);
      return;
   }

   HBITMAP hbmp = CreateCompatibleBitmap(hdc, cx, cy);
   if (!hbmp) {
      DeleteDC(mdc);
      DeleteDC(hdc);
      return;
   }

   RECT desRect;
   if (!GetWindowRect(desHwnd, &desRect)) {
      DeleteObject(hbmp);
      DeleteDC(mdc);
      DeleteDC(hdc);
      return;
   }

   int desWidth = desRect.right - desRect.left;
   int desHeight = desRect.bottom - desRect.top;


   if (cx > 0 && cy > 0 && desWidth > 0 && desHeight > 0) {

      BITMAPINFO binfo;
      ZeroMemory(&binfo, sizeof(BITMAPINFO));
      binfo.bmiHeader.biBitCount = 32;
      binfo.bmiHeader.biCompression = 0;
      binfo.bmiHeader.biHeight = cy;
      binfo.bmiHeader.biPlanes = 1;
      binfo.bmiHeader.biSizeImage = 0;
      binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      binfo.bmiHeader.biWidth = cx;

      SetDIBits(hdc, hbmp, 0, cy, bytes, &binfo, DIB_RGB_COLORS);
      SelectObject(mdc, hbmp);


      HDC mhdc = CreateCompatibleDC(hdc);
      if (mhdc) {
         HBITMAP b2map = CreateCompatibleBitmap(hdc, desWidth, desHeight);
         if (b2map) {
            SelectObject(mhdc, b2map);
            StretchBlt(hdc, 0, 0, desWidth, desHeight, mdc, 0, 0, cx, cy, SRCCOPY);
            BITMAPINFO desBitmapInfo;
            ZeroMemory(&desBitmapInfo, sizeof(BITMAPINFO));
            desBitmapInfo.bmiHeader.biBitCount = 32;
            desBitmapInfo.bmiHeader.biCompression = 0;
            desBitmapInfo.bmiHeader.biHeight = desHeight;
            desBitmapInfo.bmiHeader.biPlanes = 1;
            desBitmapInfo.bmiHeader.biSizeImage = desWidth*desHeight * 4;
            desBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            desBitmapInfo.bmiHeader.biWidth = desWidth;

            unsigned char *data = new unsigned char[desWidth*desHeight * 4];
            if (data) {
               memset(data, 0, desWidth*desHeight * 4);
               int ret;
               ret = GetDIBits(mhdc, b2map, 0, desHeight, data, &desBitmapInfo, DIB_RGB_COLORS);

               for (int y = 0; y < desHeight; y++) {
                  for (int x = 0; x < desWidth; x++) {
                     unsigned char a = data[y*desWidth * 4 + x * 4 + 0];
                     unsigned char r = data[y*desWidth * 4 + x * 4 + 1];
                     unsigned char g = data[y*desWidth * 4 + x * 4 + 2];
                     unsigned char b = data[y*desWidth * 4 + x * 4 + 3];
                     unsigned int rgba = b << 24 | g << 16 | r << 8 | a;
                     *(unsigned int *)&data[(desHeight - y)*desWidth * 4 + x * 4 + 0] = rgba;
                  }
               }


               ret = SetDIBits(mhdc, b2map, 0, desHeight, data, &desBitmapInfo, DIB_RGB_COLORS);
               //sprintf(dbg,"SetDIBits %d\n",ret);OutputDebugStringA(dbg);

               static int index = 0;
               index++;
               if (index == 30) {
                  SaveBitmapToFile(b2map, L"D:\\a.bmp");
               }



               SelectObject(mhdc, b2map);
               // BitBlt(hdc,0,0,desWidth,desHeight,mhdc,0,0,SRCCOPY);


               delete data;
               data = NULL;
            }
            DeleteObject(b2map);
         }
         DeleteDC(mhdc);
      }
   }

   DeleteObject(hbmp);
   DeleteDC(mdc);
   DeleteDC(hdc);
}
//��Ⱦ֮ǰ��Ԥ����
bool DeviceSource::Preprocess() {
   //���û�в����˳�
   if (!bCapturing)
      return true;


   //������������ָ��
   SampleData *lastSample = NULL;
   DShowVideoManagerEnterMutex();
   if (!m_device || !IsDShowVideoFilterExist(this)) {
      m_bDeviceExist = false;
      DShowVideoManagerLeaveMutex();
      return true;
   }

   unsigned char *buffer = NULL;
   unsigned long long  bufferSize = 0;
   unsigned long long  timestamp = 0;

   if (m_device->GetNextVideoBuffer((void **)&buffer, &bufferSize, &timestamp, m_setting)) {
      if (buffer != NULL) {
         lastSample = new SampleData();
         lastSample->lpData = (LPBYTE)malloc(bufferSize);
         memcpy(lastSample->lpData, buffer, bufferSize);
         lastSample->dataLength = bufferSize;
         newCX = lastSample->cx = m_setting.cx;
         newCY = lastSample->cy = m_setting.cy;
         lastSample->timestamp = timestamp;
         m_LastSyncTime = GetQPCTimeMS();
      }
      m_device->ReleaseVideoBuffer();
   }

   //���CPU����
   int numThreads = MAX(OSGetTotalCores() - 2, 1);

   if (lastSample) {
      //��������
      if (previousTexture) {
         Texture *tmp = m_deviceSourceTexture;
         m_deviceSourceTexture = previousTexture;
         previousTexture = tmp;
      }

      //��ǰ��
      deinterlacer.curField =
         //ȥ��������ΪGPU
         deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU ?
         //
         false :
         //TFF ���� \ BFF  �׳�

         //����ΪBFF
         (deinterlacer.fieldOrder == FIELD_ORDER_BFF);

      deinterlacer.bNewFrame = true;

      //-------------------ɫ�ʿռ�ת��------------------
      //֮��ͼ���Ƶ�texture��
      //ɫ�ʿռ�ΪRGB
	  //Log(L"DeviceSource::Preprocess(): 1036 DeviceOutputType: %d", m_setting.colorType);

       //if (m_setting.colorType == DeviceOutputType_RGB24) {
	  if (m_setting.colorType == DeviceOutputType_RGB) {
         if (m_deviceSourceTexture && lastSample != NULL && lastSample->lpData != NULL) {
            ChangeSize();
            HDCRender(m_setting.colorType, m_renderHWND, lastSample->lpData, renderCX, renderCY);
            if (m_deviceSourceTexture && lastSample && lastSample->lpData) {
				double dataVerify = (1.0*lastSample->dataLength) / (lastSample->cx*lastSample->cy);
				if (4.0 == dataVerify)
				{
					//Log(L"DeviceSource::Preprocess(): SUCC( 4.0 ): timestamp  %I64d, dataLength:%ld cx:%d,cy:%d", 
						//lastSample->timestamp,  lastSample->dataLength, lastSample->cx, lastSample->cy);
					m_deviceSourceTexture->SetImage(lastSample->lpData, GS_IMAGEFORMAT_BGRX, linePitch);
				}
				else if (3.0 == dataVerify)
				{
					//Log(L"DeviceSource::Preprocess(): SUCC( 3.0 ): timestamp  %I64d,  dataLength:%ld cx:%d,cy:%d", 
						//lastSample->timestamp, lastSample->dataLength, lastSample->cx, lastSample->cy);
					m_deviceSourceTexture->SetImage(lastSample->lpData, GS_IMAGEFORMAT_BGR, linePitch);
				}
				else
				{
					//Log(L"DeviceSource::Preprocess(): ERROR: timestamp  %I64d,  dataLength:%ld cx:%d,cy:%d", 
					//	lastSample->timestamp, lastSample->dataLength, lastSample->cx, lastSample->cy);
				}
            }
            bReadyToDraw = true;
         }
      }
	  /*else if (m_setting.colorType == DeviceOutputType_RGB32) {
		  if (m_deviceSourceTexture && lastSample != NULL && lastSample->lpData != NULL) {
			  ChangeSize();
			  HDCRender(m_setting.colorType, m_renderHWND, lastSample->lpData, renderCX, renderCY);
			  if (m_deviceSourceTexture && lastSample && lastSample->lpData) {
				  m_deviceSourceTexture->SetImage(lastSample->lpData, GS_IMAGEFORMAT_BGRX, linePitch);
			  }
			  bReadyToDraw = true;
		  }
	  }
	  else if (m_setting.colorType == DeviceOutputType_ARGB32) {
		  if (m_deviceSourceTexture && lastSample != NULL && lastSample->lpData != NULL) {
			  ChangeSize();
			  HDCRender(m_setting.colorType, m_renderHWND, lastSample->lpData, renderCX, renderCY);
			  if (m_deviceSourceTexture && lastSample && lastSample->lpData) {
				  m_deviceSourceTexture->SetImage(lastSample->lpData, GS_IMAGEFORMAT_BGRA, linePitch);
			  }
			  bReadyToDraw = true;
		  }
	  }*/
      //ɫ�ʿռ�ΪYUV420
      else if (m_setting.colorType == DeviceOutputType_I420 || m_setting.colorType == DeviceOutputType_YV12) {
         //���߳�ת��
         if (bUseThreadedConversion) {
            //
            if (!bFirstFrame) {
               List<HANDLE> events;
               for (int i = 0; i < numThreads; i++) {
                  ConvertData data = convertData[i];
                  events << data.hSignalComplete;
               }

               WaitForMultipleObjects(numThreads, events.Array(), TRUE, INFINITE);

               if (m_deviceSourceTexture)
                  m_deviceSourceTexture->SetImage(lpImageBuffer, GS_IMAGEFORMAT_RGBX, texturePitch);

               bReadyToDraw = true;
            } else
               bFirstFrame = false;

            ChangeSize();

            for (int i = 0; i < numThreads; i++)
               lastSample->AddRef();

            for (int i = 0; i < numThreads; i++) {
               convertData[i].input = lastSample->lpData;
               convertData[i].sample = lastSample;
               convertData[i].pitch = texturePitch;
               convertData[i].output = lpImageBuffer;
               convertData[i].mLinePitch = linePitch;
               convertData[i].mLineShift = lineShift;
               SetEvent(convertData[i].hSignalConvert);
            }
         } else {
            LPBYTE lpData;
            UINT pitch;

            ChangeSize();

            if (m_deviceSourceTexture->Map(lpData, pitch)) {
               //����ѧ
               PackPlanar(lpData, lastSample->lpData, renderCX, renderCY, pitch, 0, renderCY, linePitch, lineShift);
               m_deviceSourceTexture->Unmap();
            }

			//Log(L"DeviceSource::Preprocess(): YUY2: timestamp %I64d,dataLength:%ld cx:%d, cy:%d, linePitch:%ud, lineShift: %ud", lastSample->timestamp,
			//	lastSample->dataLength, lastSample->cx, lastSample->cy, linePitch, lineShift);

            bReadyToDraw = true;
         }
      } else if (m_setting.colorType == DeviceOutputType_YVYU
                 || m_setting.colorType == DeviceOutputType_YUY2) {
         LPBYTE lpData;
         UINT pitch;

         ChangeSize();

         if (m_deviceSourceTexture->Map(lpData, pitch)) {
            Convert422To444(lpData, lastSample->lpData, pitch, true);
            HDCRender(m_setting.colorType, m_renderHWND, lpData, renderCX, renderCY);
            m_deviceSourceTexture->Unmap();
         }

         bReadyToDraw = true;
      } else if (m_setting.colorType == DeviceOutputType_UYVY
                 || m_setting.colorType == DeviceOutputType_HDYC) {
         LPBYTE lpData;
         UINT pitch;

         ChangeSize();

         if (m_deviceSourceTexture->Map(lpData, pitch)) {
            Convert422To444(lpData, lastSample->lpData, pitch, false);
            m_deviceSourceTexture->Unmap();
         }

         bReadyToDraw = true;
      }
	  else /*if(m_setting.colorType == DeviceOutputType_UNKNOWN)*/
	  {
		  //DeviceSource::ChangeSize;
		  //Log(L"DeviceSource::Preprocess: DeviceOutputType: %d", m_setting.colorType);
	  }
      //if (lastSample) {
      //    delete lastSample;
      //    lastSample = nullptr;
      //}
      lastSample->Release();

      //------------------------ɫ�ʿռ�ת�����------------------------------
      //-------------------------------��������Ƶ�ȥ����������-------------------
      //�Ѿ���ͼ������д�뵽������
      if (
         //׼���û���
         bReadyToDraw &&
         //ȥ�������Ͳ�Ϊ��
         deinterlacer.type != DEINTERLACING_NONE &&
         //ȥ������ΪGPU����
         deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU &&
         //ȥ��������Ϊ��
         deinterlacer.texture.get() &&
         //ȥ����������ɫ����Ϊ��
         deinterlacer.pixelShader.Shader()) {

         //-------------------------������ȾĿ��Ϊȥ�����Ŀ��---------------------------
         SetRenderTarget(deinterlacer.texture.get());

         //��õ�ǰ�Ķ�����ɫ��
         Shader *oldVertShader = GetCurrentVertexShader();
         //����ȥ�����ڵĶ�����ɫ��
         LoadVertexShader(deinterlacer.vertexShader.get());
         //��õ�ǰ��������ɫ��
         Shader *oldShader = GetCurrentPixelShader();
         //����ȥ�����ڵ�������ɫ��
         LoadPixelShader(deinterlacer.pixelShader.Shader());

         Ortho(0.0f, float(deinterlacer.imageCX), float(deinterlacer.imageCY), 0.0f, -100.0f, 100.0f);

         SetViewport(0.0f, 0.0f, float(deinterlacer.imageCX), float(deinterlacer.imageCY));

         if (previousTexture)
            LoadTexture(previousTexture, 1);
         //-------------------��texture���Ƶ�ȥ����������---------------------
         if (m_deviceSourceTexture) {
            DrawSpriteEx(m_deviceSourceTexture, 0xFFFFFFFF, 0.0f, 0.0f,
                         float(deinterlacer.imageCX),
                         float(deinterlacer.imageCY),
                         0.0f, 0.0f, 1.0f, 1.0f);
         }

         if (previousTexture)
            LoadTexture(nullptr, 1);

         LoadPixelShader(oldShader);
         LoadVertexShader(oldVertShader);
         deinterlacer.isReady = true;
      }
      m_device->ReleaseVideoBuffer();
   }

   DShowVideoManagerLeaveMutex();
   return true;
}


void DeviceSource::Render(const Vect2 &pos, const Vect2 &size, SceneRenderType renderType) {
   if (m_blackBackgroundTexture&&size.x > 0 && size.y > 0 && SceneRenderType::SceneRenderType_Preview == renderType) {
      float x, x2;
      x = pos.x;
      x2 = x + size.x;

      float y = pos.y,
         y2 = y + size.y;

      DrawSprite(m_blackBackgroundTexture, 0xFFFFFFFF, x, y, x2, y2);
   }
   QWORD currentTime = GetQPCTimeMS();
   if (m_LastSyncTime == 0) {
      m_LastSyncTime = currentTime;
   }

   bool isHasNoData = (currentTime - m_LastSyncTime) > 3000;
   if (m_bIsHasNoData != isHasNoData) {
      m_bIsHasNoData = isHasNoData;
      //if (m_bIsHasNoData) {
      //   //Log(L"DeviceSource::Render NoData currentTime [%lu]", currentTime);
      //   //Log(L"DeviceSource::Render NoData m_LastSyncTime [%lu]", m_LastSyncTime);
      //} else {
      //   //Log(L"DeviceSource::Render HasData currentTime [%lu]", currentTime);
      //   //Log(L"DeviceSource::Render HasData m_LastSyncTime [%lu]", m_LastSyncTime);
      //}
   }

   if (SceneRenderType::SceneRenderType_Desktop == renderType&&IsWindowVisible(m_renderHWND)) {
   
      QWORD currentTime = GetQPCTimeMS();
      if (currentTime - m_LastTickTime > 100) {
         m_LastTickTime = currentTime;
         Shader *currentPixelShader = NULL;
         //��ɫת����ɫ��
         if (colorConvertShader) {
            currentPixelShader = GetCurrentPixelShader();
            //������ɫת����ɫ��
            LoadPixelShader(colorConvertShader);
            //����ɫ������٤��ֵ
            colorConvertShader->SetFloat
               (colorConvertShader->GetParameterByName(TEXT("gamma")), 1.0f);

            //
            float mat[16];
            bool actuallyUse709 = !!m_setting.use709;

            if (actuallyUse709)
               memcpy(mat, yuvToRGB709[0], sizeof(float)* 16);
            else
               memcpy(mat, yuvToRGB601[0], sizeof(float)* 16);

            //����YUV MAT
            colorConvertShader->SetValue(
               colorConvertShader->GetParameterByName(TEXT("yuvMat")),
               mat, sizeof(float)* 16);
         }

         bool bFlip = false;
         if (m_setting.colorType != DeviceOutputType_RGB)
		 //if (m_setting.colorType>DeviceOutputType_ARGB32 /*&& m_setting.colorType<DeviceOutputType_Y41P*/)
            bFlip = !bFlip;

         DWORD opacity255 = 0xFFFFFF;

         Texture *tex =
            //ȥ����ʹ��GPU
            (deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU
            //ȥ��������ǿ�
            && deinterlacer.texture.get()) ?
            //ȥ��������
            deinterlacer.texture.get() :
            //��ǰ������
            m_deviceSourceTexture;


         OBSGraphics_Present();
         OBSGraphics_Flush();

         OBSAPI_EnterSceneMutex();
         if (m_renderView != NULL) {
            OBSGraphics_DestoryRenderTargetView(m_renderView);
            m_renderView = NULL;
         }

         m_renderView = OBSGraphics_CreateRenderTargetView(m_renderHWND, 0, 0);
         OBSGraphics_SetRenderTargetView(m_renderView);
         OBSAPI_LeaveSceneMutex();


         EnableBlending(FALSE);
         ClearColorBuffer(0x000000);

         RECT rect;
         GetClientRect(m_renderHWND, &rect);
         float w = rect.right - rect.left;
         float h = rect.bottom - rect.top;
         float k = w / h;

         float w2 = renderCX;
         float h2 = renderCY;
         float k2 = w2 / h2;

         Ortho(0.0f, w, h, 0.0f, -100.0f, 100.0f);
         SetViewport(0.0f, 0.0f, w, h);
         float x, x2, y, y2;

         if (k > k2) {
            x = 0;
            y = 0 - (w / k2 - h) / 2;
            x2 = x + w;
            y2 = y + h + (w / k2 - h);
         } else {
            y = 0;
            x = 0 - (h*k2 - w) / 2;
            y2 = y + h;
            x2 = x + w + (h*k2 - w);
         }

         if (!bFlip) {
            float tmp_y = y2;
            y2 = y;
            y = tmp_y;
         }

         //ȥ����˫��֡��
         if (deinterlacer.doublesFramerate) {
            //��������
            if (!deinterlacer.curField)
               DrawSpriteEx(tex, (opacity255 << 24) | 0xFFFFFF, x, y, x2, y2, 0.f, 0.0f, .5f, 1.f);
            //����
            else
               DrawSpriteEx(tex, (opacity255 << 24) | 0xFFFFFF, x, y, x2, y2, .5f, 0.0f, 1.f, 1.f);
         }
         //������������
         else
            DrawSprite(tex, (opacity255 << 24) | 0xFFFFFF, x, y, x2, y2);
         if (currentPixelShader) {
            LoadPixelShader(currentPixelShader);
         }
         OBSGraphics_Present();
         OBSGraphics_Flush();
         OBSGraphics_SetRenderTargetView(NULL);
      }

   }

   if (tipTexture&&!isHasNoData&&SceneRenderType::SceneRenderType_Preview == renderType) {
      if (size.x != 0 && size.y != 0 && tipTexture->Width() != 0 && tipTexture->Height() != 0) {
         int x, y, w, h;
         if (tipTexture->Width() <= size.x&&tipTexture->Height() <= size.y) {
            w = tipTexture->Width();
            h = tipTexture->Height();
            x = int(size.x - w);
            x /= 2;
            y = size.y - h;
            y /= 2;

            x += pos.x;
            y += pos.y;
         } else {
            float ktip = tipTexture->Width();
            ktip /= tipTexture->Height();
            float ksize = size.x;
            ksize /= size.y;
            if (ktip > ksize) {
               w = size.x;
               h = w / ktip;
               x = pos.x;
               y = pos.y + (size.y - h) / 2;
            } else {
               h = size.y;
               w = h*ktip;
               y = pos.y;
               x = pos.x + (size.x - w) / 2;
            }
         }

         DrawSprite(tipTexture, 0xFFFFFFFF, x, y, x + w, y + h);
      }
   }

   //-------------------------��Ⱦ---------------------------------
   //����Ϊ��
   if (m_deviceSourceTexture
       //׼����ȥ����
       && bReadyToDraw
       //ȥ�������
       && deinterlacer.isReady) {

      //��õ�ǰ��������ɫ��
      Shader *oldShader = GetCurrentPixelShader();
      SamplerState *sampler = NULL;

      //��ɫת����ɫ��
      if (colorConvertShader) {
         //������ɫת����ɫ��
         LoadPixelShader(colorConvertShader);
         //����ɫ������٤��ֵ
         colorConvertShader->SetFloat
            (colorConvertShader->GetParameterByName(TEXT("gamma")), 1.0f);

         //
         float mat[16];
         bool actuallyUse709 = !!m_setting.use709;

         if (actuallyUse709)
            memcpy(mat, yuvToRGB709[0], sizeof(float)* 16);
         else
            memcpy(mat, yuvToRGB601[0], sizeof(float)* 16);

         //����YUV MAT
         colorConvertShader->SetValue(
            colorConvertShader->GetParameterByName(TEXT("yuvMat")),
            mat, sizeof(float)* 16);
      }
      bool bFlip = false;
      if (m_setting.colorType != DeviceOutputType_RGB)
	  //if (m_setting.colorType > DeviceOutputType_ARGB32 /*&& m_setting.colorType < DeviceOutputType_Y41P*/)
         bFlip = !bFlip;

      //��ʼ�ͽ�������
      float x, x2;
      x = pos.x;
      x2 = x + size.x;

      float y = pos.y,
         y2 = y + size.y;
      if (!bFlip) {
         y2 = pos.y;
         y = y2 + size.y;
      }


      DWORD opacity255 = 0xFFFFFF;

      Texture *tex =
         //ȥ����ʹ��GPU
         (deinterlacer.processor == DEINTERLACING_PROCESSOR_GPU
         //ȥ��������ǿ�
         && deinterlacer.texture.get()) ?
         //ȥ��������
         deinterlacer.texture.get() :
         //��ǰ������
         m_deviceSourceTexture;

      //ȥ����˫��֡��
      if (deinterlacer.doublesFramerate) {
         //��������
         if (!deinterlacer.curField)
            DrawSpriteEx(tex, (opacity255 << 24) | 0xFFFFFF, x, y, x2, y2, 0.f, 0.0f, .5f, 1.f);
         //����
         else
            DrawSpriteEx(tex, (opacity255 << 24) | 0xFFFFFF, x, y, x2, y2, .5f, 0.0f, 1.f, 1.f);
      }
      //������������
      else
         DrawSprite(tex, (opacity255 << 24) | 0xFFFFFF, x, y, x2, y2);

      //ȥ������Ҫ�µ�һ֡
      if (deinterlacer.bNewFrame) {
         deinterlacer.curField = !deinterlacer.curField;
         deinterlacer.bNewFrame = false;
         //prevent switching from the second field to the first field
      }

      if (colorConvertShader)
         LoadPixelShader(oldShader);
   }

   if (!m_bDeviceExist&&m_disconnectTexture&&SceneRenderType::SceneRenderType_Preview == renderType) {
      //��ʼ�ͽ�������
      float x, x2;
      x = pos.x;
      x2 = x + size.x;

      float y = pos.y,
         y2 = y + size.y;
      bool bFlip = false;
      if (m_setting.colorType != DeviceOutputType_RGB)
	  //if (m_setting.colorType > DeviceOutputType_ARGB32 /*&& m_setting.colorType < DeviceOutputType_Y41P*/)
         bFlip = !bFlip;


      if (!bFlip) {
         y2 = pos.y;
         y = y2 + size.y;
      }


      int tx;
      int ty;
      int tw;
      int th;


      int cx = x;
      int cy = y;
      int cw = (x2 - x) / 2;
      int ch = (y2 - y) / 2;
      int ow = size.x;
      int oh = size.y;

      if (ow < m_disconnectTexture->Width() || oh<m_disconnectTexture->Height()) {
         float texK = cw;
         texK /= ch;
         float tipK = m_disconnectTexture->Width();
         tipK /= m_disconnectTexture->Height();
         if (texK>tipK) {
            th = ch;
            tw = ch*tipK;
         } else {
            tw = cw;
            if (tipK != 0) {
               th = tw / tipK;
            } else {
               th = m_disconnectTexture->Height();
            }
         }

         tx = (cw * 2 - tw) / 2 + cx;
         ty = (ch * 2 - th) / 2 + cy;

      } else {
         float sizeK = size.x;
         sizeK /= ow;
         sizeK *= m_disconnectTexture->Width();
         tw = sizeK;


         sizeK = size.y;
         sizeK /= oh;
         sizeK *= m_disconnectTexture->Height();
         th = sizeK;

         tx = (size.x - tw) / 2 + pos.x;
         ty = (size.y - th) / 2 + pos.y;
      }

      DrawSprite(m_disconnectTexture, 0xFFFFFFFF, tx, ty, tw + tx, th + ty);
   } else if (isHasNoData&&SceneRenderType::SceneRenderType_Preview == renderType) {
      int imgx = 0, imgy = 0, imgw = 0, imgh = 0;
      if (size.x != 0 && size.y != 0 && m_failedTexture->Width() != 0 && m_failedTexture->Height() != 0) {

         if (m_failedTexture->Width() <= size.x&&m_failedTexture->Height() <= size.y) {
            imgw = m_failedTexture->Width();
            imgh = m_failedTexture->Height();
            imgx = size.x - imgw;
            imgx /= 2;
            imgy = size.y - imgh;
            imgy /= 2;

            imgx += pos.x;
            imgy += pos.y;
         } else {
            float ktip = m_failedTexture->Width();
            ktip /= m_failedTexture->Height();
            float ksize = size.x;
            ksize /= size.y;
            if (ktip > ksize) {
               imgw = size.x;
               imgh = imgw / ktip;
               imgx = pos.x;
               imgy = pos.y + (size.y - imgh) / 2;
            } else {
               imgh = size.y;
               imgw = imgh*ktip;
               imgy = pos.y;
               imgx = pos.x + (size.x - imgw) / 2;
            }
         }

         DrawSprite(m_failedTexture, 0xFFFFFFFF, imgx, imgy, imgx + imgw, imgy + imgh);
      }

      if (m_deviceNameTexture&&imgw != 0 && imgh != 0) {
         int x, y, w, h;
         if (size.x <= imgw * 3) {
            w = size.x;
         } else {
            w = imgw * 3;
         }

         float ktip = m_deviceNameTexture->Width();
         ktip /= m_deviceNameTexture->Height();

         h = w / ktip;


         x = imgx + (imgw - w) / 2;
         y = imgy + imgh;


         DrawSprite(m_deviceNameTexture, 0xFFFFFFFF, x, y, x + w, y + h);
      }
   }
}

void DeviceSource::UpdateSettings() {
   OBSAPI_EnterSceneMutex();

   bool bWasCapturing = bCapturing;
   if (bWasCapturing)
      Stop();

   UnloadFilters();
   LoadFilters();

   if (bWasCapturing)
      Start();

   OBSAPI_LeaveSceneMutex();
}
void DeviceSource::Reload() {
   UpdateSettings();
}

void DeviceSource::Convert422To444(LPBYTE convertBuffer, LPBYTE lp422, UINT pitch, bool bLeadingY) {
   DWORD size = lineSize;
   DWORD dwDWSize = size >> 2;

   if (bLeadingY) {
      for (UINT y = 0; y < renderCY; y++) {
         LPDWORD output = (LPDWORD)(convertBuffer + (y*pitch));
         LPDWORD inputDW = (LPDWORD)(lp422 + (y*linePitch) + lineShift);
         LPDWORD inputDWEnd = inputDW + dwDWSize;

         while (inputDW < inputDWEnd) {
            register DWORD dw = *inputDW;

            output[0] = dw;
            dw &= 0xFFFFFF00;
            dw |= BYTE(dw >> 16);
            output[1] = dw;

            output += 2;
            inputDW++;
         }
      }
   } else {
      for (UINT y = 0; y < renderCY; y++) {
         LPDWORD output = (LPDWORD)(convertBuffer + (y*pitch));
         LPDWORD inputDW = (LPDWORD)(lp422 + (y*linePitch) + lineShift);
         LPDWORD inputDWEnd = inputDW + dwDWSize;

         while (inputDW < inputDWEnd) {
            register DWORD dw = *inputDW;

            output[0] = dw;
            dw &= 0xFFFF00FF;
            dw |= (dw >> 16) & 0xFF00;
            output[1] = dw;

            output += 2;
            inputDW++;
         }
      }
   }
}
//����DSHOW Video Filter
ImageSource * STDCALL CreateDShowVideoFilterVideoSource(XElement *data) {
   DeviceSource *source = new DeviceSource();
   Log(L"CreateDShowVideoFilterVideoSource %p", source);
   if (source) {
      if (!source->Init(data)) {
         delete source;
         return NULL;
      }
   }
   return source;
}

