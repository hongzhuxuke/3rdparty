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
#include "dshowcapture.hpp"
#include "IDShowPlugin.h"
#include <map>
#define VIDEOBUFFERCOUNT  2
struct VideoBuffer {
   unsigned char *data = nullptr;
   size_t size = 0;
   long long startTime;
   long long endTime;
   VideoFormat format;
   int cx;
   int cy;
   void Clear() {
      if (this->data != NULL) {
         delete this->data;
         this->data = NULL;
      }
      this->size = 0;
      startTime = endTime = -1;
      cx = cy = 0;
   }
   VideoBuffer() {
      data = NULL;
      size = 0;
      startTime = -1;
      endTime = -1;
      cx = cy = 0;
   }
   ~VideoBuffer() {
      if (this->data != NULL) {
         delete this->data;
         this->data = NULL;
      }
      this->size = 0;
   }
   void SetData(unsigned char *data, size_t size, long long startTime,
                long long endTime, VideoFormat format, int cx, int cy) {
      if (!data || size == 0) {
         return;
      }
      if (size != this->size) {
         if (this->data != NULL) {
            delete this->data;
            this->data = NULL;
         }
         this->size = size;
         this->data = new unsigned char[this->size];
      }

      memcpy(this->data, data, size);
      this->startTime = startTime;
      this->endTime = startTime;
      this->format = format;
      this->cx = cx;
      this->cy = cy;
   }
};

class DShowVideoFilterDevice :public IDShowVideoFilterDevice {
public:
   //���캯��
   DShowVideoFilterDevice();
   //��������
   virtual ~DShowVideoFilterDevice();
   //�����Ƶ����
   virtual bool GetNextAudioBuffer(void **buffer, unsigned int *numFrames, unsigned long long *timestamp);
   //�����Ƶ����
   virtual bool GetNextVideoBuffer(void **buffer,
                                   unsigned long long *bufferSize,
                                   unsigned long long *timestamp,
                                   VideoDeviceSetting &setting);
   //�ͷ���Ƶ������
   virtual void ReleaseVideoBuffer();
   //������Ƶ������
   virtual bool SetVideoConfig(VideoDeviceSetting setting);
   //�����Ƶ����
   bool GetAudioConfig(
      UINT &mInputBitsPerSample,
      bool &bFloat,
      UINT &mInputSamplesPerSec,
      UINT &mInputChannels,
      UINT &mInputBlockSize
      );



   //���ɫ�ʿռ�
   bool GetVideoDeviceSettingsWithColorType(VideoDeviceSetting *setting);
   //�����Ƶ�����������Ϣ
   bool GetDShowVideoFrameInfoList(FrameInfoList *frameInfoList, FrameInfo *currentFrameInfo);
   //�����豸����
   bool SetDeviceDefaultAttribute(int width, int height, int frameInternal, VideoFormat format);


   bool Start(DeviceInfo info,
              VideoDeviceSetting &setting,
              DShowDeviceType type,
              DShowDevicePinType pinType,
              HANDLE handle,
              HANDLE pFunc);
   bool InternalStart();
   void Stop(DShowDevicePinType type);

   void OnVideoData(const DShow::VideoConfig &config, unsigned char *data, size_t size, long long startTime, long long endTime);
   void OnAudioData(const DShow::AudioConfig &config, unsigned char *data, size_t size, long long startTime, long long endTime);
   bool Equal(DeviceInfo info, DShowDeviceType type);
private:
   DShow::Device *   m_pDevice = nullptr;
   DeviceInfo        m_deviceInfo;
   DShowDeviceType   m_deviceType;
   HANDLE            m_audioMutex;
   HANDLE            m_videoMutex;
   int               m_videoBufferIndex;
   int               m_videoOutIndex;
   VideoBuffer       m_videoBuffer[VIDEOBUFFERCOUNT];
   bool              m_videoBufferLocked;


   DShow::AudioConfig m_audioConfig;
   HANDLE            m_audioFunc;
   HANDLE            m_audioParam;

   unsigned char* m_videoDataBuffer = nullptr;
   unsigned long long m_videoDataLen;

};
class DShowVideoFilterDeviceManager {
public:
   DShowVideoFilterDeviceManager();
   ~DShowVideoFilterDeviceManager();
   DShowVideoFilterDevice *GetDeviceFilter(
      IDShowVideoFilterDevice *&device,
      DeviceInfo deviceInfo,
      VideoDeviceSetting &setting,
      DShowDeviceType deviceType,
      DShowDevicePinType pinType,
      HANDLE handle,
      HANDLE pFunc);
   bool ReleaseDShowVideoFilter(HANDLE handle);
   bool IsDShowVideoFilterExist(HANDLE handle);
   bool GetDShowVideoFrameInfoList(DeviceInfo deviceInfo,
                                   FrameInfoList *frameInfoList, FrameInfo *currentFrameInfo);
   //��������
   bool SetDeviceDefaultAttribute(
      DeviceInfo device,
      UINT width,
      UINT height,
      int frameInternal,
      VideoFormat format);

   void EnterMutex();
   void LeaveMutex();

private:
   //������豸����ӳ��
   std::map<HANDLE, DShowVideoFilterDevice *>  m_DeviceList;
   std::map<HANDLE, DShowDevicePinType>        m_DevicePinTypeList;
   HANDLE hDeviceFilterMutex = NULL;
};



class VideoDeviceAttributeManager {
public:
   VideoDeviceAttributeManager(FUNCFileOpen,
                               FUNCFileClear,
                               FUNCFileRead,
                               FUNCFileWrite,
                               FUNCFileClose);
   ~VideoDeviceAttributeManager();
   //�����豸����
   bool SetDeviceDefaultAttribute(
      DeviceInfo deviceInfo,
      UINT width,
      UINT height,
      int frameInternal,
      DeinterlacingType type,
      VideoFormat format);

   //��ȡ�豸����
   bool GetDeviceDefaultAttribute(
      DeviceInfo deviceInfo,
      UINT &width,
      UINT &height,
      int &frameInternal,
      DeinterlacingType &type,
      VideoFormat &format);

   //����豸��
   int GetDeviceDisplayNumber(DeviceInfo device);
   //�����豸�б�
   void SyncDeviceList(DeviceList &deviceList);
   //
private:
   void LoadConfig();
   void SaveConfig();
private:
   std::map<DeviceInfo, DeviceInfoAttribute> m_DeviceAttributes;
   DeviceList m_deviceList;
   FUNCFileOpen m_vhopen;
   FUNCFileClear m_vhclear;
   FUNCFileRead m_vhread;
   FUNCFileWrite m_vhwrite;
   FUNCFileClose m_vhclose;
};
