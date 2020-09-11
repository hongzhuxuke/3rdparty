#include "Utility.h"
#include "IDeckLinkDevice.h"
#include "MediaAudioSource.h"
#include "IMediaOutput.h"
#include "Logging.h"

MediaAudioSource::MediaAudioSource(IMediaOutput* mediaOutput, UINT dstSampleRateHz)
:IAudioSource(eAudioSource_Media),mMediaOutput(mediaOutput) {
   mDstSampleRateHz = dstSampleRateHz;
   mIsMediaOutInit = false;
   gLogger->logInfo("%s\n", __FUNCTION__);
}


MediaAudioSource::~MediaAudioSource() {
   gLogger->logInfo("%s\n",__FUNCTION__);
   mIsMediaOutInit = false;
}

void MediaAudioSource::ClearAudioBuffer() {
   //mSampleBuffer.Clear();
   if (mMediaOutput) {
      mMediaOutput->ResetPlayFileAudioData();
   }
}


bool MediaAudioSource::GetNextBuffer(void **buffer, UINT *numFrames, QWORD *timestamp) {
   bool ret = false;
   if (mSampleBuffer.Num() < mSampleSegmentSizeIn100Ms) {
      void *newbuffer = NULL;
      UINT newNumFrames = 0;
      QWORD newTime = 0;
      bool tret = mMediaOutput->GetNextAudioBuffer(&newbuffer, &newNumFrames, &newTime);
      if (tret == true) {
         mSampleBuffer.AppendArray((BYTE*)newbuffer, mInputBlockSize*newNumFrames);
         mLastestAudioSampleTime = newTime;
      }
   }
   if (mSampleBuffer.Num() >= mSampleSegmentSizeIn100Ms) {
      memcpy(mOutputBuffer.Array(), mSampleBuffer.Array(), mSampleSegmentSizeIn100Ms);
      mSampleBuffer.RemoveRange(0, mSampleSegmentSizeIn100Ms);
      *buffer = mOutputBuffer.Array();
      *numFrames = mSampleFrameCountIn100MS;
      *timestamp = mLastestAudioSampleTime - mSampleBuffer.Num() / mSampleSegmentSizeIn100Ms;
      ret = true;
   }
   return ret;
}
void MediaAudioSource::ReleaseBuffer() {

}
CTSTR MediaAudioSource::GetDeviceName() const {
   return NULL;
   /*if (mMediaOutput) {
      return mDeckLinkDevice->GetDeckLinkDeviceName();
   } else {
      return NULL;
   }*/
}

bool MediaAudioSource::Initialize() {
   bool ret = false;
   bool  bFloat = false;
   DWORD inputChannelMask;

   ret = mMediaOutput->GetAudioParam(mInputChannels, mInputSamplesPerSec, mInputBitsPerSample);
   if (ret == true) {
      mInputBlockSize = (mInputChannels*mInputBitsPerSample) / 8;
      inputChannelMask = 0;
      InitAudioData(bFloat, mInputChannels, mInputSamplesPerSec, mInputBitsPerSample, mInputBlockSize, inputChannelMask);
      mSampleFrameCountIn100MS = mInputSamplesPerSec / 100;
      mSampleSegmentSizeIn100Ms = mInputBlockSize*mSampleFrameCountIn100MS;
      mOutputBuffer.SetSize(mSampleSegmentSizeIn100Ms);
      mIsMediaOutInit = true;
      gLogger->logInfo("MediaAudioSource::Initialize init device sucess mInputChannels= %d mInputSamplesPerSec=%d mInputBitsPerSample=%d.",
                       mInputChannels, mInputSamplesPerSec, mInputBitsPerSample);
   } else {
      mIsMediaOutInit = false;
   }
   return ret;
}
