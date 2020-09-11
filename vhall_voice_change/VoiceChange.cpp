/*********************************************************************************
  *Copyright(C),2018-2019,Vhall Live Co.Lmt.
  *ModuleName: VoiceChange
  *Author:     Xia Yang
  *Version:    1.6
  *Date:       2018-11-15
  *Description:Change the audial feature of PCM data  
**********************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include "VoiceChange.h"

using namespace std;

#define CHANNEL_NUM_MAX 2
#define SAMPLE_RATE_MIN 8000
#define SAMPLE_RATE_MAX 48000

#define MAXSCALE_SHORT  65536
#define FILTER_Q        14
#define FILTER_SCALE    16384
#define PI              3.14159265358979323846
#define TWOPI           (2 * PI)


//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define CLIP(x, min, max) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))

VoiceChange::VoiceChange()
{
   bIsCheck = false;
   mChannelNum = 0;
   mSampleRate = 0;
   mFormat = AFMT_UNKNOW;
   mProcessType = VOICETYPE_NONE;
   mLastType = VOICETYPE_NONE;
   mPitchChange = 0;

   mBytePerSample = 0;
   mBytePerUnit = 0;
   mBytesZeros = 0;
   mBytesFlushOut = 0;
   mBytesRemain = 0;

   mBufferInput = new FIFOAudioBuffer;
   mBufferMiddle1 = new FIFOAudioBuffer;
   mBufferMiddle2 = new FIFOAudioBuffer;
   mBufferOutputShort = new FIFOAudioBuffer;
   mBufferOutputFloat = new FIFOAudioBuffer;

   mTempSamplesUnaligned = nullptr;
   mTempSamples = nullptr;
   mUselessSamples = nullptr;

   mIsFirstFrame = true;
   mRequireSamples = 0;
   mSkipFract = 0;
   mSkipEst = 0;
   //int mSkipInt;
   mTempo = 1;
   mRate = 1;
   mOverlapSamples = 0;
   mWindowSamples = 0;
   mSeekSamples = 0;

   maxnorm = 0;
   maxnormf = 1e8;
   overlapDividerBitsNorm = 0;
   overlapDividerBitsPure = 0;

   mWeightShort = 0;
   mWeightFloat = 0;

   mFilterLength = 0;
   mCutOffFreq = 0;
   mCoeffsShort = nullptr;
   mCoeffsFloat = nullptr;
}

VoiceChange::~VoiceChange()
{
   if (mTempSamplesUnaligned != nullptr) {
      delete[]mTempSamplesUnaligned;
      mTempSamplesUnaligned = nullptr;
      mTempSamples = nullptr;
   }
   if (mBufferInput != nullptr) {
      delete mBufferInput;
      mBufferInput = nullptr;
   }
   if (mBufferMiddle1 != nullptr) {
      delete mBufferMiddle1;
      mBufferMiddle1 = nullptr;
   }
   if (mBufferMiddle2 != nullptr) {
      delete mBufferMiddle2;
      mBufferMiddle2 = nullptr;
   }
   if (mBufferOutputShort != nullptr) {
      delete mBufferOutputShort;
      mBufferOutputShort = nullptr;
   }
   if (mBufferOutputFloat != nullptr) {
      delete mBufferOutputFloat;
      mBufferOutputFloat = nullptr;
   }
   if (mCoeffsShort != nullptr) {
      delete mCoeffsShort;
      mCoeffsShort = nullptr;
   }
   if (mCoeffsFloat != nullptr) {
      delete mCoeffsFloat;
      mCoeffsFloat = nullptr;
   }
   if (mUselessSamples != nullptr) {
      delete mUselessSamples;
      mUselessSamples = nullptr;
   }
}

ErrCode VoiceChange::Init(AudioParam * param)
{
   ErrCode ret;
   mChannelNum = param->nChannel;
   mSampleRate = param->sampleRate;
   mFormat = param->format;
   mProcessType = param->type;
   mPitchChange = param->pitchChange;
   ret = Validate();
   if (ret != AUDIOPRCS_ERROR_SUCCESS) {
      return ret;
   }
   /*if (mFormat == AFMT_SHORT) {
      mBytePerSample = sizeof(short);
   }
   else if (mFormat == AFMT_FLOAT) {
      mBytePerSample = sizeof(float);
   }*/
   mBytePerSample = sizeof(short);
   mBytePerUnit = mBytePerSample * mChannelNum;
   mBytesZeros = 0;
   mBytesFlushOut = 0;
   mBytesRemain = 0;

   if (param->type == VOICETYPE_USRDEFINE) {
      double pitchShift = mPitchChange / 120;
      mRate = exp(0.69314718056 * pitchShift);
      mLastType = VOICETYPE_USRDEFINE;
   }
   else {
      mRate = exp(0.69314718056 * 0.25);//0.25 is the pitch change
      mLastType = VOICETYPE_CUTE;
   }
   
   mTempo = 1/mRate;

   int newOvl;
   double base;
   base = (double)(mSampleRate * 8) / 1000; // overlap 8ms
   overlapDividerBitsPure = (int)(log(base) / log(2.0) + 0.5) - 1;
   if (overlapDividerBitsPure > 9) overlapDividerBitsPure = 9;
   if (overlapDividerBitsPure < 3) overlapDividerBitsPure = 3;
   newOvl = (int)pow(2.0, (int)overlapDividerBitsPure + 1);    // +1 => account for -1 above
   if (newOvl > mOverlapSamples) {
      mOverlapSamples = newOvl;
   }
   overlapDividerBitsNorm = overlapDividerBitsPure;

   mWindowSamples = (79 * mSampleRate)/1000; //sequence 79ms //TODO: try to compare with 40ms
   if (mWindowSamples < 2 * mOverlapSamples) {
      mWindowSamples = 2 * mOverlapSamples;
   }
   mSeekSamples = (19 * mSampleRate)/1000;//seek 19ms //TODO: try to compare wiht 15ms
   mSkipEst = mTempo * (mWindowSamples - mOverlapSamples);
   mRequireSamples = MAX((int)(mSkipEst + 0.5) + mOverlapSamples, mWindowSamples) + mSeekSamples;

   int initSize;
   initSize = mRequireSamples * mBytePerUnit * 2;
   if (!mBufferInput->init(initSize) || !mBufferOutputShort->init(initSize) || !mBufferMiddle1->init(initSize) || !mBufferMiddle2->init(initSize)) {
      return AUDIOPRCS_ERROR_BUFFER_MEMO;
   }
   if (mFormat == AFMT_FLOAT && !mBufferOutputFloat->init(initSize)) {
      return AUDIOPRCS_ERROR_BUFFER_MEMO;
   }


   mIsFirstFrame = true;
   mSkipFract = 0;

   maxnorm = 0;
   maxnormf = 1e8;
   
   
   if (mTempSamplesUnaligned != nullptr) {
      delete[]mTempSamplesUnaligned;
      mTempSamplesUnaligned = nullptr;
      mTempSamples = nullptr;
   }
   mTempSamplesUnaligned = new char[mOverlapSamples * mBytePerUnit + 16];
   mTempSamples = (char *)ALIGN_POINTER_16(mTempSamplesUnaligned);
   memset(mTempSamplesUnaligned, 0, mOverlapSamples * mBytePerUnit + 16);

   if (mUselessSamples != nullptr) {
      delete mUselessSamples;
      mUselessSamples = nullptr;
   }
   mUselessSamples = new char[mRequireSamples * mBytePerUnit];
   memset(mUselessSamples, 0, mRequireSamples * mBytePerUnit);

   mWeightShort = 0;
   mWeightFloat = 0;

   mFilterLength = 64;
   if (mRate > 1.0) {
      mCutOffFreq = 0.5 / mRate;
   }
   else {
      mCutOffFreq = 0.5 * mRate;
   }
   mCoeffsShort = new short[mFilterLength];
   mCoeffsFloat = new float[mFilterLength];
   memset(mCoeffsShort, 0, mFilterLength * sizeof(short));
   memset(mCoeffsFloat, 0, mFilterLength * sizeof(float));
   CalcCoeffs();

   return AUDIOPRCS_ERROR_SUCCESS;
}

