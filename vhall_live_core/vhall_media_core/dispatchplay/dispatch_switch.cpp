//
//  dispatch_switch.cpp
//  VhallLiveApi
//
//  Created by ilong on 2017/11/27.
//  Copyright © 2017年 vhall. All rights reserved.
//

#include "dispatch_switch.h"
#include <talk/base/messagehandler.h>
#include <talk/base/thread.h>
#include <talk/base/asynchttprequest.h>
#include <talk/base/httpcommon-inl.h>
#include <talk/base/sigslot.h>
#include <json/json.h>
#include "../common/live_define.h"
#include "../common/vhall_log.h"
#include "../common/auto_lock.h"
#include "../utility/utility.h"
#include "../common/live_message.h"

#define ORIGINAL_RESOLUTION_KEY "same"
#define AUDIO_RESOLUTION_KEY    "a"
#define SAME_CAROUSEL_COUNT     2 //原画质轮播次数
#define TOKEN_TIMEOUT           5*60*1000 // ms
#define SWITCH_DELAYED_TIME     1000 // ms

class DispatchSwitch::WorkMessageHandler:public talk_base::MessageHandler,public sigslot::has_slots<>{
   
public:
   WorkMessageHandler():mObserver(NULL){
      
   }
   
   ~WorkMessageHandler(){
      
   }
   
   void SetObserver(DispatchSwitch*observer){
      mObserver = observer;
   }
   
private:
   virtual void OnMessage(talk_base::Message* msg) override{
      switch (msg->message_id) {
         case MSG_STOP_BUFFER_CHECK_TIME:{
            if (mObserver) {
               mObserver->StopBufferTimeCheck();
            }
         }
            break;
         case MSG_HTTP_REQUEST:{
            if (mObserver&&mObserver->mDelegate) {
               if (mObserver->mDelegate) {
                  mObserver->mDispatchUrl = mObserver->mDelegate->GetDispatchUrl();
               }
               mObserver->mDelegate->OnDispatchWithUrl(mObserver->mDispatchUrl);
            }
         }
            break;
         case MSG_START_PLAY:{
            StringMessageData * obs = static_cast<StringMessageData*>(msg->pdata);
            mObserver->mDelegate->OnStopPlay();
            mObserver->mDelegate->OnStartWithUrl(obs->str_,mObserver->mDispatchResolution);
         }
            break;
         case MSG_START_DISPATCH_DATA:{
            StringMessageData * obs = static_cast<StringMessageData*>(msg->pdata);
            if (mObserver) {
               int ret = mObserver->StartWithDispatchData(&obs->str_);
               if (ret>=0) {
                  int ret =mObserver->StartWithResolution(mObserver->mDispatchResolution);
                  if (ret>=0) {
                     mObserver->SwitchPlayCDN();
                  }
               }else{
                  mObserver->OnSetPlayEvent(kVHallConnectError, "connect error!");
               }
            }
         }
            break;
         case MSG_SWITCH_RESOLUTION:{
            if (mObserver) {
               StringMessageData * obs = static_cast<StringMessageData*>(msg->pdata);
               int ret =mObserver->StartWithResolution(obs->str_);
               if (ret>=0) {
                  mObserver->SwitchPlayCDN();
               }
            }
         }
            break;
         case MSG_SWITCH_PLAY_CDN:{
            if (mObserver) {
               mObserver->SwitchPlayCDN();
            }
         }
            break;
         case MSG_EVENT:{
            if (mObserver) {
               CodeMessageData * obs = static_cast<CodeMessageData*>(msg->pdata);
               mObserver->OnSetPlayEvent(obs->code_, obs->content_);
            }
         }
            break;
         default:
            break;
      }
      VHALL_DEL(msg->pdata);
   }

private:
   DispatchSwitch *mObserver;
};

