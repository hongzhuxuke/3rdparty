#include "StdAfx.h"
#include <stdio.h>
#include <ShlObj.h>

#include "PluginBase.h"
#include "CRPluginDef.h"

#include "DebugTrace.h"


CPluginBase::STRU_CR_MESSAGE_ITEM::STRU_CR_MESSAGE_ITEM()
: m_dwSenderID(0)
, m_dwMessageID(0)
, m_pData(NULL)
, m_dwLen(0)
{
}

CPluginBase::STRU_CR_MESSAGE_ITEM::~STRU_CR_MESSAGE_ITEM()
{
	if(NULL != m_pData)
	{
		CGlobalMemPool::Free(m_pData);
	}
	m_pData = NULL;
}

BOOL CPluginBase::STRU_CR_MESSAGE_ITEM::SetData(const void * apData, DWORD adwLen)
{
	if(NULL != m_pData)
	{
		CGlobalMemPool::Free(m_pData);
	}
	m_pData = NULL;
	m_dwLen = 0;

	if(NULL == apData || adwLen < 1)
	{
		return TRUE;
	}

	m_pData = CGlobalMemPool::Malloc(adwLen);

	if(NULL == m_pData)
	{
		return FALSE;
	}

	memcpy(m_pData, apData, adwLen);
	m_dwLen = adwLen;

	return TRUE;
}

CPluginBase::CPluginBase(const GUID& aoPLuginGuid, DWORD adwSenderID, DWORD adwCycleMsgDealNum)
: m_dwSenderID(adwSenderID)
, m_dwCycleMsgDealNum(adwCycleMsgDealNum)
, m_lRefCount(0)
, m_pMessageDispatcher(NULL)
, m_pCRPluginManager(NULL)
, m_pCRUIThreadMgr(NULL)
, m_bIsPluginCreated(FALSE)
, m_lCurrentDealMessage(0)
{
	m_oPluginInfo.m_oCRPID = aoPLuginGuid;
}

CPluginBase::~CPluginBase(void)
{
	ASSERT(FALSE == m_bIsPluginCreated);
	ASSERT(m_oCRMessageQueue.empty());
}

//取得某个插件对象接口：IPluginObject
HRESULT CPluginBase::GetCRPluginObject(REFIID aoPluginID, void ** appObject)
{
	VH::CComPtr<IPluginManager> ptrCRPluginManager;
	//取得插件管理器
	GetPluginManager(ptrCRPluginManager);

	//不能取得插件管理器
	if(NULL == ptrCRPluginManager)
	{
		return CRE_FALSE;
	}

	//从插件管理器查询
	return ptrCRPluginManager->GetPlugin(aoPluginID, (IPluginObject**)appObject);
}

//获取某个插件对象的接口，由IPluginObject去查询
HRESULT CPluginBase::GetCRPluginInterface(REFIID aoPluginID, REFIID aoIID, void ** appObject)
{
	VH::CComPtr<IPluginObject> ptrCRPluginObject;
	//先取得对象
	if(CRE_OK != GetCRPluginObject(aoPluginID, ptrCRPluginObject))
	{
		return CRE_FALSE;
	}

	//取得失败
	if(NULL == ptrCRPluginObject)
	{
		return CRE_FALSE;
	}

	//查询指定接口
	return ptrCRPluginObject->QueryInterface(aoIID, appObject);
}

//投递消息
HRESULT CPluginBase::PostCRMessage(DWORD adwMessageID, void * apData, DWORD adwLen)
{
	VH::CComPtr<IMessageDispatcher> ptrMessageDispatcher;
	//取得消息派发器
	GetMessageDispatcher(ptrMessageDispatcher);

	//不能取得消息派发器
	if(NULL == ptrMessageDispatcher)
	{
		return CRE_FALSE;
	}

	return ptrMessageDispatcher->PostCRMessage(m_dwSenderID, adwMessageID, apData, adwLen);
}

