#ifndef __VH_CONST_DEFF__H_INCLUDE__
#define __VH_CONST_DEFF__H_INCLUDE__

#include "VH_Macro.h"
#include <windows.h>

#define APP_NAME  L"VhallLive"
#define MENGZHU_APP_NAME  L"MengZhuLive"

#include <map>
#include <list>
using namespace std;

//���CR������Ƴ���
#define DEF_MAX_CRP_NAME_LEN			63
//���CR�����������
#define DEF_MAX_CRP_DESCRIP_LEN			511
//���CR����汾��������
#define DEF_MAX_CRP_VER_DESCRIP_LEN		1023
//���CR�����������
#define DEF_MAX_CRP_DEPEND_NUM			150
#define DEF_MAX_EVENT_MSG_LEN          2048

//////////////////////////////////   ����ģ����Ϣ��������Begin    //////////////////////////
#define DEF_MESSAGE_BEGIN        1000

#define DEF_RECV_SETTINGUI_BEGIN      1100        // ����ģ�������Ϣ������ʼֵ
#define DEF_RECV_SETTINGUI_END        1199        // ����ģ�������Ϣ���ͽ���ֵ

#define DEF_RECV_COMMONTOOLKIT_BEGIN      1200        // COMMONTOOLKITģ�������Ϣ������ʼֵ
#define DEF_RECV_COMMONTOOLKIT_END        1299        // COMMONTOOLKITģ�������Ϣ���ͽ���ֵ

#define DEF_RECV_MAINUI_BEGIN      1300        // MAINUIģ�������Ϣ������ʼֵ
#define DEF_RECV_MAINUI_END        1399        // MAINUIģ�������Ϣ���ͽ���ֵ

#define DEF_RECV_OBSCONTROL_BEGIN      1400        // OBSCONTROLģ�������Ϣ������ʼֵ
#define DEF_RECV_OBSCONTROL_END        1499        // OBSCONTROLģ�������Ϣ���ͽ���ֵ


#define DEF_RECV_VHALLRIGHETEXTRAWIDGET_BEGIN   1500
#define DEF_RECV_VHALLRIGHETEXTRAWIDGET_END     1599


//////////////////////////////////   ����ģ������Begin    //////////////////////////
enum ENUM_UI_PLUGIN_TYPE
{
	ENUM_PLUGIN_RUNTIME,					// VhallLive����ʱ

   ENUM_PLUGIN_COMMONTOOLKIT,				// COMMONTOOLKITģ��
   ENUM_PLUGIN_MAINUI,				   // MAINUIģ
   ENUM_PLUGIN_OBSCONTROL,				   // OBSCONTROLģ��
   ENUM_PLUGIN_VHALLRIGHETEXTRAWIDGET, //VHALLRIGHETEXTRAWIDGET ģ��
	ENUM_PLUGIN_END,
};

enum MediaCoreEvent {
   Network_disconnect_notify = 1,
   Network_connect_failed = -100,
   Network_server_disconnect,
   Network_local_invalid,
   Network_rtmp_header_send_failed,
   Network_audio_header_send_failed,
   Network_video_header_send_failed,
   Network_bandwidth_Insufficient,
   Network_publish_success = 10,
   Network_switchlines_success = 11,
   Network_network_quality_poor = 12,
   Network_prepare_for_connect = 13,
   Network_to_connect = 14,
   Network_to_push_connect = 15,
};

struct DropFrameStatusCollect {
   unsigned long dropFrameRate_1sec;
   unsigned long dropFrameRate_3sec;
   unsigned long dropFrameRate_5sec;
   unsigned long dropFrameRate_10sec;
   unsigned long dropFrame3[3];
   unsigned long dropFrame5[5];
   unsigned long dropFrame10[10];
   unsigned long dropFrameRate_total;
   unsigned long totalTimeCount;
};

struct StreamStatus {
   char serverIP[20];
   char streamID[128];
   unsigned long currentDroppedFrames;
   unsigned long bytesSpeed;
   unsigned long droppedFrames;
   unsigned long sumFrames;
   UINT chunk_szie;
   UINT connect_count;
   DropFrameStatusCollect m_DropFrameStatusCollect;
};

