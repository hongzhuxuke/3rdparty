#ifndef MESSAGE_MAINUI_DEFINE_H
#define MESSAGE_MAINUI_DEFINE_H

#include "VH_ConstDeff.h"


enum MSG_MAINUI_DEF {
   ///////////////////////////////////接收事件///////////////////////////////////
   MSG_MAINUI_WIDGET_SHOW = DEF_RECV_MAINUI_BEGIN,	// 显示Widget
   MSG_MAINUI_CLICK_CONTROL,		                  // CLICK控制
   MSG_MAINUI_CLICK_ADD_WNDSRC,		               // 添加软件源
   MSG_MAINUI_LIST_CHANGE,		                     // 列表改变
   MSG_MAINUI_VOLUME_CHANGE,		                  // 音量改变
   MSG_MAINUI_NOISE_VALUE_CHANGE,                  // 修改降噪阀值
   MSG_MAINUI_SHOW_AUDIOSETTING,		               // 高级声卡设置
   MSG_MAINUI_CLOSE_SYS_SETTINGWND,                // 通知关闭设置窗口
   MSG_MAINUI_MUTE,		                           // 静音设置
   MSG_MAINUI_VEDIOPLAY_PLAY,		                  // 插播播放/暂停
   MSG_MAINUI_VEDIOPLAY_STOPPLAY,		            // 停止插播
   MSG_MAINUI_VEDIOPLAY_ADDFILE,		               // 添加文件
   MSG_MAINUI_VEDIOPLAY_SHOW,		                  // 现实插播
   MSG_MAINUI_MAINUI_MOVE,                         //窗口移动
   MSG_MAINUI_MAINUI_PLAYLISTCHG,                  //播放列表改变
   //MSG_MAINUI_SELECT_PATH,                         //选择保存路径
   MSG_MAINUI_SAVE_SETTING,                        //设置保存
   MSG_MAINUI_MODIFY_TEXT,                         //修改文本
   MSG_MAINUI_MGR_CAMERA,                          //添加摄像头
   MSG_MAINUI_MGR_CLOSEWND,                        //添加摄像头关闭窗口
   MSG_MAINUI_MODIFY_CAMERA,                       //修改摄像头
   MSG_MAINUI_MODIFY_SAVE,                         //修改保存
   
   //MSG_MAINUI_CAMERA_CHANGE,                     //摄像头切换
   
   MSG_MAINUI_MODIFY_IMAGE,                        //修改图片

   
   MSG_MAINUI_CAMERA_SELECT,                       //摄像设备选中
   MSG_MAINUI_CAMERA_FULL,                         //摄像设备全屏
   MSG_MAINUI_CAMERA_DEVICECHG,                    //设备切换(修改中存在)
   //MSG_MAINUI_CAMERA_OPTION_SAVE,                  //设备选项保存
   MSG_MAINUI_CAMERA_UPDATE_CACHE,                 //更新摄像设备缓存
   MSG_MAINUI_CAMERA_SETTING,                      //摄像头设置
   MSG_MAINUI_CAMERA_DELETE,                       //删除摄像头
   
   MSG_MAINUI_DEVICE_CHANGE,                       //硬件设备改变(插/拔)
   MSG_MAINUI_DELETE_MONITOR,                       //停止桌面共享
   MSG_MAINUI_SETTING_CLOSE,                       //关闭系统设置窗口
  // MSG_MAINUI_SETTING_OPENCURDIR,                  //打开当前目录
   MSG_MAINUI_SETTING_CONFIRM,                     //设置确定
   MSG_MAINUI_SETTING_FOCUSIN,                     //主窗口得到焦点

   MSG_MAINUI_DO_CLOSE_MAINWINDOW,                 //关闭主窗口

   MSG_MAINUI_SHOW_AUDIO_SETTING_CARD,             //呼出音频设置界面
   MSG_MAINUI_HIDE_AREA,                           //隐藏区域共享界面