ErrCode VoiceChange::Validate()
{
   if (mChannelNum < 1 || mChannelNum > CHANNEL_NUM_MAX) {
      return AUDIOPRCS_ERROR_CHANNELNUM;
   }
   if (mSampleRate < SAMPLE_RATE_MIN || mSampleRate > SAMPLE_RATE_MAX ) {
      return AUDIOPRCS_ERROR_SAMPLERATE;
   }
   if (mFormat < AFMT_SHORT || mFormat > AFMT_FLOAT) {
      return AUDIOPRCS_ERROR_DATAFMT;
   }
   if (mProcessType == VOICETYPE_USRDEFINE) {
      if (mPitchChange < -100 || mPitchChange > 100) {
         mPitchChange = 0;
         return AUDIOPRCS_ERROR_PITCH;
      }
   }
   else {
      if (mPitchChange < -100 || mPitchChange > 100) {
         mPitchChange = 0;
      }
   }
   bIsCheck = true;
   return AUDIOPRCS_ERROR_SUCCESS;
}

ErrCode VoiceChange::Process(char *inputData, int sizeInBytes)
{
   //check input parameter
   if (sizeInBytes % mBytePerSample || sizeInBytes < 0) {
      return AUDIOPRCS_ERROR_INPUTSIZE;
   }

   if (inputData == nullptr) {
      return AUDIOPRCS_ERROR_INOUT_NULLPTR;
   }
   //anything to do?
   if ((mProcessType == VOICETYPE_NONE ||(mProcessType == VOICETYPE_USRDEFINE && mPitchChange == 0))&& sizeInBytes > 0) {
      //int ret;
      mBytesRemain += sizeInBytes;
      if (mFormat == AFMT_FLOAT) {
         mBufferOutputFloat->Input(inputData, sizeInBytes);
      }
      else if (mFormat == AFMT_SHORT) {
         mBufferOutputShort->Input(inputData, sizeInBytes);
      }
      return AUDIOPRCS_ERROR_SUCCESS;
   }
   else if (mProcessType == VOICETYPE_NONE || (mProcessType == VOICETYPE_USRDEFINE && mPitchChange == 0) || sizeInBytes == 0) {
      return AUDIOPRCS_ERROR_NOTHING_DONE;
   }
  
   int nBytesIn = 0;
   int nBytesOut = 0;
   //int outNum = 0;

   if (mFormat == AFMT_FLOAT) {
      int cvtBytes = sizeInBytes * sizeof(short) / sizeof(float);
      nBytesIn = FloatToShort((float *)inputData, (short *)mBufferInput->DataEndPtrBefore(cvtBytes), sizeInBytes);
      if (nBytesIn != cvtBytes) {
         return AUDIOPRCS_ERROR_BUFFER_MEMO;
      }
      mBufferInput->DataEndPtrAfter(cvtBytes);
      nBytesIn = sizeInBytes;
   }
   else if (mFormat == AFMT_SHORT) {
      nBytesIn = sizeInBytes;
      if (mBufferInput->Input(inputData, nBytesIn) < nBytesIn) {
         return AUDIOPRCS_ERROR_BUFFER_MEMO;
      }
   }
   if (mBytesZeros > nBytesIn) {
      return AUDIOPRCS_ERROR_INPUTSIZE;
   }
   mBytesRemain += (nBytesIn - mBytesZeros);
   mBytesZeros = 0;
   mBytesFlushOut = 0;
   //TODO no convert version
   if (mProcessType == VOICETYPE_CUTE || mProcessType == VOICETYPE_USRDEFINE) {
      nBytesOut = ProcessCore();
   }
  
   mBytesFlushOut = nBytesOut;
   if (nBytesOut > 0) {
      if (mFormat == AFMT_FLOAT) {
         //convert to float
         int cvtBytes = nBytesOut * sizeof(float) / sizeof(short);
         cvtBytes = ShortToFloat((short *)mBufferOutputShort->GetDataPtr(), (float *)mBufferOutputFloat->DataEndPtrBefore(cvtBytes), nBytesOut);
         mBufferOutputFloat->DataEndPtrAfter(cvtBytes);
         if (nBytesOut != cvtBytes * sizeof(short) / sizeof(float)) {
            return AUDIOPRCS_ERROR_BUFFER_MEMO;
         }
         mBufferOutputShort->Remove(nBytesOut);
         mBytesFlushOut = cvtBytes;
      }
      /*else if (mFormat == AFMT_SHORT) {
         mBytesRemain -= nBytesOut;
      }*/
   }
   return AUDIOPRCS_ERROR_SUCCESS;
}

