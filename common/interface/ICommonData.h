#ifndef __I_COMMON_DATA_H__
#define __I_COMMON_DATA_H__

#include "VH_IUnknown.h"
#include <string>
#include "PublishInfo.h"
using namespace std;

//桌面启动登录响应信息
class LoginRespInfo {
public:
   LoginRespInfo() {

   }
   LoginRespInfo& operator=(const LoginRespInfo& right) {
      this->plugins_url = right.plugins_url;
      this->interact_plugins_url = right.interact_plugins_url;
      this->list_url = right.list_url;
      this->chat_url = right.chat_url;
      this->avatar = right.avatar;
      this->nick_name = right.nick_name;
      this->userid = right.userid;
      this->phone = right.phone;
      this->token = right.token;
      return *this;
   }
public:
   QString plugins_url;             //直播文档插件页面url
   QString interact_plugins_url;    //互动文档插件页面url
   QString list_url;                //活动列表url
   QString chat_url;                //聊天嵌入url
   QString avatar;                  //头像地址url
   QString token;                   //用户token
   QString nick_name;               //昵称
   QString userid;                  //用户id
   QString phone;                   //用户电话号码
};

// 公共数据
// {E17F81D0-3309-40B2-8474-42CCB1EB77EE}
DEFINE_GUID(IID_ICommonData,
            0xe17f81d0, 0x3309, 0x40b2, 0x84, 0x74, 0x42, 0xcc, 0xb1, 0xeb, 0x77, 0xee);

//启动方式
enum eStartMode
{
	eStartMode_desktop = 0,	//桌面启动
	eStartMode_private,			//私有协议
	eStartMode_flash,				//flash
	eStartMode_flashNoDispatch
};

class ICommonData : public VH_IUnknown {
public:
   virtual void STDMETHODCALLTYPE SetLoginRespInfo(const LoginRespInfo& loginRespInfo) = 0;
   virtual LoginRespInfo STDMETHODCALLTYPE GetLoginRespInfo() = 0;

   virtual BOOL STDMETHODCALLTYPE SetStreamInfo(PublishInfo* apStreamInfo) = 0;
   virtual BOOL STDMETHODCALLTYPE GetStreamInfo(PublishInfo& oStreamInfo) = 0;

   virtual void STDMETHODCALLTYPE SetCurVersion(wstring wsCurVersion) = 0;
   virtual void STDMETHODCALLTYPE GetCurVersion(wstring& wsCurVersion) = 0;

   virtual void STDMETHODCALLTYPE SetPublishState(bool bStart) = 0;
   virtual bool STDMETHODCALLTYPE GetPublishState() = 0;

   virtual void STDMETHODCALLTYPE GetOutputInfo(void *) = 0;
   virtual void STDMETHODCALLTYPE SetOutputInfo(void *) = 0;

   //virtual wstring STDMETHODCALLTYPE GetAppPath() = 0;
   virtual long STDMETHODCALLTYPE GetReportID() = 0;

   virtual void STDMETHODCALLTYPE ReportEvent(string wsEvent) = 0;
   virtual string STDMETHODCALLTYPE GetEvents() = 0;

   //virtual void STDMETHODCALLTYPE SetStartUpMode(const wchar_t *mode) = 0;
   //virtual void STDMETHODCALLTYPE GetStartUpMode(wchar_t *mode) = 0;
   virtual void STDMETHODCALLTYPE SetStartMode(const int startMode) = 0;
   virtual int STDMETHODCALLTYPE GetStartMode() = 0;
   /**
   * 说明：设置聊天过滤权限,嘉宾端不显示聊天过滤按钮。
   *
   * 参数：bEnable: false 不显示聊天过滤按键。
   */
   virtual void SetShowChatFilterBtn(bool bEnable) = 0;
   virtual bool IsShowChatFilterBtn() = 0;
   /**
   * 说明：设置本地用户是否可以踢出、禁言用户列表中成员。
   *
   * 参数：当bEnable=false，成员列表中的成员不显示踢出、禁言按钮。
   */
   virtual void SetMembersManager(bool bEnable) = 0;
   virtual bool GetMembersManagerState() = 0;
   /**
   * 说明：设置本次连麦是否显示公告。
   *
   * 参数：bEnable = false 时在下拉菜单中不显示公告一项。
   */
   virtual void SetWebinarNotice(bool bEnable) = 0;
   virtual bool GetWebinarNoticeState() = 0;
   /**
   * 说明：设置本次连麦是否显示全员禁言。
   *
   * 参数：bEnable = false 时在下拉菜单中不显示公告一项。
   */
   virtual void SetShowChatForbidBtn(bool bEnable) = 0;
   virtual bool IsShowChatForbidBtn() = 0;
   /**
   *说明：无极版用户（普通版），聊天列表区域不显示【聊天过滤】按钮
   */
   virtual void SetLoginUserHostType(int type) = 0;
   virtual int GetLoginUserHostType() = 0;
   /**
   * 说明：设置当前直播类型
   * 参数：0，直播；1：互动直播；2：口令互动直播 3:vhall互动
   */
   virtual void SetLiveType(int type) = 0;
   virtual int GetLiveType() = 0;

   virtual void SetUserID(const char *id) = 0;
   virtual char* GetUserID() = 0;
   /**
   * 设置打点录制当前http请求的处理状态。由于打点录制不同连续抛出相同的状态请求，所以需要本地控制一下。
   */
   virtual void SetCutRecordState(int state) = 0;
   virtual int GetCurRecordState() = 0;
};
#endif // __I_COMMON_DATA_H__ 
