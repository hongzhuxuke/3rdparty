#ifndef __PLUGIN_BASE__H_INCLUDE__
#define __PLUGIN_BASE__H_INCLUDE__

#pragma once

#include "IMessageDispatcher.h"
#include "IPluginManager.h"
#include "IUIThreadManager.h"

#include "CriticalSection.h"
#include "MemPoolBase.h"
#include "VH_ConstDeff.h"
#include "VH_TypeDeff.h"

//CR插件基类
class CPluginBase : public IPluginObject
{
public:
	//CR消息项
	struct STRU_CR_MESSAGE_ITEM : public CMemPoolBase
	{
		DWORD			m_dwSenderID;
		DWORD			m_dwMessageID;
		void *			m_pData;
		DWORD			m_dwLen;
	public:
		STRU_CR_MESSAGE_ITEM();
		~STRU_CR_MESSAGE_ITEM();
	public:
		BOOL SetData(const void * apData, DWORD adwLen);
	};

public:
	//定义CR消息队列
	typedef std::list<STRU_CR_MESSAGE_ITEM*>	CRMessageQueue;

public:
	CPluginBase(const GUID& aoPLuginGuid, DWORD adwSenderID, DWORD adwCycleMsgDealNum);
	virtual ~CPluginBase(void);

public:
	//----------接口控制部分------------------------------------------------------------
	//取得某个插件对象接口：IPluginObject
	HRESULT GetCRPluginObject(REFIID aoPluginID, void ** appObject);
	//获取某个插件对象的接口，由IPluginObject去查询
	HRESULT GetCRPluginInterface(REFIID aoPluginID, REFIID aoIID, void ** appObject);

	//----------消息控制部分------------------------------------------------------------
	//投递消息
	HRESULT PostCRMessage(DWORD adwMessageID, void * apData, DWORD adwLen);
	//投递消息，需要Pack函数的
	template<typename T>
	HRESULT PostCRMessage(DWORD adwMessageID, T& aoPackData);
	//派发缓存消息，如果是非UI模块，则需要定时调用该函数，产生 InstanceDealCRMessage 回调
	BOOL DispatchCachedMessage();

	//----------线程控制部分------------------------------------------------------------
	//启动模块的UI线程使用
	DWORD BeginUIThread(DWORD adwThreadIndex);
	//关闭模块的UI线程使用
	BOOL EndUIThread(DWORD adwThreadIndex);

	//--------------注册预处理消息---------------------------------------------
	// 注册预处理消息
	BOOL RegistUIPreMsg(BOOL abIsReg);
	// TraceLog 参数awstrDllName是不包含.dll的模块名称，此方法不能输出exe的日志
   void InitDebugTrace(std::wstring awstrDllName, int aiTraceLevel, bool abUseSameLog = false);
protected:
	//取得插件管理器接口:IPluginManager
	HRESULT GetPluginManager(void ** appCRPluginManager);
	//取得消息派发器接口:IMessageDispatcher
	HRESULT GetMessageDispatcher(void ** appMessageDispatcher);
	//取得线程管理器接口:IUIThreadManager
	HRESULT GetUIThreadManager(void ** appCRUIThreadManager);

public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** appvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

protected:
	//创建对象
	virtual HRESULT STDMETHODCALLTYPE Create();
	//销毁对象
	virtual HRESULT STDMETHODCALLTYPE Destroy();
	//刷新插件，插件需要重新读取资源（由插件定义更新内容，这里只是一个通知，也可以不作任何操作）
	virtual HRESULT STDMETHODCALLTYPE Update();
	//取得GUID
	virtual HRESULT STDMETHODCALLTYPE GetPluginGUID(GUID& aoGuid);
	//取得插件信息
	virtual HRESULT STDMETHODCALLTYPE GetPluginInfo(STRU_CR_PLUGIN_INFO& aoPluginInfo);
	//通知连接到插件管理器
	virtual HRESULT STDMETHODCALLTYPE ConnectPluginManager(IPluginManager * apCRPluginManager);
	//断开插件管理器
	virtual HRESULT STDMETHODCALLTYPE DisconnectPluginManager();

protected:
	//----------------------------------------------------------------------------------------------------------------
	//IMessageEvent接口
	BEGIN_INTERFACE_PART(MessageEvent, IMessageEvent)
		INIT_INTERFACE_PART(CPluginBase, MessageEvent)

