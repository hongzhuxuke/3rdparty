#include "x264_encoder.h"
#include "../common/vhall_log.h"
#include "../utility/utility.h"
#include <string.h>
#include <stdlib.h>
#include "../common/live_sys.h"
#include "../3rdparty/json/json.h"

#define  MAX_ABR_BITRATE_RATIO 1.5

enum ColorPrimaries {
   ColorPrimaries_BT709 = 1,
   ColorPrimaries_Unspecified,
   ColorPrimaries_BT470M = 4,
   ColorPrimaries_BT470BG,
   ColorPrimaries_SMPTE170M,
   ColorPrimaries_SMPTE240M,
   ColorPrimaries_Film,
   ColorPrimaries_BT2020
};

enum ColorTransfer {
   ColorTransfer_BT709 = 1,
   ColorTransfer_Unspecified,
   ColorTransfer_BT470M = 4,
   ColorTransfer_BT470BG,
   ColorTransfer_SMPTE170M,
   ColorTransfer_SMPTE240M,
   ColorTransfer_Linear,
   ColorTransfer_Log100,
   ColorTransfer_Log316,
   ColorTransfer_IEC6196624,
   ColorTransfer_BT1361,
   ColorTransfer_IEC6196621,
   ColorTransfer_BT202010,
   ColorTransfer_BT202012
};

enum ColorMatrix {
   ColorMatrix_GBR = 0,
   ColorMatrix_BT709,
   ColorMatrix_Unspecified,
   ColorMatrix_BT470M = 4,
   ColorMatrix_BT470BG,
   ColorMatrix_SMPTE170M,
   ColorMatrix_SMPTE240M,
   ColorMatrix_YCgCo,
   ColorMatrix_BT2020NCL,
   ColorMatrix_BT2020CL
};

enum VideoResolution {
   VideoResolution_Error = 0,
   VideoResolution_360p = 360,
   VideoResolution_480p = 480,
   VideoResolution_540p = 540,
   VideoResolution_720p = 720,
   VideoResolution_768p = 768,
   VideoResolution_1080p = 1080,
   VideoResolution_2160p = 2160
};

void get_x264_log(void *param, int i_level, const char *psz, va_list argptr);

NaluUnit* MallocNalu(const int& naluSize){
   NaluUnit* newNaluUnit = NULL;
   newNaluUnit = (NaluUnit*)calloc(1, sizeof(NaluUnit));
   if (newNaluUnit == NULL){
      LOGE("MallocNalu malloc newNaluUnit failed. ");
      return NULL;
   }
   newNaluUnit->size = naluSize;
   newNaluUnit->data = (unsigned char*)malloc(naluSize);
   if (newNaluUnit->data == NULL){
      LOGE("MallocNalu malloc newNaluUnit data failed. ");
      free(newNaluUnit);
      return NULL;
   }
   return newNaluUnit;
}

X264Encoder::X264Encoder():
mX264Encoder(NULL),
mIsPadCBR(false),
mIsUseCFR(true),
mKeyframeInterval(4),
mIsUseCBR(false),
//mYuvBuffer(NULL),
//mPicInBuffer(NULL),
mIsInited(false),
mReconfigType(Reconfig_KeepSame),
mSceneType(SceneType_Unknown)
{
   mFrameCount = 0;
   mFrameFaildCount = 0;
   memset(&mSEINaluUnit, 0, sizeof(NaluUnit));
   memset(&mHeaderUnit, 0, sizeof(NaluUnit));
   mFileH264 = NULL;
   mFileEncodeYUV = NULL;
   mFileFrameInfo = NULL;
}

X264Encoder::~X264Encoder() {
   this->Destroy();
}

void X264Encoder::Destroy() {
   LOGI("X264Encoder::destroy.");
   if(mX264Encoder){
      x264_encoder_close(mX264Encoder);
      mX264Encoder = NULL;
      x264_picture_clean(&mX264PicIn);
   }
   //VHALL_DEL(mYuvBuffer);
   //VHALL_DEL(mPicInBuffer);
   mFrameTimestampQueue.clear();
   if (mFileH264 != NULL){
      fclose(mFileH264);
      mFileH264 = NULL;
   }
   if (mFileEncodeYUV != NULL){
      fclose(mFileEncodeYUV);
      mFileEncodeYUV = NULL;
   }
   if (mFileFrameInfo != NULL){
      fclose(mFileFrameInfo);
      mFileFrameInfo = NULL;
   }
}