int VoiceChange::GetOutputSize()
{
   if (mFormat == AFMT_FLOAT) {
      return mBufferOutputFloat->GetBufPos() - mBytesZeros;
   }
   else if (mFormat == AFMT_SHORT) {
      return mBufferOutputShort->GetBufPos() - mBytesZeros;
   }
   return -1;
}

int VoiceChange::GetOutputData(char * outputData, int outputBytes)
{
   int nBytesOut = 0;
   if (mFormat == AFMT_FLOAT) {
      if (outputBytes > mBytesRemain) {
         outputBytes = mBytesRemain;
      }
      nBytesOut = mBufferOutputFloat->Output(outputData, outputBytes);
      mBytesRemain -= nBytesOut;
      if (mBytesRemain == 0 && mBufferOutputFloat->GetBufPos() != 0) {
         mBufferOutputFloat->Remove(mBufferOutputFloat->GetBufPos());
      }
      return nBytesOut;
   }
   else if (mFormat == AFMT_SHORT) {
      if (outputBytes > mBytesRemain) {
         outputBytes = mBytesRemain;
      }
      nBytesOut = mBufferOutputShort->Output(outputData, outputBytes);
      mBytesRemain -= nBytesOut;
      if (mBytesRemain == 0 && mBufferOutputShort->GetBufPos() != 0) {
         mBufferOutputShort->Remove(mBufferOutputShort->GetBufPos());
      }
      return nBytesOut;
   }
   return -1;
}

ErrCode VoiceChange::Flush()
{
   ErrCode ret = AUDIOPRCS_ERROR_SUCCESS;
   if (mBytesRemain <= 0) {
      mBytesRemain = 0;
      return AUDIOPRCS_ERROR_NOTHING_DONE;
   }
   int i;
   char *zeros = new char[128 * mBytePerUnit];
   memset(zeros, 0, 128 * mBytePerUnit);
   //stop when no data remain or more than 500 * 128 samples
   int currRemain = mBytesRemain;
   for (i = 0; (currRemain > 0) && (i < 500); i++ ) {
      mBytesZeros += 128 * mBytePerUnit;
      ret = Process(zeros, mBytePerUnit * 128);
      currRemain -= mBytesFlushOut;
      if (ret != AUDIOPRCS_ERROR_SUCCESS) {
         return ret;
      }
   }
   return AUDIOPRCS_ERROR_SUCCESS;
}

ErrCode VoiceChange::ChangeVoiceType(VoiceChangeType newType)
{
   ErrCode ret = AUDIOPRCS_ERROR_SUCCESS;
   if (newType == mProcessType) {
      ret = AUDIOPRCS_ERROR_NOTHING_DONE;
   }
   else if (mProcessType == VOICETYPE_NONE) {
      mProcessType = newType;
      mIsFirstFrame = true;
      if (newType != mLastType) {
         if (newType == VOICETYPE_USRDEFINE) {
            double pitchShift = mPitchChange / 120;
            mRate = exp(0.69314718056 * pitchShift);
            mLastType = VOICETYPE_USRDEFINE;
         }
         else {
            mRate = exp(0.69314718056 * 0.25);//0.25 is the pitch change
            mLastType = VOICETYPE_CUTE;
         }
         mTempo = 1 / mRate;
         mSkipEst = mTempo * (mWindowSamples - mOverlapSamples);
         mRequireSamples = MAX((int)(mSkipEst + 0.5) + mOverlapSamples, mWindowSamples) + mSeekSamples;
         if (mRate > 1.0) {
            mCutOffFreq = 0.5 / mRate;
         }
         else {
            mCutOffFreq = 0.5 * mRate;
         }
      }
      

   }
   else {
      ret = Flush();
      //mBufferInput->Remove(mBufferInput->GetBufPos());
      mBufferMiddle1->Remove(mBufferMiddle1->GetBufPos());
      mBufferMiddle2->Remove(mBufferMiddle2->GetBufPos());
      mProcessType = newType;
      mIsFirstFrame = true;
      if (newType != VOICETYPE_NONE && newType != mLastType) {
         if (newType == VOICETYPE_USRDEFINE) {
            double pitchShift = mPitchChange / 120;
            mRate = exp(0.69314718056 * pitchShift);
            mLastType = VOICETYPE_USRDEFINE;
         }
         else {
            mRate = exp(0.69314718056 * 0.25);//0.25 is the pitch change
            mLastType = VOICETYPE_CUTE;
         }
         mTempo = 1 / mRate;
         mSkipEst = mTempo * (mWindowSamples - mOverlapSamples);
         mRequireSamples = MAX((int)(mSkipEst + 0.5) + mOverlapSamples, mWindowSamples) + mSeekSamples;
         if (mRate > 1.0) {
            mCutOffFreq = 0.5 / mRate;
         }
         else {
            mCutOffFreq = 0.5 * mRate;
         }
      }
   }
   return ret;
}