		//与 IMessageDispatcher 的连接
		virtual HRESULT STDMETHODCALLTYPE Connect(IMessageDispatcher * apMessageDispatcher);
		//断开与 IMessageDispatcher 的连接
		virtual HRESULT STDMETHODCALLTYPE Disconnect();
		//接收数据
		virtual HRESULT STDMETHODCALLTYPE OnRecvMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen);

	END_INTERFACE_PART(MessageEvent);
	//----------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------
	//IUIThreadObject
	BEGIN_INTERFACE_PART(UIThreadObject, IUIThreadObject)
		INIT_INTERFACE_PART(CPluginBase, UIThreadObject)

		//线程管理器主动调用，通知连接到线程管理器
		virtual HRESULT STDMETHODCALLTYPE Connect(IUIThreadManager * apCRUIThreadManager);
		//线程管理器主动调用，通知断开线程管理器，对象保存的管理器指针不再保证有效
		virtual HRESULT STDMETHODCALLTYPE Disconnect();
		//初始化，UI线程工作对象调用Begin时，由UI线程管理器与期望的UI线程交涉，由期望UI线程回调，通常纯逻辑模块不需要使用这个功能
		virtual HRESULT STDMETHODCALLTYPE Init(DWORD dwThreadID);
		//反初始化，UI线程工作对象调用End时，由UI线程管理器与期望的UI线程交涉，由期望UI线程回调，通常纯逻辑模块不需要使用这个功能
		virtual HRESULT STDMETHODCALLTYPE UnInit();
		//通知处理消息派发器推送的缓存消息，这个函数由Begin时指定的UI线程定时调用，通常所有需要操作UI界面的插件模块的消息处理，都要在这个函数调用中
		virtual HRESULT STDMETHODCALLTYPE DealCachedMessage(DWORD dwThreadID);

	END_INTERFACE_PART(UIThreadObject);
	//----------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------
	//IPluginManagerEvent接口
	BEGIN_INTERFACE_PART(PluginManagerEvent, IPluginManagerEvent)
		INIT_INTERFACE_PART(CPluginBase, PluginManagerEvent)

		//通知某一个插件被加载到管理器中
		virtual HRESULT STDMETHODCALLTYPE OnPluginLoaded(IPluginObject * apPluginObject);
		//通知某一个插件从管理器卸载之前
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoadBefore(IPluginObject * apPluginObject);
		//通知某一个插件已从管理器卸载
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoaded(REFGUID aoCRPGuid);
		//是否可以加载某一个插件
		virtual HRESULT STDMETHODCALLTYPE IsCanLoadPlugin(REFGUID aoCRPGuid);
		//是否可以卸载
		virtual HRESULT STDMETHODCALLTYPE IsCanUnLoadPlugin(IPluginObject * apPluginObject);

	END_INTERFACE_PART(PluginManagerEvent);
	//----------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------
	//IPluginManagerEvent接口
	BEGIN_INTERFACE_PART(UIPreMessageEvent, IUIPreMessageEvent)
		INIT_INTERFACE_PART(CPluginBase, UIPreMessageEvent)

		//通知注册的UI线程工作对象，你所期望的UI线程有消息预处理
		virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage(MSG* apMsg);

	END_INTERFACE_PART(UIPreMessageEvent);
	//----------------------------------------------------------------------------------------------------------------

