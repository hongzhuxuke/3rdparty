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


#include "Main.h"
#include <dwmapi.h>
#include <list>
#include <vector>

#include "VHMonitorCapture.h"
using namespace std;

#define NUM_CAPTURE_TEXTURES 2

class DesktopImageSource : public ImageSource {
   Texture *renderTextures[NUM_CAPTURE_TEXTURES];
   Texture *lastRendered = nullptr;
   Texture *mClientImageTexture = nullptr;
   Shader   *colorKeyShader = nullptr, *alphaIgnoreShader = nullptr;
   int      curCaptureTexture = 0;
   XElement *data = nullptr;

   bool     bUseColorKey = false, bUsePointFiltering = false;
   DWORD    keyColor;
   UINT     keySimilarity, keyBlend;

   UINT     opacity;
   int      gamma;
   VHD_WindowInfo m_windowInfo;
   float m_width;
   float m_height;

public:
   DesktopImageSource(UINT frameTime, XElement *data) {
      for (UINT i = 0; i < NUM_CAPTURE_TEXTURES; i++)
         renderTextures[i] = NULL;

      this->data = data;

      UpdateSettings();
      curCaptureTexture = 0;
      colorKeyShader = CreatePixelShaderFromFile(TEXT("shaders\\ColorKey_RGB.pShader"));
      alphaIgnoreShader = CreatePixelShaderFromFile(TEXT("shaders\\AlphaIgnore.pShader"));

      String clientImage = OBSAPI_GetAppPath();
      clientImage += TEXT("\\screen.png");
      mClientImageTexture =CreateTextureFromFile(clientImage, TRUE);
   }

   ~DesktopImageSource() {      
       for (int i = 0; i < NUM_CAPTURE_TEXTURES; i++) {
           SafeDelete(renderTextures[i]);
       }
      SafeDelete(alphaIgnoreShader);
      SafeDelete(colorKeyShader);
      SafeDelete(mClientImageTexture);
   }
   HDC GetCurrentHDC() {
      HDC hDC = NULL;

    Texture *captureTexture = renderTextures[curCaptureTexture];
    if (captureTexture)
         captureTexture->GetDC(hDC);

      return hDC;
   }

   void ReleaseCurrentHDC(HDC hDC) {
      Texture *captureTexture = renderTextures[curCaptureTexture];
      if(captureTexture) {
         captureTexture->ReleaseDC();
      }
   }

   void EndPreprocess() {
      lastRendered = renderTextures[curCaptureTexture];
      if (++curCaptureTexture == NUM_CAPTURE_TEXTURES)
         curCaptureTexture = 0;
   }

   bool Preprocess() {
      if(m_windowInfo.type == VHD_Desktop) {
         float newW = GetSystemMetrics(SM_CXSCREEN);
         float newH = GetSystemMetrics(SM_CYSCREEN);
         if(m_width != newW || m_height != newH)
         {
            UpdateSettings();
            EndPreprocess();
            return true;
         }
      }
      
      HDC hDC;
      if (hDC = GetCurrentHDC()) {
         if(!VHD_Render(m_windowInfo,hDC,m_width,m_height,true)){            
            ReleaseCurrentHDC(hDC);
            EndPreprocess();
            return false;
         }
         ReleaseCurrentHDC(hDC);
         if((m_windowInfo.rect.right-m_windowInfo.rect.left!=(int)m_width)||
            (m_windowInfo.rect.bottom - m_windowInfo.rect.top != (int)m_height)) {
            data->SetHexListPtr(L"winInfo", (unsigned char *)& m_windowInfo, sizeof(VHD_WindowInfo));
            UpdateSettings();
            EndPreprocess();
            return true;
         }
      } 
      if(m_windowInfo.type==VHD_Window&&IsIconic(m_windowInfo.hwnd)) {
         return true;
      }
      EndPreprocess();
      return true;
   }