VoiceChangeType VoiceChange::GetChangeType()
{
   return mProcessType;
}

int VoiceChange::ProcessCore()
{
   if (bIsCheck == false) {
      ErrCode ret;
      ret = Validate();
      if (ret != AUDIOPRCS_ERROR_SUCCESS) {
         return ret;
      }
   }
   int nBytesOut = 0;
   FIFOAudioBuffer *mSrc, *mDst;
 
   mSrc = mBufferInput;
   mDst = mBufferMiddle1;
   WSOLAprocess(mSrc, mDst);

   mSrc = mBufferMiddle1;
   mDst = mBufferOutputShort;
   nBytesOut = Interpolate(mSrc, mDst);

   return nBytesOut;
}
int VoiceChange::FloatToShort(float *src, short *dst, int bytes)
{
   //check input parameter
   if (src == nullptr || dst == nullptr || bytes < 0 || bytes % (sizeof(float))) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   int i, samples;
   samples = bytes / sizeof(float);
   for (i = 0; i < samples; i++) {
      dst[i] = (short)CLIP(src[i] * 32768.0 ,  -32768.0 , 32767.0);
   }
   return samples * sizeof(short);
}
int VoiceChange::ShortToFloat(short *src, float *dst, int bytes)
{
   //check input parameter
   if (src == nullptr || dst == nullptr || bytes < 0 || bytes % (sizeof(short))) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   int i, samples;
   double conv = 1.0 / 32768.0;
   samples = bytes / sizeof(short);
   for (i = 0; i < samples; i++) {
      dst[i] = (float)(src[i] * conv);
   }
   return samples * sizeof(float);
}

int VoiceChange::WSOLAprocess(FIFOAudioBuffer *srcBuffer, FIFOAudioBuffer *dstBuffer)
{
   int delta = 0;
   int tmpLength = 0;
   int skipInt = 0;
   int rmLength = 0;
   
   
   while (srcBuffer->GetBufPos() / mBytePerUnit >= mRequireSamples) {
      //srcBuffer->OutputToBuffer(dstBuffer, mRequireSamples * mBytePerUnit);
      //continue;
      if (mIsFirstFrame == false) {
         rmLength = 0;
         //find best delta
         delta = FindDelta(srcBuffer->GetDataPtr());
         //delta = 0;
         //mix the samples at position of "delta" in SrcBuffer with TmpSample
         DoOLA(dstBuffer->DataEndPtrBefore(mOverlapSamples * mBytePerUnit), srcBuffer->GetDataPtr(), (unsigned int)delta);
         dstBuffer->DataEndPtrAfter(mOverlapSamples * mBytePerUnit);
         delta += mOverlapSamples;

         //in case buffer overflow
         if (srcBuffer->GetBufPos() / mBytePerUnit < (mWindowSamples - mOverlapSamples + delta)) {
            continue;
         }
         
         //this part should be saved in mTmpSamples waiting for mixing
         tmpLength = (mWindowSamples - 2 * mOverlapSamples);
         memcpy(mTempSamples, srcBuffer->GetDataPtr() + (delta + tmpLength) * mBytePerUnit, mOverlapSamples * mBytePerUnit);
         
         srcBuffer->Output(mUselessSamples + rmLength, delta * mBytePerUnit);
         rmLength += delta;

         //this part could be copied into output buffer directly
         srcBuffer->CopyToBuffer(dstBuffer, tmpLength * mBytePerUnit);
      }
      else {
         mIsFirstFrame = false;
         int skip = (int)(mTempo * mOverlapSamples + 0.5);
         delta = 0;
         //TODO ASM OPTIMIZATION
         mSkipFract -= skip;

         //in case buffer overflow
         if (srcBuffer->GetBufPos() / mBytePerUnit < (mWindowSamples - mOverlapSamples + delta)) {
            continue;
         }

         //this part could be copied into output buffer directly
         tmpLength = (mWindowSamples - 2 * mOverlapSamples);
         srcBuffer->OutputToBuffer(dstBuffer, tmpLength * mBytePerUnit);
         rmLength = tmpLength;

         //this part should be saved in mTmpSamples waiting for mixing
         memcpy(mTempSamples, srcBuffer->GetDataPtr() + tmpLength * mBytePerUnit, mOverlapSamples * mBytePerUnit);
      }
      //remove processed samples
      mSkipFract += mSkipEst;
      skipInt = (int)mSkipFract;
      mSkipFract -= skipInt;
      if (skipInt >= rmLength) {
         srcBuffer->Remove((skipInt - rmLength) * mBytePerUnit);
      }
      else {
         srcBuffer->Insert(mUselessSamples + skipInt * mBytePerUnit, (rmLength - skipInt) * mBytePerUnit, 0);
      }      
   }
   return 0;
}