enum ENABLE_NOTICE_ERR_MSG {
   ENABLE_NOTICE_ERR_MSG = 0,
   DISABLE_NOTICE_ERR_MSG = 1,
};

enum PLAY_MEDIA_STATE {
   PLAYUI_PLAYSTATE = 0,
   PLAYUI_PLAYSTATE_STOPPING = -2,
   PLAYUI_PLAYSTATE_FAIL = -1,
   PLAYUI_PLAYSTATE_NOTHING = 0,
   PLAYUI_PLAYSTATE_OPENING = 1,
   PLAYUI_PLAYSTATE_BUFFERING = 2,
   PLAYUI_PLAYSTATE_PALYERING = 3,
   PLAYUI_PLAYSTATE_PAUSED = 4,
   PLAYUI_PLAYSTATE_STOP = 5,
   PLAYUI_PLAYSTATE_END = 6,
   PLAYUI_PLAYSTATE_ERROR = 7
};

enum MOUSE_PROCESS_EVENT {
   MOUSE_LEFT_DOWN = 0,
   MOUSE_LEFT_UP = 1,
   MOUSE_RIGHT_DOWN = 2,
   MOUSE_RIGHT_UP = 3,
   MOUSE_MOVE = 4
};

enum SOURCE_MODIFY_TYPE {
   MODIFY_TYPE_FULL_SCREEN  = 0,
   MODIFY_TYPE_MOVE_TOP     = 1,
   MODIFY_TYPE_MOVE_BOTTOM  = 2,
   MODIFY_TYPE_MODIFY       = 3,
   MODIFY_TYPE_DELETE       = 4
};

//�����Ϣ��������ϢID
enum ENUM_PLUGIN_THREAD_ID {
   ENUM_PLUGIN_THREAD_UI,                                  // UI�߳�ID
   ENUM_PLUGIN_THREAD_END,
};
typedef enum {
   TYPE_COREAUDIO,
   TYPE_DECKLINK,
   TYPE_DSHOW_AUDIO,
   TYPE_MEDIAOUT,
   //��������Ƶ�豸�е���Ƶ�豸
   TYPE_DSHOW_VIDEO
}TYPE_RECODING_SOURCE;
//��Ƶ��ʽ
enum class VideoFormat {
   Any,
   Unknown,

   /* raw formats */
   ARGB = 100,
   XRGB,

   /* planar YUV formats */
   I420 = 200,
   NV12,
   YV12,
   Y800,

   /* packed YUV formats */
   YVYU = 300,
   YUY2,
   UYVY,
   HDYC,

   /* encoded formats */
   MJPEG = 400,
   H264
};

#define DEF_MAX_HTTP_URL_LEN					2048			// ���HTTP����URL����

#define USAGE_URL QString("http://bbs.vhall.com/viewtopic.php?f=5&t=15")
#define QA_URL QString("http://bbs.vhall.com/viewtopic.php?f=6&t=9")
#define MONITOR_URL QString("http://la.e.vhall.com:1780/login?k=%1&id=%2&s=%3&token=%4")           //���URL

//OBS�ײ����ϲ�ͨ����Ϣ
#define WM_USER_MODIFYWINDOW	(WM_USER + 100)
#define WM_USER_MODIFYTEXT	(WM_USER + 101)       
#define WM_USER_MODIFYIMAGE	(WM_USER + 102)
#define WM_USER_MODIFYCAMERA	(WM_USER + 103)
#define WM_USER_MODIFY_REGION	(WM_USER + 111)
#define WM_USER_MODIFY_MONITOR	(WM_APP + 112)
#define WM_USER_MODIFY_MEDIA	(WM_APP + 113)

#define WM_USER_DELETE_CAMERA	(WM_USER + 104)
#define WM_USER_DELETE_TEXT	(WM_USER + 105)
#define WM_USER_DELETE_PIC	(WM_APP + 106)
#define WM_USER_DELETE_MONITOR	(WM_USER + 107)
#define WM_USER_DELETE_REGION	(WM_USER + 108)
#define WM_USER_DELETE_WINDOWS	(WM_APP + 109)
#define WM_USER_DELETE_MEDIA	(WM_APP + 110)

