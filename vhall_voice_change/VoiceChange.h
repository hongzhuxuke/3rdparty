/*********************************************************************************
  *Copyright(C),2018-2019,Vhall Live Co.Lmt.
  *ModuleName: VoiceChange
  *Author:     Xia Yang
  *Version:    1.8
  *Date:       2019-01-03
  *Description:Change the audial feature of PCM data  
**********************************************************************************/


#ifndef VHALL_VOICE_CHANGE_HH
#define VHALL_VOICE_CHANGE_HH

#include <iostream>
#include "AudioBuffer.h"

//processing chunk size(in milliseconds)
//NOTICE:still testing, this value might be changed in the future
#define VOICECHANGE_CHUNKSIZE_MS 40

enum ErrCode {
   AUDIOPRCS_ERROR_UNKNOWN = -1,       /* 未知错误 */
   AUDIOPRCS_ERROR_SUCCESS,            /* 成功 */
   AUDIOPRCS_ERROR_NOTHING_DONE,       /* 内部未进行任何处理，将输入直接输出或无内容输出 */
   AUDIOPRCS_ERROR_CHANNELNUM,         /* 声道数错误，仅支持单声道、双声道 */
   AUDIOPRCS_ERROR_DATAFMT,            /* 数据格式错误，支持类型见DataFormat定义 */
   AUDIOPRCS_ERROR_SAMPLERATE,         /* 采样率错误，支持8000-48000Hz采样率 */
   AUDIOPRCS_ERROR_PITCH,              /* 用户自定义条件下，输入声调调整参数超过限定范围 */
   AUDIOPRCS_ERROR_INOUT_NULLPTR,      /* 输入or/and输出指针为空 */
   AUDIOPRCS_ERROR_INPUTSIZE,          /* 输入字节数错误 */
   AUDIOPRCS_ERROR_BUFFER_MEMO,        /* buffer内存分配错误 */
   AUDIOPRCS_ERROR_NOT_INIT           /* 未进行初始化 */
};

enum DataFormat {
   AFMT_UNKNOW = 0,
   AFMT_SHORT,       /* 16位定点 */
   AFMT_FLOAT        /* 32位浮点 */
};

enum VoiceChangeType {
   VOICETYPE_NONE = 0,        /* 不变声 */
   VOICETYPE_CUTE,             /* 变声 */
   VOICETYPE_USRDEFINE        /* 用户自定义声调 */
};

class AudioParam {
 public:
  AudioParam() {
    nChannel = 0;
    sampleRate = 0;
    format = AFMT_UNKNOW;
    type = VOICETYPE_NONE;
    pitchChange = 3;
  }
  int nChannel;         /* 声道数 */
  int sampleRate;       /* 采样率 */
  DataFormat format;    /* 采样数据格式 */
  VoiceChangeType type; /* 变声模式 */
  double pitchChange;   /* 用户自定义声调模式下使用，取值范围[-100,100] */
};


//typedef int(*OutputCallback)(FIFOAudioBuffer *outBuffer);

class VoiceChange {
public:
   //construction
   VoiceChange();
   //destruction
   ~VoiceChange();

   /*************************************************************************
    Init(...)
    
    Initialize the instance
    
    Input:                                                                 
        - param    		: input audio format
    
    Return value			: AUDIOPRCS_ERROR_SUCCESS - OK,
                           AUDIOPRCS_ERROR_NOTHING_DONE - input is equal to
                                  output, or nothing have been done
                           other - Error
   **************************************************************************/
   ErrCode Init(AudioParam *param);

   /*************************************************************************
   / Process(...)
   /
   / Do the voice change(or just copy)
   /
   / Input:
   /     - inputData   	: input audio data
   /     - sizeInBytes		: bytes of input data
   / Output:
   /     - outputData		: output audio data
   /     - sizeInBytes		: bytes of output data
   /
   / Return value			: AUDIOPRCS_ERROR_SUCCESS - OK,
   /                        AUDIOPRCS_ERROR_NOTHING_DONE - input is equal to
   /                               output, or nothing have been done
   /                        other - Error
   **************************************************************************/
   ErrCode Process(char *inputData, int sizeInBytes);