   void Render(const Vect2 &pos, const Vect2 &size,SceneRenderType renderType) {
      SamplerState *sampler = NULL;
      Vect2 ulCoord = Vect2(0.0f, 0.0f),lrCoord = Vect2(1.0f, 1.0f);
      if(SceneRenderType::SceneRenderType_Desktop == renderType) {
         return ;
      }

      if(renderType==SceneRenderType::SceneRenderType_Preview&&m_windowInfo.type==VHD_Desktop){
#ifdef SHOW_IMAGE_MODE
         if (mClientImageTexture != NULL) {
            DrawSpriteEx(mClientImageTexture, 0xFFFFFFFF,
               pos.x,
               pos.y,
               pos.x + size.x,
               pos.y + size.y,
               0,
               0,
               1,
               1);
         }
#else
      if (mClientImageTexture != NULL) {
         DrawSpriteEx(lastRendered, 0xFFFFFFFF,
            pos.x,
            pos.y,
            pos.x + size.x,
            pos.y + size.y,
            0,
            0,
            1,
            1);
      }
#endif // DEBUG
         return ;
      }


      if (lastRendered) {
         float fGamma = float(-(gamma - 100) + 100) * 0.01f;
         Shader *lastPixelShader = GetCurrentPixelShader();
         float fOpacity = float(opacity)*0.01f;
         DWORD opacity255 = DWORD(fOpacity*255.0f);
         if (bUseColorKey) {
            LoadPixelShader(colorKeyShader);
            HANDLE hGamma = colorKeyShader->GetParameterByName(TEXT("gamma"));
            if (hGamma)
               colorKeyShader->SetFloat(hGamma, fGamma);

            float fSimilarity = float(keySimilarity)*0.01f;
            float fBlend = float(keyBlend)*0.01f;

            colorKeyShader->SetColor(colorKeyShader->GetParameter(2), keyColor);
            colorKeyShader->SetFloat(colorKeyShader->GetParameter(3), fSimilarity);
            colorKeyShader->SetFloat(colorKeyShader->GetParameter(4), fBlend);

         } else {
            LoadPixelShader(alphaIgnoreShader);
            HANDLE hGamma = alphaIgnoreShader->GetParameterByName(TEXT("gamma"));
            if (hGamma)
               alphaIgnoreShader->SetFloat(hGamma, fGamma);
         }

         if (bUsePointFiltering) {
            SamplerInfo samplerinfo;
            samplerinfo.filter = GS_FILTER_POINT;
            sampler = CreateSamplerState(samplerinfo);
            LoadSamplerState(sampler, 0);
         }
         DrawSpriteExRotate(lastRendered, (opacity255 << 24) | 0xFFFFFF,
            pos.x, pos.y, pos.x + size.x, pos.y + size.y, 0.0f,
            ulCoord.x, ulCoord.y,
            lrCoord.x, lrCoord.y, 0.0f); 
         if (bUsePointFiltering) 
             delete(sampler); 

         LoadPixelShader(lastPixelShader);
      }
   }

   Vect2 GetSize() const {      
      return Vect2(float(m_width), float(m_height));
   }