#define MAX_DEVICE_ID_LEN 1024
//����
struct DeviceInfo {
   //������ʾ������
   wchar_t m_sDeviceDisPlayName[MAX_DEVICE_ID_LEN + 1];
   //�豸�ڲ�����
   wchar_t m_sDeviceName[MAX_DEVICE_ID_LEN + 1];
   //�豸��DeviceId
   wchar_t m_sDeviceID[MAX_DEVICE_ID_LEN + 1];

	int mIndex;//�û������豸������
   //�豸����
   TYPE_RECODING_SOURCE m_sDeviceType;
   
   DeviceInfo() {
      memset(this, 0, sizeof(DeviceInfo));
   };
   bool operator==(const DeviceInfo &other)const {
      if(m_sDeviceType!=other.m_sDeviceType)
      {
         return false;
      }
      
      if(wcscmp(m_sDeviceName,other.m_sDeviceName)!=0)
      {
         return false;
      }

      if(wcscmp(m_sDeviceID,other.m_sDeviceID)!=0)
      {
         return false;
      }
      
      return true;
   }
   bool operator <(const DeviceInfo &other) const
   {
      if(m_sDeviceType!=other.m_sDeviceType)
      {
         return m_sDeviceType<other.m_sDeviceType;
      }

      int ret=wcscmp(m_sDeviceName,other.m_sDeviceName);
      if(ret!=0)
      {
         return ret<0;
      }
      ret=wcscmp(m_sDeviceID,other.m_sDeviceID);
      if(ret!=0)
      {
         return ret<0;
      }

      return false;
   }
   
};

struct DeviceList
{
   typedef std::list<DeviceInfo>::iterator iterator;
   std::list<DeviceInfo> m_deviceList;
   std::list<DeviceInfo>::iterator begin(){return m_deviceList.begin();}
   std::list<DeviceInfo>::iterator end(){return m_deviceList.end();}
   std::list<DeviceInfo>::iterator find(DeviceInfo d){
      auto itor=m_deviceList.begin();
      for(;itor!=m_deviceList.end();itor++)
      {
         if(*itor==d)
         {
            break;
         }
      }
      return itor;
   }
   size_t size(){return m_deviceList.size();}
   void push_back(DeviceInfo &info)
   {
      for(auto itor=m_deviceList.begin();itor!=m_deviceList.end();itor++)
      {
         if(*itor==info)
         {      
            if(wcscmp(itor->m_sDeviceDisPlayName,info.m_sDeviceName)==0)
            {
               return ;
            }
         }
      }
      m_deviceList.push_back(info);
   }
   void Remove(DeviceInfo &info)
   {
      for(auto itor=m_deviceList.begin();itor!=m_deviceList.end();itor++)
      {
         if(*itor==info)
         {
            m_deviceList.erase(itor);
            return ;
         }
      }
   }
   void clear()
   {
      m_deviceList.clear();
   }

};

//��Ƶ�豸��������Ϣ
struct FrameInfo {
   UINT64 minFrameInterval, maxFrameInterval;
   UINT minCX, minCY;
   UINT maxCX, maxCY;
   UINT xGranularity, yGranularity;
   bool bUsingFourCC;
   VideoFormat format;
   bool operator == (FrameInfo & other)
   {
      #define CHECKEQUAL(X) if(this->X!=other.X) return false;
      CHECKEQUAL(minFrameInterval);
      CHECKEQUAL(maxFrameInterval);
      CHECKEQUAL(minCX);
      CHECKEQUAL(minCY);
      CHECKEQUAL(maxCX);
      CHECKEQUAL(maxCY);
      CHECKEQUAL(xGranularity);
      CHECKEQUAL(yGranularity);
      CHECKEQUAL(bUsingFourCC);
      CHECKEQUAL(format);
      return true;
   }
   bool operator <(FrameInfo & other)
   {
      if (maxCX<other.maxCX)
      {
         return true;
      }
      else if (maxCX == other.maxCX)
      {
         if (maxCY<other.maxCY)
         {
            return true;
         }
      }
      return false;
   }
   bool ResolutionEqual(FrameInfo & other)
   {
      #define EQUAL(X) if(this->X!=other.X){return false;}
      EQUAL(minCX);
      EQUAL(minCY);
      EQUAL(maxCX);
      EQUAL(maxCY);
      return true;
   }
};
struct FrameInfoList
{
   typedef void (*Func_Append)(FrameInfo,void *);
   void Append(FrameInfo info)
   {
      if(appendFunc){
         appendFunc(info, &infos);
      }
      else {
         PushBack(info);
      }
   }
   void PushBack(FrameInfo info)
   {
      infos.push_back(info);
   }
   std::list<FrameInfo> infos;
   Func_Append appendFunc=NULL;
   //typedef std::list<FrameInfo *> FrameInfoList;
};



