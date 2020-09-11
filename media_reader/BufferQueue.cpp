#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "BufferQueue.h"



void v_mtuex_init(v_mutex_t * mutex) {
   InitializeCriticalSection(mutex);

}
void v_cond_init(v_cond_t * cond) {
   *cond = CreateEvent(NULL, TRUE, FALSE, NULL);
}

void v_lock_mutex(v_mutex_t * mutex) {
   EnterCriticalSection(mutex);
}
void v_unlock_mutex(v_mutex_t * mutex) {
   LeaveCriticalSection(mutex);
}

void v_cond_signal(v_cond_t * cond) {
   SetEvent(*cond);
}
void v_cond_wait(v_cond_t * cond, v_mutex_t * mutex) {
   LeaveCriticalSection(mutex);
   WaitForSingleObject(*cond, -1);
   EnterCriticalSection(mutex);
}
void v_cond_wait_timeout(v_cond_t * cond, v_mutex_t * mutex, DWORD timeInMs) {
   LeaveCriticalSection(mutex);
   WaitForSingleObject(*cond, timeInMs);
   EnterCriticalSection(mutex);
}

void v_mtuex_destory(v_mutex_t * mutex) {
   DeleteCriticalSection(mutex);
}

void v_cond_destory(v_cond_t * cond) {
   CloseHandle(*cond);
}



BufferQueue::BufferQueue(long bufferSize, int maxQueueSize) :
mBufferSize(bufferSize),
mMaxQueueSize(maxQueueSize),
mDataUnitCnt(0) {
   init();
}

BufferQueue::~BufferQueue() {
   destory();
}


long BufferQueue::GetBufferSize() {
   return mBufferSize;
}
DataUnit* BufferQueue::MallocDataUnit() {
   DataUnit* dataUnit = NULL;
   if (PopUnitfromQueue(&mFreeQueue, &dataUnit, false) < 0) {
      if (mMaxQueueSize < mDataUnitCnt) {
         return NULL;
      } else {
         dataUnit = (DataUnit*)malloc(DataUnitSize);
         if (dataUnit) {
            dataUnit->unitBuffer = (unsigned char*)malloc(mBufferSize);
            dataUnit->unitBufferSize = mBufferSize;
            dataUnit->timestap = 0;
            dataUnit->next = NULL;
            mDataUnitCnt++;
         }
      }
   }
   return dataUnit;
}

int BufferQueue::GetUnitCount(){
   return mDataUnitCnt;
}

int BufferQueue::GetBufferQueueSize() {
	return GetQueueSize(&mBufferQueue);
}

void BufferQueue::FreeDataUnit(DataUnit* dataUnit) {
   AppendUnit2Queue(&mFreeQueue, dataUnit);
}
DataUnit* BufferQueue::GetDataUnit(bool block) {
   DataUnit* dataUnit = NULL;
   int ret = PopUnitfromQueue(&mBufferQueue, &dataUnit, block);
   //assert 
   return dataUnit;
}
int BufferQueue::PutDataUnit(DataUnit* dataUnit) {
   return AppendUnit2Queue(&mBufferQueue, dataUnit);
}
void BufferQueue::Flush() {
   DataUnit* dataUnit = NULL;
   while (PopUnitfromQueue(&mBufferQueue, &dataUnit, true)) {
      AppendUnit2Queue(&mFreeQueue, dataUnit);
   }
}

void BufferQueue::init() {
   memset(&mFreeQueue, 0, QueueSize);
   memset(&mBufferQueue, 0, QueueSize);
   v_mtuex_init(&mFreeQueue.mutex);
   v_cond_init(&mFreeQueue.cond);
   v_mtuex_init(&mBufferQueue.mutex);
   v_cond_init(&mBufferQueue.cond);
}
void BufferQueue::destory() {
   //free memory
   FreeQueue(&mFreeQueue);
   FreeQueue(&mBufferQueue);
   v_mtuex_destory(&mFreeQueue.mutex);
   v_cond_destory(&mFreeQueue.cond);
   v_mtuex_destory(&mBufferQueue.mutex);
   v_cond_destory(&mBufferQueue.cond);
}

int BufferQueue::AppendUnit2Queue(Queue *q, DataUnit *dataUnit) {
   dataUnit->next = NULL;
   v_lock_mutex(&q->mutex);
   if (q->tail == NULL) {
      q->head = dataUnit;
   } else {
      q->tail->next = dataUnit;
   }
   q->tail = dataUnit;
   q->unitNumber++;
   v_cond_signal(&q->cond);
   v_unlock_mutex(&q->mutex);
   return 1;
}
int BufferQueue::PopUnitfromQueue(Queue *q, DataUnit **dataUnit, bool block) {
   int ret = 0;
   v_lock_mutex(&q->mutex);
   while (true) {
      *dataUnit = q->head;
      if (q->head != NULL) {
         q->head = q->head->next;
         if (q->head == NULL)
            q->tail = NULL;
         q->unitNumber--;
         ret = 1;
         break;
      } else if (block) {
         v_cond_wait(&q->cond, &q->mutex);
      } else {
         ret = -1;
         break;
      }
   }
   v_unlock_mutex(&q->mutex);
   return ret;
}
int BufferQueue::GetQueueSize(Queue *q) {
   int ret = 0;
   v_lock_mutex(&q->mutex);
   ret = q->unitNumber;
   v_unlock_mutex(&q->mutex);
   return ret;
}

void BufferQueue::FreeQueue(Queue *q) {
   v_lock_mutex(&q->mutex);
   while (true) {
      DataUnit*dataUnit = q->head;
      if (q->head != NULL) {
         q->head = q->head->next;
         if (q->head == NULL)
            q->tail = NULL;
         q->unitNumber--;
         free(dataUnit->unitBuffer);
         free(dataUnit);
      } else {
         break;
      }
   }
   v_unlock_mutex(&q->mutex);
}