bool X264Encoder::Init(LivePushParam * param) {
   mIsInited = false;
   mLiveParam = param;
   int ret = -1;
   this->Destroy();
   LOGI("X264Encoder::Init.");
   //mBitrate = param->bit_rate/1000;
   mOriginWidth = param->width;
   mOriginHeight = param->height;
   mFrameRate = param->frame_rate;
   mWidthMb = (param->width + 15) / 16;
   mHeightMb = (param->height + 15) / 16;
   mFrameNum = 0;
   
   if (param->gop_interval > 0){
      mKeyframeInterval = param->gop_interval;
   }
   
   mSceneType = SceneType_Natural;//default scene type is natural
   
   mIsQualityLimited = param->is_quality_limited;
   mIsAdjustBitrate = param->is_adjust_bitrate;
   mIsRequestKeyframe = false;
   
   //Invalid situation: 1. Height or/and Width is not a even number; 2. Height or/and Width is negative
   if ( mOriginHeight & 1 || mOriginWidth & 1 || mOriginHeight < 0 || mOriginWidth < 0){
      mResolutionLevel = VideoResolution_Error;
      LOGE("x264_encoder : Encoder get an invalid resolution parameter.");
      return false;
   }
   //we only support several typical resolution, other similar type may consider as one of them
   //FIXME:The aspect ratio consider to be only 4:3 or 16:9, and a square bar image may be assigned too much bitrate
   if ((mOriginWidth <= 480 && mOriginHeight <= 360)||(mOriginWidth <= 360 && mOriginHeight <= 480)){
      mResolutionLevel = VideoResolution_360p;
   }
   else if ((mOriginWidth <= 640 && mOriginHeight <= 480)||(mOriginWidth <= 480 && mOriginHeight <= 640)){
      mResolutionLevel = VideoResolution_480p;
   }
   else if ((mOriginWidth <= 960 && mOriginHeight <= 540)||(mOriginWidth <= 540 && mOriginHeight <= 960)){
      mResolutionLevel = VideoResolution_540p;
   }
   else if ((mOriginWidth <= 1280 && mOriginHeight <= 720)||(mOriginWidth <= 720 && mOriginHeight <= 1280)){
      mResolutionLevel = VideoResolution_720p;
   }
   else if ((mOriginWidth <= 1366 && mOriginHeight <= 768)||(mOriginWidth <= 768 && mOriginHeight <= 1366)){
      mResolutionLevel = VideoResolution_768p;
   }
   else if ((mOriginWidth <= 1920 && mOriginHeight <= 1080)||(mOriginWidth <= 1080 && mOriginHeight <= 1920)){
      mResolutionLevel = VideoResolution_1080p;
   }
   //we offer a level set for 4K-Resolution, but performance may be poor
   else if ((mOriginWidth <= 4096 && mOriginHeight <= 2160)||(mOriginWidth <= 2160 && mOriginHeight <= 4096)){
      mResolutionLevel = VideoResolution_2160p;
   }
   //Don't support resolution higher than 4096x2160, considering it invalid
   else{
      mResolutionLevel = VideoResolution_Error;
      LOGW("x264_encoder : Frame size is too large!");
   }
   //high_codec_open setting part 1
   if (param->high_codec_open == 0){
      mQPLimitedLevel = 5;
   }
   else if (param->high_codec_open < 9 && param->high_codec_open > 0){
      mQPLimitedLevel = 4 + (param->high_codec_open - 1) / 3;
   }
   else{
      LOGW("X264Encoder: Param high_codec_open is invalid, please check the configuration");
      mQPLimitedLevel = 5;
   }
   //bitrate setting
   mOriginBitrate = param->bit_rate / 1000;
   if (!BitrateClassify(mOriginBitrate)){
      LOGE("x264_encoder : Input bitrate invalid.");
      return false;
   }
   
   memset(&mX264ParamData, 0, sizeof(mX264ParamData));
   mProfile ="high";
   mPreset ="superfast";
   const char*tune = x264_tune_names[7];
   if (x264_param_default_preset(&mX264ParamData, mPreset.c_str(), tune)){
      LOGE("Failed to set mX264Encoder defaults: %s/%s", mPreset.c_str(), " ");
      return false;
   }
   //vba
   mX264ParamData.b_repeat_headers = 0; //I 有pps sps
   //http://blog.csdn.net/lzx995583945/article/details/43446259
   //mX264ParamData.b_deterministic = false;
   //mX264ParamData.rc.i_vbv_max_bitrate = (int)(mBitrate * MAX_ABR_BITRATE_RATIO); //vbv-maxrate
   // mX264ParamData.rc.i_vbv_buffer_size = mBitrate ; //vbv-bufsize
   if (param->high_codec_open == 9){
      mX264ParamData.rc.i_qp_max = 37;
      mX264ParamData.rc.i_qp_min = 19;
      mX264ParamData.rc.i_vbv_buffer_size = 0;
      mX264ParamData.rc.i_vbv_max_bitrate = 0;
      mQPLimitedLevel = 7;
      
   }
   
   LOGI("X264Encoder::Init. bitrate=%d", (int)mBitrate);
   
   if (mIsUseCBR) {
      if (mIsPadCBR)
         mX264ParamData.rc.b_filler = 1;
      mX264ParamData.rc.i_bitrate = mBitrate;
      mX264ParamData.rc.i_rc_method = X264_RC_ABR;
      mX264ParamData.rc.f_rf_constant = 0.0f;
      mX264ParamData.rc.f_rate_tolerance = MAX_ABR_BITRATE_RATIO;
   } else {
      mReconfigType = Reconfig_Init;
      mX264ParamData.rc.i_rc_method = X264_RC_CRF;
      if (!RateControlConfig()){
         LOGE("Failed to initialize the x264 encoder rate control parameter.");
         return false;
      }
   }
   
   mX264ParamData.b_vfr_input = !mIsUseCFR;
   mX264ParamData.vui.b_fullrange = 0;
   mX264ParamData.vui.i_colorprim = ColorPrimaries_BT709;
   mX264ParamData.vui.i_transfer = ColorTransfer_IEC6196621;
   mX264ParamData.vui.i_colmatrix =
   mOriginWidth >= 1280 || mOriginHeight > 576? ColorMatrix_BT709:ColorMatrix_SMPTE170M;
   mX264ParamData.i_bframe=0;//no b-frame
   mX264ParamData.rc.i_lookahead = 0;// no lookahead
   mX264ParamData.i_keyint_max = mFrameRate*mKeyframeInterval;
   mX264ParamData.i_keyint_min = mFrameRate*mKeyframeInterval;
   mX264ParamData.i_fps_num = mFrameRate;
   mX264ParamData.i_fps_den = 1;
   //mX264ParamData.i_timebase_num = 1;
   //mX264ParamData.i_timebase_den = 1000;
   if (param->is_encoder_debug){
      mX264ParamData.i_log_level = X264_LOG_INFO;
      mX264ParamData.b_save_log = 1;
      mX264ParamData.analyse.b_psnr = 1;
   }
   else{
      mX264ParamData.pf_log = get_x264_log;
      mX264ParamData.i_log_level = X264_LOG_ERROR;
   }
   //high_codec_open setting part 2
   if (param->high_codec_open == 0){
      mX264ParamData.rc.i_qp_max = 37;
      mX264ParamData.rc.i_qp_min = 19;
   }else if (param->high_codec_open < 9 && param->high_codec_open > 0){
      mX264ParamData.rc.i_qp_max = 36 - param->high_codec_open;
      mX264ParamData.rc.i_qp_min = 18 - param->high_codec_open / 3;
   }else{
      LOGW("X264Encoder: Param high_codec_open is invalid, please check the configuration");
      mX264ParamData.rc.i_qp_max = 37;
      mX264ParamData.rc.i_qp_min = 19;
   }
   
   mX264ParamData.b_cabac = 1;
   mX264ParamData.i_scenecut_threshold = 0;
   mX264ParamData.rc.i_aq_mode = 0;
   mX264ParamData.b_sliced_threads = 0;
   mX264ParamData.i_video_preprocess_flag = param->video_process_filters;
   mCurrentProcessFlag = param->video_process_filters;
   
   
   if (strcmp(mProfile.c_str(), "main") == 0)
      mX264ParamData.i_level_idc = 41; // to ensure compatibility with portable devices
   
   if (param->encode_pix_fmt == ENCODE_PIX_FMT_YUV420SP_NV21) {
      mX264ParamData.i_csp = X264_CSP_NV21;
   }else if(param->encode_pix_fmt == ENCODE_PIX_FMT_YUV420SP_NV12){
      mX264ParamData.i_csp = X264_CSP_NV12;
   }else if(param->encode_pix_fmt == ENCODE_PIX_FMT_YUV420P_I420){
      mX264ParamData.i_csp = X264_CSP_I420;
   }else if(param->encode_pix_fmt == ENCODE_PIX_FMT_YUV420P_YV12){
      mX264ParamData.i_csp = X264_CSP_YV12;
   }else{
      mX264ParamData.i_csp = X264_CSP_NV21;
   }
   
   x264_picture_init(&mX264PicIn);
   mX264ParamData.i_width = mOriginWidth;
   mX264ParamData.i_height = mOriginHeight;
   
   if (x264_param_apply_profile(&mX264ParamData, mProfile.c_str())){
      LOGE("Failed to x264_param_apply_profile profile=%s", mProfile.c_str());
      return false;
   }
   
   ret = x264_picture_alloc(&mX264PicIn, mX264ParamData.i_csp, mX264ParamData.i_width, mX264ParamData.i_height);
   if(ret < 0){
      LOGE("Failed to x264_picture_alloc %dx%d", mOriginWidth, mOriginHeight);
      return false;
   }
   
   mX264Encoder = x264_encoder_open(&mX264ParamData);
   if (!mX264Encoder){
      LOGE("Failed to open X264Encoder profile=%s", mProfile.c_str());
      return false;
   }
   
   //   mYuvBuffer = (unsigned char*)calloc(mOriginWidth * mOriginHeight * 3 / 2, 1);
   //   mPicInBuffer = (unsigned char*)calloc(mOriginWidth * mOriginHeight * 3 / 2, 1);
   //   if(mYuvBuffer == NULL /*|| mPicInBuffer == NULL*/){
   //      LOGE("calloc yuv/picin buffer failed.");
   //      return false;
   //   }
   
   mFrameCount = 0;
   mFrameFaildCount = 0;
   
   //for debug
   if (param->is_encoder_debug && mFileH264 == NULL){
      mFileH264 = fopen("H264Stream.264","wb");
   }
   if (param->is_saving_data_debug){
      if (mFileEncodeYUV == NULL){
         mFileEncodeYUV = fopen("EncodeData.yuv", "wb");
      }
      if (mFileFrameInfo == NULL){
         mFileFrameInfo = fopen("FrameInfo.txt", "wb");
         fprintf(mFileFrameInfo, "Scenetype:0 - Unknown; 1 - Natural; 2 - Artificial\n");
      }
   }
   
   mIsInited = true;
   return true;
}

