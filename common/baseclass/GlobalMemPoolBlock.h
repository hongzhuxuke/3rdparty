#ifndef __CR_GLOBALMEMORYPOOL__H_INCLUDE__
#define __CR_GLOBALMEMORYPOOL__H_INCLUDE__

#define	DEF_MAX_BRACK_COUNT			1024
#define	DEF_BASE_BLOCK_SIZE			32
//这索引为自由大小项
#define	DEF_LAST_BLOCK_INDEX		((1024 * 1024) / DEF_BASE_BLOCK_SIZE)
//表最大索引
#define DEF_MAX_TABLE_SIZE			4096

typedef struct STRU_MEMORY_ITEM
{
	void			*pAddress;			//数据地址
	void			*pUserAddress;		//用户地址
	unsigned long	uMemorySize;		//内存大小
	unsigned long	uUserMemorySize;	//用户内存大小
	int				iBlockIndex;		//块索引项
	int				iState;				//状态 enum_memory_state
	unsigned long	uTag;				//标记用于追踪各类型分配情况
#ifdef _DEBUG
	char			szFile[MAX_PATH +1];//文件名
	long			lCodeLine;			//行号
	long			lID;				//当前唯一标识
	INT64			i64MallocTime;		//被分配的时间
#endif
	int				iPrevIndex;			//上一个索引
	int				iNextIndex;			//下一个索引
	STRU_MEMORY_ITEM();
}*PSTRU_MEMORY_ITEM;

class CGlobalMemPool
{
private:
	enum ENUM_MEMORY_STATE
	{
		enum_memory_state_unknown = 0,
		enum_memory_state_unalloc = 1,
		enum_memory_state_free = 2,
		enum_memory_state_used = 3,
	};

	static PSTRU_MEMORY_ITEM	m_pMemoryTableList[DEF_MAX_TABLE_SIZE];
	static unsigned long		m_uMemoryTableCount;

	static unsigned long		m_uReuseLimitSize;			//超过该值则不放到内存池中,直接释放掉

	static unsigned long		m_uTotalAllocMemorySize;	//总分配内存总大小
	static unsigned long		m_uTotalMemorySize;			//内存总大小
	static unsigned long		m_uFreeMemorySize;			//空闲内存大小
	static unsigned long		m_uMinMemorySize;			//最小内存大小

	static int					m_uUsedMemoryIndex;			//已使用的首个内存项索引
	static int					m_uFreeMemoryIndex[DEF_LAST_BLOCK_INDEX + 1];			//空闲的首个内存项索引
	static int					m_uUnAllocMemoryIndex;		//未分配首个内存项索引

	static int					m_uUsedMemoryTailIndex;		//已使用的尾部内存项索引
	static int					m_uFreeMemoryTailIndex[DEF_LAST_BLOCK_INDEX + 1];		//空闲的尾部内存项索引
	static int					m_uUnAllocMemoryTailIndex;	//未分配尾部内存项索引
	static BOOL					m_bInit;					//初始化的
	static long					m_lID;						//ID
#ifdef _DEBUG
	static long					m_lBrackList[DEF_MAX_BRACK_COUNT];//最多1024个断点
	static long					m_lBrackPos;				//断点位置
#endif

	static long					m_lInitCount;
	//windows OS
#ifdef WIN32
	static CRITICAL_SECTION		m_oSection;
#else
	static pthread_mutex_t		m_hMutex;
#endif

public:
	CGlobalMemPool(void);
	~CGlobalMemPool(void);

#ifdef _DEBUG
	//分配内存
	static void* Malloc(unsigned long auSize,unsigned long auTag = 0,const char *aszFile = NULL, long alCodeLine = 0);
	static void* Calloc(unsigned long auNum,unsigned long auSize,unsigned long auTag = 0,const char *aszFile = NULL, long alCodeLine = 0);
	//释放内存
	static void Free(void *apUserData,const char *aszFile = NULL, long alCodeLine = 0);
#else
	//分配内存
	static void* Malloc(unsigned long auSize,unsigned long auTag = 0);
	static void* Calloc(unsigned long auNum,unsigned long auSize,unsigned long auTag = 0);
	//释放内存
	static void Free(void *apUserData);
#endif
	//重新分配内存
	static void* Realloc(void *apUserData,unsigned long auNewSize);
	//回收无用的内存，尽量保持在最小内存
	static void Recovery(void);
	//设置最小内存使用
	static void SetMinMemoryPool(unsigned long auSize);
	//设置重新使用限制值，超过这个峰值的会立即回收
	static void SetReuseLimitSize(unsigned long auSize);
	//释放全部，注意使用不在进行安全检查
	static void FreeAll();
	//返回指定标记已分配内存个数
	static unsigned long GetUsedCountByTag(unsigned long auTag);
	//输没有释放的内存地址和标记
#ifdef _DEBUG
	//参数秒数
	static void PrintUseMem(INT64 i64UseTime);
#else
	static void PrintUseMem();
#endif
	//设置断点
	static void SetBrackPoint(long lID);

	//返回内已分配的内存总数
	static long GetMallocMemorySize();

private:
	//释放
	static void Release();
	//初始化
	static void	Init();

private:
#ifdef _DEBUG
	//中断在指定ID分配
	static void CheckBrack(long lID);
#endif
	//分配一个内存表项
#ifdef _DEBUG
	static int AllocMemoryItem(unsigned long auSize,const char *aszFile = NULL,long alCodeLine = 0);
#else
	static int AllocMemoryItem(unsigned long auSize);
#endif
	//创建一个内存表项
	static int CreateMemoryItem(unsigned long auSize);
	//重设大小
	static bool AddTable();
	//释放内存项
#ifdef _DEBUG
	static void FreeMemoryItem(void *apUserAddress,const char *aszFile = NULL, long alCodeLine = 0);
#else
	static void FreeMemoryItem(void *apUserAddress);
#endif
	//根据指针找回索引
	static int GetIndexByUserAddress(void *apUserAddress);
	//根据索引释放内存
	static void DeleteByIndex(unsigned long auIndex);

	//从未分配中移除
	static int RemoveFromUnalloc(unsigned long auIndex);
	//添加到未分配
	static int AddToUnalloc(unsigned long auIndex);

	//从空闲中移除
	static int RemoveFromFree(unsigned long auIndex);
	//添加到空闲中
	static int AddToFree(unsigned long auIndex);

	//从已使用中移除
	static int RemoveFromUsed(unsigned long auIndex);
	//添加到已使用
	static int AddToUsed(unsigned long auIndex);

	//根据索引返回该内存的大小
	static int GetSizeByIndex(unsigned long auIndex);

	//根据大小返回块项索引
	static int GetBlockIndexBySize(unsigned long auSize);

	__inline static void  Lock()
	{
		if(!m_bInit && m_lInitCount++ == 0)
		{
			//这种情况会出现，全局重载new时，__mp_init_fun 没有被调用，所有结构还没有被实例
			Init();
		};
#ifdef WIN32
		EnterCriticalSection(&m_oSection);
#else
		pthread_mutex_lock(&m_hMutex);
#endif
	};
	__inline static void UnLock()
	{
#ifdef WIN32
		LeaveCriticalSection(&m_oSection);
#else
		pthread_mutex_unlock(&m_hMutex);
#endif
	};

	__inline static PSTRU_MEMORY_ITEM GetItemByIndex(int iIndex);

	friend struct __crmp_init;
};

static struct __crmp_init
{
	__crmp_init()
	{
		CGlobalMemPool::Init();
	};
}
__crmp_init_fun;

#endif //__CR_GLOBALMEMORYPOOL__H_INCLUDE__