//��Ƶ�豸
struct DShowVideoDevice
{  
   //�豸
   DeviceInfo device;
   //������Ϣ
   FrameInfoList frameInfos;
};

#define  CanSkipUpdate     1  //�Ƿ���������������1Ϊ����

#define	SUCCESS_CHECK_VER	0	//�ɹ�
#define	FAILED_CHECK_VER	1	//ʧ��
#define	UPDATE_CHECK_VER	2	//����
//�豸��ɫ�ʿռ�
enum DeviceColorType {
   DeviceOutputType_RGB,
   //DeviceOutputType_RGB32,
   //DeviceOutputType_ARGB32,

   //planar 4:2:0
   DeviceOutputType_I420,
   DeviceOutputType_YV12,

   //packed 4:2:2
   DeviceOutputType_YVYU,
   DeviceOutputType_YUY2,
   DeviceOutputType_UYVY,
   DeviceOutputType_HDYC,
   ////��ʱ ������  
   //DeviceOutputType_Y41P,
   //DeviceOutputType_YVU9,
   //DeviceOutputType_MPEG2_VIDEO,
   //DeviceOutputType_H264,
   //DeviceOutputType_dvsl,
   //DeviceOutputType_dvsd,
   //DeviceOutputType_dvhd,
   //DeviceOutputType_MJPG,
   //DeviceOutputType_UNKNOWN
};

//ȥ��������
enum DeinterlacingType {
    DEINTERLACING_NONE,
    DEINTERLACING_DISCARD,
    DEINTERLACING_RETRO,
    DEINTERLACING_BLEND,
    DEINTERLACING_BLEND2x,
    DEINTERLACING_LINEAR,
    DEINTERLACING_LINEAR2x,
    DEINTERLACING_YADIF,
    DEINTERLACING_YADIF2x,
    DEINTERLACING__DEBUG,
    DEINTERLACING_TYPE_LAST
};


//ȥ������
enum DeinterlacingFieldOrder {
    FIELD_ORDER_NONE,
    FIELD_ORDER_TFF = 1,
    FIELD_ORDER_BFF,
};
//ȥ����ִ��
enum DeinterlacingProcessor {
    DEINTERLACING_PROCESSOR_CPU = 1,
    DEINTERLACING_PROCESSOR_GPU,
};
//ȥ��������
struct DeinterlacerConfig {
    int    type, fieldOrder, processor;
    bool   doublesFramerate;
};
//�豸����
enum DShowDeviceType
{
   DShowDeviceType_Video,
   DShowDeviceType_Audio
};
//��������
enum DShowDevicePinType
{
   DShowDevicePinType_Video,
   DShowDevicePinType_Audio
};

struct VideoDeviceSetting
{
   //֡��   
   UINT64          frameInterval;
   //�ֱ���
   UINT cx;
   UINT cy;
   //ȥ��������
   DeinterlacingType type;

   
   //�Ƿ�ʹ��YUV420 ��709�ռ�
   bool use709;
   //ɫ�ʿռ�   
   DeviceColorType colorType;
};
//1��� 2ɾ�� 3�޸Ĳ�ͬid 4�޸���ͬid���ñ仯
enum enum_device_operator
{
   device_operator_add=1,
   device_operator_del,
   //�޸�����
   device_operator_modify,
};
//
enum enum_checkbox_status
{
   checkbox_status_checked,
   checkbox_status_unchecked
};
struct SampleData {
   //IMediaSample *sample;
   LPBYTE lpData = nullptr;
   long dataLength;
   int cx, cy;
   LONGLONG timestamp;
   volatile long refs = 0;