   void UpdateSettings() {
      OutputDebugStringA("################### DesktopImageSource UpdateSettings ###################\n");
      OBSAPI_EnterSceneMutex();      
      data->GetHexListPtr(L"winInfo", (unsigned char *)& m_windowInfo, sizeof(VHD_WindowInfo));
      gamma = 100;
      bool bNewUseColorKey = data->GetInt(TEXT("useColorKey"), 0) != 0;
      keyColor = data->GetInt(TEXT("keyColor"), 0xFFFFFFFF);
      keySimilarity = data->GetInt(TEXT("keySimilarity"), 10);
      keyBlend = data->GetInt(TEXT("keyBlend"), 0);
      bUsePointFiltering = data->GetInt(TEXT("usePointFiltering"), 0) != 0;
      bUseColorKey = bNewUseColorKey;
      opacity = data->GetInt(TEXT("opacity"), 100);

      //only first screen
      if(m_windowInfo.type==VHD_Desktop) {
         m_windowInfo.rect.right=GetSystemMetrics(SM_CXSCREEN)+m_windowInfo.rect.left;
         m_windowInfo.rect.bottom=GetSystemMetrics(SM_CYSCREEN)+m_windowInfo.rect.top;
      }   
      
      m_width = m_windowInfo.rect.right - m_windowInfo.rect.left;
      m_height = m_windowInfo.rect.bottom - m_windowInfo.rect.top;

      OBSApiLog("DesktopImageSource UpdateSetting CreateGDITexture m_width:%d  m_height:%d\n", m_width, m_height);

      for (UINT i = 0; i < NUM_CAPTURE_TEXTURES; i++) {
         if(renderTextures[i]!=NULL) {
            delete renderTextures[i];
            renderTextures[i]=NULL;
         }
         Texture *tex=CreateGDITexture(m_width, m_height);
         if(tex == NULL) {            
            OBSApiLog("DesktopImageSource UpdateSetting CreateGDITexture Failed! %d %d\n",m_width, m_height);
            continue;
         }
         renderTextures[i] = tex;
         
         HDC hdc=NULL;
         renderTextures[i]->GetDC(hdc);
         if(hdc) {
            HBRUSH hbrush = CreateSolidBrush(RGB(0,0,0));
            SelectObject(hdc, hbrush);
            Rectangle(hdc,0,0, m_width, m_height);
            DeleteObject(hbrush);
            renderTextures[i]->ReleaseDC();
         }
      }
      
      for (UINT i = 0; i < NUM_CAPTURE_TEXTURES; i++) {
         Preprocess();
         EndPreprocess();
      }
      OBSAPI_LeaveSceneMutex();

      if(VHD_Desktop==m_windowInfo.type) {
         OBSApiLog("DesktopImageSource UpdateSetting [VHD_Desktop] %d %d\n",VHD_Desktop,m_windowInfo.type);
      }
      else if(VHD_Window==m_windowInfo.type){
         OBSApiLog("DesktopImageSource UpdateSetting [VHD_Window] %d %d\n",VHD_Window,m_windowInfo.type);
         OBSApiLog("DesktopImageSource UpdateSetting [VHD_Window]size %d %d %d %d\n",
         m_windowInfo.rect.right ,m_windowInfo.rect.left, m_windowInfo.rect.bottom ,m_windowInfo.rect.top);
      }
      else if(VHD_AREA==m_windowInfo.type){
         OBSApiLog("DesktopImageSource UpdateSetting [VHD_AREA] %d %d\n",VHD_AREA,m_windowInfo.type);
      }
      else {
         OBSApiLog("DesktopImageSource UpdateSetting %d\n",m_windowInfo.type);
      } 
   }

   void SetInt(CTSTR lpName, int iVal) {
      if (scmpi(lpName, TEXT("useColorKey")) == 0) {
         bool bNewVal = iVal != 0;
         bUseColorKey = bNewVal;
      } else if (scmpi(lpName, TEXT("keyColor")) == 0) {
         keyColor = (DWORD)iVal;
      } else if (scmpi(lpName, TEXT("keySimilarity")) == 0) {
         keySimilarity = iVal;
      } else if (scmpi(lpName, TEXT("keyBlend")) == 0) {
         keyBlend = iVal;
      } else if (scmpi(lpName, TEXT("opacity")) == 0) {
         opacity = (UINT)iVal;
      } else if (scmpi(lpName, TEXT("gamma")) == 0) {
         gamma = iVal;
         if (gamma < 50)        gamma = 50;
         else if (gamma > 175)  gamma = 175;
      }
   }
};

ImageSource* STDCALL CreateDesktopSource(XElement *data) {
   if (!data)
      return NULL;

   return new DesktopImageSource(OBSAPI_GetFrameTime(), data);
}

static bool STDCALL ConfigureDesktopSource2(XElement *element, bool bInitialize, int dialogID) {
   return true;
}

bool STDCALL ConfigureDesktopSource(XElement *element, bool bInitialize) {
   return ConfigureDesktopSource2(element, bInitialize, IDD_CONFIGUREDESKTOPSOURCE);
}

bool STDCALL ConfigureWindowCaptureSource(XElement *element, bool bInitialize) {
   return ConfigureDesktopSource2(element, bInitialize, IDD_CONFIGUREWINDOWCAPTURE);
}

bool STDCALL ConfigureMonitorCaptureSource(XElement *element, bool bInitialize) {
   return ConfigureDesktopSource2(element, bInitialize, IDD_CONFIGUREMONITORCAPTURE);
}
