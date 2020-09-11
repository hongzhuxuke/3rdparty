#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include "GlobalMemPoolBlock.h"

//表默认大小
#define	DEFAULT_DATABLOCK_SIZE				4096

#define GETINDEX(t,i) (((t) << 16) | ((i) & 0xFFFF))
#define GETINDEXBASE(t) ((t) << 16)

PSTRU_MEMORY_ITEM CGlobalMemPool::m_pMemoryTableList[DEF_MAX_TABLE_SIZE] = {0};
unsigned long CGlobalMemPool::m_uMemoryTableCount = 0;

unsigned long CGlobalMemPool::m_uReuseLimitSize = 512 * 1024;

unsigned long CGlobalMemPool::m_uTotalAllocMemorySize = 0;
unsigned long CGlobalMemPool::m_uTotalMemorySize = 0;
unsigned long CGlobalMemPool::m_uFreeMemorySize = 0;
unsigned long CGlobalMemPool::m_uMinMemorySize = 3 * 1024 * 1024;

int CGlobalMemPool::m_uUsedMemoryIndex = -1;
int CGlobalMemPool::m_uFreeMemoryIndex[DEF_LAST_BLOCK_INDEX + 1] = {0};
int CGlobalMemPool::m_uUnAllocMemoryIndex = -1;

int CGlobalMemPool::m_uUsedMemoryTailIndex = -1;
int CGlobalMemPool::m_uFreeMemoryTailIndex[DEF_LAST_BLOCK_INDEX + 1] = {0};
int CGlobalMemPool::m_uUnAllocMemoryTailIndex = -1;

BOOL CGlobalMemPool::m_bInit = FALSE;
long CGlobalMemPool::m_lID = 0;
long CGlobalMemPool::m_lInitCount = 0;

#ifdef WIN32
	CRITICAL_SECTION CGlobalMemPool::m_oSection = {0};
#else
	pthread_mutex_t CGlobalMemPool::m_hMutex = {0};
#endif

#ifdef _DEBUG
long CGlobalMemPool::m_lBrackList[DEF_MAX_BRACK_COUNT] = {0};
long CGlobalMemPool::m_lBrackPos = 0;
#endif

typedef struct STRU_MEMORY_HEAD
{
	ULONG	uIndex;

}*PSTRU_MEMORY_HEAD;

#ifdef _DEBUG
#include <sys/timeb.h>
INT64 __GetSystemTime()
{
	struct timeb loTimeb;
	ftime(&loTimeb);
	return loTimeb.time;
}
#endif

CGlobalMemPool::CGlobalMemPool(void)
{
}

CGlobalMemPool::~CGlobalMemPool(void)
{
}

//释放
void CGlobalMemPool::Release()
{
	FreeAll();
}

//分配内存
#ifdef _DEBUG
void* CGlobalMemPool::Malloc(unsigned long auSize,unsigned long auTag,const char *aszFile,long alCodeLine)
#else
void* CGlobalMemPool::Malloc(unsigned long auSize,unsigned long auTag)
#endif
{
	if(auSize < 1)
		auSize = 1;

	//以256字节为一块申请
	auSize = (auSize + DEF_BASE_BLOCK_SIZE - 1) / DEF_BASE_BLOCK_SIZE * DEF_BASE_BLOCK_SIZE;
	
	Lock();
#ifdef _DEBUG
	int iIndex = AllocMemoryItem(auSize,aszFile,alCodeLine);
#else
	int iIndex = AllocMemoryItem(auSize);
#endif
	ASSERT(iIndex >= 0);
	if(iIndex < 0)
	{
		UnLock();
		return NULL;
	}

	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(iIndex);
	lpItem->uTag = auTag;
#ifdef _DEBUG
	if(aszFile)
	{
		strncpy(lpItem->szFile,aszFile,MAX_PATH);
		lpItem->lCodeLine = alCodeLine;
		lpItem->lID = m_lID;
	}
	else
	{
		lpItem->szFile[0] = 0;
		lpItem->lCodeLine = 0;
		lpItem->lID = m_lID;
	}
	lpItem->i64MallocTime = __GetSystemTime();
	CheckBrack(m_lID);
	m_lID ++;
#endif
	void *pUserAddress = lpItem->pUserAddress;
	UnLock();
	return pUserAddress;
}

