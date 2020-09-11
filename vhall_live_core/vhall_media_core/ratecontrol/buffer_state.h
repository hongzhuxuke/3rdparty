#ifndef __BUFFERSTATE_H__
#define __BUFFERSTATE_H__

#include <stdint.h>

class BufferState
{
public:
   BufferState(){};
   virtual ~BufferState(){};
	/*
	* get the number of packet in the buffer
	*/
	virtual int GetQueueSize() = 0;
	/*
	* get the maximal number of packet that can be buffered in the buffer
	*/
	virtual int GetMaxNum() = 0;
	/*
	* get the duration of buffered media data in time (ms)
	*/
	virtual uint32_t GetQueueDataDuration() = 0;
	/*
	* get the data size of buffered media data in Byte
	*/
	virtual int GetQueueDataSize() = 0;
};

#endif
