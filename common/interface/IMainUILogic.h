#ifndef __I_MAINUI_LOGIC__H_INCLUDE__
#define __I_MAINUI_LOGIC__H_INCLUDE__

#include "VH_IUnknown.h"
#include <QString>
#include <qwidget.h>
// {42021DBD-4314-44D7-A227-C9B85C0A5A9E}
DEFINE_GUID(IID_IMainUILogic,
            0x42021dbd, 0x4314, 0x44d7, 0xa2, 0x27, 0xc9, 0xb8, 0x5c, 0xa, 0x5a, 0x9e);

class IMainUILogic : public VH_IUnknown {
public:
   virtual void STDMETHODCALLTYPE GetMainUIWidget(void** apMainUIWidget,int type = 0) = 0;

   virtual void STDMETHODCALLTYPE GetContentWidget(void** apContentWidget, int type = 0) = 0;

	virtual int STDMETHODCALLTYPE GetMediaPlayUIWidth(int type = 0) = 0;

   virtual void STDMETHODCALLTYPE GetHwnd(void** apMsgWnd, void** apRenderWnd) = 0;  
   
   virtual void STDMETHODCALLTYPE SetTipForceHide(bool) = 0;

   virtual void STDMETHODCALLTYPE SetDesktopSharingUIShow(bool) = 0;
   
   virtual void STDMETHODCALLTYPE GetDesktopSharingUIWinId(HWND *) = 0;
   
   virtual void STDMETHODCALLTYPE SetMicMute(bool bMute) = 0;
   
   virtual void STDMETHODCALLTYPE SetSpeakerMute(bool bMute) = 0;

   virtual void STDMETHODCALLTYPE AddImageWidget(void *) = 0;
   
   virtual void STDMETHODCALLTYPE RemoveImageWidget(void *) = 0;
   
   virtual int STDMETHODCALLTYPE GetVHDialogShowCount() = 0; 
   
   virtual void STDMETHODCALLTYPE FadoutTip(wchar_t *) = 0;
      
   virtual void STDMETHODCALLTYPE UpdateVolumn(int) = 0;

   virtual bool STDMETHODCALLTYPE ISHasActivityList() = 0;

   //virtual void STDMETHODCALLTYPE AddRightExWidget(void *,bool fromShareUi = false) = 0;

   virtual void STDMETHODCALLTYPE SetShrink(bool) = 0;
   
   virtual void * STDMETHODCALLTYPE GetLiveToolWidget() = 0;

   virtual void STDMETHODCALLTYPE KitoutEvent(bool , wchar_t *userId,wchar_t *role) = 0;

   virtual void STDMETHODCALLTYPE OnlineEvent(bool , wchar_t *userId,wchar_t *role,wchar_t *name, int synType) = 0;

   virtual bool STDMETHODCALLTYPE IsShowMainWidget() = 0;

   virtual void STDMETHODCALLTYPE RemoveExtraWidget() = 0;

   virtual void STDMETHODCALLTYPE SetHttpProxy(const bool enable = false, const char* ip = NULL,const char* port = NULL,const char* userName = NULL,const char* pwd = NULL) = 0;

   virtual void * STDMETHODCALLTYPE GetParentWndForTips() = 0;

   virtual void STDMETHODCALLTYPE EnableVoiceTranslate(bool enable = false) = 0;
   virtual bool STDMETHODCALLTYPE GetVoiceTranslate() = 0;

   virtual void STDMETHODCALLTYPE OpenVoiceTranslateFun(bool open = false) = 0;
   virtual bool STDMETHODCALLTYPE IsOpenVoiceTranslateFunc() = 0;
   virtual void STDMETHODCALLTYPE SetVoiceTranslateFontSize(int size) = 0;

   virtual bool STDMETHODCALLTYPE IsShareDesktop() = 0;
   virtual bool STDMETHODCALLTYPE IsInteractive() = 0;
   //virtual QString STDMETHODCALLTYPE CommitString() = 0;
   virtual QString STDMETHODCALLTYPE StrPointURL() = 0;
   virtual bool STDMETHODCALLTYPE IsStopRecord() = 0;
   virtual void STDMETHODCALLTYPE SetRecordState(const int iState) = 0;
   virtual void STDMETHODCALLTYPE SetCutRecordDisplay(const int iCutRecord) = 0;
   virtual bool STDMETHODCALLTYPE IsRecordBtnhide() = 0;
   virtual void STDMETHODCALLTYPE SetVedioPlayUi(QWidget* pVedioPlayUI, int iLiveType) = 0;