#ifdef _DEBUG
void* CGlobalMemPool::Calloc(unsigned long auNum,unsigned long auSize,unsigned long auTag,const char *aszFile,long alCodeLine)
#else
void* CGlobalMemPool::Calloc(unsigned long auNum,unsigned long auSize,unsigned long auTag)
#endif
{
	unsigned long lLen = auNum * auSize;
#ifdef _DEBUG
	void *pAddress = Malloc(lLen,auTag,aszFile,alCodeLine);
#else
	void *pAddress = Malloc(lLen,auTag);
#endif

	if(pAddress)
		memset(pAddress,0,lLen);

	return pAddress;
}

//释放内存
#ifdef _DEBUG
void CGlobalMemPool::Free(void *apUserData,const char *aszFile,long alCodeLine)
#else
void CGlobalMemPool::Free(void *apUserData)
#endif
{
	//ASSERT(apUserData);
	if(!apUserData)
		return;
	Lock();
#ifdef _DEBUG
	FreeMemoryItem(apUserData,aszFile,alCodeLine);
#else
	FreeMemoryItem(apUserData);
#endif
	UnLock();
}

//重新分配内存
void* CGlobalMemPool::Realloc(void *apUserData,unsigned long auNewSize)
{
	if(NULL == apUserData)
	{
		return Malloc(auNewSize);
	}

	Lock();
	int iIndex = GetIndexByUserAddress(apUserData);
	ASSERT(iIndex >= 0);
	if(iIndex < 0)
	{
		UnLock();
		return NULL;
	}
	
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(iIndex);
	if(lpItem->pUserAddress == NULL || lpItem->iState != enum_memory_state_used)
	{
		ASSERT(FALSE);
		UnLock();
		return NULL;
	}

	if(lpItem->uUserMemorySize >= auNewSize)
	{
		UnLock();
		return apUserData;
	}

#ifdef _DEBUG
	int iNewIndex = AllocMemoryItem(auNewSize,NULL,0);
#else
	int iNewIndex = AllocMemoryItem(auNewSize);
#endif

	ASSERT(iNewIndex >= 0);
	if(iNewIndex < 0)
	{
		UnLock();
		return NULL;
	}

	PSTRU_MEMORY_ITEM lpNewItem = GetItemByIndex(iNewIndex);
	
	//复制数据
	memcpy(lpNewItem->pUserAddress,lpItem->pUserAddress,min(lpNewItem->uUserMemorySize,lpItem->uUserMemorySize));
	//释放原来的
	FreeMemoryItem(apUserData);

	void *pUserAddress = lpNewItem->pUserAddress;
	UnLock();

	return pUserAddress;
}

//回收无用的内存，尽量保持在最小内存
void CGlobalMemPool::Recovery(void)
{
	if(m_uTotalAllocMemorySize <= m_uMinMemorySize || m_uFreeMemorySize == 0)
		return;

	//从大块的开始
	for(int i = DEF_LAST_BLOCK_INDEX; i >= 0; i--)
	{
		Lock();
		int uIndex = m_uFreeMemoryIndex[i];
		while(uIndex >= 0 && m_uTotalAllocMemorySize > m_uMinMemorySize && m_uFreeMemorySize > 0)
		{
			PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(uIndex);

			int uNextIndex = lpItem->iNextIndex;
			if(lpItem->iState == enum_memory_state_free)
			{
				//释放
				if(RemoveFromFree(uIndex) >= 0)
					DeleteByIndex(uIndex);
			}
			uIndex = uNextIndex;
		}
		UnLock();

		//小于限制可以结束
		if(m_uTotalAllocMemorySize <= m_uMinMemorySize || m_uFreeMemorySize <= 0)
			break;
	}

}

//设置最小内存使用
void CGlobalMemPool::SetMinMemoryPool(unsigned long auSize)
{
	Lock();
	m_uMinMemorySize = auSize;
	Recovery();
	UnLock();
}