//派发缓存消息
BOOL CPluginBase::DispatchCachedMessage()
{
	for(DWORD i = 0; i < m_dwCycleMsgDealNum; i ++)
	{
		STRU_CR_MESSAGE_ITEM * pCRMessage = NULL;

		//do
		{
			m_oCriticalSectionMQ.Lock();
			if(m_oCRMessageQueue.size() > 0)
			{
				pCRMessage = m_oCRMessageQueue.front();
				m_oCRMessageQueue.pop_front();
			}
			m_oCriticalSectionMQ.UnLock();
		}
		//while(0);

		//没有消息了
		if(NULL == pCRMessage)
		{
			break;
		}
		//通知处理消息
		InstanceDealCRMessage(pCRMessage->m_dwSenderID, pCRMessage->m_dwMessageID, pCRMessage->m_pData, pCRMessage->m_dwLen);

		//处理完后释放内存
		if(NULL != pCRMessage)
		{
			delete pCRMessage;
			pCRMessage = NULL;
		}

		//累加处理消息数量
		::InterlockedIncrement(&m_lCurrentDealMessage);

		//检查内存池
		TimerCheckMemPool();
	}

	return TRUE;
}

//启动模块的UI线程使用
DWORD CPluginBase::BeginUIThread(DWORD adwThreadIndex)
{
	VH::CComPtr<IUIThreadManager> ptrCRUIThreadManager;
	//取得UI线程管理器
	GetUIThreadManager(ptrCRUIThreadManager);
	//不能取得UI线程管理器
	if(NULL == ptrCRUIThreadManager)
	{
		return 0;
	}

	VH::CComPtr<IUIThreadObject> ptrUIThreadObject;
	//取得UI线程对象
	QueryInterface(IID_IUIThreadObject, ptrUIThreadObject);
	//不能取得UI线程对象
	if(NULL == ptrUIThreadObject)
	{
		return 0;
	}
	//启动UI线程
	return ptrCRUIThreadManager->Begin(adwThreadIndex, ptrUIThreadObject);
}

//关闭模块的UI线程使用
BOOL CPluginBase::EndUIThread(DWORD adwThreadIndex)
{
	VH::CComPtr<IUIThreadManager> ptrCRUIThreadManager;
	//取得UI线程管理器
	GetUIThreadManager(ptrCRUIThreadManager);
	//不能取得UI线程管理器
	if(NULL == ptrCRUIThreadManager)
	{
		return FALSE;
	}

	VH::CComPtr<IUIThreadObject> ptrUIThreadObject;
	//取得UI线程对象
	QueryInterface(IID_IUIThreadObject, ptrUIThreadObject);
	//不能取得UI线程对象
	if(NULL == ptrUIThreadObject)
	{
		return FALSE;
	}

	//启动UI线程
	return (CRE_OK == ptrCRUIThreadManager->End(adwThreadIndex, ptrUIThreadObject));
}

// 注册预处理消息
BOOL CPluginBase::RegistUIPreMsg(BOOL abIsReg)
{
	//取得UI线程管理器
	VH::CComPtr<IUIThreadManager> ptrCRUIThreadManager;
	if (CRE_OK != GetUIThreadManager(ptrCRUIThreadManager))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//不能取得UI线程管理器
	if(NULL == ptrCRUIThreadManager)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//取得UI线程对象
	VH::CComPtr<IUIPreMessageEvent> ptrUIPreMessageEvent;
	if (CRE_OK != QueryInterface(IID_IUIPreMessageEvent, ptrUIPreMessageEvent))
	{
		ASSERT(FALSE);
		return CRE_FALSE;
	}
	//不能取得UI线程对象
	if(NULL == ptrUIPreMessageEvent)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (abIsReg)
	{
		ptrCRUIThreadManager->RegisterUIPreMessageEvent(ptrUIPreMessageEvent);
	}
	else
	{
		ptrCRUIThreadManager->UnRegisterUIPreMessageEvent(ptrUIPreMessageEvent);
	}

	return TRUE;
}

//取得插件管理器接口:IPluginManager
HRESULT CPluginBase::GetPluginManager(void ** appCRPluginManager)
{
	CCriticalAutoLock loGuard(m_oCriticalSectionPM);

	if(NULL == m_pCRPluginManager)
	{
		return CRE_FALSE;
	}

	return m_pCRPluginManager->QueryInterface(IID_IPluginManager, appCRPluginManager);
}

//取得消息派发器接口:IMessageDispatcher
HRESULT CPluginBase::GetMessageDispatcher(void ** appMessageDispatcher)
{
	CCriticalAutoLock loGuard(m_oCriticalSectionPM);

	if(NULL == m_pMessageDispatcher)
	{
		return CRE_FALSE;
	}

	return m_pMessageDispatcher->QueryInterface(IID_IMessageDispatcher, appMessageDispatcher);
}

