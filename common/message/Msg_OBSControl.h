#ifndef MESSAGE_OBSCONTROL_DEFINE_H
#define MESSAGE_OBSCONTROL_DEFINE_H

#include "VH_ConstDeff.h"
#include "vhmonitorcapture.h"
#include <string>
#include <windows.h>

using namespace std;

enum MSG_OBSCONTROL_DEF {
   ///////////////////////////////////接收事件///////////////////////////////////
   MSG_OBSCONTROL_PUBLISH = DEF_RECV_OBSCONTROL_BEGIN,		// 推流
   MSG_OBSCONTROL_TEXT,                                     //文本控制
   MSG_OBSCONTROL_IMAGE,                                     //图片控制
   MSG_OBSCONTROL_DESKTOPSHARE,                             //桌面共享
   MSG_OBSCONTROL_WINDOWSRC,                                //软件源
   MSG_OBSCONTROL_SHAREREGION,                              //区域共享
   MSG_OBSCONTROL_PROCESSSRC,                               //处理源相关
   MSG_OBSCONTROL_ADDCAMERA,                                //添加摄像头

   MSG_OBSCONTROL_AUDIO_NOTIFY,                             //音频变化通知   
   MSG_OBSCONTROL_STREAMSTATUS_CHANGE,                      //流状态改变
   MSG_OBSCONTROL_AUDIO_CAPTURE,                             //音频捕获
	MSG_OBSCONTROL_VIDIO_HIGHQUALITYCOD,								//视频高质量编码
   MSG_OBSCONTROL_STREAM_NOTIFY,                             //流信息
   MSG_OBSCONTROL_STREAM_RESET,                              //流信息
   MSG_OBSCONTROL_STREAM_PUSH_SUCCESS,                       //推流成功
   MSG_OBSCONTROL_PUSH_AMF0,                                //推送AMF0消息
   MSG_OBSCONTROL_VOICE_TRANSITION,                                //语音转换文本回显
   MSG_OBSCONTROL_ENABLE_VT,                                //使能实时字幕
   MSG_OBSCONTROL_CLOSE_AUDIO_DEV,                              //关闭音频设备
   //MSG_OBSCONTROL_RECORD_COMMIT,                              //提交当前打点录制
}; 


// 推流
struct STRU_OBSCONTROL_PUBLISH {
   bool m_bIsStartPublish;
   bool m_bIsSaveFile;
   bool m_bServerPlayback;
   bool m_bExit;
   bool m_bDispatch;
   bool m_bIsCloseAppWithPushing;
   bool m_bInteractive;
   bool m_bMediaCoreReConnect; //true:表示底层链接异常需要重新开始推流
   float m_iQuality;
   wchar_t m_wzSavePath[MAX_PATH_LEN + 1];
   struct Dispatch_Param m_bDispatchParam; 
public:
   STRU_OBSCONTROL_PUBLISH();
};

#define TextLanguage_Mandarin    L"普通话"
#define TextLanguage_Cantonese   L"粤语"
#define TextLanguage_Lmz         L"四川话"

typedef enum lan_type{
   LAN_TYPE_PTH = 0, //普通话
   LAN_TYPE_YU = 1,  //粤语
   LAN_TYPE_SC = 2,  //四川话
}LAN_TYPE;

struct STRU_VT_INFO {
   bool bEnable;  //使能信息
   int fontSize;  //字体大小
   int lan;       //语音类型；
public:
   STRU_VT_INFO() {
      bEnable = false;
      fontSize = 18;
      lan = LAN_TYPE_PTH;
   };
};


struct PLUGIN_DATA {
   char* data;
   int length;
public:
   PLUGIN_DATA();
};

struct VOICE_TRANS_MSG 
{
   WCHAR* data;
   int length;

public:
   VOICE_TRANS_MSG(){
      data = NULL;
      length = 0;
   };
   VOICE_TRANS_MSG(const WCHAR* inputData, const int len) {
      data = new WCHAR[len + len];
      wmemset(data, 0, len + len);
      wcsncpy(data, inputData, len);
      //wcsncpy_s(data, wcslen(data), inputData, wcslen(inputData));
      length = len;
   };
};

struct  RECORD_STATE_CHANGE
{
	int iState;
public:
	RECORD_STATE_CHANGE(){
		iState = 0;
	};

	RECORD_STATE_CHANGE(const int state){
		iState = state;
	};

};

// 软件源控制
struct STRU_OBSCONTROL_WINDOWSRC {
   VHD_WindowInfo m_windowInfo;
public:
   STRU_OBSCONTROL_WINDOWSRC();
};

// 区域共享控制
struct STRU_OBSCONTROL_SHAREREGION {
   RECT m_rRegionRect;
public:
   STRU_OBSCONTROL_SHAREREGION();
};

// 处理源相关
struct STRU_OBSCONTROL_PROCESSSRC {
   DWORD m_dwType;
public:
   STRU_OBSCONTROL_PROCESSSRC();
};

// 管理摄像头
struct STRU_OBSCONTROL_ADDCAMERA {
   enum_device_operator m_dwType;           
   DeviceInfo m_deviceInfo;
   DataSourcePosType m_PosType;
   HWND m_renderHwnd = NULL;
public:
   STRU_OBSCONTROL_ADDCAMERA() {
      memset(this, 0, sizeof(STRU_OBSCONTROL_ADDCAMERA));
   }
};

enum enum_stream_status_type
{
   stream_status_connect,
   stream_status_disconnect
};

#define STREAMSTATUS_MSG_MAXLEN 1024
struct STRU_OBSCONTROL_STREAMSTATUS
{
   enum_stream_status_type m_eType;
   int m_iStreamCount;
   WCHAR m_wzMsg[STREAMSTATUS_MSG_MAXLEN + 1];
};

// 音频捕获
struct STRU_OBSCONTROL_AUDIO_CAPTURE {
   DeviceInfo info;
   bool isNoise;					//噪音阀
   int openThreshold;     //噪音阀上限
   int closeThreshold;    //噪音阀下限

	int iKbps;						//音频码率
	int iAudioSampleRate; //音频采样率
	bool bNoiseReduction; //降         噪
	float fMicGain;				//麦克风增益
	//bool bRecord;				//是否录制

public:
   STRU_OBSCONTROL_AUDIO_CAPTURE();
};

// 设备选项保存
struct STRU_OBSCONTROL_VIDIO_SET {
	DeviceInfo m_DeviceInfo;
	int high_codec_open;  //高质量编码
public:
	STRU_OBSCONTROL_VIDIO_SET();
};

#endif //MESSAGE_OBSCONTROL_DEFINE_H