//设置重新使用限制值，超过这个峰值的会立即回收
void CGlobalMemPool::SetReuseLimitSize(unsigned long auSize)
{
	Lock();
	m_uReuseLimitSize = auSize;
	Recovery();
	UnLock();
}

//释放全部，注意使用不在进行安全检查
void CGlobalMemPool::FreeAll()
{
	int i = 0;

	Lock();
	for(unsigned long i = 0 ; i < m_uMemoryTableCount ; i ++)
	{
		for(unsigned long j = 0; j < DEFAULT_DATABLOCK_SIZE; j ++)
		{
			if(m_pMemoryTableList[i][j].pAddress)
			{
				::free(m_pMemoryTableList[i][j].pAddress);
			}
		}
		::free(m_pMemoryTableList[i]);
	}

	m_uMemoryTableCount = 0;

	m_uTotalAllocMemorySize = 0;
	m_uTotalMemorySize = 0;
	m_uFreeMemorySize = 0;
	
	m_uUsedMemoryIndex = -1;
	for(i = 0; i <= DEF_LAST_BLOCK_INDEX; i++)
		m_uFreeMemoryIndex[i] = -1;
	m_uUnAllocMemoryIndex = -1;

	m_uUsedMemoryTailIndex = -1;
	for(i = 0; i <= DEF_LAST_BLOCK_INDEX; i++)
		m_uFreeMemoryTailIndex[i] = -1;
	m_uUnAllocMemoryTailIndex = -1;

	UnLock();
}

//返回指定标记已分配内存个数
unsigned long CGlobalMemPool::GetUsedCountByTag(unsigned long auTag)
{
	Lock();
	if(m_uUsedMemoryIndex < 0)
	{
		UnLock();
		return 0;
	}

	int uIndex = m_uUsedMemoryIndex;
	unsigned long uTagCount = 0;
	while(uIndex >= 0)
	{
		PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(uIndex);
		if(lpItem->uTag == auTag)
		{
			uTagCount ++;
		}
		uIndex = lpItem->iNextIndex;
	}
	UnLock();
	return uTagCount;
}

//输没有释放的内存地址和标记
#ifdef _DEBUG
void CGlobalMemPool::PrintUseMem(INT64 i64UseTime)
#else
void CGlobalMemPool::PrintUseMem()
#endif
{
	Lock();
	if(m_uUsedMemoryIndex < 0)
	{
#ifdef TraceLog0
		TraceLog0("没有正在使用的内存块！\n");
#endif
		UnLock();
		return;
	}

	int uIndex = m_uUsedMemoryIndex;

#ifdef _DEBUG
	INT64 i64Time = __GetSystemTime();
#endif

	int iCount = 0;
	while(uIndex >= 0)
	{
		iCount ++;
		PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(uIndex);

#ifdef _DEBUG
		//使用时间是否达到
		if(lpItem->i64MallocTime > 0 && (i64Time - lpItem->i64MallocTime < i64UseTime))
		{
			uIndex = lpItem->iNextIndex;
			continue;
		}
#endif
			
#ifdef _DEBUG
	#ifdef TraceLog0
		TraceLog0("内存块：No.%d, ID = %ld, Addr = %08X , Size = %lu ,Tag = %lu \n\t file = %s , line = %ld \n",
			iCount, lpItem->lID, lpItem->pUserAddress, lpItem->uUserMemorySize, lpItem->uTag, lpItem->szFile,lpItem->lCodeLine);
	#endif
#else
	#ifdef TraceLog0
		TraceLog0("内存块：No.%d, Addr = %08X , Size = %lu  ,Tag = %lu \n", iCount, lpItem->pUserAddress, lpItem->uUserMemorySize, lpItem->uTag);
	#endif
#endif

		uIndex = lpItem->iNextIndex;
	}
	UnLock();
}

//设置断点
void CGlobalMemPool::SetBrackPoint(long lID)
{
#ifdef _DEBUG
	Lock();
	if(m_lBrackPos < DEF_MAX_BRACK_COUNT)
	{
		m_lBrackList[m_lBrackPos++] = lID;
	}
	UnLock();
#endif
}