int VoiceChange::FindDelta(char *src)
{
   int bestPos;
   double bestCorr;
   int i;
   double norm;

   bestCorr = -FLT_MAX;
   bestPos = 0;

   // Scans for the best correlation value by testing each possible position
   // over the permitted range.
   /*if (mFormat == AFMT_SHORT) {
	   bestCorr = calcCrossCorr((short *)src, (short *)mTempSamples, norm);
   }
   else if (mFormat == AFMT_FLOAT) {
	   bestCorr = calcCrossCorr((float *)src, (float *)mTempSamples, norm);
   }*/
   bestCorr = calcCrossCorr((short *)src, (short *)mTempSamples, norm);
   bestCorr = (bestCorr + 0.1) * 0.75;

#pragma omp parallel for
   for (i = 1; i < mSeekSamples; i++)
   {
      double corr;
      // Calculates correlation value for the mixing position corresponding to 'i'
#ifdef _OPENMP
        // in parallel OpenMP mode, can't use norm accumulator version as parallel executor won't
        // iterate the loop in sequential order
      corr = calcCrossCorr(refPos + channels * i, pMidBuffer, norm);
#else
        // In non-parallel version call "calcCrossCorrAccumulate" that is otherwise same
        // as "calcCrossCorr", but saves time by reusing & updating previously stored 
        // "norm" value
	  /*if (mFormat == AFMT_SHORT) {
		  corr = calcCrossCorrAccumulate((short *)src + mChannelNum * i, (short *)mTempSamples, norm);
	  }
	  else if (mFormat == AFMT_FLOAT) {
		  corr = calcCrossCorrAccumulate((float *)src + mChannelNum * i, (float *)mTempSamples, norm);
	  }*/
     corr = calcCrossCorrAccumulate((short *)src + mChannelNum * i, (short *)mTempSamples, norm);
#endif
      // heuristic rule to slightly favour values close to mid of the range
      double tmp = (double)(2 * i - mSeekSamples) / (double)mSeekSamples;
      corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

      // Checks for the highest correlation value
      if (corr > bestCorr)
      {
         // For optimal performance, enter critical section only in case that best value found.
         // in such case repeat 'if' condition as it's possible that parallel execution may have
         // updated the bestCorr value in the mean time
#pragma omp critical
         if (corr > bestCorr)
         {
            bestCorr = corr;
            bestPos = i;
         }
      }
   }
   /*if (mFormat == AFMT_SHORT) {
	   adaptNormalizer();
   }*/
   adaptNormalizer();
   // clear cross correlation routine state if necessary (is so e.g. in MMX routines).
   clearCrossCorrState();

   return bestPos;
}

void VoiceChange::DoOLA(char * dst, char * src, unsigned int delta)
{
   // mono sound.
   if (mChannelNum == 1)
   {
      /*if (mFormat == AFMT_SHORT) {*/
         int i,j;
         short *output, *input, *mid;
         output = (short *)dst;
         input = (short *)src + delta * mChannelNum;
         mid = (short *)mTempSamples;
         for (i = 0, j = mOverlapSamples; i < mOverlapSamples; i++, j--)
         {
            output[i] = (input[i] * (short)i + mid[i] * (short)j) / mOverlapSamples;
         }
      /*}
      else if (mFormat == AFMT_FLOAT) {
         int i;
         float fScale;
         float f1;
         float f2;
         float *output, *input, *mid;
         output = (float *)dst;
         input = (float *)src + delta;
         mid = (float *)mTempSamples;

         fScale = 1.0f / (float)mOverlapSamples;

         f1 = 0;
         f2 = 1.0f;

         for (i = 0; i < 2 * (int)mOverlapSamples; i ++)
         {
            output[i] = input[i] * f1 + mid[i] * f2;
            f1 += fScale;
            f2 -= fScale;
         }
      }*/
   }
   // stereo sound
   else if (mChannelNum == 2)
   {
      /*if (mFormat == AFMT_SHORT) {*/
         int i;
         short temp;
         int cnt2;
         short *output, *input, *mid;
         output = (short *)dst;
         input = (short *)src + delta * mChannelNum;
         mid = (short *)mTempSamples;
         for (i = 0; i < mOverlapSamples; i++)
         {
            temp = (short)(mOverlapSamples - i);
            cnt2 = 2 * i;
            output[cnt2] = (input[cnt2] * i + mid[cnt2] * temp) / mOverlapSamples;
            output[cnt2 + 1] = (input[cnt2 + 1] * i + mid[cnt2 + 1] * temp) / mOverlapSamples;
         }
      /*}
      else if (mFormat == AFMT_FLOAT) {
         int i;
         float fScale;
         float f1;
         float f2;
         float *output, *input, *mid;
         output = (float *)dst;
         input = (float *)src + delta;
         mid = (float *)mTempSamples;

         fScale = 1.0f / (float)mOverlapSamples;

         f1 = 0;
         f2 = 1.0f;

         for (i = 0; i < 2 * (int)mOverlapSamples; i += 2)
         {
            output[i + 0] = input[i + 0] * f1 + mid[i + 0] * f2;
            output[i + 1] = input[i + 1] * f1 + mid[i + 1] * f2;

            f1 += fScale;
            f2 -= fScale;
         }
      }*/
   }
}

double VoiceChange::calcCrossCorr(float * mixingPos, float * compare, double & norm)
{
   double corr;
   double norm0;
   int i;

   corr = norm0 = 0;
   // Same routine for stereo and mono. For Stereo, unroll by factor of 2.
   // For mono it's same routine yet unrollsd by factor of 4.
   for (i = 0; i < mChannelNum * mOverlapSamples; i += 4)
   {
      corr += mixingPos[i] * compare[i] +
         mixingPos[i + 1] * compare[i + 1];

      norm0 += mixingPos[i] * mixingPos[i] +
         mixingPos[i + 1] * mixingPos[i + 1];

      // unroll the loop for better CPU efficiency:
      corr += mixingPos[i + 2] * compare[i + 2] +
         mixingPos[i + 3] * compare[i + 3];

      norm0 += mixingPos[i + 2] * mixingPos[i + 2] +
         mixingPos[i + 3] * mixingPos[i + 3];
   }

   norm = norm0;
   return corr / sqrt((norm0 < 1e-9 ? 1.0 : norm0));
}