   MSG_MAINUI_DESKTOP_SHOW_CAMERALIST,             //显示摄像头列表
   MSG_MAINUI_DESKTOP_SHOW_DELCAMERA,             //移除摄像头
   MSG_MAINUI_DESKTOP_SHOW_SETTING,                //显示设置
   //MSG_MAINUI_DESKTIO_SHOW_CHAR,                   //显示聊天
   MSG_MAINUI_RECORD_CHANGE,                   //录制状态改变
   MSG_MAINUI_SHOW_LOGIN,                       //显示登陆界面
   MSG_MAINUI_HTTP_TASK,                        //http任务队列
	MSG_MAINUI_CLOSE_VHALL_ACTIVE,					//关闭微吼互动
   MSG_MAINUI_LOG,
   MSG_MAINUI_VHALL_ACTIVE_EVENT,                //互动事件。
   MSG_MAINUI_VHALL_ACTIVE_DEVICE_SELECT,
   MSG_MAINUI_VHALL_ACTIVE_SocketIO_MSG,
};

struct STRU_VHALL_ACTIVE_EVENT {
   bool mbHasVideo;//互动直播流是否传输视频流。
   int mEventType;
   char mEventData[DEF_MAX_EVENT_MSG_LEN];
public:
   STRU_VHALL_ACTIVE_EVENT();
};

struct STRU_VHALL_ACTIVE_DEV_EVENT {
   char mCameraID[DEF_MAX_EVENT_MSG_LEN];
   char mMicID[DEF_MAX_EVENT_MSG_LEN];
   char mPlayerID[DEF_MAX_EVENT_MSG_LEN];
   int mCameraIndex = 0;
   int mMicIndex = 0;
   int mPlayerIndex = 0;
public:
   STRU_VHALL_ACTIVE_DEV_EVENT() {
      memset(mCameraID,0, DEF_MAX_EVENT_MSG_LEN);
      memset(mMicID, 0, DEF_MAX_EVENT_MSG_LEN);
      memset(mPlayerID, 0, DEF_MAX_EVENT_MSG_LEN);
   }
};

// 显示widget
struct STRU_MAINUI_WIDGET_SHOW {
   BOOL m_bIsShow;
   BOOL m_bIsWebinarPlug;
   enum_show_type m_eType;
   bool *bExit;
   bool bLoadExtraRightWidget;
   wchar_t token[4096];
   wchar_t m_listUrl[4096];
   wchar_t m_plugUrl[4096];
   wchar_t m_webinar_plug[4096];
   wchar_t m_imgUrl[4096];
   wchar_t m_userName[4096];
   wchar_t m_streamName[4096];
   wchar_t m_webinarName[4096];
   wchar_t m_version[256];
   wchar_t m_roomid[512];
   wchar_t m_roompwd[512];
   bool mbIsPwdLogin;
   bool m_bShowTeaching;
   void **pgNameCore;
   wchar_t chat_url[512];
public:
   STRU_MAINUI_WIDGET_SHOW();
};

enum VHALL_ACTIVE_EXIT_TYPE {
	EXIT_CLOSE = 0,
	EXIT_KICKOUT = 1,	  //被踢出
};

struct STRU_CLOSE_VHALL_ACTIVE {
	bool exitToLiveList;
	int exitReason;
public:
	STRU_CLOSE_VHALL_ACTIVE() {
		exitToLiveList = true;
		exitReason = EXIT_CLOSE;
	}
};

enum enum_control_type {
   control_ShowSetting = 0,     //设置
   control_Minimize,            //最小化
   control_CloseApp,            //关闭

   control_CaptureSrc,          //共享源
   control_AddCamera,           //添加摄像头
   control_MultiMedia,          //多媒体
   control_Record,              //录制
   control_StartLive,           //推流         ExtraData(0代表停止推流 1代表开始推流) 
   control_LiveTool,            //直播插件
                                                                      