//返回内已分配的内存总数
long CGlobalMemPool::GetMallocMemorySize()
{
	return m_uTotalAllocMemorySize;
}

STRU_MEMORY_ITEM::STRU_MEMORY_ITEM()
{
	memset(this, 0, sizeof(STRU_MEMORY_ITEM));
}

//分配一个内存表项
#ifdef _DEBUG
int CGlobalMemPool::AllocMemoryItem(unsigned long auSize,const char *aszFile,long alCodeLine)
#else
int CGlobalMemPool::AllocMemoryItem(unsigned long auSize)
#endif
{
	int allocIndex = -1;
	int iBlockIndex = GetBlockIndexBySize(auSize);
	int unIndex = m_uFreeMemoryIndex[iBlockIndex];

	if(iBlockIndex == DEF_LAST_BLOCK_INDEX)
	{
		//如果是最后一个索引
		while(unIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(unIndex);

			if(lpItem->pUserAddress && lpItem->uUserMemorySize >= auSize)
			{
				allocIndex = unIndex;
				break;
			}
			unIndex = lpItem->iNextIndex;	
		}
	}
	else if(unIndex >= 0)
	{
		//如果不是最后一个索引
		allocIndex = unIndex;
	}

	if(allocIndex >= 0)
	{
		//找到了，移动到已使用中
		allocIndex = RemoveFromFree((unsigned long)allocIndex);
		if(allocIndex < 0)
		{
			ASSERT(allocIndex >= 0);
			return allocIndex;
		}
		allocIndex = AddToUsed((unsigned long)allocIndex);
		ASSERT(allocIndex >= 0);
		return allocIndex;
	}

	allocIndex = CreateMemoryItem(auSize);

	if(allocIndex >= 0)
	{
#if defined ( _PRINT_MALLOC_LINE) && defined ( _DEBUG)
		try
		{
			if(aszFile)
			{
				TraceLog0("CGlobalMemPool::Malloc at File: %s, Line = %d\n",aszFile,alCodeLine);
			}
			else
			{
				PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(allocIndex);
				TraceLog0("CGlobalMemPool::Malloc ID = %d, Tag = %d\n",lpItem->lID, lpItem->uTag);
			}
		}
		catch(...)
		{
		}
#endif
		allocIndex = AddToUsed((unsigned long)allocIndex);
		ASSERT(allocIndex >= 0);
	}
	return allocIndex;
}

//创建一个内存表项
int CGlobalMemPool::CreateMemoryItem(unsigned long auSize)
{
	if(auSize < 1)
	{
		ASSERT(FALSE);
		return -1;
	}

	int allocIndex = -1;
	if(m_uUnAllocMemoryIndex < 0)
	{
		//没有未分配的项，需要扩展索引表
		if(!AddTable())
		{
			ASSERT(FALSE);
			return -3;
		}
	}

	if(m_uUnAllocMemoryIndex >= 0)
	{
		PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(m_uUnAllocMemoryIndex);

		if(lpItem->pUserAddress)
		{
			ASSERT(FALSE);
			return -2;
		}
		lpItem->pAddress = NULL;
		lpItem->pUserAddress = NULL;
		lpItem->pAddress = ::malloc(auSize + sizeof(STRU_MEMORY_HEAD));
		if(lpItem->pAddress == NULL)
		{
			return -3;
		}
		lpItem->pUserAddress = (void*)((BYTE*)(lpItem->pAddress) + sizeof(STRU_MEMORY_HEAD));
		lpItem->uUserMemorySize = auSize;
		lpItem->uMemorySize = auSize + sizeof(STRU_MEMORY_HEAD);
		lpItem->iBlockIndex = GetBlockIndexBySize(auSize);
		//填充头
		PSTRU_MEMORY_HEAD pmh = (PSTRU_MEMORY_HEAD)lpItem->pAddress;
		pmh->uIndex = m_uUnAllocMemoryIndex;

		//从顶端移除添加到自由当中
		allocIndex = RemoveFromUnalloc(m_uUnAllocMemoryIndex);
		if(allocIndex < 0)
		{
			::free(lpItem->pAddress);
			lpItem->pAddress = NULL;
			lpItem->pUserAddress = NULL;
			ASSERT(FALSE);
			return -4;
		}
		//已分配总数
		m_uTotalAllocMemorySize += (auSize + sizeof(STRU_MEMORY_HEAD));
		//m_uTotalMemorySize += auSize;
	}
	return allocIndex;
}