   int GetOutputSize();
   int GetOutputData(char *outputData, int outputBytes);

   /*************************************************************************
   / Flush(...)
   /
   / Return value			: AUDIOPRCS_ERROR_SUCCESS - OK,
   /                        AUDIOPRCS_ERROR_NOTHING_DONE - input is equal to
   /                               output, or nothing have been done
   /                        other - Error
   **************************************************************************/
   ErrCode Flush();

   /*************************************************************************
   / ChangeVoiceType(...)
   /
   / Change the process type after initialization.
   / NOTICE:this function try to flush the data inside processor first
   /
   / Input:
   /     - newType    		: new process mode want to be
   /
   / Return value			: AUDIOPRCS_ERROR_SUCCESS - OK,
   /                        AUDIOPRCS_ERROR_NOTHING_DONE - input is equal to
   /                               output, or nothing have been done
   /                        other - Error
   **************************************************************************/
   ErrCode ChangeVoiceType(VoiceChangeType newType);

   /*************************************************************************
   / GetChangeType(...)
   /
   / get current process mode
   /
   / Return value			: current process mode
   **************************************************************************/
   VoiceChangeType  GetChangeType();

private:
   bool bIsCheck;
   bool mIsFirstFrame;

   int mChannelNum;
   int mSampleRate;
   int mBytePerSample;
   int mBytePerUnit;
   int mBytesZeros;
   int mBytesRemain;
   int mBytesFlushOut;
   int mRequireSamples;
   int mOverlapSamples;
   int mWindowSamples;
   int mSeekSamples;
   int mFilterLength;
   double mPitchChange;
   DataFormat mFormat;
   VoiceChangeType mProcessType;
   VoiceChangeType mLastType;
   char *mTempSamplesUnaligned;
   char *mTempSamples;
   char *mUselessSamples;
   FIFOAudioBuffer *mBufferInput;
   FIFOAudioBuffer *mBufferMiddle1;
   FIFOAudioBuffer *mBufferMiddle2;
   FIFOAudioBuffer *mBufferOutputShort;
   FIFOAudioBuffer *mBufferOutputFloat;

   int overlapDividerBitsNorm;
   int overlapDividerBitsPure;
   int mWeightShort;
   double mWeightFloat;
   double mSkipFract;
   double mSkipEst;
   double mRate;
   double mTempo;
   
   double mCutOffFreq;
   short *mCoeffsShort;
   float *mCoeffsFloat;

   unsigned long maxnorm;
   float maxnormf;

   //common
   ErrCode Validate();
   int ProcessCore();
   int FloatToShort(float *src, short *dst, int bytes);
   int ShortToFloat(short *src, float *dst, int bytes);

   // concerning WSOLA
   int WSOLAprocess(FIFOAudioBuffer *srcBuffer, FIFOAudioBuffer *dstBuffer);
   int FindDelta(char *src);
   void DoOLA(char *dst, char *src, unsigned int delta);
   double calcCrossCorr(float *mixingPos, float *compare, double &norm);
   double calcCrossCorr(short *mixingPos, short *compare, double &norm);
   double calcCrossCorrAccumulate(float *mixingPos, float *compare, double &norm);
   double calcCrossCorrAccumulate(short *mixingPos, short *compare, double &norm);
   void clearCrossCorrState();
   void adaptNormalizer();

   //concerning FIR filter
   int SmoothFilter(FIFOAudioBuffer *srcBuffer, FIFOAudioBuffer *dstBuffer);
   int DoFilter(short *src, short *dst, int samples);
   int DoFilter(float *src, float *dst, int samples);
   bool CalcCoeffs();

   //concerning Interpolate
   int Interpolate(FIFOAudioBuffer *srcBuffer, FIFOAudioBuffer *dstBuffer);
   int DoInterpolate(short *src, short *dst, int samples);
   int DoInterpolate(float *src, float *dst, int samples);
};
#endif // !VHALL_VOICE_CHANGE_HH



