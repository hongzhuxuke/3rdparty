#ifndef __CR_GLOBALMEMORYPOOL__H_INCLUDE__
#define __CR_GLOBALMEMORYPOOL__H_INCLUDE__

#define	DEF_MAX_BRACK_COUNT			1024
#define	DEF_BASE_BLOCK_SIZE			32
//������Ϊ���ɴ�С��
#define	DEF_LAST_BLOCK_INDEX		((1024 * 1024) / DEF_BASE_BLOCK_SIZE)
//���������
#define DEF_MAX_TABLE_SIZE			4096

typedef struct STRU_MEMORY_ITEM
{
	void			*pAddress;			//���ݵ�ַ
	void			*pUserAddress;		//�û���ַ
	unsigned long	uMemorySize;		//�ڴ��С
	unsigned long	uUserMemorySize;	//�û��ڴ��С
	int				iBlockIndex;		//��������
	int				iState;				//״̬ enum_memory_state
	unsigned long	uTag;				//�������׷�ٸ����ͷ������
#ifdef _DEBUG
	char			szFile[MAX_PATH +1];//�ļ���
	long			lCodeLine;			//�к�
	long			lID;				//��ǰΨһ��ʶ
	INT64			i64MallocTime;		//�������ʱ��
#endif
	int				iPrevIndex;			//��һ������
	int				iNextIndex;			//��һ������
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

	static unsigned long		m_uReuseLimitSize;			//������ֵ�򲻷ŵ��ڴ����,ֱ���ͷŵ�

	static unsigned long		m_uTotalAllocMemorySize;	//�ܷ����ڴ��ܴ�С
	static unsigned long		m_uTotalMemorySize;			//�ڴ��ܴ�С
	static unsigned long		m_uFreeMemorySize;			//�����ڴ��С
	static unsigned long		m_uMinMemorySize;			//��С�ڴ��С

	static int					m_uUsedMemoryIndex;			//��ʹ�õ��׸��ڴ�������
	static int					m_uFreeMemoryIndex[DEF_LAST_BLOCK_INDEX + 1];			//���е��׸��ڴ�������
	static int					m_uUnAllocMemoryIndex;		//δ�����׸��ڴ�������

	static int					m_uUsedMemoryTailIndex;		//��ʹ�õ�β���ڴ�������
	static int					m_uFreeMemoryTailIndex[DEF_LAST_BLOCK_INDEX + 1];		//���е�β���ڴ�������
	static int					m_uUnAllocMemoryTailIndex;	//δ����β���ڴ�������
	static BOOL					m_bInit;					//��ʼ����
	static long					m_lID;						//ID
#ifdef _DEBUG
	static long					m_lBrackList[DEF_MAX_BRACK_COUNT];//���1024���ϵ�
	static long					m_lBrackPos;				//�ϵ�λ��
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
	//�����ڴ�
	static void* Malloc(unsigned long auSize,unsigned long auTag = 0,const char *aszFile = NULL, long alCodeLine = 0);
	static void* Calloc(unsigned long auNum,unsigned long auSize,unsigned long auTag = 0,const char *aszFile = NULL, long alCodeLine = 0);
	//�ͷ��ڴ�
	static void Free(void *apUserData,const char *aszFile = NULL, long alCodeLine = 0);
#else
	//�����ڴ�
	static void* Malloc(unsigned long auSize,unsigned long auTag = 0);
	static void* Calloc(unsigned long auNum,unsigned long auSize,unsigned long auTag = 0);
	//�ͷ��ڴ�
	static void Free(void *apUserData);
#endif
	//���·����ڴ�
	static void* Realloc(void *apUserData,unsigned long auNewSize);
	//�������õ��ڴ棬������������С�ڴ�
	static void Recovery(void);
	//������С�ڴ�ʹ��
	static void SetMinMemoryPool(unsigned long auSize);
	//��������ʹ������ֵ�����������ֵ�Ļ���������
	static void SetReuseLimitSize(unsigned long auSize);
	//�ͷ�ȫ����ע��ʹ�ò��ڽ��а�ȫ���
	static void FreeAll();
	//����ָ������ѷ����ڴ����
	static unsigned long GetUsedCountByTag(unsigned long auTag);
	//��û���ͷŵ��ڴ��ַ�ͱ��
#ifdef _DEBUG
	//��������
	static void PrintUseMem(INT64 i64UseTime);
#else
	static void PrintUseMem();
#endif
	//���öϵ�
	static void SetBrackPoint(long lID);

	//�������ѷ�����ڴ�����
	static long GetMallocMemorySize();

private:
	//�ͷ�
	static void Release();
	//��ʼ��
	static void	Init();

private:
#ifdef _DEBUG
	//�ж���ָ��ID����
	static void CheckBrack(long lID);
#endif
	//����һ���ڴ����
#ifdef _DEBUG
	static int AllocMemoryItem(unsigned long auSize,const char *aszFile = NULL,long alCodeLine = 0);
#else
	static int AllocMemoryItem(unsigned long auSize);
#endif
	//����һ���ڴ����
	static int CreateMemoryItem(unsigned long auSize);
	//�����С
	static bool AddTable();
	//�ͷ��ڴ���
#ifdef _DEBUG
	static void FreeMemoryItem(void *apUserAddress,const char *aszFile = NULL, long alCodeLine = 0);
#else
	static void FreeMemoryItem(void *apUserAddress);
#endif
	//����ָ���һ�����
	static int GetIndexByUserAddress(void *apUserAddress);
	//���������ͷ��ڴ�
	static void DeleteByIndex(unsigned long auIndex);

	//��δ�������Ƴ�
	static int RemoveFromUnalloc(unsigned long auIndex);
	//��ӵ�δ����
	static int AddToUnalloc(unsigned long auIndex);

	//�ӿ������Ƴ�
	static int RemoveFromFree(unsigned long auIndex);
	//��ӵ�������
	static int AddToFree(unsigned long auIndex);

	//����ʹ�����Ƴ�
	static int RemoveFromUsed(unsigned long auIndex);
	//��ӵ���ʹ��
	static int AddToUsed(unsigned long auIndex);

	//�����������ظ��ڴ�Ĵ�С
	static int GetSizeByIndex(unsigned long auIndex);

	//���ݴ�С���ؿ�������
	static int GetBlockIndexBySize(unsigned long auSize);

	__inline static void  Lock()
	{
		if(!m_bInit && m_lInitCount++ == 0)
		{
			//�����������֣�ȫ������newʱ��__mp_init_fun û�б����ã����нṹ��û�б�ʵ��
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
