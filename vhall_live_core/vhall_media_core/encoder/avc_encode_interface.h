//
//  VACEncodeInterface.hpp
//  VinnyLive
//
//  Created by ilong on 2016/11/7.
//  Copyright © 2016年 vhall. All rights reserved.
//

#ifndef VACEncodeInterface_hpp
#define VACEncodeInterface_hpp

#include <stdio.h>
#include <live_sys.h>
#include <stdint.h>
#include <live_get_status.h>

struct LivePushParam;
struct LiveExtendParam;

class AVCEncodeInterface:public LiveGetStatus
{
public:
    AVCEncodeInterface(){};
    virtual ~AVCEncodeInterface(){};
    virtual bool Init(LivePushParam * param) = 0;
    virtual int Encode(const char * indata,
                int insize,
                char * outdata,
                int * p_out_size,
                int * p_frame_type,
                uint32_t in_ts,
                uint32_t *out_ts,LiveExtendParam *extendParam=NULL) = 0;
   virtual bool GetSpsPps(char*data,int *size) = 0;
   virtual bool RequestKeyframe() = 0;
};

#endif /* VACEncodeInterface_hpp */
