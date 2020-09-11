
#ifndef _BUFFER_QUEUE_INCLUDE_
#define _BUFFER_QUEUE_INCLUDE_ 

typedef CRITICAL_SECTION v_mutex_t;
typedef HANDLE v_cond_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

void v_mtuex_init(v_mutex_t * mutex);
void v_cond_init(v_cond_t * cond);

void v_lock_mutex(v_mutex_t * mutex);
void v_unlock_mutex(v_mutex_t * mutex);
void v_cond_signal(v_cond_t * cond);
void v_cond_wait(v_cond_t * cond, v_mutex_t * mutex);
void v_cond_wait_timeout(v_cond_t * cond, v_mutex_t * mutex, DWORD timeInMs);

void v_mtuex_destory(v_mutex_t * cond);
void v_cond_destory(v_cond_t * cond);

typedef struct DataUnit {
   unsigned char* unitBuffer;
   uint64_t unitBufferSize;
   uint64_t dataSize;
   uint64_t timestap;
   DataUnit* next;
}DataUnit;
const int DataUnitSize = sizeof(DataUnit);

typedef struct Queue {
   DataUnit* head;
   DataUnit* tail;
   int        unitNumber;
   v_mutex_t mutex;
   v_cond_t cond;
}Queue;
const int QueueSize = sizeof(Queue);

class BufferQueue {
public:
   BufferQueue(long bufferSize, int maxQueueSize);
   virtual ~BufferQueue();
   long GetBufferSize();
   int GetUnitCount();
public:
   DataUnit* MallocDataUnit();
   void FreeDataUnit(DataUnit* dataUnit);

   DataUnit* GetDataUnit(bool block);
   int PutDataUnit(DataUnit*);
   void Flush();
private:
   void init();
   void destory();
   int AppendUnit2Queue(Queue *q, DataUnit *dataUnit);
   int PopUnitfromQueue(Queue *q, DataUnit **dataUnit, bool block);
   int GetQueueSize(Queue *q);
   void FreeQueue(Queue *q);
private:
   //for unused
   Queue mFreeQueue;
   Queue mBufferQueue;
   long  mBufferSize;
   int   mMaxQueueSize;
   int   mDataUnitCnt;
};


#endif