//取得线程管理器接口:IUIThreadManager
HRESULT CPluginBase::GetUIThreadManager(void ** appCRUIThreadManager)
{
	CCriticalAutoLock loGuard(m_oCriticalSectionUTM);

	if(NULL == m_pCRUIThreadMgr)
	{
		return CRE_FALSE;
	}

	return m_pCRUIThreadMgr->QueryInterface(IID_IUIThreadManager, appCRUIThreadManager);
}

HRESULT STDMETHODCALLTYPE CPluginBase::QueryInterface(REFIID riid, void ** appvObject)
{
	if(NULL == appvObject)
	{
		return CRE_INVALIDARG;
	}

	if(riid == IID_VHIUnknown)
	{
		*appvObject = (VH_IUnknown*)this;
		AddRef();
		return CRE_OK;
	}
	else if(riid == IID_IPluginObject)
	{
		*appvObject = (IPluginObject*)this;
		AddRef();
		return CRE_OK;
	}
	else if(riid == IID_IMessageEvent)
	{
		return m_xMessageEvent.QueryInterface(IID_IMessageEvent, appvObject);
	}
	else if(riid == IID_IUIThreadObject)
	{
		return m_xUIThreadObject.QueryInterface(IID_IUIThreadObject, appvObject);
	}
	else if(riid == IID_IPluginManagerEvent)
	{
		return m_xPluginManagerEvent.QueryInterface(IID_IPluginManagerEvent, appvObject);
	}
	else if(riid == IID_IUIPreMessageEvent)
	{
		return m_xUIPreMessageEvent.QueryInterface(IID_IUIPreMessageEvent, appvObject);
	}
	else
	{
		return InstanceQueryInterface(riid, appvObject);
	}
	return CRE_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CPluginBase::AddRef(void)
{
	::InterlockedIncrement(&m_lRefCount);
	return m_lRefCount;
}

ULONG STDMETHODCALLTYPE CPluginBase::Release(void)
{
	::InterlockedDecrement(&m_lRefCount);
	return m_lRefCount;
}

//创建对象
HRESULT STDMETHODCALLTYPE CPluginBase::Create()
{
   TRACE6("%s start\n",__FUNCTION__);
	if(m_bIsPluginCreated)
	{
      TRACE6("%s m_bIsPluginCreated ok\n", __FUNCTION__);
		return CRE_OK;
	}

	HRESULT hr = InstanceCreatePlugin();
	if(CRE_OK == hr)
	{
      TRACE6("%s m_bIsPluginCreated CRE_OK == hr\n", __FUNCTION__);
		m_bIsPluginCreated = TRUE;
	}
	else
	{
      TRACE6("%s m_bIsPluginCreated return hr;\n", __FUNCTION__);
		return hr;
	}
	VH::CComPtr<IMessageEvent> ptrMessageEvent;
	//查询消息事件接口
	QueryInterface(IID_IMessageEvent, ptrMessageEvent);

	//不能取得消息事件接口
	if(NULL == ptrMessageEvent)
	{
      TRACE6("%s m_bIsPluginCreated NULL == ptrMessageEvent\n", __FUNCTION__);
		return CRE_FALSE;
	}

	CCriticalAutoLock loGuard(m_oCriticalSectionMD);

	//消息派发器空
	if(NULL == m_pMessageDispatcher)
	{
      TRACE6("%s m_bIsPluginCreated NULL == m_pMessageDispatche\n", __FUNCTION__);
		return CRE_FALSE;
	}
   TRACE6("%s m_bIsPluginCreated InstanceInitRegMessagee\n", __FUNCTION__);
	//先清空
	m_oCRMessageIDQueue.clear();
	//初始化需要注册的消息队列
	InstanceInitRegMessage(m_oCRMessageIDQueue);

	//注册所有消息
	CRMessageIDQueue::iterator itor = m_oCRMessageIDQueue.begin();
	for(; itor != m_oCRMessageIDQueue.end(); itor ++)
	{
		DWORD dwMessageID = (*itor);
		//注册一条消息
		m_pMessageDispatcher->RegisterMessage(dwMessageID, 0, ptrMessageEvent);
	}
   TRACE6("%s m_bIsPluginCreated RegisterMessage ok\n", __FUNCTION__);
   TRACE6("%s end\n", __FUNCTION__);
	return CRE_OK;
}

//销毁对象
HRESULT STDMETHODCALLTYPE CPluginBase::Destroy()
{
	if(FALSE == m_bIsPluginCreated)
	{
		return CRE_OK;
	}

	m_bIsPluginCreated = FALSE;

	HRESULT hr = InstanceDestroyPlugin();
	do
	{
		VH::CComPtr<IMessageEvent> ptrMessageEvent;
		//查询消息事件接口
		QueryInterface(IID_IMessageEvent, ptrMessageEvent);

		//不能取得消息事件接口
		if(NULL == ptrMessageEvent)
		{
			break;
		}

		CCriticalAutoLock loGuard(m_oCriticalSectionMD);

		//消息派发器空
		if(NULL == m_pMessageDispatcher)
		{
			break;
		}

		//反注册所有消息
		CRMessageIDQueue::iterator itor = m_oCRMessageIDQueue.begin();
		for(; itor != m_oCRMessageIDQueue.end(); itor ++)
		{
			DWORD dwMessageID = (*itor);
			//反注册一条消息
			m_pMessageDispatcher->UnRegisterMessage(dwMessageID, ptrMessageEvent);
		}
	}
	while(0);

	m_oCriticalSectionMQ.Lock();
	while(1)
	{
		if(m_oCRMessageQueue.empty())
		{
			break;
		}
		STRU_CR_MESSAGE_ITEM * pCRMessage = m_oCRMessageQueue.front();
		m_oCRMessageQueue.pop_front();
		if(NULL != pCRMessage)
		{
			delete pCRMessage;
			pCRMessage = NULL;
		}
	}
	m_oCriticalSectionMQ.UnLock();
	return hr;

}

//刷新插件，插件需要重新读取资源（由插件定义更新内容，这里只是一个通知，也可以不作任何操作）
HRESULT STDMETHODCALLTYPE CPluginBase::Update()
{
	return CRE_NOTIMPL;
}

//取得GUID
HRESULT STDMETHODCALLTYPE CPluginBase::GetPluginGUID(GUID& aoGuid)
{
	aoGuid = m_oPluginInfo.m_oCRPID;
	return CRE_OK;
}

//取得插件信息
HRESULT STDMETHODCALLTYPE CPluginBase::GetPluginInfo(STRU_CR_PLUGIN_INFO& aoPluginInfo)
{
	aoPluginInfo = m_oPluginInfo;
	return CRE_OK;
}

//通知连接到插件管理器
HRESULT STDMETHODCALLTYPE CPluginBase::ConnectPluginManager(IPluginManager * apCRPluginManager)
{
	CCriticalAutoLock loGuard(m_oCriticalSectionPM);

	m_pCRPluginManager = apCRPluginManager;
	if(NULL != m_pCRPluginManager)
	{
		m_pCRPluginManager->AddRef();
		m_pCRPluginManager->RegisterEvent(&m_xPluginManagerEvent);
	}

	return CRE_OK;
}

//断开插件管理器
HRESULT STDMETHODCALLTYPE CPluginBase::DisconnectPluginManager()
{
	CCriticalAutoLock loGuard(m_oCriticalSectionPM);

	if(NULL != m_pCRPluginManager)
	{
		m_pCRPluginManager->UnRegisterEvent(&m_xPluginManagerEvent);
		m_pCRPluginManager->Release();
		m_pCRPluginManager = NULL;
	}

	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
//IMessageEvent接口
STDMETHODIMP_(ULONG) CPluginBase::XMessageEvent::AddRef()
{
	METHOD_PROLOGUE_(CPluginBase, MessageEvent);
    return pThis->AddRef();
}

STDMETHODIMP_(ULONG) CPluginBase::XMessageEvent::Release()
{                            
	METHOD_PROLOGUE_(CPluginBase, MessageEvent);
    return pThis->Release();
}

STDMETHODIMP CPluginBase::XMessageEvent::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE_(CPluginBase, MessageEvent);

	if(riid == IID_VHIUnknown)
	{
		*ppvObj = (VH_IUnknown*)pThis;
		AddRef();
	}
	else if(riid == IID_IMessageEvent)
	{
		*ppvObj = (IMessageEvent*)this;
		AddRef();
	}
	else
	{
		return CRE_NOINTERFACE;
	}
	return CRE_OK;
}

//与 IMessageDispatcher 的连接
HRESULT STDMETHODCALLTYPE CPluginBase::XMessageEvent::Connect(IMessageDispatcher * apMessageDispatcher)
{
	METHOD_PROLOGUE_(CPluginBase, MessageEvent);

	CCriticalAutoLock loGuard(pThis->m_oCriticalSectionMD);

	if(pThis->m_pMessageDispatcher)
	{
		pThis->m_pMessageDispatcher->Release();
	}

	pThis->m_pMessageDispatcher = apMessageDispatcher;

	if(pThis->m_pMessageDispatcher)
	{
		pThis->m_pMessageDispatcher->AddRef();
	}

	return CRE_OK;
}
//断开与 IMessageDispatcher 的连接
HRESULT STDMETHODCALLTYPE CPluginBase::XMessageEvent::Disconnect()
{
	METHOD_PROLOGUE_(CPluginBase, MessageEvent);

	CCriticalAutoLock loGuard(pThis->m_oCriticalSectionMD);

	if(pThis->m_pMessageDispatcher)
	{
		pThis->m_pMessageDispatcher->Release();
		pThis->m_pMessageDispatcher = NULL;
	}

	return CRE_OK;
}

//接收数据
HRESULT STDMETHODCALLTYPE CPluginBase::XMessageEvent::OnRecvMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen)
{
	METHOD_PROLOGUE_(CPluginBase, MessageEvent);

	if(FALSE == pThis->m_bIsPluginCreated)
	{
		return CRE_FALSE;
	}

	STRU_CR_MESSAGE_ITEM * pCRMessage = NULL;
	//分配
	pCRMessage = new STRU_CR_MESSAGE_ITEM();

	//分配失败
	if(NULL == pCRMessage)
	{
		return CRE_FALSE;
	}

	//保存参数
	pCRMessage->m_dwSenderID = adwSenderID;
	pCRMessage->m_dwMessageID = adwMessageID;
	pCRMessage->SetData(apData, adwLen);

	pThis->m_oCriticalSectionMQ.Lock();
	pThis->m_oCRMessageQueue.push_back(pCRMessage);
	pThis->m_oCriticalSectionMQ.UnLock();

	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
//IUIThreadObject接口
STDMETHODIMP_(ULONG) CPluginBase::XUIThreadObject::AddRef()
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
    return pThis->AddRef();
}

