#ifndef MESSAGE_OBSCONTROL_DEFINE_H
#define MESSAGE_OBSCONTROL_DEFINE_H

#include "VH_ConstDeff.h"
#include "vhmonitorcapture.h"
#include <string>
#include <windows.h>

using namespace std;

enum MSG_OBSCONTROL_DEF {
   ///////////////////////////////////�����¼�///////////////////////////////////
   MSG_OBSCONTROL_PUBLISH = DEF_RECV_OBSCONTROL_BEGIN,		// ����
   MSG_OBSCONTROL_TEXT,                                     //�ı�����
   MSG_OBSCONTROL_IMAGE,                                     //ͼƬ����
   MSG_OBSCONTROL_DESKTOPSHARE,                             //���湲��
   MSG_OBSCONTROL_WINDOWSRC,                                //���Դ
   MSG_OBSCONTROL_SHAREREGION,                              //������
   MSG_OBSCONTROL_PROCESSSRC,                               //����Դ���
   MSG_OBSCONTROL_ADDCAMERA,                                //�������ͷ

   MSG_OBSCONTROL_AUDIO_NOTIFY,                             //��Ƶ�仯֪ͨ   
   MSG_OBSCONTROL_STREAMSTATUS_CHANGE,                      //��״̬�ı�
   MSG_OBSCONTROL_AUDIO_CAPTURE,                             //��Ƶ����
	MSG_OBSCONTROL_VIDIO_HIGHQUALITYCOD,								//��Ƶ����������
   MSG_OBSCONTROL_STREAM_NOTIFY,                             //����Ϣ
   MSG_OBSCONTROL_STREAM_RESET,                              //����Ϣ
   MSG_OBSCONTROL_STREAM_PUSH_SUCCESS,                       //�����ɹ�
   MSG_OBSCONTROL_PUSH_AMF0,                                //����AMF0��Ϣ
   MSG_OBSCONTROL_VOICE_TRANSITION,                                //����ת���ı�����
   MSG_OBSCONTROL_ENABLE_VT,                                //ʹ��ʵʱ��Ļ
   MSG_OBSCONTROL_CLOSE_AUDIO_DEV,                              //�ر���Ƶ�豸
   //MSG_OBSCONTROL_RECORD_COMMIT,                              //�ύ��ǰ���¼��
}; 


// ����
struct STRU_OBSCONTROL_PUBLISH {
   bool m_bIsStartPublish;
   bool m_bIsSaveFile;
   bool m_bServerPlayback;
   bool m_bExit;
   bool m_bDispatch;
   bool m_bIsCloseAppWithPushing;
   bool m_bInteractive;
   bool m_bMediaCoreReConnect; //true:��ʾ�ײ������쳣��Ҫ���¿�ʼ����
   float m_iQuality;
   wchar_t m_wzSavePath[MAX_PATH_LEN + 1];
   struct Dispatch_Param m_bDispatchParam; 
public:
   STRU_OBSCONTROL_PUBLISH();
};

#define TextLanguage_Mandarin    L"��ͨ��"
#define TextLanguage_Cantonese   L"����"
#define TextLanguage_Lmz         L"�Ĵ���"

typedef enum lan_type{
   LAN_TYPE_PTH = 0, //��ͨ��
   LAN_TYPE_YU = 1,  //����
   LAN_TYPE_SC = 2,  //�Ĵ���
}LAN_TYPE;

struct STRU_VT_INFO {
   bool bEnable;  //ʹ����Ϣ
   int fontSize;  //�����С
   int lan;       //�������ͣ�
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

// ���Դ����
struct STRU_OBSCONTROL_WINDOWSRC {
   VHD_WindowInfo m_windowInfo;
public:
   STRU_OBSCONTROL_WINDOWSRC();
};

// ���������
struct STRU_OBSCONTROL_SHAREREGION {
   RECT m_rRegionRect;
public:
   STRU_OBSCONTROL_SHAREREGION();
};

// ����Դ���
struct STRU_OBSCONTROL_PROCESSSRC {
   DWORD m_dwType;
public:
   STRU_OBSCONTROL_PROCESSSRC();
};

// ��������ͷ
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

// ��Ƶ����
struct STRU_OBSCONTROL_AUDIO_CAPTURE {
   DeviceInfo info;
   bool isNoise;					//������
   int openThreshold;     //����������
   int closeThreshold;    //����������

	int iKbps;						//��Ƶ����
	int iAudioSampleRate; //��Ƶ������
	bool bNoiseReduction; //��         ��
	float fMicGain;				//��˷�����
	//bool bRecord;				//�Ƿ�¼��

public:
   STRU_OBSCONTROL_AUDIO_CAPTURE();
};

// �豸ѡ���
struct STRU_OBSCONTROL_VIDIO_SET {
	DeviceInfo m_DeviceInfo;
	int high_codec_open;  //����������
public:
	STRU_OBSCONTROL_VIDIO_SET();
};

#endif //MESSAGE_OBSCONTROL_DEFINE_H