   inline SampleData() { 
       refs = 1; 
   }
   inline ~SampleData() { 
       if (lpData)
           free(lpData); 
   }

   inline void AddRef() { ++refs; }
   inline void Release() {
      if (!InterlockedDecrement(&refs))
         delete this;
   }
};

struct ConvertData {
   LPBYTE input = 0, output = 0;
   SampleData *sample = nullptr;
   HANDLE hSignalConvert = nullptr, hSignalComplete = nullptr;
   bool   bKillThread;
   UINT   width, height;
   UINT   pitch;
   UINT   startY, endY;
   UINT   mLinePitch, mLineShift;
};
enum VideoOutputType {
   VideoOutputType_None,
   VideoOutputType_RGB24,
   VideoOutputType_RGB32,
   VideoOutputType_ARGB32,

   VideoOutputType_I420,
   VideoOutputType_YV12,

   VideoOutputType_Y41P,
   VideoOutputType_YVU9,

   VideoOutputType_YVYU,
   VideoOutputType_YUY2,
   VideoOutputType_UYVY,
   VideoOutputType_HDYC,

   VideoOutputType_MPEG2_VIDEO,
   VideoOutputType_H264,

   VideoOutputType_dvsl,
   VideoOutputType_dvsd,
   VideoOutputType_dvhd,

   VideoOutputType_MJPG
};
//����
struct DeviceInfoAttribute
{
   UINT width;
   UINT height;
   int frameInternal;
   DeinterlacingType type;
   VideoFormat format;
   DeviceInfoAttribute()
   {
      memset(this,0,sizeof(DeviceInfoAttribute));
   }
   bool isEmpty()
   {
      return width==0&&
      height==0&&
      frameInternal==0;
   }
};
//����Դλ�õ�����
enum DataSourcePosType
{
   //�Զ���
   enum_PosType_custom,

   //ȫ��
   enum_PosType_fullScreen,

   //���½�
   emum_PosType_rightDown,

   //�Զ��ж�
   enum_PosType_auto
};
typedef enum {
   SRC_SOURCE_UNKNOW=-1,
   SRC_DSHOW_DEVICE,
   SRC_MONITOR_AREA,
   SRC_MONITOR,
   SRC_WINDOWS,
   SRC_TEXT,
   SRC_PIC,
   SRC_MEDIA_OUT,
   SRC_SOURCE_PERSISTENT
}SOURCE_TYPE;

#define CLASS_NAME_UNKNOW        L"UNKNOWCLASS"
#define CLASS_NAME_DEVICE        L"DeviceCapture"
#define CLASS_NAME_DECKLINK      L"DeckLinkCapture"
#define CLASS_NAME_TEXT          L"TextSource"
#define CLASS_NAME_BITMAP        L"BitmapImageSource"
#define CLASS_NAME_MONITOR       L"MonitorCaptureSource"
#define CLASS_NAME_WINDOW        L"WindowCaptureSource"
#define CLASS_NAME_AREA          L"AREACaptureSource"
#define CLASS_NAME_MEDIAOUTPUT   L"MediaOutputSource"

typedef enum right_page {
   RIGHT_PAGE_CHAT = 0,    //����ҳ������
   RIGHT_PAGE_NOTICE = 1,  //����ҳ������
}RIGHT_PAGE;

#define DEBUG_NEW_ADDRESS(X) \
{\
   char buf[512];\
   sprintf(buf,"debug new [%ld][%s][%s][%d][%p]\n",GetCurrentThreadId(),__FILE__,__FUNCTION__,__LINE__,X);\
   OutputDebugStringA(buf);\
}

typedef void (*FuncNotifyService)(char *msg);
typedef bool (*FuncCreateTextureByText)(wchar_t *text,unsigned char *&buf,int &w,int &h);
typedef void (*FuncFreeMemory)(unsigned char *);
typedef bool (*FuncCheckFileTexture)(void* src,void* des);
enum class SceneRenderType{
   SceneRenderType_Stream,
   SceneRenderType_Preview,
   SceneRenderType_Desktop
};
#if 0
enum SceneType {
   e_scenetype_none = 0,  //�޳���
   e_scenetype_unnatural,  //����Ȼ����
   e_scenetype_natural,  //��Ȼ����
};
#endif
enum enum_show_type {
   widget_show_type_Main = 0,                //��������
   widget_show_type_Activites,               //��б�
   widget_show_type_InterActive,             //��������
   widget_show_type_logIn,                   //��½����
   widget_show_type_vhallActive,					//΢�𻥶�
};

