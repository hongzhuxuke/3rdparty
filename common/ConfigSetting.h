#ifndef UTILSETTING_H
#define UTILSETTING_H

#include <QVariant>
#include <QString>

class ConfigSetting {
public:
   static bool writeValue(const QString& path, const QString& group, const QString& user_key, const QVariant &value);
   //static bool readInit(QString path, QString group, QString user_key, QString &user_value);
   static bool WriteInt(const QString& path, const QString& group, const QString& user_key, const int& value);
   static int ReadInt(const QString& path, const QString& group, const QString& user_key, const int& defaultValue);
   static QString ReadString(const QString& path, const QString& group, const QString& user_key, const QString& defaultValue);
};

#define USER_DEFAULT          "user"
#define GROUP_DEFAULT         "default"
#define KEY_RECORD_MODE       "recode"
#define KEY_RECORD_PATH       "path"
#define KEY_RECORD_FILENAME   "filename"
//#define KEY_RECORD_FILESNUM   "filesnum" //1 ����һ���ļ�  0 ���ɶ���ļ�
//�ֱ���
#define KEY_PUBLISH_QUALITY   "quality"
#define KEY_CHK_VER_URL      "verUrl"
#define KEY_AUDIO_SYNC       "synctime"
#define KEY_CUSTOM_RESOLUTION    "custom_resolution"
#define KEY_CUSTOM_BITRATE       "custom_bitrate"

//Ĭ����·
#define KEY_VIDEO_LINE        "video_line"
#define KEY_BITRATE_360P      "bitrate_360p"
#define KEY_BITRATE_540P      "bitrate_540p"
#define KEY_BITRATE_720P      "bitrate_720p"
#define KEY_BITRATE_1080P     "bitrate_1080p"
#define KEY_BITRATE_AUDO      "bitrate_auto"
#define KEY_AUDO_Enable       "auto_enable"
#define KEY_MENGZHU_APP       "isMengZhu"

#define MEDIA_STREAM_WIDTH       "mediaWidth"
#define MEDIA_STREAM_HEIGHT       "mediaHeight"
#define MEDIA_STREAM_BW      "mediaBW"
#define MEDIA_STREAM_FRAMERATE "mediaFrameRate"

#define DESKTOP_STREAM_WIDTH       "DeskTopWidth"
#define DESKTOP_STREAM_HEIGHT       "DeskTopHeight"
#define DESKTOP_STREAM_BW      "DeskTopBW"
#define DESKTOP_STREAM_FRAMERATE "DeskTopFrameRate"
//////////////////////////////////////////////////////////////////////////
//��Ļ
#define KEY_VT_FONT_SIZE      "vt_font_size"
#define KEY_VT_LANGUAGE       "vt_language"


#define KEY_VCODEC_FPS       "fps"
#define KEY_VCODEC_GOP       "gop"

#define KEY_VCODEC_HIGH_PROFILE_OPEN   "high_codec_open"//����������
#define KEY_AUTO_SPEED              "autospeed"
#define KEY_SWSCALE_TYPE            "scaletype"

#define KEY_CAMERA_RESOLUTION_X     "ResolutionX"
#define KEY_CAMERA_RESOLUTION_Y     "ResolutionY"
#define KEY_CAMERA_FRAMEINTERVAL    "FrameInterval"
#define KEY_CAMERA_DEINTERLACE      "DeinterLace"

//�Ƿ���������
#define KEY_AUDIO_ISNOISE           "isNoise"
#define KEY_AUDIO_NOISE_VALUE       "noiseValue"
#define KEY_AUDIO_KBPS              "kbps"               //��Ƶ����
#define KEY_VHALL_LIVE              "vhall_live"         //΢��ֱ��
#define KEY_VHALL_HELPER            "vhall_helper"
#define KEY_VHALL_ICON_NAME         "icon_name"
#define KEY_VHALL_LOGO_HIDE         "vhall_logo_hide"    //΢��logo�����ֶΣ���ȡ��value=1 ��ʾ��������vhall_logo����
#define KEY_VHALL_USER_REGISTER_HIDE         "hide_user_register"    //��½�����Ƿ������û�ע�����
#define KEY_VHALL_APPUPDATE_ENABLE  "vhall_app_update_enable" //����Ӧ�ø����Ƿ�������ȡ��value=1 ��ʾ��������������
#define KEY_VHALL_HIDE_MEMBER_LIST  "vhall_hide_member_list"   //���س�Ա�б�1 ���ء�0��ʾ
#define KEY_VHALL_SHARE_LIVE_PIC    "vhall_share_pic_downloadurl"   //���ض�ά�����ɵ�ַ
#define KEY_AUDIO_SAMPLERATE        "samplerate"			//��Ƶ������
#define KEY_AUDIO_NOISEREDUCTION    "noisereduction"     //��˷罵��
#define KEY_AUDIO_MICGAIN           "micgain"            //��˷�����
#define KEY_VHALL_ShareURL         "vhallShareUrl"    //��������
#define KEY_VHALL_BU                "bu"    //ƽ̨����

#define KEY_LOGREPORT_URL           "logReportUrl"       //��־�ϱ�·��
//#define KEY_VIDEO_HIGHQUALITYCOD   "highQualityCod"    //����������

//ǿ�Ƶ�����
#define KEY_AUDIO_FORCEMONO         "force_mono"

//�Զ����ɻط�
#define KEY_SERVER_PLAYBACK         "server_playback"
//#define KEY_AUDIODEVICE_INFO  "audiodeviceinfo"

#define DEFAULT_VCODEC_FPS   25
#define DEFAULT_VCODEC_GOP   4
#define DEFAULT_VCODEC_HIGH_PROFILE_OPEN   0

//#define CONFIGPATH QString("./config.ini")
#define CONFIGPATH L"config.ini"
#define VHALL_TOOL_CONFIG   L"vhall_tool_config.ini"

#define CONFIGPATH_DEVICE L"audiodevice.ini"

#define KEY_URL_DOMAIN "domainUrl"

//�������
#define DEBUGFORM "debugform"
#define DEBUGFILE "debugfile"
#define DEBUGJS "debugjs"

//�������
#define PROXY_OPEN "proxy_open"
#define PROXY_HOST "proxy_host"
#define PROXY_PORT "proxy_port"
#define PROXY_USERNAME "proxy_username"
#define PROXY_PASSWORD "proxy_password"
#define PROXY_TYPE   "proxy_type"   //ʹ�õĴ����������� PROXY_CONFIG_TYPE

//��·��չ
#define PROXY_NOLINE "proxy_noLine"

enum eLOG {
	eLOG_Show = 0, //��ʾlog
	eLOG_Hide = 1 //����log
};
typedef enum PROXY_CONFIG_TYPE {
   NoProxyConfig = 0,
   ProxyConfig_Http = 1,        //�ֶ�����http����
   ProxyConfig_Browser = 2      //���������ȡ��������
}_proxy_config_type;

//ʵʱ��Ļ����
#define SUBTITLE_FONT_SIZE       "subtitle_font_size"       //�����С
#define SUBTITLE_FONT_COLOR      "subtitle_font_color"      //������ɫ
#define SUBTITLE_FONT_STYLE      "subtitle_font_style"      //������ʽ

//media_core��־
#define MEDIA_CORE_LOG_LEVEL "media_core_log_level"

//����
#define DEBUG_AUDIO_LISTEN "audio_listen"

//std::wstring	GetAppPath2();

#define X264_EX_PARAM
#define X264_EX_PARAM_VIDEO_PROCESS_FILTERS  "video_process_filters"
#define X264_EX_PARAM_IS_ADJUST_BITRATE      "is_adjust_bitrate"
#define X264_EX_PARAM_IS_QUALITY_LIMITED     "is_quality_limited"
#define X264_EX_PARAM_IS_ENCODER_DEBUG       "is_encoder_debug"
#define X264_EX_PARAM_IS_SAVING_DATA_DEBUG   "is_saving_data_debug"


//Audio CaptureLoop Config
// 0 - 31
#define AUDIO_PRIORITY "audio_priority"
// 1 - 5
#define AUDIO_INTERVAL "audio_interval"

#define AUDIO_DEFAULT_DEVICE "audio_default_device"

//#define AUDIO_BITRATE "audio_bitrate"

#define GET_VERIFY_CODE_TIME "get_verify_code"

#define USER_AUTH_TOKEN "user_auth_token"
#define USER_PWD_LOGIN_CLIENT_UNIQUE   "client_unique"
#define LOG_ID_BASE "logIdBase"
#define DUMP_SERVER_URL "dumpURL"
#define LAST_DUMP_UPLOAD_TIME "dump_upload_time"

#define TENCENT_APPID "appid"
#define TENCENT_SDKAPPID "sdk_appid"
#define TENCENT_BIZID "bizid"
#define TENCENT_ACCOUNT_TYPE "account_type"
//#define RECORD_FILES_NUM "files_num"
//�ı���ʾ
#define  VHALL_LIVE_TEXT      QString::fromWCharArray(L"΢��ֱ��")
#define  VHALL_LIVE_HELPER    QString::fromWCharArray(L"΢������") 

#endif // UTILSETTING_H
