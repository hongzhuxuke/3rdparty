#include "vhall_live_player.h"
#include "../demuxers/http_flv_demuxer.h"
#include "../common/vhall_log.h"
#include "../demuxers/rtmp_reader.h"
#include "../decoder/media_decode.h"
#include "media_render.h"

#define MAX_ENCODED_PKT_NUM       100

VHallLivePlayer::VHallLivePlayer(){
   mStart = false;
   mRtmpReader = NULL;
   mMediaDecode = NULL;
   mMediaRender = NULL;
   mParam = NULL;
   mObs = NULL;
   mPlayerInit = false;
   mPreMuxerType = MUXER_NONE;
   Init();
}

VHallLivePlayer::~VHallLivePlayer() {
   Stop();
   Destory();
   mPlayerInit = false;
}

int VHallLivePlayer::LiveSetParam(LivePlayerParam *param){
   if (param) {
      mParam = param;
      return 0;
   }
   return -1;
}

void VHallLivePlayer::AddObs(LiveObs * obs){
   mObs = obs;
}

void VHallLivePlayer::NotifyEvent(const int type, const EventParam &param){
   if (mObs) {
      mObs->OnEvent(type, param.mDesc);
   }
}

int VHallLivePlayer::NotifyVideoData(const char * data, int size, int w, int h){
   int ret = -1;
   if (mObs) {
      ret = mObs->OnRawVideo(data, size, w, h);
   }
   return ret;
}

int VHallLivePlayer::NotifyAudioData(const char * data, int size){
   int ret = -1;
   if (mObs) {
      ret = mObs->OnRawAudio(data, size);
   }
   return ret;
}

int VHallLivePlayer::OnHWDecodeVideo(const char *data, int size, int w, int h, int64_t ts){
   int ret = -1;
   if (mObs) {
      ret =  mObs->OnHWDecodeVideo(data, size, w, h, ts);
   }
   return ret;
}

int VHallLivePlayer::NotifyJNIDetachVideoThread(){
   int ret = -1;
   if (mObs) {
      ret =  mObs->OnJNIDetachVideoThread();
   }
   return ret;
}

int VHallLivePlayer::NotifyJNIDetachAudioThread(){
   int ret = -1;
   if (mObs) {
      ret =  mObs->OnJNIDetachAudioThread();
   }
   return ret;
}

LivePlayerParam * VHallLivePlayer::GetParam() const{
   return mParam;
}

void VHallLivePlayer::SetPlayerBufferTime(int time){
   mPlayerBufferTime = time;
}

int VHallLivePlayer::GetPlayerBufferTime(){
   return mPlayerBufferTime;
}

DecodedVideoInfo * VHallLivePlayer::GetHWDecodeVideo(){
   DecodedVideoInfo* info = NULL;
   if(mObs){
      info = mObs->GetHWDecodeVideo();
   }
   return info;
}

void VHallLivePlayer::SetBufferTime(const int& bufferTime){
   if (mMediaDecode) {
      uint64_t maxBufferTimeInMs = bufferTime*1000;
      maxBufferTimeInMs = MAX(MIN_BUFFER_TIME, maxBufferTimeInMs);
      maxBufferTimeInMs = MIN(maxBufferTimeInMs, MAX_BUFFER_TIME);
      mMediaDecode->SetMaxBufferTimeInMs(maxBufferTimeInMs);
   }
}

void VHallLivePlayer::Stop(){
   mStart = false;
   if(mRtmpReader){
      mRtmpReader->Stop();
   }
   
   if(mMediaDecode)
      mMediaDecode->Destory();
   
   if(mMediaRender)
      mMediaRender->Destory();
}

bool VHallLivePlayer::Start(const char *url){
   mStart = true;
   mRtmpReader->Start(url);
   return true;
}

void VHallLivePlayer::Init(){
   try {
      mMediaDecode = new MediaDecode(this);
      mMediaRender = new MediaRender(this);
      mPlayerInit = true;
   } catch (std::exception& e) {
      mPlayerInit = false;
   }
}

void VHallLivePlayer::SetDemuxer(VHMuxerType muxer_type){
   if (mPreMuxerType != muxer_type) {
       VHALL_DEL(mRtmpReader);
      if (RTMP_MUXER == muxer_type) {
         RtmpReader *reader = new RtmpReader(this);
         mRtmpReader = static_cast<DemuxerInterface*>(reader);
         mPreMuxerType = muxer_type;
      }else if (HTTP_FLV_MUXER == muxer_type){
         HttpFlvDemuxer *reader = new HttpFlvDemuxer(this);
         mRtmpReader = static_cast<DemuxerInterface*>(reader);
         mPreMuxerType = muxer_type;
      }
      mPreMuxerType = muxer_type;
   }
   mRtmpReader->SetParam(mParam);
   mRtmpReader->ClearMediaInNotify();
   mRtmpReader->AddMediaInNotify(mMediaDecode);
   mMediaDecode->ClearMediaInNotify();
   mMediaDecode->AddMediaInNotify(mMediaRender);
}

void VHallLivePlayer::Destory() {
   VHALL_DEL(mRtmpReader);
   VHALL_DEL(mMediaDecode);
   VHALL_DEL(mMediaRender);
}