STDMETHODIMP_(ULONG) CPluginBase::XUIThreadObject::Release()
{                            
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
    return pThis->Release();
}

STDMETHODIMP CPluginBase::XUIThreadObject::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);

	if(riid == IID_VHIUnknown)
	{
		*ppvObj = (VH_IUnknown*)pThis;
		AddRef();
	}
	else if(riid == IID_IUIThreadObject)
	{
		*ppvObj = (IUIThreadObject*)this;
		AddRef();
	}
	else
	{
		return CRE_NOINTERFACE;
	}
	return CRE_OK;
}

//通知连接到插件线程管理器
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::Connect(IUIThreadManager * apCRUIThreadManager)
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);

	CCriticalAutoLock loGuard(pThis->m_oCriticalSectionUTM);

	if(pThis->m_pCRUIThreadMgr)
	{
		pThis->m_pCRUIThreadMgr->Release();
	}

	pThis->m_pCRUIThreadMgr = apCRUIThreadManager;

	if(pThis->m_pCRUIThreadMgr)
	{
		pThis->m_pCRUIThreadMgr->AddRef();
	}

	return CRE_OK;
}

//断开插件线程管理器
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::Disconnect()
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);

	CCriticalAutoLock loGuard(pThis->m_oCriticalSectionUTM);

	if(pThis->m_pCRUIThreadMgr)
	{
		pThis->m_pCRUIThreadMgr->Release();
		pThis->m_pCRUIThreadMgr = NULL;
	}

	return CRE_OK;
}