DispatchSwitch::DispatchSwitch():
mWorkThread(NULL),
mDelegate(NULL),
mUrls(NULL){
   vhall_lock_init(&mMutex);
   mIsBufferStop = true;
   mDispatchSwitchModel = AUTO_SWITCH_ORIGINAL_RESOLUTION;
   mWorkThread = new(std::nothrow) talk_base::Thread();
   if (mWorkThread==NULL) {
      LOGE("mWorkThread new error!");
   }
   mMessageHandler = new(std::nothrow) WorkMessageHandler();
   if (mMessageHandler) {
      mMessageHandler->SetObserver(this);
   }else{
      LOGE("mMessageHandler new error!");
   }
   mUrlList.clear();
   mResolutionList.clear();
   mDispatchResolution = "480p";
   mProtocolUrl = "rtmp_url";
   mDispatchTime = 0;
   mUrlListIndex = 0;
   mMsgCount = 0;
}

DispatchSwitch::~DispatchSwitch(){
   VHALL_THREAD_DEL(mWorkThread);
   VHALL_DEL(mMessageHandler);
   VHALL_DEL(mUrls);
   vhall_lock_destroy(&mMutex);
}

int DispatchSwitch::Init(DispatchSwitchModel dispatchSwitchModel){
   mDispatchTime = 0;
   mUrlListIndex = 0;
   mMsgCount = 0;
   mDispatchSwitchModel = dispatchSwitchModel;
   Reset();
   return 0;
}

void DispatchSwitch::SetDelegate(Delegate *delegate){
   VhallAutolock lock(&mMutex);
   mDelegate = delegate;
}

int DispatchSwitch::SetDispatchData(const std::string &data){
   if (mWorkThread) {
      mWorkThread->Post(mMessageHandler,MSG_START_DISPATCH_DATA,new StringMessageData(data));
   }
   return 0;
}

void DispatchSwitch::Reset(){
   VhallAutolock lock(&mMutex);
   mIsBufferStop = true;
   mUrlList.clear();
   VHALL_DEL(mUrls);
}

int DispatchSwitch::StartWithResolution(const std::string &resolution){
   bool hasResolution = false;
   for (auto&key:mResolutionList) {
      if (resolution==key) {
         hasResolution = true;
         break;
      }
   }
   if (hasResolution==false&&mResolutionList.size()>0) {
      if (mDispatchSwitchModel == NO_SWITCH_ORIGINAL_RESOLUTION) {
         if (mDelegate) {
            mDelegate->OnPlayEvent(kVHallPlayError, "no matching resolution was found.");
         }
         return -1;
      }
      std::string &resolution = mResolutionList.back();
      if (resolution!=ORIGINAL_RESOLUTION_KEY) {
         for (auto&tmp:mResolutionList) {
            if (tmp!=AUDIO_RESOLUTION_KEY) {
               mDispatchResolution = tmp;
               break;
            }
            mDispatchResolution = tmp;
         }
      }else{
         mDispatchResolution = resolution;
      }
   }else{
      mDispatchResolution = resolution;
   }
   mUrlList.clear();
   if (mUrls&&mUrls->isObject()&&mUrls->isMember(mDispatchResolution.c_str())) {
      for (int i = 0; i<(*mUrls)[mDispatchResolution].size(); i++) {
         std::string::size_type pos=0;
         VHJson::Value & value = (*mUrls)[mDispatchResolution][i];
         if (value.isMember(mProtocolUrl)) {
            std::string url = value[mProtocolUrl].asString();
            if ((pos=url.find("token",0))==std::string::npos) {
               url = url+"?token="+Utility::TokenTransition(mTokenStr);
            }
            mUrlList.push_back(std::move(url));
         }else{
            LOGE("mProtocolUrl not has CDN data!");
         }
      }
   }else if(mUrls&&mUrls->isArray()){
      mDispatchResolution = ORIGINAL_RESOLUTION_KEY;
      for (int i = 0; i<mUrls->size(); i++) {
         std::string::size_type pos=0;
         VHJson::Value & value = (*mUrls)[i];
         if (value.isMember(mProtocolUrl)) {
            std::string url = value[mProtocolUrl].asString();
            if ((pos=url.find("token",0))==std::string::npos) {
               url = url+"?token="+Utility::TokenTransition(mTokenStr);
            }
            mUrlList.push_back(std::move(url));
         }else{
            LOGE("mProtocolUrl not has CDN data!");
         }
      }
   }else{
      LOGE("not has CDN data!");
   }
   return 0;
}

