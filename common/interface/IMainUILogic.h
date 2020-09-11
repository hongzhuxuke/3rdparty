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
   @brief 打开文件播放。
   @details 开始播放本地音频\视频文件，播放文件前，最好先调用isValidMediaFile()检查文件的可用性。
   @param [in] szMediaFile 文件路径(可以是本地文件路径，也可以是一个网络文件的url);
   @remark
   1、支持的文件类型:<br/>
   *.aac,*.ac3,*.amr,*.ape,*.mp3,*.flac,*.midi,*.wav,*.wma,*.ogg,*.amv,
   *.mkv,*.mod,*.mts,*.ogm,*.f4v,*.flv,*.hlv,*.asf,*.avi,*.wm,*.wmp,*.wmv,
   *.ram,*.rm,*.rmvb,*.rpm,*.rt,*.smi,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,*.m2v,
   *.mp2v, *.tp,*.tpr,*.ts,*.m4b,*.m4p,*.m4v,*.mp4,*.mpeg4,*.3g2,*.3gp,*.3gp2,
   *.3gpp,*.mov,*.pva,*.dat,*.m1v,*.m2p,*.m2t,*.m2ts,*.m2v,*.mp2v,*.pss,*.pva,
   *.ifo,*.vob,*.divx,*.evo,*.ivm,*.mkv,*.mod,*.mts,*.ogm,*.scm,*.tod,*.vp6,*.webm,*.xlmv。<br/>
   2、目前sdk会对大于640*480的视频裁剪到640*480;<br/>
   3、文件播放和系统声音采集不应该同时打开，否则文件播放的声音又会被系统声音采集到，出现重音现象;
   @note
   屏幕分享和播片功能都是通过辅路流传输，所以屏幕分享和播片互斥使用;
   辅路流被自己占用，设备操作回调中，返回错误码AV_ERR_EXCLUSIVE_OPERATION(错误码见github上的错误码表);
   被房间内其他成员占用，设备操作回调中，返回错误码AV_ERR_RESOURCE_IS_OCCUPIED(错误码见github上的错误码表);
   */
   virtual int OpenPlayMediaFile(const char*  szMediaFile,int liveType) = 0;
   /**
   @brief 关闭文件播放。
   */
   virtual void ClosePlayMediaFile(int liveType) = 0;
   /**
   @brief 从头播放文件。
   @return 操作结果，NO_ERR表示无错误。
   @note 只有在处于播放状态下(E_PlayMediaFilePlaying)，此接口才有效，否则返回ERR_WRONG_STATE;
   */
   virtual int RestartMediaFile(int liveType) = 0;
   /**
   @brief 暂停播放文件。
   @return 操作结果，NO_ERR表示无错误。
   */
   virtual int PausePlayMediaFile(int liveType) = 0;
   /**
   @brief 恢复播放文件。
   @return 操作结果，NO_ERR表示无错误。
   */
   virtual int	ResumePlayMediaFile(int liveType) = 0;
   /**
   @brief 设置播放文件进度。
   @param [in] n64Pos 播放位置(单位: 秒)
   @return 操作结果，NO_ERR表示无错误。
   */
   virtual int SetPlayMediaFilePos(const signed long long& n64Pos,int liveType) = 0;
   /**
   @brief 获取播放文件进度。
   @param [out] n64Pos 当前播放位置(单位: 秒)
   @param [out] n64MaxPos 当前所播放文件的总长度(单位: 秒)
   @return 操作结果，NO_ERR表示无错误。
   */
   virtual int GetPlayMediaFilePos(signed long long& n64Pos, signed long long& n64MaxPos, int liveType) = 0;
   /**
   @brief 获取当前文件播放状态。
   @return 当前文件播放状态.
   */
   virtual int GetPlayFileState(int liveType) = 0;

	virtual int SetPlayMediaFileVolume(int vol, int liveType) = 0;
};
#endif // __I_MAINUI_LOGIC__H_INCLUDE__ 