//重设大小
bool CGlobalMemPool::AddTable()
{
	PSTRU_MEMORY_ITEM newList = NULL;
	newList = (PSTRU_MEMORY_ITEM)::malloc(sizeof(STRU_MEMORY_ITEM) * DEFAULT_DATABLOCK_SIZE);

	if(newList == NULL)
	{
#ifdef TraceLog0
		TraceLog0("CGlobalMemPool::AddTable 增加表失败 TableSize = %d, MallocSize = %d\n",m_uMemoryTableCount + 1, sizeof(STRU_MEMORY_ITEM) * DEFAULT_DATABLOCK_SIZE * (m_uMemoryTableCount + 1));
#endif
		ASSERT(newList);
		return false;
	}

	int oldCount = m_uMemoryTableCount++;
	memset(newList,0,DEFAULT_DATABLOCK_SIZE * sizeof(STRU_MEMORY_ITEM));
	m_pMemoryTableList[oldCount] = newList;
	int iIndexBase = GETINDEXBASE(oldCount);

	//修改未使用索引
	int iPrevIndex = -1;
	if(m_uUnAllocMemoryIndex < 0)
	{
		m_uUnAllocMemoryIndex = iIndexBase;
	}

	if(m_uUnAllocMemoryTailIndex >= 0)
	{
		iPrevIndex = m_uUnAllocMemoryTailIndex;
	}
	m_uUnAllocMemoryTailIndex = GETINDEX(oldCount, DEFAULT_DATABLOCK_SIZE - 1);

	//填充索引表
	for(int i = 0 ; i < DEFAULT_DATABLOCK_SIZE ; i ++)
	{
		newList[i].iPrevIndex = iPrevIndex;
		newList[i].iState = enum_memory_state_unalloc;
		if(i < DEFAULT_DATABLOCK_SIZE - 1)
		{
			newList[i].iNextIndex = iIndexBase + i + 1;
		}
		else
		{
			newList[i].iNextIndex = -1;
		}
		iPrevIndex = iIndexBase + i;
	}
	return true;
}

//释放内存项
#ifdef _DEBUG
void CGlobalMemPool::FreeMemoryItem(void *apUserAddress,const char *aszFile,long alCodeLine)
#else
void CGlobalMemPool::FreeMemoryItem(void *apUserAddress)
#endif
{
	ASSERT(apUserAddress);
	if(!apUserAddress)
		return;

	int iIndex = GetIndexByUserAddress(apUserAddress);
	ASSERT(iIndex >= 0);
	if(iIndex < 0)
		return;

	int iIndex1 = 0;
	iIndex1 = RemoveFromUsed(iIndex);
	ASSERT(iIndex1 >= 0);
	if((unsigned long)GetSizeByIndex(iIndex) >= m_uReuseLimitSize)
	{
		DeleteByIndex(iIndex);
	}
	else
	{
		iIndex1 = AddToFree(iIndex);
		ASSERT(iIndex1 >= 0);
#ifdef _DEBUG
		PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(iIndex);
		if(lpItem)
		{
			if(aszFile)
			{
				strncpy(lpItem->szFile,aszFile,MAX_PATH);
				lpItem->lCodeLine = alCodeLine;
			}
			else
			{
				lpItem->szFile[0] = 0;
				lpItem->lCodeLine = 0;
			}
			lpItem->i64MallocTime = __GetSystemTime();
		}
#endif
	}
}