   //共享源的子项(扩展项)
   control_MonitorSrc = 100,	//桌面共享
   control_WindowSrc,				//软件共享
   control_RegionShare,			//区域演示

   //多媒体的子项(扩展项)
   control_VideoSrc,
   control_AddImage,
   control_AddText,
   control_VoiceTranslate,
   //录制扩展子项
   control_StartRecord,
   control_RecordSuspendOrRecovery

};

//服务器打点录制的操作
enum eRecordReTyp
{
	eRecordReTyp_Start = 1,	        //开始  /恢复
	eRecordReTyp_Suspend = 2,	    //暂停
	eRecordReTyp_Stop =3,			//结束
};

enum enum_exit_reason {
   reason_none = 0,
   reason_kickOut = 1,//被主持人踢出。
};

// 点击控制
struct STRU_MAINUI_CLICK_CONTROL {
   enum_control_type m_eType;
   enum_exit_reason m_eReason;
   DWORD_PTR m_dwExtraData;          //附加数据
   int m_globalX;
   int m_globalY;
   bool m_bIsRoomDisConnect;   //用于互动直播，SDK网络端口处理。
public:
   STRU_MAINUI_CLICK_CONTROL();
};

enum enum_change_type {
   change_None = 0,              //None
   change_Mic,              //Mic
   change_Speaker,            //Speaker
   change_MicEnhance,            //Enhance
   change_VedioPlay,            //VedioPlay
};

// 列表改变
struct STRU_MAINUI_LIST_CHANGE {
   enum_change_type m_eType;
   int m_nIndex;
public:
   STRU_MAINUI_LIST_CHANGE();
};

// 音量改变
struct STRU_MAINUI_VOLUME_CHANGE {
   enum_change_type m_eType;
   float m_nVolume;
public:
   STRU_MAINUI_VOLUME_CHANGE();
};

enum enum_mute_type {
   mute_None = 0,              //None
   mute_Mic,                  //Mic
   mute_Speaker,              //Speaker
};

// 静音
struct STRU_MAINUI_MUTE {
   enum_mute_type m_eType;
   BOOL m_bMute;
public:
   STRU_MAINUI_MUTE();
};

// 插播播放点击
struct STRU_MAINUI_PLAY_CLICK {
   BOOL m_bIsPlay;
public:
   STRU_MAINUI_PLAY_CLICK();
};

// 摄像头控制
struct STRU_MAINUI_CAMERACTRL {
   enum_device_operator m_dwType;         //1添加 2删除
   DeviceInfo m_Device;
   bool m_bFullScreen;         //全屏
public:
   STRU_MAINUI_CAMERACTRL();
};

// 设备改变
struct STRU_MAINUI_DEVICE_CHANGE {
   WCHAR m_wzDeviceID[MAX_DEVICE_ID_LEN + 1];
   BOOL m_bAdd;         //插入设备，FALSE为拔出
public:
   STRU_MAINUI_DEVICE_CHANGE();
};

typedef enum {
   WND_TYPE_NONE = 0,
   WND_TYPE_CENTER = 1, //中间大窗
   WND_TYPE_TOP = 2,    //顶端小窗
}WND_TYPE;

//修改设备
struct STRU_MAINUI_DEVICE_MODIFY {
   DeviceInfo srcDevice;
   DeviceInfo desDevice;
   DataSourcePosType posType;
public:
   STRU_MAINUI_DEVICE_MODIFY();
};

//设备全屏
struct STRU_MAINUI_DEVICE_FULL {
   DeviceInfo m_deviceInfo;
   DataSourcePosType posType;
public:
   STRU_MAINUI_DEVICE_FULL();
};

struct STRU_MAINUI_CHECKSTATUS {
   enum_checkbox_status status;
public:
   STRU_MAINUI_CHECKSTATUS();
};

struct STRU_MAINUI_DELETECAMERA{
   HWND hwnd;
public:
   STRU_MAINUI_DELETECAMERA();
};




#endif //MESSAGE_MAINUI_DEFINE_H