double VoiceChange::calcCrossCorr(short * mixingPos, short * compare, double & norm)
{
	long corr;
	unsigned long lnorm;
	int i;

	corr = lnorm = 0;
	// Same routine for stereo and mono. For stereo, unroll loop for better
	// efficiency and gives slightly better resolution against rounding. 
	// For mono it same routine, just  unrolls loop by factor of 4
	for (i = 0; i < mChannelNum * mOverlapSamples; i += 4)
	{
		corr += (mixingPos[i] * compare[i] +
			mixingPos[i + 1] * compare[i + 1]) >> overlapDividerBitsNorm;  // notice: do intermediate division here to avoid integer overflow
		corr += (mixingPos[i + 2] * compare[i + 2] +
			mixingPos[i + 3] * compare[i + 3]) >> overlapDividerBitsNorm;
		lnorm += (mixingPos[i] * mixingPos[i] +
			mixingPos[i + 1] * mixingPos[i + 1]) >> overlapDividerBitsNorm; // notice: do intermediate division here to avoid integer overflow
		lnorm += (mixingPos[i + 2] * mixingPos[i + 2] +
			mixingPos[i + 3] * mixingPos[i + 3]) >> overlapDividerBitsNorm;
	}

	if (lnorm > maxnorm)
	{
		// modify 'maxnorm' inside critical section to avoid multi-access conflict if in OpenMP mode
#pragma omp critical
		if (lnorm > maxnorm)
		{
			maxnorm = lnorm;
		}
	}
	// Normalize result by dividing by sqrt(norm) - this step is easiest 
	// done using floating point operation
	norm = (double)lnorm;
	return (double)corr / sqrt((norm < 1e-9) ? 1.0 : norm);
}

double VoiceChange::calcCrossCorrAccumulate(float * mixingPos, float * compare, double & norm)
{
   double corr;
   int i;

   corr = 0;

   // cancel first normalizer tap from previous round
   for (i = 1; i <= mChannelNum; i++)
   {
      norm -= mixingPos[-i] * mixingPos[-i];
   }

   // Same routine for stereo and mono. For Stereo, unroll by factor of 2.
   // For mono it's same routine yet unrollsd by factor of 4.
   for (i = 0; i < mChannelNum * mOverlapSamples; i += 4)
   {
      corr += mixingPos[i] * compare[i] +
         mixingPos[i + 1] * compare[i + 1] +
         mixingPos[i + 2] * compare[i + 2] +
         mixingPos[i + 3] * compare[i + 3];
   }

   // update normalizer with last samples of this round
   for (int j = 0; j < mChannelNum; j++)
   {
      i--;
      norm += mixingPos[i] * mixingPos[i];
   }

   return corr / sqrt((norm < 1e-9 ? 1.0 : norm));
}

double VoiceChange::calcCrossCorrAccumulate(short * mixingPos, short * compare, double & norm)
{
	long corr;
	unsigned long lnorm;
	int i;

	// cancel first normalizer tap from previous round
	lnorm = 0;
	for (i = 1; i <= mChannelNum; i++)
	{
		lnorm -= (mixingPos[-i] * mixingPos[-i]) >> overlapDividerBitsNorm;
	}

	corr = 0;
	// Same routine for stereo and mono. For stereo, unroll loop for better
	// efficiency and gives slightly better resolution against rounding. 
	// For mono it same routine, just  unrolls loop by factor of 4
	for (i = 0; i < mChannelNum * mOverlapSamples; i += 4)
	{
		corr += (mixingPos[i] * compare[i] +
			mixingPos[i + 1] * compare[i + 1]) >> overlapDividerBitsNorm;  // notice: do intermediate division here to avoid integer overflow
		corr += (mixingPos[i + 2] * compare[i + 2] +
			mixingPos[i + 3] * compare[i + 3]) >> overlapDividerBitsNorm;
	}

	// update normalizer with last samples of this round
	for (int j = 0; j < mChannelNum; j++)
	{
		i--;
		lnorm += (mixingPos[i] * mixingPos[i]) >> overlapDividerBitsNorm;
	}

	norm += (double)lnorm;
	if (norm > maxnorm)
	{
		maxnorm = (unsigned long)norm;
	}

	// Normalize result by dividing by sqrt(norm) - this step is easiest 
	// done using floating point operation	
	return (double)corr / sqrt((norm < 1e-9) ? 1.0 : norm);
}

void VoiceChange::clearCrossCorrState()
{
	;//actually we don't need to do anything
}

void VoiceChange::adaptNormalizer()
{
	// Do not adapt normalizer over too silent sequences to avoid averaging filter depleting to
	// too low values during pauses in music
	if ((maxnorm > 1000) || (maxnormf > 40000000))
	{
		//norm averaging filter
		maxnormf = 0.9f * maxnormf + 0.1f * (float)maxnorm;

		if ((maxnorm > 800000000) && (overlapDividerBitsNorm < 16))
		{
			// large values, so increase divider
			overlapDividerBitsNorm++;
			if (maxnorm > 1600000000) overlapDividerBitsNorm++; // extra large value => extra increase
		}
		else if ((maxnormf < 1000000) && (overlapDividerBitsNorm > 0))
		{
			// extra small values, decrease divider
			overlapDividerBitsNorm--;
		}
	}

	maxnorm = 0;
}