//NV21->I420
int X264Encoder::Encode(const char * indata,
                        int insize,
                        char *outdata,
                        int *p_out_size,
                        int *p_frame_type,
                        uint32_t in_ts,
                        uint32_t *out_ts,
                        LiveExtendParam *extendParam)
{
   int frameSize = 0;
   x264_nal_t *nalOut;
   int nalOutNum = 0;
   *p_frame_type = VIDEO_P_FRAME;
   if(mX264Encoder == NULL){
      LOGW("X264Encoder::Encode x264encoder not init.");
      return -1;
   }
   memcpy(mX264PicIn.img.plane[0], indata, insize);
   
   int newSceneType = SceneType_Unknown;
   int oldSceneType = SceneType_Unknown;
   int luma_size = mOriginWidth * mOriginHeight;
   int chroma_size = luma_size / 4;
   //preprocessing
   if (extendParam == NULL){
      mX264PicIn.i_scene_type = SceneType_Natural;
      mX264PicIn.b_is_same_last = 0;
   }
   else{
      mX264PicIn.i_scene_type = extendParam->scene_type;
      mX264PicIn.b_is_same_last = extendParam->same_last;
   }
   int ret = vhall_video_preprocess_process(mX264Encoder, &mX264PicIn);
   if (ret<0){
      LOGE("video preprocess failed! %d",ret);
   }
   //for debug
   if (mLiveParam->is_saving_data_debug){
      fwrite(mX264PicIn.img.plane[0], luma_size + chroma_size * 2, 1, mFileEncodeYUV);
      fprintf(mFileFrameInfo, "Scenetype of frame [%d] is [%d]\n", mFrameNum, mX264PicIn.i_scene_type);
      if (mX264PicIn.b_is_same_last == 1){
         fprintf(mFileFrameInfo, "and it is SAME AS LAST FRAME\n");
      }
      mFrameNum++;
   }
   //check if need to reconfigure
   if (mX264PicIn.i_scene_type == SceneType_Unknown){
      newSceneType = SceneType_Natural;
   }
   else{
      newSceneType = mX264PicIn.i_scene_type;
   }
   
   if (newSceneType != mSceneType){
      mReconfigType = Reconfig_SceneCut;
      oldSceneType = mSceneType;
      mSceneType = newSceneType;
   }
   if (mCurrentProcessFlag != mLiveParam->video_process_filters){
      mReconfigType = Reconfig_Process;
      mCurrentProcessFlag = mLiveParam->video_process_filters;
   }
   if (mReconfigType){
      if (!RateControlConfig()){
         LOGE("x264 reconfig failed. ");
         if (mReconfigType == Reconfig_SceneCut){
            mSceneType = oldSceneType;
         }
      }
   }
   
   //check if have a key-frame request
   if (mIsRequestKeyframe){
      mX264PicIn.i_type = X264_TYPE_IDR;
      mIsRequestKeyframe = false;
   }
   else{
      mX264PicIn.i_type = X264_TYPE_AUTO;
   }
   mX264PicIn.i_pts = in_ts;
   frameSize = x264_encoder_encode(mX264Encoder, &nalOut, &nalOutNum, &mX264PicIn, &mX264PicOut );
   if(frameSize < 0){
      LOGE("x264_encoder_encode failed. ");
      mFrameFaildCount++;
      return -1;
   }
   if (mLiveParam->is_encoder_debug && frameSize > 0){
      fwrite(nalOut->p_payload, frameSize, 1, mFileH264);
   }
   //编码成功，时间戳入堆栈
   mFrameTimestampQueue.push_back(in_ts);
   if(nalOutNum < 0){
      LOGE("no frame, this frame is cached. ");
      return 0;
   }
   //if frame duplication is being used, the shift will be insignificant, so just don't bother adjusting audio
   if(frameSize > 0 &&mFrameTimestampQueue.size()>0){
      if (mX264PicOut.i_type == X264_TYPE_KEYFRAME||mX264PicOut.i_type == X264_TYPE_I||mX264PicOut.i_type == X264_TYPE_IDR) {
         *p_frame_type = VIDEO_I_FRAME;
      }else if(mX264PicOut.i_type == X264_TYPE_P||mX264PicOut.i_type == X264_TYPE_BREF){
         *p_frame_type = VIDEO_P_FRAME;
      }else{
         *p_frame_type = VIDEO_B_FRAME;
      }
      memcpy(outdata, nalOut->p_payload, frameSize);
      *p_out_size = frameSize;
      *out_ts = *mFrameTimestampQueue.begin();
      mFrameTimestampQueue.pop_front();
      mFrameCount++;
      return frameSize;
   }else {
       LOGW("no frame,this frame is cached");
      return 0;
   }
}

