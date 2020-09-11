/*********************************************************************************
  *Copyright(C),2018-2019,Vhall Live Co.Lmt.
  *ModuleName: AudioBuffer
  *Author:     Xia Yang
  *Version:    1.6
  *Date:       2018-11-15
  *Description:A FIFO pipe buffer, used for some audio process  
**********************************************************************************/

#ifndef VHALL_AUDIO_BUFFER_HH
#define VHALL_AUDIO_BUFFER_HH

#if defined(_WIN64)||defined(__LP64__)
//#ifdef _WIN64 || __LP64__
typedef unsigned long long ptrtype;
#else
typedef unsigned long ptrtype;
#endif

#define ALIGN_POINTER_16(x)      ( ( (ptrtype)(x) + 15 ) & ~(ptrtype)15 )

class FIFOAudioBuffer {
public:
   FIFOAudioBuffer();
   ~FIFOAudioBuffer();
   bool init(int size);
   int Input(char *src, int bytes);
   int Insert(char *src, int bytes, int pos);
   int Output(char *dst, int bytes);
   int OutputToBuffer(FIFOAudioBuffer *other, int bytes);
   int Copy(char *dst, int bytes);
   int CopyToBuffer(FIFOAudioBuffer *other, int bytes);
   int Remove(int bytes);
   int GetBufferSize();
   int GetBufPos();
   char *GetDataPtr();
   char *DataEndPtrBefore(int bytes);
   int DataEndPtrAfter(int bytes);

private:
   char* mData;
   char* mDataUnaligned;
   int mBufferByteSize;
   int mBufferBytePos;

   int CapacityEnsure(int req);
   //bool clear();
};

#endif // !VHALL_AUDIO_BUFFER_HH