int DispatchSwitch::Start(const std::string &dispatch_resolution, const std::string &protocol_url){
   VhallAutolock lock(&mMutex);
   mIsBufferStop = true;
   if (mWorkThread) {
      if (!mWorkThread->started()) {
         mWorkThread->Start();
      }
      mWorkThread->Restart();
   }
   if (!dispatch_resolution.empty()) {
      mDispatchResolution = dispatch_resolution;
   }
   if (!protocol_url.empty()&&mProtocolUrl!=protocol_url) {
      mProtocolUrl = protocol_url;
      goto Dispatch;
   }
   if (mUrls==NULL) {
      goto Dispatch;
   }
   if (mMsgCount>0||(Utility::GetTimestampMs()-mDispatchTime)>TOKEN_TIMEOUT||(mUrlList.size()<=0||mResolutionList.size()<=0)){
Dispatch:
      mMsgCount = 0;
      mWorkThread->Post(mMessageHandler,MSG_HTTP_REQUEST);
   }else{
      mUrlListIndex = 0;
      mMsgCount = 0;
      mWorkThread->Post(mMessageHandler,MSG_SWITCH_RESOLUTION,new StringMessageData(mDispatchResolution));
   }
   return 0;
}

void DispatchSwitch::Stop(){
   VhallAutolock lock(&mMutex);
   if (mWorkThread) {
      mWorkThread->Clear(mMessageHandler);
      mWorkThread->Stop();
   }
}

void DispatchSwitch::SetPlayEvent(const int code,const std::string &content){
   if (mWorkThread) {
      mWorkThread->Post(mMessageHandler,MSG_EVENT,new CodeMessageData(code,content));
   }
}

void DispatchSwitch::OnSetPlayEvent(const int code, const std::string &content){
   switch (code) {
      case kVHallConnectError:
      case kVHallPlayError:{
         if (mDispatchResolution==ORIGINAL_RESOLUTION_KEY&&mUrlListIndex<mUrlList.size()*SAME_CAROUSEL_COUNT) {
            mWorkThread->PostDelayed(SWITCH_DELAYED_TIME, mMessageHandler,MSG_SWITCH_PLAY_CDN);
            return;
         }else if(mDispatchResolution!=ORIGINAL_RESOLUTION_KEY){
            int coefficient = mDispatchSwitchModel==NO_SWITCH_ORIGINAL_RESOLUTION?SAME_CAROUSEL_COUNT:1;
            if(mUrlListIndex<mUrlList.size()*coefficient){
               mWorkThread->PostDelayed(SWITCH_DELAYED_TIME, mMessageHandler,MSG_SWITCH_PLAY_CDN);
               return;
            }else if(HasSameResolution()&&mDispatchSwitchModel==AUTO_SWITCH_ORIGINAL_RESOLUTION){
               mUrlListIndex = 0;
               mWorkThread->PostDelayed(SWITCH_DELAYED_TIME, mMessageHandler,MSG_SWITCH_RESOLUTION,new StringMessageData(ORIGINAL_RESOLUTION_KEY));
               return;
            }
         }
      }
         break;
      case kVHallStartBuffering:{
         if (mIsBufferStop==false) {
            return;
         }
         mIsBufferStop = false;
         if (mWorkThread&&mDelegate) {
            int delay = mDelegate->GetBufferTimeSec();
            mWorkThread->PostDelayed(MAX(delay, DEFAULT_GOP_TIME)*1000, mMessageHandler,MSG_STOP_BUFFER_CHECK_TIME);
         }
      }
         break;
      case kVHallStopBuffering:{
         mIsBufferStop = true;
         if (mWorkThread) {
            mWorkThread->Clear(mMessageHandler,MSG_STOP_BUFFER_CHECK_TIME);
         }
      }
         break;
      default:
         break;
   }
   if (mDelegate) {
      if (code==kVHallPlayError||code==kVHallConnectError) {
         mDelegate->OnStopPlay();
      }
      mDelegate->OnPlayEvent(code, content);
   }
}