bool X264Encoder::LiveGetRealTimeStatus(VHJson::Value &value){
   value["Name"] = VHJson::Value("X264Encoder");
   
   //TODO may need to make it thread safe.
   value["width"] = VHJson::Value(mOriginWidth);
   value["height"] = VHJson::Value(mOriginHeight);
   value["frame_rate"] = VHJson::Value(mFrameRate);
   value["bitrate"] = VHJson::Value(mBitrate);
   value["gop_size"] = VHJson::Value(mKeyframeInterval);
   value["profile"] = VHJson::Value(mProfile);
   value["preset"] = VHJson::Value(mPreset);
   
   value["frame_success_count"] = VHJson::Value(mFrameCount);
   value["frame_faild_count"] = VHJson::Value(mFrameFaildCount);
   
   return true;
}

bool X264Encoder::GetSpsPps(char*data,int *size){
   if (data!=NULL&&mX264Encoder!=NULL) {
      x264_nal_t *nalOut;
      int lenght = 0;
      int nalOutNum = 0;
      int ret = x264_encoder_headers(mX264Encoder, &nalOut, &nalOutNum);
      if (ret>0) {
         for (int i = 0; i < nalOutNum; ++i)
         {
            switch (nalOut[i].i_type)
            {
               case NAL_SPS:
                  memcpy(data, nalOut[i].p_payload, nalOut[i].i_payload);
                  lenght += nalOut[i].i_payload;
                  break;
               case NAL_PPS:
                  memcpy(data+lenght, nalOut[i].p_payload, nalOut[i].i_payload);
                  lenght += nalOut[i].i_payload;
                  break;
               default:
                  break;
            }
         }
         *size = lenght;
         if (mFileH264 != NULL){
            fwrite(data, *size, 1, mFileH264);
         }
      }
      return true;
   }
   return false;
}

void get_x264_log(void *param, int i_level, const char *psz, va_list argptr) {
   char buffer[4096];
   char *psz_prefix;
   switch( i_level )
   {
      case X264_LOG_ERROR:
         psz_prefix = (char *)"error";
         break;
      case X264_LOG_WARNING:
         psz_prefix = (char *)"warning";
         break;
      case X264_LOG_INFO:
         psz_prefix = (char *)"info";
         break;
      case X264_LOG_DEBUG:
         psz_prefix = (char *)"debug";
         break;
      default:
         psz_prefix = (char *)"unknown";
         break;
   }
   fprintf( stderr, "x264 [%s]: ", psz_prefix );
   vfprintf( stderr, psz, argptr );
   vsprintf( buffer, psz, argptr );
   LOGW("x264 [%s]: %s", psz_prefix,  buffer);
}

//functions for rate control based on network
int X264Encoder::GetResolution(){
   
   if (!mIsInited ){
      return VideoResolution_Error;
   }
   return mResolutionLevel;
   
}

int X264Encoder::GetBitrate(){
   //Invalid situation : 1.have not initialized yet;2. bitrate isn't positive
   if (!mIsInited || mBitrate <= 0){
      return 0;
   }
   return mBitrate;
}

bool X264Encoder::SetBitrate(int bitrate){
   if (mReconfigType && !mIsInited){
      LOGW("x264_encoder : Encoder is reconfiguring or not initialized! Bitrate set failed!");
      return false;
   }
   if (!mIsAdjustBitrate){
      LOGW("x264_encoder : Bitrate adjestment is turn off! Bitrate set failed!");
      return false;
   }
   if (bitrate <= 0){
      LOGE("x264_encoder : Can't set a negative bitrate!");
      return false;
   }
   if (bitrate == mBitrate){
      return true;
   }
   //If input value is unreliable, classify it.
   BitrateClassify(bitrate);
   mBitrate = bitrate;
   mReconfigType = Reconfig_SetBitrate;
   return true;
}

