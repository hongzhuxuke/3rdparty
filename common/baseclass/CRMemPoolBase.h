#pragma once
#include "GlobalMemPoolBlock.h"

class CMemPoolBase
{
public:
	inline void * operator new (unsigned int size)
	{
		return CGlobalMemPool::Malloc((unsigned long)size);
	}

	inline void * operator new[] (unsigned int size)
	{
		return CGlobalMemPool::Malloc((unsigned long)size);
	}

	inline void operator delete (void *p)
	{
		CGlobalMemPool::Free(p);
	}

	inline void operator delete[] (void *p)
	{
		CGlobalMemPool::Free(p);
	}

	inline void operator delete (void *p,const char *szFile,long lCodeLine)
	{
		CGlobalMemPool::Free(p);
	}

	inline void operator delete[] (void *p,const char *szFile,long lCodeLine)
	{
		CGlobalMemPool::Free(p);
	}

	inline void operator delete (void *p, long alTagBlockID, const char *szFile, long lCodeLine)
	{
		CGlobalMemPool::Free(p);
	}

	inline void operator delete[] (void *p, long alTagBlockID, const char *szFile, long lCodeLine)
	{
		CGlobalMemPool::Free(p);
	}

	inline void * operator new (unsigned int size, unsigned int tag)
	{
		return CGlobalMemPool::Malloc((unsigned long)size,(unsigned long)tag);
	}

	inline void * operator new[] (unsigned int size, unsigned int tag)
	{
		return CGlobalMemPool::Malloc((unsigned long)size,(unsigned long)tag);
	}

#ifdef _DEBUG
	inline void * operator new (unsigned int size, const char *szFile, long lCodeLine)
	{
		return CGlobalMemPool::Malloc((unsigned long)size, 0, szFile, lCodeLine);
	}

	inline void * operator new[] (unsigned int size, const char *szFile, long lCodeLine)
	{
		return CGlobalMemPool::Malloc((unsigned long)size, 0, szFile, lCodeLine);
	}
	inline void * operator new (unsigned int size, long alTagBlockID, const char *szFile, long lCodeLine)
	{
		return CGlobalMemPool::Malloc((unsigned long)size, alTagBlockID, szFile, lCodeLine);
	}

	inline void * operator new[] (unsigned int size, long alTagBlockID, const char *szFile, long lCodeLine)
	{
		return CGlobalMemPool::Malloc((unsigned long)size, alTagBlockID, szFile, lCodeLine);
	}
#endif

};

#ifndef DEF_CR_MEMPOOL_STRUCT
#define	DEF_CR_MEMPOOL_STRUCT(T) \
	struct T##_MP : public CMemPoolBase, public T \
	{ \
	};
#endif //~DEF_CR_MEMPOOL_STRUCT