int VoiceChange::SmoothFilter(FIFOAudioBuffer * srcBuffer, FIFOAudioBuffer * dstBuffer)
{
   int srcSamples = srcBuffer->GetBufPos() / mBytePerUnit;
   int outSamples = 0;

   /*if (mFormat == AFMT_FLOAT) {
      float *src = (float *)srcBuffer->GetDataPtr();
      float *dst = (float *)dstBuffer->DataEndPtrBefore(srcSamples * mBytePerUnit);
      outSamples = DoFilter(src, dst, srcSamples);
   }
   else if (mFormat == AFMT_SHORT) {
      short *src = (short *)srcBuffer->GetDataPtr();
      short *dst = (short *)dstBuffer->DataEndPtrBefore(srcSamples * mBytePerUnit);
      outSamples = DoFilter(src, dst, srcSamples);
   }*/
   short *src = (short *)srcBuffer->GetDataPtr();
   short *dst = (short *)dstBuffer->DataEndPtrBefore(srcSamples * mBytePerUnit);
   outSamples = DoFilter(src, dst, srcSamples);

   srcBuffer->Remove(outSamples * mBytePerUnit);
   dstBuffer->DataEndPtrAfter(outSamples * mBytePerUnit);
   return 0;
}

int VoiceChange::DoFilter(short * src, short * dst, int samples)
{
   int j, end, nStep;
   //check input parameter
   if (src == nullptr || dst == nullptr) {
      return -1;
   }
   //check member vars
   if (mFilterLength <= 0 || mCoeffsShort == nullptr) {
      return -1;
   }
   end = mChannelNum * (samples - mFilterLength);
   nStep = mChannelNum;

#pragma omp parallel for
   for (j = 0; j < end; j += nStep)
   {
      const short *ptr;
      long suml, sumr;
      int i;

      suml = sumr = 0;
      ptr = src + j;

      for (i = 0; i < mFilterLength; i += 4)
      {
         // loop is unrolled by factor of 4 here for efficiency
         if (mChannelNum == 2) {
            suml += ptr[2 * i + 0] * mCoeffsShort[i + 0] +
               ptr[2 * i + 2] * mCoeffsShort[i + 1] +
               ptr[2 * i + 4] * mCoeffsShort[i + 2] +
               ptr[2 * i + 6] * mCoeffsShort[i + 3];
            sumr += ptr[2 * i + 1] * mCoeffsShort[i + 0] +
               ptr[2 * i + 3] * mCoeffsShort[i + 1] +
               ptr[2 * i + 5] * mCoeffsShort[i + 2] +
               ptr[2 * i + 7] * mCoeffsShort[i + 3];
         }
         else if (mChannelNum == 1) {
            suml += ptr[i + 0] * mCoeffsShort[i + 0] +
               ptr[i + 1] * mCoeffsShort[i + 1] +
               ptr[i + 2] * mCoeffsShort[i + 2] +
               ptr[i + 3] * mCoeffsShort[i + 3];
         }
      }
      if (mChannelNum == 2) {
         suml >>= FILTER_Q;
         sumr >>= FILTER_Q;
         suml = CLIP(suml, -32768, 32767);
         sumr = CLIP(sumr, -32768, 32767);

         dst[j] = (short)suml;
         dst[j + 1] = (short)sumr;
      }
      else if (mChannelNum == 1) {
         suml >>= FILTER_Q;
         suml = CLIP(suml, -32768, 32767);

         dst[j] = (short)suml;
      }
   }
   return samples - mFilterLength;
}

int VoiceChange::DoFilter(float * src, float * dst, int samples)
{
   int j, end , nStep;
   double dScaler = 1.0 / (double)(FILTER_SCALE);
   //check input parameter
   if (src == nullptr || dst == nullptr) {
      return -1;
   }
   //check member vars
   if (mFilterLength <= 0 || mCoeffsFloat == nullptr) {
      return -1;
   }
   end = mChannelNum * (samples - mFilterLength);
   nStep = mChannelNum;

#pragma omp parallel for
   for (j = 0; j < end; j += nStep)
   {
      const float *ptr;
      double suml, sumr;
      int i;

      suml = sumr = 0;
      ptr = src + j;

      for (i = 0; i < mFilterLength; i += 4)
      {
         // loop is unrolled by factor of 4 here for efficiency
         if(mChannelNum == 2){
            suml += ptr[2 * i + 0] * mCoeffsFloat[i + 0] +
               ptr[2 * i + 2] * mCoeffsFloat[i + 1] +
               ptr[2 * i + 4] * mCoeffsFloat[i + 2] +
               ptr[2 * i + 6] * mCoeffsFloat[i + 3];
            sumr += ptr[2 * i + 1] * mCoeffsFloat[i + 0] +
               ptr[2 * i + 3] * mCoeffsFloat[i + 1] +
               ptr[2 * i + 5] * mCoeffsFloat[i + 2] +
               ptr[2 * i + 7] * mCoeffsFloat[i + 3];
         }else if(mChannelNum == 1){
            suml += ptr[i + 0] * mCoeffsFloat[i + 0] +
               ptr[i + 1] * mCoeffsFloat[i + 1] +
               ptr[i + 2] * mCoeffsFloat[i + 2] +
               ptr[i + 3] * mCoeffsFloat[i + 3];
         }
         
      }
      if (mChannelNum == 2) {
         suml *= dScaler;
         sumr *= dScaler;

         dst[j] = (float)suml;
         dst[j + 1] = (float)sumr;
      }
      else if (mChannelNum == 1) {
         suml *= dScaler;
         dst[j] = (float)suml;
      } 
   }
   return samples - mFilterLength;
}