   virtual bool STDMETHODCALLTYPE IsLoadUrlFinished() = 0;
   virtual char* STDMETHODCALLTYPE GetMsgToken() = 0;
   /**
   @brief ���ļ����š�
   @details ��ʼ���ű�����Ƶ\��Ƶ�ļ��������ļ�ǰ������ȵ���isValidMediaFile()����ļ��Ŀ����ԡ�
   @param [in] szMediaFile �ļ�·��(�����Ǳ����ļ�·����Ҳ������һ�������ļ���url);
   @remark
   1��֧�ֵ��ļ�����:<br/>
   *.aac,*.ac3,*.amr,*.ape,*.mp3,*.flac,*.midi,*.wav,*.wma,*.ogg,*.amv,
   *.mkv,*.mod,*.mts,*.ogm,*.f4v,*.flv,*.hlv,*.asf,*.avi,*.wm,*.wmp,*.wmv,
   *.ram,*.rm,*.rmvb,*.rpm,*.rt,*.smi,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,*.m2v,
   *.mp2v, *.tp,*.tpr,*.ts,*.m4b,*.m4p,*.m4v,*.mp4,*.mpeg4,*.3g2,*.3gp,*.3gp2,
   *.3gpp,*.mov,*.pva,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,*.m2v,*.mp2v,*.pss,*.pva,
   *.ifo,*.vob,*.divx,*.evo,*.ivm,*.mkv,*.mod,*.mts,*.ogm,*.scm,*.tod,*.vp6,*.webm,*.xlmv��<br/>
   2��Ŀǰsdk��Դ���640*480����Ƶ�ü���640*480;<br/>
   3���ļ����ź�ϵͳ�����ɼ���Ӧ��ͬʱ�򿪣������ļ����ŵ������ֻᱻϵͳ�����ɼ�����������������;
   @note
   ��Ļ����Ͳ�Ƭ���ܶ���ͨ����·�����䣬������Ļ����Ͳ�Ƭ����ʹ��;
   ��·�����Լ�ռ�ã��豸�����ص��У����ش�����AV_ERR_EXCLUSIVE_OPERATION(�������github�ϵĴ������);
   ��������������Առ�ã��豸�����ص��У����ش�����AV_ERR_RESOURCE_IS_OCCUPIED(�������github�ϵĴ������);
   */
   virtual int OpenPlayMediaFile(const char*  szMediaFile,int liveType) = 0;
   /**
   @brief �ر��ļ����š�
   */
   virtual void ClosePlayMediaFile(int liveType) = 0;
   /**
   @brief ��ͷ�����ļ���
   @return ���������NO_ERR��ʾ�޴���
   @note ֻ���ڴ��ڲ���״̬��(E_PlayMediaFilePlaying)���˽ӿڲ���Ч�����򷵻�ERR_WRONG_STATE;
   */
   virtual int RestartMediaFile(int liveType) = 0;
   /**
   @brief ��ͣ�����ļ���
   @return ���������NO_ERR��ʾ�޴���
   */
   virtual int PausePlayMediaFile(int liveType) = 0;
   /**
   @brief �ָ������ļ���
   @return ���������NO_ERR��ʾ�޴���
   */
   virtual int	ResumePlayMediaFile(int liveType) = 0;
   /**
   @brief ���ò����ļ����ȡ�
   @param [in] n64Pos ����λ��(��λ: ��)
   @return ���������NO_ERR��ʾ�޴���
   */
   virtual int SetPlayMediaFilePos(const signed long long& n64Pos,int liveType) = 0;
   /**
   @brief ��ȡ�����ļ����ȡ�
   @param [out] n64Pos ��ǰ����λ��(��λ: ��)
   @param [out] n64MaxPos ��ǰ�������ļ����ܳ���(��λ: ��)
   @return ���������NO_ERR��ʾ�޴���
   */
   virtual int GetPlayMediaFilePos(signed long long& n64Pos, signed long long& n64MaxPos, int liveType) = 0;
   /**
   @brief ��ȡ��ǰ�ļ�����״̬��
   @return ��ǰ�ļ�����״̬.
   */
   virtual int GetPlayFileState(int liveType) = 0;

	virtual int SetPlayMediaFileVolume(int vol, int liveType) = 0;
};
#endif // __I_MAINUI_LOGIC__H_INCLUDE__ 