void DispatchSwitch::PushMsgInfo(){
   mMsgCount++;
}

std::string DispatchSwitch::GetNextPlayUrl(){
   if (mUrlList.size()>0) {
      int remainder = mUrlListIndex%mUrlList.size();
      if (remainder<mUrlList.size()) {
         std::string url = mUrlList.at(remainder);
         mUrlListIndex++;
         return std::move(url);
      }
   }
   return "";
}

int DispatchSwitch::StartWithDispatchData(const std::string *data){
   if (data&&data->size()>0) {
      VHJson::Reader reader;
      VHJson::Value root;
      if (!reader.parse(*data, root, false)){
         LOGE("data json pares error!");
         goto fail;
      }
      if (!root.isObject()) {
         LOGE("data json is not object!");
         goto fail;
      }
      VHJson::Value &value = root["code"];
      if ((value.isString()&&atoi(root["code"].asCString())!=200)||(value.isInt()&&value.asInt()!=200)) {
         LOGE("data code is not 200!");
         goto fail;
      }
      if (!root["data"].isMember(mProtocolUrl+"s")) {
         LOGE("data not has rtmp_urls member!");
         goto fail;
      }
      VHALL_DEL(mUrls);
      mUrls = new VHJson::Value(root["data"][mProtocolUrl+"s"]);
      if (mUrls->isNull()) {
         goto fail;
      }
      mTokenStr = root["data"]["token"].asString();
   }else{
      fail:
      if (mDelegate) {
         VHJson::Reader reader;
         VHJson::Value root;
         std::string urls = mDelegate->GetDefaultPlayUrls();
         if (reader.parse(urls, root, false)){
            VHALL_DEL(mUrls);
            mUrls = new VHJson::Value(root);
            if (mUrls&&mUrls->isObject()) {
               VHJson::Value::Members mem = mUrls->getMemberNames();
               if (mem.size()<=0) {
                  mDispatchResolution = ORIGINAL_RESOLUTION_KEY;
               }
            }else{
               mDispatchResolution = ORIGINAL_RESOLUTION_KEY;
            }
         }else{
            LOGI("not has default urls!");
            return -1;
         }
      }
   }
   mDispatchTime = Utility::GetTimestampMs();
   mUrlListIndex = 0;
   mResolutionList.clear();
   if (mUrls&&mUrls->isObject()) {
     mResolutionList = mUrls->getMemberNames();
      VHJson::Value array(VHJson::arrayValue);
      for (auto&key:mResolutionList) {
         array.append(VHJson::Value(key));
      }
      if (mDelegate) {
         VHJson::FastWriter root;
         mDelegate->OnSupportResolutions(root.write(array));
      }
   }else if(mUrls&&mUrls->isArray()){
      VHJson::Value array(VHJson::arrayValue);
      array.append(VHJson::Value(ORIGINAL_RESOLUTION_KEY));
      if (mDelegate) {
         VHJson::FastWriter root;
         mDelegate->OnSupportResolutions(root.write(array));
      }
   }
   return 0;
}

int DispatchSwitch::SwitchPlayCDN(){
   if (mMsgCount>0||(Utility::GetTimestampMs()-mDispatchTime)>TOKEN_TIMEOUT) {
      Start();
   }else{
      mCurrentUrl = GetNextPlayUrl();
      if (!mCurrentUrl.empty()) {
         mWorkThread->Post(mMessageHandler,MSG_START_PLAY,new StringMessageData(mCurrentUrl));
      }else{
         SetPlayEvent(kVHallConnectError, "not has fit url.");
      }
   }
   return -1;
}

void DispatchSwitch::StopBufferTimeCheck(){
   if (mIsBufferStop==false&&mWorkThread) {
      mIsBufferStop = true;
      SetPlayEvent(kVHallPlayError,"buffering timeout");
   }
}

bool DispatchSwitch::HasSameResolution(){
   bool hasResolution = false;
   for (auto&key:mResolutionList) {
      if (ORIGINAL_RESOLUTION_KEY==key) {
         hasResolution = true;
         break;
      }
   }
   return hasResolution;
}