bool X264Encoder::RateControlConfig(){
   if (!mReconfigType){
      return true;
   }
   if (mSceneType == SceneType_Natural){
      switch (mResolutionLevel){
         case VideoResolution_360p:
            switch ((int)mBitrate){
               case 100:
                  mX264ParamData.rc.f_rf_constant = 33;
                  break;
               case 150:
                  mX264ParamData.rc.f_rf_constant = 30;
                  break;
               case 200:
                  mX264ParamData.rc.f_rf_constant = 28;
                  break;
               case 250:
                  mX264ParamData.rc.f_rf_constant = 26;
                  break;
               case 350:
                  mX264ParamData.rc.f_rf_constant = 24;
                  break;
               case 425:
                  mX264ParamData.rc.f_rf_constant = 23;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 22;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 26;
            }
            mX264ParamData.rc.i_vbv_max_bitrate = mBitrate ;
            mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate * 1.2;
            break;
         case VideoResolution_480p:
            switch ((int)mBitrate){
               case 150:
                  mX264ParamData.rc.f_rf_constant = 33;
                  break;
               case 200:
                  mX264ParamData.rc.f_rf_constant = 30;
                  break;
               case 300:
                  mX264ParamData.rc.f_rf_constant = 28;
                  break;
               case 400:
                  mX264ParamData.rc.f_rf_constant = 26;
                  break;
               case 525:
                  mX264ParamData.rc.f_rf_constant = 24;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 23;
                  break;
               case 800:
                  mX264ParamData.rc.f_rf_constant = 22;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 26;
            }
            mX264ParamData.rc.i_vbv_max_bitrate = mBitrate;
            mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate * 1.2;
            break;
         case VideoResolution_540p:
            switch ((int)mBitrate){
               case 200:
                  mX264ParamData.rc.f_rf_constant = 33;
                  break;
               case 300:
                  mX264ParamData.rc.f_rf_constant = 30;
                  break;
               case 400:
                  mX264ParamData.rc.f_rf_constant = 28;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 26;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 24;
                  break;
               case 850:
                  mX264ParamData.rc.f_rf_constant = 23;
                  break;
               case 1100:
                  mX264ParamData.rc.f_rf_constant = 22;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 26;
            }
            mX264ParamData.rc.i_vbv_max_bitrate = mBitrate;
            mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate * 1.2;
            break;
         case VideoResolution_720p:
            switch ((int)mBitrate){
               case 350:
                  mX264ParamData.rc.f_rf_constant = 33;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 30;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 28;
                  break;
               case 800:
                  mX264ParamData.rc.f_rf_constant = 27;
                  break;
               case 1000:
                  mX264ParamData.rc.f_rf_constant = 25;
                  break;
               case 1400:
                  mX264ParamData.rc.f_rf_constant = 23;
                  break;
               case 2000:
                  mX264ParamData.rc.f_rf_constant = 21;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 27;
            }
            mX264ParamData.rc.i_vbv_max_bitrate = mBitrate;
            mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate * 1.3;
            break;
         case VideoResolution_768p:
            switch ((int)mBitrate){
               case 350:
                  mX264ParamData.rc.f_rf_constant = 33;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 30;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 28;
                  break;
               case 800:
                  mX264ParamData.rc.f_rf_constant = 27;
                  break;
               case 1100:
                  mX264ParamData.rc.f_rf_constant = 25;
                  break;
               case 1500:
                  mX264ParamData.rc.f_rf_constant = 23;
                  break;
               case 2200:
                  mX264ParamData.rc.f_rf_constant = 21;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 27;
            }
            mX264ParamData.rc.i_vbv_max_bitrate = mBitrate;
            mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate * 1.3;
            break;
         case VideoResolution_1080p:
            switch ((int)mBitrate){
               case 700:
                  mX264ParamData.rc.f_rf_constant = 33;
                  break;
               case 1000:
                  mX264ParamData.rc.f_rf_constant = 30;
                  break;
               case 1300:
                  mX264ParamData.rc.f_rf_constant = 28;
                  break;
               case 1600:
                  mX264ParamData.rc.f_rf_constant = 27;
                  break;
               case 2000:
                  mX264ParamData.rc.f_rf_constant = 25;
                  break;
               case 2700:
                  mX264ParamData.rc.f_rf_constant = 23;
                  break;
               case 3800:
                  mX264ParamData.rc.f_rf_constant = 22;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 27;
            }
            mX264ParamData.rc.i_vbv_max_bitrate = mBitrate;
            mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate * 1.3;
            break;
         case VideoResolution_2160p:
            switch ((int)mBitrate){
               case 2500:
                  mX264ParamData.rc.f_rf_constant = 33;
                  break;
               case 3500:
                  mX264ParamData.rc.f_rf_constant = 31;
                  break;
               case 4800:
                  mX264ParamData.rc.f_rf_constant = 29;
                  break;
               case 6000:
                  mX264ParamData.rc.f_rf_constant = 27;
                  break;
               case 7500:
                  mX264ParamData.rc.f_rf_constant = 25;
                  break;
               case 10000:
                  mX264ParamData.rc.f_rf_constant = 23;
                  break;
               case 15000:
                  mX264ParamData.rc.f_rf_constant = 22;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 27;
            }
            mX264ParamData.rc.i_vbv_max_bitrate = mBitrate;
            mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate * 1.4;
            break;
         default:
            return false;
      }
   }
   //In artificial scene, we using a big vbv buffer to ensure the quality of I frame
   else if (mSceneType == SceneType_Artificial){
      switch (mResolutionLevel){
         case VideoResolution_360p:
            switch ((int)mBitrate){
               case 100:
                  mX264ParamData.rc.f_rf_constant = 36;
                  mX264ParamData.rc.i_vbv_max_bitrate = 150 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 450 * 0.8;
                  break;
               case 150:
                  mX264ParamData.rc.f_rf_constant = 35;
				  mX264ParamData.rc.i_vbv_max_bitrate = 200 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 450 * 0.8;
                  break;
               case 200:
                  mX264ParamData.rc.f_rf_constant = 34;
				  mX264ParamData.rc.i_vbv_max_bitrate = 200 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 600 * 0.8;
                  break;
               case 250:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 250 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 600 * 0.8;
                  break;
               case 350:
                  mX264ParamData.rc.f_rf_constant = 32;
				  mX264ParamData.rc.i_vbv_max_bitrate = 250 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 750 * 0.9;
                  break;
               case 425:
                  mX264ParamData.rc.f_rf_constant = 31;
				  mX264ParamData.rc.i_vbv_max_bitrate = 350 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 750 * 0.9;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 30;
                  mX264ParamData.rc.i_vbv_max_bitrate = 350;
                  mX264ParamData.rc.i_vbv_buffer_size = 1050;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 250 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 600 * 0.8;
            }
            break;
         case VideoResolution_480p:
            switch ((int)mBitrate){
               case 150:
                  mX264ParamData.rc.f_rf_constant = 36;
				  mX264ParamData.rc.i_vbv_max_bitrate = 200 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 600 * 0.8;
                  break;
               case 200:
                  mX264ParamData.rc.f_rf_constant = 35;
				  mX264ParamData.rc.i_vbv_max_bitrate = 300 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 600 * 0.8;
                  break;
               case 300:
                  mX264ParamData.rc.f_rf_constant = 34;
				  mX264ParamData.rc.i_vbv_max_bitrate = 300 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 900 * 0.8;
                  break;
               case 400:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 400 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 900 * 0.8;
                  break;
               case 525:
                  mX264ParamData.rc.f_rf_constant = 32;
				  mX264ParamData.rc.i_vbv_max_bitrate = 400 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 1200 * 0.9;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 31;
				  mX264ParamData.rc.i_vbv_max_bitrate = 525 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 1200 * 0.9;
                  break;
               case 800:
                  mX264ParamData.rc.f_rf_constant = 30;
                  mX264ParamData.rc.i_vbv_max_bitrate = 525;
                  mX264ParamData.rc.i_vbv_buffer_size = 1575;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 400 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 900 * 0.8;
            }
            break;
         case VideoResolution_540p:
            switch ((int)mBitrate){
               case 200:
                  mX264ParamData.rc.f_rf_constant = 36;
				  mX264ParamData.rc.i_vbv_max_bitrate = 300 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 900 * 0.8;
                  break;
               case 300:
                  mX264ParamData.rc.f_rf_constant = 35;
				  mX264ParamData.rc.i_vbv_max_bitrate = 400 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 900 * 0.8;
                  break;
               case 400:
                  mX264ParamData.rc.f_rf_constant = 34;
				  mX264ParamData.rc.i_vbv_max_bitrate = 400 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1200 * 0.8;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 500 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1200 * 0.8;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 32;
				  mX264ParamData.rc.i_vbv_max_bitrate = 500 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 1500 * 0.9;
                  break;
               case 850:
                  mX264ParamData.rc.f_rf_constant = 31;
				  mX264ParamData.rc.i_vbv_max_bitrate = 700 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 1500 * 0.9;
                  break;
               case 1100:
                  mX264ParamData.rc.f_rf_constant = 30;
                  mX264ParamData.rc.i_vbv_max_bitrate = 700;
                  mX264ParamData.rc.i_vbv_buffer_size = 2100;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 500 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1200 * 0.8;
            }
            break;
         case VideoResolution_720p:
            switch ((int)mBitrate){
               case 350:
                  mX264ParamData.rc.f_rf_constant = 36;
				  mX264ParamData.rc.i_vbv_max_bitrate = 500 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1500 * 0.8;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 35;
				  mX264ParamData.rc.i_vbv_max_bitrate = 650 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1500 * 0.8;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 34;
				  mX264ParamData.rc.i_vbv_max_bitrate = 650 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1950 * 0.8;
                  break;
               case 800:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 800 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1950 * 0.8;
                  break;
               case 1000:
                  mX264ParamData.rc.f_rf_constant = 32;
				  mX264ParamData.rc.i_vbv_max_bitrate = 800 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 2400 * 0.9;
                  break;
               case 1400:
                  mX264ParamData.rc.f_rf_constant = 31;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1000 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 2400 * 0.9;
                  break;
               case 2000:
                  mX264ParamData.rc.f_rf_constant = 30;
                  mX264ParamData.rc.i_vbv_max_bitrate = 1000;
                  mX264ParamData.rc.i_vbv_buffer_size = 3000;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 800 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1950 * 0.8;
            }
            break;
         case VideoResolution_768p:
            switch ((int)mBitrate){
               case 350:
                  mX264ParamData.rc.f_rf_constant = 36;
				  mX264ParamData.rc.i_vbv_max_bitrate = 500 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1500 * 0.8;
                  break;
               case 500:
                  mX264ParamData.rc.f_rf_constant = 35;
				  mX264ParamData.rc.i_vbv_max_bitrate = 650 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1500 * 0.8;
                  break;
               case 650:
                  mX264ParamData.rc.f_rf_constant = 34;
				  mX264ParamData.rc.i_vbv_max_bitrate = 650 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1950 * 0.8;
                  break;
               case 800:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 800 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1950 * 0.8;
                  break;
               case 1100:
                  mX264ParamData.rc.f_rf_constant = 32;
				  mX264ParamData.rc.i_vbv_max_bitrate = 800 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 2400 * 0.9;
                  break;
               case 1500:
                  mX264ParamData.rc.f_rf_constant = 31;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1000 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 2400 * 0.9;
                  break;
               case 2200:
                  mX264ParamData.rc.f_rf_constant = 30;
                  mX264ParamData.rc.i_vbv_max_bitrate = 1000;
                  mX264ParamData.rc.i_vbv_buffer_size = 3000;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 800 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 1950 * 0.8;
            }
            break;
         case VideoResolution_1080p:
            switch ((int)mBitrate){
               case 700:
                  mX264ParamData.rc.f_rf_constant = 36;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1000 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 3000 * 0.8;
                  break;
               case 1000:
                  mX264ParamData.rc.f_rf_constant = 35;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1300 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 3000 * 0.8;
                  break;
               case 1300:
                  mX264ParamData.rc.f_rf_constant = 34;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1300 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 3900 * 0.8;
                  break;
               case 1600:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1600 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 3900 * 0.8;
                  break;
               case 2000:
                  mX264ParamData.rc.f_rf_constant = 32;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1600 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 4800 * 0.9;
                  break;
               case 2700:
                  mX264ParamData.rc.f_rf_constant = 31;
				  mX264ParamData.rc.i_vbv_max_bitrate = 2000 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 4800 * 0.9;
                  break;
               case 3800:
                  mX264ParamData.rc.f_rf_constant = 30;
                  mX264ParamData.rc.i_vbv_max_bitrate = 2000;
                  mX264ParamData.rc.i_vbv_buffer_size = 6000;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 1600 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 3900 * 0.8;
            }
            break;
         case VideoResolution_2160p:
            switch ((int)mBitrate){
               case 2500:
                  mX264ParamData.rc.f_rf_constant = 36;
				  mX264ParamData.rc.i_vbv_max_bitrate = 3500 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 10500 * 0.8;
                  break;
               case 3500:
                  mX264ParamData.rc.f_rf_constant = 35;
				  mX264ParamData.rc.i_vbv_max_bitrate = 4800 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 10500 * 0.8;
                  break;
               case 4800:
                  mX264ParamData.rc.f_rf_constant = 34;
				  mX264ParamData.rc.i_vbv_max_bitrate = 4800 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 14400 * 0.8;
                  break;
               case 6000:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 6000 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 14400 * 0.8;
                  break;
               case 7500:
                  mX264ParamData.rc.f_rf_constant = 32;
				  mX264ParamData.rc.i_vbv_max_bitrate = 6000 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 18000 * 0.9;
                  break;
               case 10000:
                  mX264ParamData.rc.f_rf_constant = 31;
				  mX264ParamData.rc.i_vbv_max_bitrate = 7500 * 0.9;
				  mX264ParamData.rc.i_vbv_buffer_size = 18000 * 0.9;
                  break;
               case 15000:
                  mX264ParamData.rc.f_rf_constant = 30;
                  mX264ParamData.rc.i_vbv_max_bitrate = 7500;
                  mX264ParamData.rc.i_vbv_buffer_size = 22500;
                  break;
               default:
                  mX264ParamData.rc.f_rf_constant = 33;
				  mX264ParamData.rc.i_vbv_max_bitrate = 6000 * 0.8;
				  mX264ParamData.rc.i_vbv_buffer_size = 14400 * 0.8;
            }
            break;
         default:
            return false;
      }
      //mX264ParamData.rc.i_vbv_buffer_size = mX264ParamData.rc.i_vbv_max_bitrate*1.3;
   }
   if (mLiveParam->high_codec_open == 9){
      mX264ParamData.rc.i_qp_max = 37;
      mX264ParamData.rc.i_qp_min = 19;
      mX264ParamData.rc.i_vbv_buffer_size = 0;
      mX264ParamData.rc.i_vbv_max_bitrate = 0;
      mQPLimitedLevel = 7;
      if (mSceneType == SceneType_Artificial){
         mX264ParamData.rc.f_rf_constant = 33;
      }
      else{
         mX264ParamData.rc.f_rf_constant = 25;
      }
   }
   
   if (mReconfigType == Reconfig_SetBitrate || mReconfigType == Reconfig_SceneCut){
      int ret;
      //TODO:create a new function to replace it, for x264_encoder_reconfig doing too many things we don't care
      ret = x264_encoder_reconfig(mX264Encoder, &mX264ParamData);
      if (ret < 0){
         LOGE("x264_encoder : x264_encoder_reconfig failed for parameter validation error!");
         return false;
      }
   }
   LOGD("x264_encoder : x264 encoder config/reconfig complete!");
   mReconfigType = Reconfig_KeepSame;
   return true;
}