bool VoiceChange::CalcCoeffs()
{
   int i, len;
   double cntTemp, temp, tempCoeff, h, w;
   double wc;
   double scaleCoeff, sum;
   double *work;
   //check parameter
   if (mFilterLength <= 0 || mCutOffFreq < 0 || mCutOffFreq > 0.5) {
      return false;
   }
   //filter length must be divisible by 8
   len = mFilterLength / 8;
   mFilterLength = len * 8;

   work = new double[mFilterLength];

   wc = 2.0 * PI * mCutOffFreq;
   tempCoeff = TWOPI / (double)mFilterLength;

   sum = 0;
   for (i = 0; i < mFilterLength; i++)
   {
      cntTemp = (double)i - (double)(mFilterLength / 2);

      temp = cntTemp * wc;
      if (temp != 0)
      {
         h = sin(temp) / temp;                     // sinc function
      }
      else
      {
         h = 1.0;
      }
      w = 0.54 + 0.46 * cos(tempCoeff * cntTemp);       // hamming window

      temp = w * h;
      work[i] = temp;

      // calc net sum of coefficients 
      sum += temp;
   }
   //check sum is positive
   if (sum <= 0) {
      return false;
   }
   // ensure a lowpass filter being designed...
   if (!(work[mFilterLength / 2] > 0) || !(work[mFilterLength / 2 + 1] > -1e-6) || !(work[mFilterLength / 2 - 1] > -1e-6)) {
      return false;
   }

   // Calculate a scaling coefficient in such a way that the result can be
   // divided by 16384 (qScale = 14)
   scaleCoeff = 16384.0f / sum;

   for (i = 0; i < mFilterLength; i++)
   {
      temp = work[i] * scaleCoeff;
      // scale & round to nearest integer
      temp += (temp >= 0) ? 0.5 : -0.5;
      // ensure no overfloods
      temp = CLIP(temp, -32768, 32767);
      mCoeffsShort[i] = (short)temp;
      mCoeffsFloat[i] = (float)temp;
   }
   delete[] work;
   return true;
}

int VoiceChange::Interpolate(FIFOAudioBuffer *srcBuffer, FIFOAudioBuffer *dstBuffer)
{
   int srcSamples;
   int outSamplesEst;
   int outSamples;

   srcSamples = srcBuffer->GetBufPos() / mBytePerUnit;
   outSamplesEst = (int)((double)srcSamples / mRate) + 8;
   outSamples = 0;

   if (srcSamples == 0) {
      return 0;
   }

   /*if (mFormat == AFMT_SHORT) {
      short *src = (short *)srcBuffer->GetDataPtr();
      short *dst = (short *)dstBuffer->DataEndPtrBefore(outSamplesEst * mBytePerUnit);
      outSamples = DoInterpolate(src, dst, srcSamples);
   }
   else if(mFormat == AFMT_FLOAT)
   {
      float *src = (float *)srcBuffer->GetDataPtr();
      float *dst = (float *)dstBuffer->DataEndPtrBefore(outSamplesEst * mBytePerUnit);
      outSamples = DoInterpolate(src, dst, srcSamples);
   }*/
   short *src = (short *)srcBuffer->GetDataPtr();
   short *dst = (short *)dstBuffer->DataEndPtrBefore(outSamplesEst * mBytePerUnit);
   outSamples = DoInterpolate(src, dst, srcSamples);

   dstBuffer->DataEndPtrAfter(outSamples * mBytePerUnit);
   srcBuffer->Remove(srcSamples * mBytePerUnit);

	return outSamples * mBytePerUnit;
}
int VoiceChange::DoInterpolate(short * src, short * dst, int samples)
{
   int i = 0;
   int end = samples - 1;
   int cnt = 0;
   int norm = 0;
   long temp1 = 0;
   long temp2 = 0;
   int scaleRate = (int)(mRate * MAXSCALE_SHORT + 0.5);

   while (cnt < end) {      
      if (mWeightShort >= MAXSCALE_SHORT || mWeightShort < 0) {
         return -1;
      }
      if (mChannelNum == 1) {
         temp1 = (MAXSCALE_SHORT - mWeightShort) * src[0] + mWeightShort * src[1];
         dst[i] = (short)(temp1 / MAXSCALE_SHORT);
         i++;
         mWeightShort += scaleRate;
         norm = mWeightShort / MAXSCALE_SHORT;
         mWeightShort -= norm * MAXSCALE_SHORT;//???:can we use % to instead
         cnt += norm;
         src += norm;
      }
      else if (mChannelNum == 2) {
         temp1 = (MAXSCALE_SHORT - mWeightShort) * src[0] + mWeightShort * src[2];
         temp2 = (MAXSCALE_SHORT - mWeightShort) * src[1] + mWeightShort * src[3];
         dst[i * 2] = (short)(temp1 / MAXSCALE_SHORT);
         dst[i * 2 + 1] = (short)(temp2 / MAXSCALE_SHORT);
         i++;
         mWeightShort += scaleRate;
         norm = mWeightShort / MAXSCALE_SHORT;
         mWeightShort -= norm * MAXSCALE_SHORT;//???:can we use % to instead
         cnt += norm;
         src += norm * 2;
      }            
   }
   samples = cnt;
   return i;
}
int VoiceChange::DoInterpolate(float * src, float * dst, int samples)
{
   int i = 0;
   int end = samples - 1;
   int cnt = 0;
   int norm = 0;
   double temp1 = 0;
   double temp2 = 0;

   while (cnt < end) {
      if ((mWeightFloat - 1) >= 1e-10 || mWeightFloat < 0) {
         return -1;
      }
      if (mChannelNum == 1) {
         temp1 = (1.0 - mWeightFloat) * src[0] + mWeightFloat * src[1];
         dst[i] = (float)temp1;
         i++;
         mWeightFloat += mRate;
         norm = (int)mWeightFloat;
         mWeightFloat -= norm;
         cnt += norm;
         src += norm;
      }
      else if (mChannelNum == 2) {
         temp1 = (1.0 - mWeightFloat) * src[0] + mWeightFloat * src[2];
         temp2 = (1.0 - mWeightFloat) * src[1] + mWeightFloat * src[3];
         dst[2 * i] = (float)temp1;
         dst[2 * i + 1] = (float)temp2;
         i++;
         mWeightFloat += mRate;
         norm = (int)mWeightFloat;
         mWeightFloat -= norm;
         cnt += norm;
         src += norm * 2;
      }
   }
   samples = cnt;
   return i;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------