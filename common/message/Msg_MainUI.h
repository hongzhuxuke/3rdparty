#ifndef MESSAGE_MAINUI_DEFINE_H
#define MESSAGE_MAINUI_DEFINE_H

#include "VH_ConstDeff.h"


enum MSG_MAINUI_DEF {
   ///////////////////////////////////�����¼�///////////////////////////////////
   MSG_MAINUI_WIDGET_SHOW = DEF_RECV_MAINUI_BEGIN,	// ��ʾWidget
   MSG_MAINUI_CLICK_CONTROL,		                  // CLICK����
   MSG_MAINUI_CLICK_ADD_WNDSRC,		               // ������Դ
   MSG_MAINUI_LIST_CHANGE,		                     // �б�ı�
   MSG_MAINUI_VOLUME_CHANGE,		                  // �����ı�
   MSG_MAINUI_NOISE_VALUE_CHANGE,                  // �޸Ľ��뷧ֵ
   MSG_MAINUI_SHOW_AUDIOSETTING,		               // �߼���������
   MSG_MAINUI_CLOSE_SYS_SETTINGWND,                // ֪ͨ�ر����ô���
   MSG_MAINUI_MUTE,		                           // ��������
   MSG_MAINUI_VEDIOPLAY_PLAY,		                  // �岥����/��ͣ
   MSG_MAINUI_VEDIOPLAY_STOPPLAY,		            // ֹͣ�岥
   MSG_MAINUI_VEDIOPLAY_ADDFILE,		               // ����ļ�
   MSG_MAINUI_VEDIOPLAY_SHOW,		                  // ��ʵ�岥
   MSG_MAINUI_MAINUI_MOVE,                         //�����ƶ�
   MSG_MAINUI_MAINUI_PLAYLISTCHG,                  //�����б�ı�
   //MSG_MAINUI_SELECT_PATH,                         //ѡ�񱣴�·��
   MSG_MAINUI_SAVE_SETTING,                        //���ñ���
   MSG_MAINUI_MODIFY_TEXT,                         //�޸��ı�
   MSG_MAINUI_MGR_CAMERA,                          //�������ͷ
   MSG_MAINUI_MGR_CLOSEWND,                        //�������ͷ�رմ���
   MSG_MAINUI_MODIFY_CAMERA,                       //�޸�����ͷ
   MSG_MAINUI_MODIFY_SAVE,                         //�޸ı���
   
   //MSG_MAINUI_CAMERA_CHANGE,                     //����ͷ�л�
   
   MSG_MAINUI_MODIFY_IMAGE,                        //�޸�ͼƬ

   
   MSG_MAINUI_CAMERA_SELECT,                       //�����豸ѡ��
   MSG_MAINUI_CAMERA_FULL,                         //�����豸ȫ��
   MSG_MAINUI_CAMERA_DEVICECHG,                    //�豸�л�(�޸��д���)
   //MSG_MAINUI_CAMERA_OPTION_SAVE,                  //�豸ѡ���
   MSG_MAINUI_CAMERA_UPDATE_CACHE,                 //���������豸����
   MSG_MAINUI_CAMERA_SETTING,                      //����ͷ����
   MSG_MAINUI_CAMERA_DELETE,                       //ɾ������ͷ
   
   MSG_MAINUI_DEVICE_CHANGE,                       //Ӳ���豸�ı�(��/��)
   MSG_MAINUI_DELETE_MONITOR,                       //ֹͣ���湲��
   MSG_MAINUI_SETTING_CLOSE,                       //�ر�ϵͳ���ô���
  // MSG_MAINUI_SETTING_OPENCURDIR,                  //�򿪵�ǰĿ¼
   MSG_MAINUI_SETTING_CONFIRM,                     //����ȷ��
   MSG_MAINUI_SETTING_FOCUSIN,                     //�����ڵõ�����

   MSG_MAINUI_DO_CLOSE_MAINWINDOW,                 //�ر�������

   MSG_MAINUI_SHOW_AUDIO_SETTING_CARD,             //������Ƶ���ý���
   MSG_MAINUI_HIDE_AREA,                           //�������������

   MSG_MAINUI_DESKTOP_SHOW_CAMERALIST,             //��ʾ����ͷ�б�
   MSG_MAINUI_DESKTOP_SHOW_DELCAMERA,             //�Ƴ�����ͷ
   MSG_MAINUI_DESKTOP_SHOW_SETTING,                //��ʾ����
   //MSG_MAINUI_DESKTIO_SHOW_CHAR,                   //��ʾ����
   MSG_MAINUI_RECORD_CHANGE,                   //¼��״̬�ı�
   MSG_MAINUI_SHOW_LOGIN,                       //��ʾ��½����
   MSG_MAINUI_HTTP_TASK,                        //http�������
	MSG_MAINUI_CLOSE_VHALL_ACTIVE,					//�ر�΢�𻥶�
   MSG_MAINUI_LOG,
   MSG_MAINUI_VHALL_ACTIVE_EVENT,                //�����¼���
   MSG_MAINUI_VHALL_ACTIVE_DEVICE_SELECT,
   MSG_MAINUI_VHALL_ACTIVE_SocketIO_MSG,
};

struct STRU_VHALL_ACTIVE_EVENT {
   bool mbHasVideo;//����ֱ�����Ƿ�����Ƶ����
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

// ��ʾwidget
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
	EXIT_KICKOUT = 1,	  //���߳�
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
   control_ShowSetting = 0,     //����
   control_Minimize,            //��С��
   control_CloseApp,            //�ر�

   control_CaptureSrc,          //����Դ
   control_AddCamera,           //�������ͷ
   control_MultiMedia,          //��ý��
   control_Record,              //¼��
   control_StartLive,           //����         ExtraData(0����ֹͣ���� 1����ʼ����) 
   control_LiveTool,            //ֱ�����
                                                                      
   //����Դ������(��չ��)
   control_MonitorSrc = 100,	//���湲��
   control_WindowSrc,				//�������
   control_RegionShare,			//������ʾ

   //��ý�������(��չ��)
   control_VideoSrc,
   control_AddImage,
   control_AddText,
   control_VoiceTranslate,
   //¼����չ����
   control_StartRecord,
   control_RecordSuspendOrRecovery

};

//���������¼�ƵĲ���
enum eRecordReTyp
{
	eRecordReTyp_Start = 1,	        //��ʼ  /�ָ�
	eRecordReTyp_Suspend = 2,	    //��ͣ
	eRecordReTyp_Stop =3,			//����
};

enum enum_exit_reason {
   reason_none = 0,
   reason_kickOut = 1,//���������߳���
};

// �������
struct STRU_MAINUI_CLICK_CONTROL {
   enum_control_type m_eType;
   enum_exit_reason m_eReason;
   DWORD_PTR m_dwExtraData;          //��������
   int m_globalX;
   int m_globalY;
   bool m_bIsRoomDisConnect;   //���ڻ���ֱ����SDK����˿ڴ���
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

// �б�ı�
struct STRU_MAINUI_LIST_CHANGE {
   enum_change_type m_eType;
   int m_nIndex;
public:
   STRU_MAINUI_LIST_CHANGE();
};

// �����ı�
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

// ����
struct STRU_MAINUI_MUTE {
   enum_mute_type m_eType;
   BOOL m_bMute;
public:
   STRU_MAINUI_MUTE();
};

// �岥���ŵ��
struct STRU_MAINUI_PLAY_CLICK {
   BOOL m_bIsPlay;
public:
   STRU_MAINUI_PLAY_CLICK();
};

// ����ͷ����
struct STRU_MAINUI_CAMERACTRL {
   enum_device_operator m_dwType;         //1��� 2ɾ��
   DeviceInfo m_Device;
   bool m_bFullScreen;         //ȫ��
public:
   STRU_MAINUI_CAMERACTRL();
};

// �豸�ı�
struct STRU_MAINUI_DEVICE_CHANGE {
   WCHAR m_wzDeviceID[MAX_DEVICE_ID_LEN + 1];
   BOOL m_bAdd;         //�����豸��FALSEΪ�γ�
public:
   STRU_MAINUI_DEVICE_CHANGE();
};

typedef enum {
   WND_TYPE_NONE = 0,
   WND_TYPE_CENTER = 1, //�м��
   WND_TYPE_TOP = 2,    //����С��
}WND_TYPE;

//�޸��豸
struct STRU_MAINUI_DEVICE_MODIFY {
   DeviceInfo srcDevice;
   DeviceInfo desDevice;
   DataSourcePosType posType;
public:
   STRU_MAINUI_DEVICE_MODIFY();
};

//�豸ȫ��
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