//根据指针找回索引
int CGlobalMemPool::GetIndexByUserAddress(void *apUserAddress)
{
	ASSERT(apUserAddress);
	if(apUserAddress == NULL)
		return -1;

	PSTRU_MEMORY_HEAD pmh = (PSTRU_MEMORY_HEAD)((BYTE*)apUserAddress - sizeof(STRU_MEMORY_HEAD));
	try{
		if(pmh->uIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(pmh->uIndex);
			if(lpItem && apUserAddress == lpItem->pUserAddress)
					return pmh->uIndex;
		}
	}
	catch(...)
	{
		//说明是错误的地址空间
	}

	int unIndex = m_uUsedMemoryIndex;
	while(unIndex >= 0)
	{
		PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(unIndex);

		if(lpItem->pUserAddress == apUserAddress)
		{
			return unIndex;
		}
		unIndex = lpItem->iNextIndex;
	}
	return -1;
}

//根据索引释放内存
void CGlobalMemPool::DeleteByIndex(unsigned long auIndex)
{
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
		return;

	ASSERT(lpItem->pAddress);
	if(lpItem->pAddress == NULL)
		return;

	m_uTotalAllocMemorySize -= lpItem->uMemorySize;

	::free(lpItem->pAddress);
	lpItem->pAddress = NULL;
	lpItem->pUserAddress = NULL;
	lpItem->uMemorySize = 0;
	lpItem->uUserMemorySize = 0;
	AddToUnalloc(auIndex);
}

//从未分配中移除
int CGlobalMemPool::RemoveFromUnalloc(unsigned long auIndex)
{
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
	{
		ASSERT(lpItem);
		return -1;
	}

	ASSERT(lpItem->iState == enum_memory_state_unalloc);
	if(lpItem->iState != enum_memory_state_unalloc)
	{
		return -2;
	}

	lpItem->iState = enum_memory_state_unknown;
	int iPrevIndex = lpItem->iPrevIndex;
	int iNextIndex = lpItem->iNextIndex;
	lpItem->iPrevIndex = -1;
	lpItem->iNextIndex = -1;

	if(iPrevIndex < 0 && iNextIndex < 0)
	{
		//空了
		m_uUnAllocMemoryIndex = -1;
		m_uUnAllocMemoryTailIndex = -1;
	}
	else
	{
		if(iPrevIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpPrevItem = GetItemByIndex(iPrevIndex);
			lpPrevItem->iNextIndex = iNextIndex;
		}
		else
		{
			//没有上一个说明是到顶了
			ASSERT(m_uUnAllocMemoryIndex == auIndex);
			m_uUnAllocMemoryIndex = iNextIndex;
		}

		if(iNextIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpNextItem = GetItemByIndex(iNextIndex);
			lpNextItem->iPrevIndex = iPrevIndex;
		}
		else
		{
			//没有下一个说明是到底了
			ASSERT(m_uUnAllocMemoryTailIndex == auIndex);
			m_uUnAllocMemoryTailIndex = iPrevIndex;
		}
	}
	return auIndex;
}

//添加到未分配
int CGlobalMemPool::AddToUnalloc(unsigned long auIndex)
{
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
	{
		ASSERT(lpItem);
		return -1;
	}

	ASSERT(lpItem->iState == enum_memory_state_unknown);
	if(lpItem->iState != enum_memory_state_unknown)
	{
		return -2;
	}
	
	lpItem->iState = enum_memory_state_unalloc;
	lpItem->iPrevIndex = m_uUnAllocMemoryTailIndex;
	lpItem->iNextIndex = -1;
	if(m_uUnAllocMemoryTailIndex >= 0)
	{
		PSTRU_MEMORY_ITEM lpUAMTItem = GetItemByIndex(m_uUnAllocMemoryTailIndex);
		lpUAMTItem->iNextIndex = auIndex;
	}

	m_uUnAllocMemoryTailIndex = auIndex;
	if(m_uUnAllocMemoryIndex < 0)
	{
		m_uUnAllocMemoryIndex = auIndex;
	}
	return auIndex;
}

