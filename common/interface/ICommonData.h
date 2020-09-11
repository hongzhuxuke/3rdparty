#ifndef __I_COMMON_DATA_H__
#define __I_COMMON_DATA_H__

#include "VH_IUnknown.h"
#include <string>
#include "PublishInfo.h"
using namespace std;

//����������¼��Ӧ��Ϣ
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
   QString plugins_url;             //ֱ���ĵ����ҳ��url
   QString interact_plugins_url;    //�����ĵ����ҳ��url
   QString list_url;                //��б�url
   QString chat_url;                //����Ƕ��url
   QString avatar;                  //ͷ���ַurl
   QString token;                   //�û�token
   QString nick_name;               //�ǳ�
   QString userid;                  //�û�id
   QString phone;                   //�û��绰����
};

// ��������
// {E17F81D0-3309-40B2-8474-42CCB1EB77EE}
DEFINE_GUID(IID_ICommonData,
            0xe17f81d0, 0x3309, 0x40b2, 0x84, 0x74, 0x42, 0xcc, 0xb1, 0xeb, 0x77, 0xee);

//������ʽ
enum eStartMode
{
	eStartMode_desktop = 0,	//��������
	eStartMode_private,			//˽��Э��
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
   * ˵���������������Ȩ��,�α��˲���ʾ������˰�ť��
   *
   * ������bEnable: false ����ʾ������˰�����
   */
   virtual void SetShowChatFilterBtn(bool bEnable) = 0;
   virtual bool IsShowChatFilterBtn() = 0;
   /**
   * ˵�������ñ����û��Ƿ�����߳��������û��б��г�Ա��
   *
   * ��������bEnable=false����Ա�б��еĳ�Ա����ʾ�߳������԰�ť��
   */
   virtual void SetMembersManager(bool bEnable) = 0;
   virtual bool GetMembersManagerState() = 0;
   /**
   * ˵�������ñ��������Ƿ���ʾ���档
   *
   * ������bEnable = false ʱ�������˵��в���ʾ����һ�
   */
   virtual void SetWebinarNotice(bool bEnable) = 0;
   virtual bool GetWebinarNoticeState() = 0;
   /**
   * ˵�������ñ��������Ƿ���ʾȫԱ���ԡ�
   *
   * ������bEnable = false ʱ�������˵��в���ʾ����һ�
   */
   virtual void SetShowChatForbidBtn(bool bEnable) = 0;
   virtual bool IsShowChatForbidBtn() = 0;
   /**
   *˵�����޼����û�����ͨ�棩�������б�������ʾ��������ˡ���ť
   */
   virtual void SetLoginUserHostType(int type) = 0;
   virtual int GetLoginUserHostType() = 0;
   /**
   * ˵�������õ�ǰֱ������
   * ������0��ֱ����1������ֱ����2�������ֱ�� 3:vhall����
   */
   virtual void SetLiveType(int type) = 0;
   virtual int GetLiveType() = 0;

   virtual void SetUserID(const char *id) = 0;
   virtual char* GetUserID() = 0;
   /**
   * ���ô��¼�Ƶ�ǰhttp����Ĵ���״̬�����ڴ��¼�Ʋ�ͬ�����׳���ͬ��״̬����������Ҫ���ؿ���һ�¡�
   */
   virtual void SetCutRecordState(int state) = 0;
   virtual int GetCurRecordState() = 0;
};
#endif // __I_COMMON_DATA_H__ 
