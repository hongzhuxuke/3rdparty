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


#pragma once

#include <memory>
#include <atomic>
void PackPlanar(LPBYTE convertBuffer, LPBYTE lpPlanar, UINT renderCX, UINT renderCY, UINT pitch, UINT startY, UINT endY, UINT linePitch, UINT lineShift);


class DeviceSource;
class DeviceSource : public ImageSource
{
    UINT enteredSceneCount;

    //---------------------------------
    bool            m_bIsHasNoData;
    bool            m_bDeviceExist;
    DeviceInfo      deviceInfo;
    IDShowVideoFilterDevice* m_device = NULL;
    VideoDeviceSetting    m_setting;
    UINT            renderCX, renderCY;
    UINT            newCX, newCY;
    UINT            imageCX, imageCY;
    UINT            linePitch, lineShift, lineSize;
    BOOL            fullRange;

    struct {
        int                         type; //DeinterlacingType
        char                        fieldOrder; //DeinterlacingFieldOrder
        char                        processor; //DeinterlacingProcessor
        bool                        curField, bNewFrame;
        bool                        doublesFramerate;
        bool                        needsPreviousFrame;
        bool                        isReady;
        std::unique_ptr<Texture>    texture;
        UINT                        imageCX, imageCY;
        std::unique_ptr<Shader>     vertexShader;
        FuturePixelShader           pixelShader;
    } deinterlacer;

    bool            bFirstFrame;
    bool            bUseThreadedConversion;
    std::atomic_bool bReadyToDraw;

    Texture         *m_deviceSourceTexture = nullptr, *previousTexture = nullptr;
    Texture         *tipTexture = nullptr;

    Texture         *m_disconnectTexture = nullptr;
    Texture         *m_failedTexture = nullptr;
    Texture         *m_blackBackgroundTexture = nullptr;
    Texture         *m_deviceNameTexture = nullptr;
    
    XElement        *mElementData = nullptr;
    UINT            texturePitch;
    UINT            m_imageBufferSize;
    bool            bCapturing = false, bFiltersLoaded = false;
    Shader          *colorConvertShader = nullptr;
    Shader          *drawShader = nullptr;

    HANDLE          hStopSampleEvent;
    HANDLE          hSampleMutex;
    SampleData      *latestVideoSample = nullptr;
    QWORD           m_LastSyncTime;
    QWORD           m_LastTickTime = 0; 
    HWND            m_renderHWND = NULL;    
    OBSRenderView   *m_renderView = NULL;

    //---------------------------------

    LPBYTE          lpImageBuffer = nullptr;
    ConvertData     *convertData = nullptr;
    HANDLE          *hConvertThreads = nullptr;

    void ChangeSize(bool bSucceeded = true, bool bForce = false);
    void KillThreads();

    String ChooseShader();
    String ChooseDeinterlacingShader();

    void Convert422To444(LPBYTE convertBuffer, LPBYTE lp422, UINT pitch, bool bLeadingY);
    void SetDeviceExist(bool);

    bool LoadFilters();
    void UnloadFilters();

    void Start();
    void Stop();

public:
    bool Init(XElement *data);
    DeviceSource();
    ~DeviceSource();

    void UpdateSettings();
    
    bool Preprocess();
    void Render(const Vect2 &pos, const Vect2 &size,SceneRenderType renderType);

    void BeginScene();
    void EndScene();

    void GlobalSourceEnterScene();
    void GlobalSourceLeaveScene();

    Vect2 GetSize() const {
      if(imageCX!=0&&imageCY!=0)
         return Vect2(float(imageCX), float(imageCY));
      else
         return Vect2(float(1280.0f),float(720.0f));
    }
    
    virtual void Reload();
};