//初始化，UI线程工作对象调用Begin时，由UI线程管理器与期望的UI线程交涉，由期望UI线程回调，通常纯逻辑模块不需要使用这个功能
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::Init(DWORD dwThreadID)
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
	return pThis->InstanceUIThreadInit(dwThreadID);
}

//反初始化，UI线程工作对象调用End时，由UI线程管理器与期望的UI线程交涉，由期望UI线程回调，通常纯逻辑模块不需要使用这个功能
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::UnInit()
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
	return pThis->InstanceUIThreadUnInit();
}

//通知处理消息派发器推送的缓存消息，这个函数由Begin时指定的UI线程定时调用，通常所有需要操作UI界面的插件模块的消息处理，都要在这个函数调用中
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::DealCachedMessage(DWORD dwThreadID)
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
	return pThis->DispatchCachedMessage() ? CRE_OK : CRE_FALSE;
}

//------------------------------------------------------------------------------------------------------------------------------
//IPluginManagerEvent接口
STDMETHODIMP_(ULONG) CPluginBase::XPluginManagerEvent::AddRef()
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
    return pThis->AddRef();
}

STDMETHODIMP_(ULONG) CPluginBase::XPluginManagerEvent::Release()
{                            
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
    return pThis->Release();
}

STDMETHODIMP CPluginBase::XPluginManagerEvent::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);

	if(riid == IID_VHIUnknown)
	{
		*ppvObj = (VH_IUnknown*)pThis;
		AddRef();
	}
	else if(riid == IID_IPluginManagerEvent)
	{
		*ppvObj = (IPluginManagerEvent*)this;
		AddRef();
	}
	else
	{
		return CRE_NOINTERFACE;
	}
	return CRE_OK;
}