//从空闲中移除
int CGlobalMemPool::RemoveFromFree(unsigned long auIndex)
{	
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
	{
		ASSERT(lpItem);
		return -1;
	}

	ASSERT(lpItem->iState == enum_memory_state_free);
	if(lpItem->iState != enum_memory_state_free)
	{
		return -2;
	}

	lpItem->iState = enum_memory_state_unknown;
	int iPrevIndex = lpItem->iPrevIndex;
	int iNextIndex = lpItem->iNextIndex;
	int iBlockIndex = lpItem->iBlockIndex;
	lpItem->iPrevIndex = -1;
	lpItem->iNextIndex = -1;

	m_uFreeMemorySize -= lpItem->uUserMemorySize;

	if(iPrevIndex < 0 && iNextIndex < 0)
	{
		//所在块项空了
		m_uFreeMemoryIndex[iBlockIndex] = -1;
		m_uFreeMemoryTailIndex[iBlockIndex] = -1;
	}
	else
	{
		if(iPrevIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpPrevItem = GetItemByIndex(iPrevIndex);
			lpPrevItem->iNextIndex = iNextIndex;
		}
		else
		{
			//没有上一个说明是到顶了
			ASSERT(m_uFreeMemoryIndex[iBlockIndex] == auIndex);
			m_uFreeMemoryIndex[iBlockIndex] = iNextIndex;
		}

		if(iNextIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpNextItem = GetItemByIndex(iNextIndex);
			lpNextItem->iPrevIndex = iPrevIndex;
		}
		else
		{
			//没有下一个说明是到底了
			ASSERT(m_uFreeMemoryTailIndex[iBlockIndex] == auIndex);
			m_uFreeMemoryTailIndex[iBlockIndex] = iPrevIndex;
		}
	}
	return auIndex;
}

//添加到空闲中
int CGlobalMemPool::AddToFree(unsigned long auIndex)
{	
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
	{
		ASSERT(lpItem);
		return -1;
	}

	ASSERT(lpItem->iState == enum_memory_state_unknown);
	if(lpItem->iState != enum_memory_state_unknown)
	{
		return -2;
	}
	
	int iBlockIndex = lpItem->iBlockIndex;
	ASSERT(iBlockIndex >= 0);

	lpItem->iState = enum_memory_state_free;
	lpItem->iPrevIndex = m_uFreeMemoryTailIndex[iBlockIndex];
	lpItem->iNextIndex = -1;
	if(m_uFreeMemoryTailIndex[iBlockIndex] >= 0)
	{
		PSTRU_MEMORY_ITEM lpFMTItem = GetItemByIndex(m_uFreeMemoryTailIndex[iBlockIndex]);
		lpFMTItem->iNextIndex = auIndex;
	}

	m_uFreeMemoryTailIndex[iBlockIndex] = auIndex;

	if(m_uFreeMemoryIndex[iBlockIndex] < 0)
	{
		m_uFreeMemoryIndex[iBlockIndex] = auIndex;
	}
	m_uFreeMemorySize += lpItem->uUserMemorySize;

	return auIndex;
}

//从已使用中移除
int CGlobalMemPool::RemoveFromUsed(unsigned long auIndex)
{
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
	{
		ASSERT(lpItem);
		return -1;
	}

	ASSERT(lpItem->iState == enum_memory_state_used);
	if(lpItem->iState != enum_memory_state_used)
	{
		return -2;
	}

	lpItem->iState = enum_memory_state_unknown;
	int iPrevIndex = lpItem->iPrevIndex;
	int iNextIndex = lpItem->iNextIndex;
	lpItem->iPrevIndex = -1;
	lpItem->iNextIndex = -1;
#ifdef _DEBUG
	lpItem->i64MallocTime = 0;
#endif

	if(iPrevIndex < 0 && iNextIndex < 0)
	{
		//空了
		m_uUsedMemoryIndex = -1;
		m_uUsedMemoryTailIndex = -1;
	}
	else
	{
		if(iPrevIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpPrevItem = GetItemByIndex(iPrevIndex);
			lpPrevItem->iNextIndex = iNextIndex;
		}
		else
		{
			//没有上一个说明是到顶了
			ASSERT(m_uUsedMemoryIndex == auIndex);
			m_uUsedMemoryIndex = iNextIndex;
		}

		if(iNextIndex >= 0)
		{
			PSTRU_MEMORY_ITEM lpNextItem = GetItemByIndex(iNextIndex);
			lpNextItem->iPrevIndex = iPrevIndex;
		}
		else
		{
			//没有下一个说明是到底了
			ASSERT(m_uUsedMemoryTailIndex == auIndex);
			m_uUsedMemoryTailIndex = iPrevIndex;
		}
	}
	return auIndex;
}