protected:
	//----------------------------------------------------------------------------------------------------------------
	//实例接口查询
	virtual HRESULT InstanceQueryInterface(REFIID riid, void ** appvObject);
	//创建插件
	virtual HRESULT InstanceCreatePlugin();
	//销毁插件
	virtual HRESULT InstanceDestroyPlugin();
	//初始化需要注册的消息
	virtual HRESULT InstanceInitRegMessage(CRMessageIDQueue& aoCRMessageIDQueue);
	//处理消息
	virtual HRESULT InstanceDealCRMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen);

	//----------------------------------------------------------------------------------------------------------------
	//UI线程初始化
	virtual HRESULT InstanceUIThreadInit(DWORD adwThreadID);
	//UI线程反初始化
	virtual HRESULT InstanceUIThreadUnInit();

	//----------------------------------------------------------------------------------------------------------------
	//通知某一个插件被加载到管理器中
	virtual HRESULT InstanceOnPluginLoaded(IPluginObject * apPluginObject);
	//通知某一个插件从管理器卸载之前
	virtual HRESULT InstanceOnPluginUnLoadBefore(IPluginObject * apPluginObject);
	//通知某一个插件已从管理器卸载
	virtual HRESULT InstanceOnPluginUnLoaded(REFGUID aoCRPGuid);
	//是否可以加载某一个插件
	virtual HRESULT InstanceIsCanLoadPlugin(REFGUID aoCRPGuid);
	//是否可以卸载
	virtual HRESULT InstanceIsCanUnLoadPlugin(IPluginObject * apPluginObject);

	//----------------------------------------------------------------------------------------------------------------
	// 预处理
	virtual HRESULT InstancePreTranslateMessage(MSG* apMsg);
	//----------------------------------------------------------------------------------------------------------------

protected:
	//检查内存池
	BOOL TimerCheckMemPool();
private:
	IMessageDispatcher *		m_pMessageDispatcher;	//消息派发器
	IPluginManager *			m_pCRPluginManager;		//插件管理器
	IUIThreadManager *		m_pCRUIThreadMgr;		//UI线程管理器

	CCriticalSection			m_oCriticalSectionMD;	//临界区 消息派发器
	CCriticalSection			m_oCriticalSectionPM;	//临界区 插件管理器
	CCriticalSection			m_oCriticalSectionUTM;	//临界区 UI线程管理器
    CCriticalSection			m_oCriticalSectionMQ;	//临界区 CR消息队列

	DWORD						m_dwSenderID;			//消息发送模块ID
	DWORD						m_dwCycleMsgDealNum;	//周期处理消息数量

	CRMessageIDQueue			m_oCRMessageIDQueue;	//需要注册的消息I列表
    CRMessageQueue				m_oCRMessageQueue;		//CR消息队列

	BOOL						m_bIsPluginCreated;		//插件是否创建
	long						m_lRefCount;			//引用计数器
	long						m_lCurrentDealMessage;	//当前处理消息数量

protected:
	STRU_CR_PLUGIN_INFO			m_oPluginInfo;			//插件信息
};

//投递消息，需要Pack函数的
template<typename T>
HRESULT CPluginBase::PostCRMessage(DWORD adwMessageID, T& aoPackData)
{
	HRESULT hResult = CRE_FALSE;

	char * pMem = NULL;

	do
	{
		//计算长度
		int iLen = aoPackData.Pack(NULL, 0);
		//没有长度
		if(iLen < 1)
		{
			break;
		}

		//分配内存
		pMem = (char*)CGlobalMemPool::Malloc(iLen);
		//分配失败
		if(NULL == pMem)
		{
			break;
		}

		//打包
		iLen = aoPackData.Pack(pMem, iLen);
		//打包失败
		if(iLen < 1)
		{
			break;
		}

		//成功后投递出去
		hResult = PostCRMessage(adwMessageID, pMem, iLen);
	}
	while(0);

	//需要释放内存
	if(pMem)
	{
		CGlobalMemPool::Free(pMem);
		pMem = NULL;
	}

	return hResult;
}

#endif //__PLUGIN_BASE__H_INCLUDE__