//通知某一个插件被加载到管理器中
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::OnPluginLoaded(IPluginObject * apPluginObject)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceOnPluginLoaded(apPluginObject);
}

//通知某一个插件从管理器卸载之前
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::OnPluginUnLoadBefore(IPluginObject * apPluginObject)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceOnPluginUnLoadBefore(apPluginObject);
}

//通知某一个插件已从管理器卸载
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::OnPluginUnLoaded(REFGUID aoCRPGuid)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceOnPluginUnLoaded(aoCRPGuid);
}

//是否可以加载某一个插件
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::IsCanLoadPlugin(REFGUID aoCRPGuid)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceIsCanLoadPlugin(aoCRPGuid);
}

//是否可以卸载
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::IsCanUnLoadPlugin(IPluginObject * apPluginObject)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceIsCanUnLoadPlugin(apPluginObject);
}

//------------------------------------------------------------------------------------------------------------------------------
//IPluginManagerEvent接口
STDMETHODIMP_(ULONG) CPluginBase::XUIPreMessageEvent::AddRef()
{
	METHOD_PROLOGUE_(CPluginBase, UIPreMessageEvent);
	return pThis->AddRef();
}

STDMETHODIMP_(ULONG) CPluginBase::XUIPreMessageEvent::Release()
{                            
	METHOD_PROLOGUE_(CPluginBase, UIPreMessageEvent);
	return pThis->Release();
}

STDMETHODIMP CPluginBase::XUIPreMessageEvent::QueryInterface(REFIID riid, void ** ppvObj)
{
	METHOD_PROLOGUE_(CPluginBase, UIPreMessageEvent);

	if(riid == IID_VHIUnknown)
	{
		*ppvObj = (VH_IUnknown*)pThis;
		AddRef();
	}
	else if(riid == IID_IUIPreMessageEvent)
	{
		*ppvObj = (IUIPreMessageEvent*)this;
		AddRef();
	}
	else
	{
		return CRE_NOINTERFACE;
	}
	return CRE_OK;
}

//预处理
HRESULT STDMETHODCALLTYPE CPluginBase::XUIPreMessageEvent::PreTranslateMessage(MSG* apMsg)
{
	METHOD_PROLOGUE_(CPluginBase, UIPreMessageEvent);
	return pThis->InstancePreTranslateMessage(apMsg);
}

//------------------------------------------------------------------------------------------------------------------------------

//查询扩展接口
HRESULT CPluginBase::InstanceQueryInterface(REFIID riid, void ** appvObject)
{
	return CRE_NOINTERFACE;
}

//创建插件
HRESULT CPluginBase::InstanceCreatePlugin()
{
	return CRE_OK;
}