//添加到已使用
int CGlobalMemPool::AddToUsed(unsigned long auIndex)
{
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
	{
		ASSERT(lpItem);
		return -1;
	}

	ASSERT(lpItem->iState == enum_memory_state_unknown);
	if(lpItem->iState != enum_memory_state_unknown)
	{
		return -2;
	}
	
	lpItem->iState = enum_memory_state_used;
	lpItem->iPrevIndex = m_uUsedMemoryTailIndex;
	lpItem->iNextIndex = -1;
	if(m_uUsedMemoryTailIndex >= 0)
	{
		PSTRU_MEMORY_ITEM lpUMTItem = GetItemByIndex(m_uUsedMemoryTailIndex);
		lpUMTItem->iNextIndex = auIndex;
	}

	m_uUsedMemoryTailIndex = auIndex;
	if(m_uUsedMemoryIndex < 0)
	{
		m_uUsedMemoryIndex = auIndex;
	}
	return auIndex;
}

//根据索引返回该内存的大小
int CGlobalMemPool::GetSizeByIndex(unsigned long auIndex)
{
	PSTRU_MEMORY_ITEM lpItem = GetItemByIndex(auIndex);
	if(NULL == lpItem)
		return -1;

	return lpItem->uUserMemorySize;
}

//根据大小返回块项索引
int CGlobalMemPool::GetBlockIndexBySize(unsigned long auSize)
{
	int iIndex = (auSize - 1) / DEF_BASE_BLOCK_SIZE;
	if(iIndex >= DEF_LAST_BLOCK_INDEX)
		iIndex = DEF_LAST_BLOCK_INDEX;
	return iIndex;
}

//初始化
void CGlobalMemPool::Init()
{
	if(m_bInit)
		return;

	int i = 0;

	m_bInit = TRUE;
	m_lInitCount ++;

	::atexit(CGlobalMemPool::Release);

#ifdef WIN32
		InitializeCriticalSection(&m_oSection);
#else
		//m_hMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
		//pthread_mutex_init(&m_hMutex,NULL);
		pthread_mutexattr_t   attr;   
		pthread_mutexattr_init(&attr);   
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);   
		pthread_mutex_init(&m_hMutex,&attr);
#endif

	for(i = 0; i <= DEF_LAST_BLOCK_INDEX; i++)
		CGlobalMemPool::m_uFreeMemoryIndex[i] = -1;
	for(i = 0; i <= DEF_LAST_BLOCK_INDEX; i++)
		CGlobalMemPool::m_uFreeMemoryTailIndex[i] = -1;
}

#ifdef _DEBUG
//中断在指定ID分配
void CGlobalMemPool::CheckBrack(long lID)
{
#ifdef _M_X64
#else
   for (long i = 0; i < m_lBrackPos; i++)
   {
      if (m_lBrackList[i] == lID)
      {
         __asm
         {
            int 3
         }
      }
   }
#endif // _M_X64
}
#endif

__inline PSTRU_MEMORY_ITEM CGlobalMemPool::GetItemByIndex(int iIndex)
{
	int iTableIndex = (iIndex >> 16);
	if(iTableIndex >= static_cast<int>(m_uMemoryTableCount) || iTableIndex < 0)
		return NULL;

	int iItemIndex = (iIndex & 0xFFFF);
	if(iItemIndex >= DEFAULT_DATABLOCK_SIZE || iItemIndex < 0)
		return NULL;

	return &m_pMemoryTableList[iTableIndex][iItemIndex];
}