//
//  Encoder.hpp
//  VinnyLive
//
//  Created by ilong on 2016/11/7.
//  Copyright © 2016年 vhall. All rights reserved.
//

#ifndef Encoder_hpp
#define Encoder_hpp

#include <stdlib.h>
#include <live_get_status.h>
#include <stdint.h>

class LiveStatusListener;
class MediaDataSend;
struct LivePushParam;
struct LiveExtendParam;
class  RateControl;
class EncodeInterface:public LiveGetStatus
{
    
public:
    EncodeInterface(){};
    virtual ~EncodeInterface(){};
    virtual int LiveSetParam(LivePushParam *param) = 0;
    virtual void SetStatusListener(LiveStatusListener * listener) = 0;
    virtual void SetOutputListener(MediaDataSend * outputListener) = 0;
    virtual void EncodeVideo(const char * data, int size, uint64_t timestamp, const LiveExtendParam *extendParam=NULL) = 0;
    virtual void EncodeVideoHW(const char * data, int size ,int type, uint64_t timestamp) = 0;
    virtual void EncodeAudio(const char * data, int size, uint64_t timestamp) = 0;
    virtual void EncodeAudioHW(const char * data, int size, uint64_t timestamp) = 0;
    virtual bool RequestKeyframe() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool isInit() = 0;
    virtual void SetRateControl(RateControl *rateControl) = 0;
};

#endif /* Encoder_hpp */