//销毁插件
HRESULT CPluginBase::InstanceDestroyPlugin()
{
	return CRE_OK;
}

//初始化需要注册的消息
HRESULT CPluginBase::InstanceInitRegMessage(CRMessageIDQueue& aoCRMessageIDQueue)
{
	return CRE_OK;
}

//处理消息
HRESULT CPluginBase::InstanceDealCRMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen)
{
	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
//UI线程初始化
HRESULT CPluginBase::InstanceUIThreadInit(DWORD adwThreadID)
{
	return CRE_OK;
}

//UI线程反初始化
HRESULT CPluginBase::InstanceUIThreadUnInit()
{
	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
//通知某一个插件被加载到管理器中
HRESULT CPluginBase::InstanceOnPluginLoaded(IPluginObject * apPluginObject)
{
	return CRE_OK;
}

//通知某一个插件从管理器卸载之前
HRESULT CPluginBase::InstanceOnPluginUnLoadBefore(IPluginObject * apPluginObject)
{
	return CRE_OK;
}

//通知某一个插件已从管理器卸载
HRESULT CPluginBase::InstanceOnPluginUnLoaded(REFGUID aoCRPGuid)
{
	return CRE_OK;
}

//是否可以加载某一个插件
HRESULT CPluginBase::InstanceIsCanLoadPlugin(REFGUID aoCRPGuid)
{
	return CRE_OK;
}

//是否可以卸载
HRESULT CPluginBase::InstanceIsCanUnLoadPlugin(IPluginObject * apPluginObject)
{
	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
// 预处理
HRESULT CPluginBase::InstancePreTranslateMessage(MSG* apMsg)
{
	return CRE_FALSE;
}

//------------------------------------------------------------------------------------------------------------------------------
//检查内存池
BOOL CPluginBase::TimerCheckMemPool()
{
	if(m_lCurrentDealMessage > 1000)
	{
		CGlobalMemPool::Recovery();
		::InterlockedExchange(&m_lCurrentDealMessage, 0);
	}
	return TRUE;
}

// TraceLog 参数awstrDllName是不包含.dll的模块名称，此方法不能输出exe的日志
void CPluginBase::InitDebugTrace(std::wstring awstrDllName, int aiTraceLevel, bool abUseSameLog/* = false*/)
{
	wchar_t lwzDllPath[MAX_PATH+1] = {0};
	//GetModuleFileName(GetModuleHandle(awstrDllName.c_str()), lwzDllPath, MAX_PATH);

   SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, lwzDllPath);
	wcscat_s(lwzDllPath, _MAX_PATH, L"\\VHallHelper");

	//std::wstring lwstrDllFullPath(lwzDllPath);
	//std::wstring lwstrDllPath = lwstrDllFullPath.substr(0, lwstrDllFullPath.rfind(L"\\"));

	//首先设置日志打印选项
	CDebugTrace::SetTraceLevel(aiTraceLevel);

	CDebugTrace::SetTraceOptions(CDebugTrace::GetTraceOptions() \
		| CDebugTrace::Timestamp & ~CDebugTrace::LogLevel \
		& ~CDebugTrace::FileAndLine | CDebugTrace::AppendToFile\
		| CDebugTrace::PrintToConsole);

	//生成TRACE文件名
	SYSTEMTIME loSystemTime;
	GetLocalTime(&loSystemTime);

	wchar_t lwzLogFileName[255] ={0};

    if (abUseSameLog)
    {
       wsprintf(lwzLogFileName, L"%s%s%s_%4d_%02d_%02d_%02d_%02d%s",
		   lwzDllPath, VH_LOG_DIR, L"GGLogFile", loSystemTime.wYear, loSystemTime.wMonth, loSystemTime.wDay, loSystemTime.wHour, 
		   loSystemTime.wMinute, L".log");
    }
    else
    {
       wsprintf(lwzLogFileName, L"%s%s%s_%4d_%02d_%02d_%02d_%02d%s", 
		   lwzDllPath, VH_LOG_DIR, awstrDllName.c_str(), loSystemTime.wYear, loSystemTime.wMonth, loSystemTime.wDay, loSystemTime.wHour, loSystemTime.wMinute, L".log");
    }

	CDebugTrace::SetLogFileName(lwzLogFileName);
}
