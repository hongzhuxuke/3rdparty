/*********************************************************************************
  *Copyright(C),2018-2019,Vhall Live Co.Lmt.
  *ModuleName: AudioBuffer
  *Author:     Xia Yang
  *Version:    1.6
  *Date:       2018-11-15
  *Description:A FIFO pipe buffer, used for some audio process  
**********************************************************************************/

#include <string.h>
#include "AudioBuffer.h"

FIFOAudioBuffer::FIFOAudioBuffer()
{
   mData = nullptr;
   mDataUnaligned = nullptr;
   mBufferByteSize = 0;
   mBufferBytePos = 0;
}

FIFOAudioBuffer::~FIFOAudioBuffer()
{
   if (mDataUnaligned != nullptr) {
      delete[]mDataUnaligned;
   }
   mData = nullptr;
   mDataUnaligned = nullptr;
   mBufferByteSize = 0;
   mBufferBytePos = 0;
}

bool FIFOAudioBuffer::init(int sizeInBytes)
{
   if (mDataUnaligned != nullptr) {
      delete[]mDataUnaligned;
   }
   mDataUnaligned = new char[sizeInBytes + 16];
   if (mDataUnaligned == nullptr) {
      mData = nullptr;
      return false;
   }
   mData = (char *)ALIGN_POINTER_16(mDataUnaligned);
   mBufferByteSize = sizeInBytes;
   mBufferBytePos = 0;
   return true;
}

int FIFOAudioBuffer::Input(char * src, int bytes)
{
   int bfsize = 0;
   if (src == nullptr) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   bfsize = CapacityEnsure(bytes + mBufferBytePos);
   if (bfsize > bytes) {
      memcpy(mData + mBufferBytePos, src, bytes);
      mBufferBytePos += bytes;
      return bytes;
   }
   else {
      return -1;
   }
}

int FIFOAudioBuffer::Insert(char * src, int bytes, int pos)
{
   int bfsize = 0;
   if (src == nullptr) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   if (pos < 0 || pos >mBufferBytePos) {
      return -1;
   }
   bfsize = CapacityEnsure(bytes + mBufferBytePos);
   if (bfsize > bytes) {
      memmove(mData + pos + bytes, mData + pos, bytes);
      memcpy(mData + pos, src, bytes);
      mBufferBytePos += bytes;
      return bytes;
   }
   else {
      return -1;
   }
}

int FIFOAudioBuffer::Output(char * dst, int bytes)
{
   int bfpos = 0;
   if (dst == nullptr || bytes < 0) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   bfpos = bytes;
   if (bytes > mBufferBytePos) {
      bfpos = mBufferBytePos;
   }
   memcpy(dst, mData, bfpos);
   mBufferBytePos -= bfpos;
   if (mBufferBytePos > 0) {
      memmove(mData, mData + bfpos, mBufferBytePos);
   }
   memset(mData + mBufferBytePos, 0, mBufferByteSize - mBufferBytePos);
   return bfpos;
}

int FIFOAudioBuffer::Copy(char * dst, int bytes)
{
   int bfpos = 0;
   if (dst == nullptr || bytes < 0) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   bfpos = bytes;
   if (bytes > mBufferBytePos) {
      bfpos = mBufferBytePos;
   }
   memcpy(dst, mData, bfpos);
   return bfpos;
}

int FIFOAudioBuffer::CopyToBuffer(FIFOAudioBuffer * other, int bytes)
{
   int bfpos = 0;
   if (other == nullptr) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   bfpos = bytes;
   if (bytes > mBufferBytePos) {
      bfpos = mBufferBytePos;
   }
   bfpos = other->Input(mData, bfpos);
   return bfpos;
}

int FIFOAudioBuffer::Remove(int bytes)
{
   int rm = 0;
   rm = bytes;
   if (rm < 0) {
      return -1;
   }
   if (rm >= mBufferBytePos) {
      memset(mData, 0, mBufferByteSize);
      rm = mBufferBytePos;
      mBufferBytePos = 0;
   }
   else {
      memmove(mData, mData + rm, mBufferBytePos - rm);
      mBufferBytePos -= rm;
   }
   return rm;
}

int FIFOAudioBuffer::OutputToBuffer(FIFOAudioBuffer * other, int bytes)
{
   int bfpos = 0;
   if (other == nullptr) {
      return -1;
   }
   if (bytes == 0) {
      return 0;
   }
   bfpos = bytes;
   if (bytes > mBufferBytePos) {
      bfpos = mBufferBytePos;
   }
   bfpos = other->Input(mData, bfpos);
   if (bfpos > 0) {
      mBufferBytePos -= bfpos;
      if (mBufferBytePos > 0) {
         memmove(mData, mData + bfpos, mBufferBytePos);
      }
   }
   else if (bfpos == -1) {
      return -1;
   }
   return bfpos;
}

int FIFOAudioBuffer::GetBufferSize()
{
   return mBufferByteSize;
}

char * FIFOAudioBuffer::GetDataPtr()
{
   return mData;
}

char * FIFOAudioBuffer::DataEndPtrBefore(int bytes)
{
   int bfsize = 0;
   if (bytes == 0) {
      return mData;
   }
   if (bytes > 0) {
      bfsize = CapacityEnsure(bytes + mBufferBytePos);
      if (bfsize >= bytes) {
         return mData + mBufferBytePos;
      }
   }
   if (bytes < 0)
   {
      bfsize = -bytes;
      if (bfsize <= mBufferBytePos) {
         return mData + mBufferBytePos;
      }
   }
   return nullptr;
}

int  FIFOAudioBuffer::DataEndPtrAfter(int bytes)
{
   if (bytes > 0 && mBufferBytePos + bytes <= mBufferByteSize) {
      mBufferBytePos += bytes;
      return mBufferBytePos;
   }
   else if (bytes <= 0 && (mBufferBytePos + bytes) >= 0) {
      mBufferBytePos += bytes;
      return mBufferBytePos;
   }
   return -1;
}

int FIFOAudioBuffer::GetBufPos()
{
   return mBufferBytePos;
}

int FIFOAudioBuffer::CapacityEnsure(int req)
{
   if (mData == nullptr || mDataUnaligned == nullptr) {
      return -1;
   }
   if (req > mBufferByteSize) {
      int multi;
      int step;
      char *tempUnligned, *temp;
      step = 1 << 20;
      if (step > mBufferByteSize) {
         step = mBufferByteSize;
      }
      multi = (req + step - 1) / step;
      tempUnligned = new char[multi * step + 16];
      temp = (char *)ALIGN_POINTER_16(tempUnligned);
      memcpy(temp, mData, mBufferBytePos);
      delete[]mDataUnaligned;
      mDataUnaligned = tempUnligned;
      mData = temp;
      mBufferByteSize = multi * step;
   }
   return mBufferByteSize;
}