bool X264Encoder::BitrateClassify(int bitrate){
   if (bitrate <= 0){
      LOGE("x264_encoder : Bitrate must be positive!");
      return false;
   }
   if (bitrate > mOriginBitrate){
      bitrate = mOriginBitrate;
   }
   //In current version, mQPLimitedLevel mustn't be lower than 5
   if (mQPLimitedLevel < 5){
      mQPLimitedLevel = 5;
   }
   //SceneType_Natural is the standard
   switch (mResolutionLevel)
   {
      case VideoResolution_360p:
         if (bitrate <= 125 || (mIsQualityLimited && mQPLimitedLevel == 1)){
            mBitrate = 100;
         }
         else if (bitrate <= 175 || (mIsQualityLimited && mQPLimitedLevel == 2)){
            mBitrate = 150;
         }
         else if (bitrate <= 225 || (mIsQualityLimited && mQPLimitedLevel == 3)){
            mBitrate = 200;
         }
         else if (bitrate <= 300 || (mIsQualityLimited && mQPLimitedLevel == 4)){
            mBitrate = 250;
         }
         else if (bitrate <= 375 || (mIsQualityLimited && mQPLimitedLevel == 5)){
            mBitrate = 350;
         }
         else if (bitrate <= 450 || (mIsQualityLimited && mQPLimitedLevel == 6)){
            mBitrate = 425;
         }
         else {
            mBitrate = 500;
         }
         break;
      case VideoResolution_480p:
         if (bitrate <= 175 || (mIsQualityLimited && mQPLimitedLevel == 1)){
            mBitrate = 150;
         }
         else if (bitrate <= 250 || (mIsQualityLimited && mQPLimitedLevel == 2)){
            mBitrate = 200;
         }
         else if (bitrate <= 350 || (mIsQualityLimited && mQPLimitedLevel == 3)){
            mBitrate = 300;
         }
         else if (bitrate <= 440 || (mIsQualityLimited && mQPLimitedLevel == 4)){
            mBitrate = 400;
         }
         else if (bitrate <= 575 || (mIsQualityLimited && mQPLimitedLevel == 5)){
            mBitrate = 525;
         }
         else if (bitrate <= 725 || (mIsQualityLimited && mQPLimitedLevel == 6)){
            mBitrate = 650;
         }
         else {
            mBitrate = 800;
         }
         break;
      case VideoResolution_540p:
         if (bitrate <= 250 || (mIsQualityLimited && mQPLimitedLevel == 1)){
            mBitrate = 200;
         }
         else if (bitrate <= 350 || (mIsQualityLimited && mQPLimitedLevel == 2)){
            mBitrate = 300;
         }
         else if (bitrate <= 450 || (mIsQualityLimited && mQPLimitedLevel == 3)){
            mBitrate = 400;
         }
         else if (bitrate <= 575 || (mIsQualityLimited && mQPLimitedLevel == 4)){
            mBitrate = 500;
         }
         else if (bitrate <= 750 || (mIsQualityLimited && mQPLimitedLevel == 5)){
            mBitrate = 650;
         }
         else if (bitrate <= 975 || (mIsQualityLimited && mQPLimitedLevel == 6)){
            mBitrate = 850;
         }
         else {
            mBitrate = 1100;
         }
         break;
      case VideoResolution_720p:
         if (bitrate <= 425 || (mIsQualityLimited && mQPLimitedLevel == 1)){
            mBitrate = 350;
         }
         else if (bitrate <= 575 || (mIsQualityLimited && mQPLimitedLevel == 2)){
            mBitrate = 500;
         }
         else if (bitrate <= 725 || (mIsQualityLimited && mQPLimitedLevel == 3)){
            mBitrate = 650;
         }
         else if (bitrate <= 900 || (mIsQualityLimited && mQPLimitedLevel == 4)){
            mBitrate = 800;
         }
         else if (bitrate <= 1200 || (mIsQualityLimited && mQPLimitedLevel == 5)){
            mBitrate = 1000;
         }
         else if (bitrate <= 1700 || (mIsQualityLimited && mQPLimitedLevel == 6)){
            mBitrate = 1400;
         }
         else {
            mBitrate = 2000;
         }
         break;
      case VideoResolution_768p:
         if (bitrate <= 425 || (mIsQualityLimited && mQPLimitedLevel == 1)){
            mBitrate = 350;
         }
         else if (bitrate <= 575 || (mIsQualityLimited && mQPLimitedLevel == 2)){
            mBitrate = 500;
         }
         else if (bitrate <= 725 || (mIsQualityLimited && mQPLimitedLevel == 3)){
            mBitrate = 650;
         }
         else if (bitrate <= 950 || (mIsQualityLimited && mQPLimitedLevel == 4)){
            mBitrate = 800;
         }
         else if (bitrate <= 1300 || (mIsQualityLimited && mQPLimitedLevel == 5)){
            mBitrate = 1100;
         }
         else if (bitrate <= 1800 || (mIsQualityLimited && mQPLimitedLevel == 6)){
            mBitrate = 1500;
         }
         else {
            mBitrate = 2200;
         }
         break;
      case VideoResolution_1080p:
         if (bitrate < 850 || (mIsQualityLimited && mQPLimitedLevel == 1)){
            mBitrate = 700;
         }
         else if (bitrate <= 1150 || (mIsQualityLimited && mQPLimitedLevel == 2)){
            mBitrate = 1000;
         }
         else if (bitrate <= 1450 || (mIsQualityLimited && mQPLimitedLevel == 3)){
            mBitrate = 1300;
         }
         else if (bitrate <= 1800 || (mIsQualityLimited && mQPLimitedLevel == 4)){
            mBitrate = 1600;
         }
         else if (bitrate <= 2350 || (mIsQualityLimited && mQPLimitedLevel == 5)){
            mBitrate = 2000;
         }
         else if (bitrate <= 3250 || (mIsQualityLimited && mQPLimitedLevel == 6)){
            mBitrate = 2700;
         }
         else {
            mBitrate = 3800;
         }
         break;
      case VideoResolution_2160p:
         if (bitrate < 3000 || (mIsQualityLimited && mQPLimitedLevel == 1)){
            mBitrate = 2500;
         }
         else if (bitrate <= 4150 || (mIsQualityLimited && mQPLimitedLevel == 2)){
            mBitrate = 3500;
         }
         else if (bitrate <= 5400 || (mIsQualityLimited && mQPLimitedLevel == 3)){
            mBitrate = 4800;
         }
         else if (bitrate <= 6750 || (mIsQualityLimited && mQPLimitedLevel == 4)){
            mBitrate = 6000;
         }
         else if (bitrate <= 8750 || (mIsQualityLimited && mQPLimitedLevel == 5)){
            mBitrate = 7500;
         }
         else if (bitrate <= 12500 || (mIsQualityLimited && mQPLimitedLevel == 6)){
            mBitrate = 10000;
         }
         else {
            mBitrate = 15000;
         }
         break;
      default:
         LOGE("x264_encoder : Resolution level is invalid! Classification failed!");
         return false;
   }
   return true;
}

bool X264Encoder::SetQualityLimited(bool onoff){
   //this parameter could be set only after init
   if (mIsInited){
      mIsQualityLimited = onoff;
      return true;
   }
   return false;
}

bool X264Encoder::SetAdjustBitrate(bool onoff){
   //this parameter could be set only after init
   if (mIsInited){
      mIsAdjustBitrate = onoff;
      return true;
   }
   return false;
}

bool X264Encoder::RequestKeyframe(){
   if (mIsRequestKeyframe){
      LOGW("x264_encoder : Keyframe requests are too frequent!");
      return false;
   }
   mIsRequestKeyframe = true;
   return true;
}
