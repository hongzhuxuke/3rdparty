//
//  dispatch_switch.hpp
//  VhallLiveApi
//
//  Created by ilong on 2017/11/27.
//  Copyright © 2017年 vhall. All rights reserved.
//

#ifndef dispatch_switch_h
#define dispatch_switch_h

#include <stdio.h>
#include <string>
#include "../common/live_sys.h"
#include "../common/live_define.h"
#include <vector>

typedef enum{
   kVHallConnectError = ERROR_WATCH_CONNECT,
   kVHallStartBuffering = START_BUFFERING,
   kVHallStopBuffering = STOP_BUFFERING,
   kVHallPlayError = ERROR_RECV,
}VHallPlayEvent;

namespace talk_base {
   class Thread;
}

namespace VHJson {
   class json;
}

class DispatchSwitch {
   
public:
   typedef enum {
      AUTO_SWITCH_ORIGINAL_RESOLUTION = 0,
      NO_SWITCH_ORIGINAL_RESOLUTION = 1,
   }DispatchSwitchModel;
   
   class Delegate {
      
   public:
      Delegate(){};
      virtual ~Delegate(){};
      virtual void OnStartWithUrl(const std::string &url, const std::string &dispatch_resolution) = 0;
      virtual void OnDispatchWithUrl(const std::string &url) = 0;
      virtual void OnStopPlay() = 0;
      virtual void OnSupportResolutions(const std::string &resolution) = 0;
      virtual void OnPlayEvent(const int code, const std::string &content) = 0;//主线程
      virtual int GetBufferTimeSec() = 0; // s主线程
      virtual std::string GetDefaultPlayUrls() = 0;
      virtual std::string GetDispatchUrl() = 0;//主线程
   };
   
   DispatchSwitch();
   ~DispatchSwitch();
   int Init(DispatchSwitchModel dispatchSwitchModel = AUTO_SWITCH_ORIGINAL_RESOLUTION);
   void SetDelegate(Delegate *delegate);
   int  Start(const std::string &dispatch_resolution="", const std::string &protocol_url="");
   void Stop();
   void PushMsgInfo();
   void SetPlayEvent(const int code, const std::string &content); //see VHallPlayEvent
   int  SetDispatchData(const std::string &data);
   void Reset();
private:
   VH_DISALLOW_COPY_AND_ASSIGN(DispatchSwitch);
   void OnSetPlayEvent(const int code, const std::string &content);
   int StartWithResolution(const std::string &resolution);
   std::string GetNextPlayUrl();
   int StartWithDispatchData(const std::string *data);
   int  SwitchPlayCDN();
   void StopBufferTimeCheck();
   bool HasSameResolution();
private:
   enum{
      MSG_STOP_BUFFER_CHECK_TIME,
      MSG_HTTP_REQUEST,
      MSG_START_PLAY,
      MSG_SWITCH_RESOLUTION,
      MSG_SWITCH_PLAY_CDN,
      MSG_START_DISPATCH_DATA,
      MSG_EVENT,
   };
   Delegate *mDelegate;
   talk_base::Thread  *mWorkThread;
   class WorkMessageHandler;
   WorkMessageHandler *mMessageHandler;
   std::string        mDispatchUrl;
   vhall_lock_t       mMutex;
   int                mUrlListIndex;
   uint64_t           mDispatchTime;
   int                mMsgCount;
   std::atomic_bool   mIsBufferStop;
   VHJson::Value      *mUrls;
   std::string        mDispatchResolution;
   std::vector<std::string> mResolutionList;
   std::vector<std::string> mUrlList;
   std::string              mTokenStr;
   std::string              mCurrentUrl;
   std::string              mProtocolUrl; //协议url信息
   DispatchSwitchModel      mDispatchSwitchModel;
};

#endif /* dispatch_switch_hpp */