enum enum_upload_live_type {
	upload_normal_live = 0,
	upload_active_live = 1,
};

typedef struct VHStartParam_st {
   char streamURLS[2048];
   char streamToken[512];
   char streamName[256];
   char channelID[256];
   char userName[256];
   char msgToken[512];
   char filterurl[512]; 
     
   wchar_t startUpCmd[4096];
   wchar_t roomId[512];
   wchar_t roompwd[512];
   unsigned int nProtocolType;
   unsigned short chat_port;
   wchar_t chat_srv[256];
   wchar_t chat_url[512];
   unsigned short msg_port;
   wchar_t msg_srv[256];

   bool forbidchat;
   bool bHideLogo;
   bool bWebinarPlug;
   bool bIsPwdLogin;

   bool bConnectToVhallService;
   bool bLoadExtraRightWidget;

   char userId[256];
   char webinarType[16];
   char accesstoken[256];
   char userRole[64];
   char scheduler[512];
   char pluginsUrl[1024];
   wchar_t webinarName[4096];

   //wchar_t startMode[32];
   enum_show_type m_eType;
   void ** pgNameCore;
}VHStartParam;
typedef long (*IPOSTCRMessageFunc)(DWORD,void *,DWORD);

#define AUDIOENGINE_MAXBUFFERING_TIME_MS 400 
#define AUDIOENGINE_PERSAMPLE_TIME_MS    10
#define VIDEORENDER_TICK                 25
class IDataReceiver {
public:
   virtual void PushAudioSegment(float *buffer, unsigned int numFrames, unsigned long long timestamp) = 0;
   virtual void PushAudioSegment(unsigned char *buffer, unsigned int numFrames, unsigned long long timestamp) = 0;
   virtual void PushVideoSegment(unsigned char *buffer, unsigned int size, unsigned long long timestamp,bool bSame) = 0;
};

struct Dispatch_Param{
   char userId[64];
   char role[64];
   char vhost[128];
   char webinar_id[128];
   char mixserver[512];
   char accesstoken[2048];
   char token[2048];
   //�Ƿ�Ϊ��α��
   int ismix;
};
#define MAX_PATH_LEN 260

#define MAX_FONT_LEN 256
#define MAX_FONT_TEXT_LEN 256

#define MAX_NAME_LEN 256
#define MAX_SOURCE_NAME_LEN 512

// ͼƬ����
struct STRU_OBSCONTROL_IMAGE {
   int m_dwType;
   float x;
   float y;
   float w;
   float h;
   wchar_t m_strPath[MAX_PATH_LEN + 1];
   wchar_t m_strSourceName[MAX_SOURCE_NAME_LEN + 1];
   bool isFullScreen;
public:
   STRU_OBSCONTROL_IMAGE() {
      memset(this, 0, sizeof(STRU_OBSCONTROL_IMAGE));
   }
};

// ���ֿ���
struct STRU_OBSCONTROL_TEXT {
   int m_x;
   int m_y;
   int m_w;
   int m_h;
   int m_iControlType;           //1�������2 �����޸� 3ɾ��
   int m_iBold;
   int m_iItalic;
   float m_ix;
   float m_iy;
   float m_iw;
   float m_ih;

   UINT m_iColor;
   int m_iUnderLine;
   wchar_t m_strFont[MAX_FONT_LEN + 1];
   wchar_t m_strText[MAX_FONT_TEXT_LEN + 1];
   wchar_t m_strSourceName[MAX_SOURCE_NAME_LEN + 1];
   unsigned char *m_textPic;
public:
   STRU_OBSCONTROL_TEXT() {
      memset(this, 0, sizeof(STRU_OBSCONTROL_TEXT));
   }
};

#define VH_LOG_DIR L"\\vhlog\\"
#endif //__VH_CONST_DEFF__H_INCLUDE__
