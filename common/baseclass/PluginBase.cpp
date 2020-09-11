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

//ȡ��ĳ���������ӿڣ�IPluginObject
HRESULT CPluginBase::GetCRPluginObject(REFIID aoPluginID, void ** appObject)
{
	VH::CComPtr<IPluginManager> ptrCRPluginManager;
	//ȡ�ò��������
	GetPluginManager(ptrCRPluginManager);

	//����ȡ�ò��������
	if(NULL == ptrCRPluginManager)
	{
		return CRE_FALSE;
	}

	//�Ӳ����������ѯ
	return ptrCRPluginManager->GetPlugin(aoPluginID, (IPluginObject**)appObject);
}

//��ȡĳ���������Ľӿڣ���IPluginObjectȥ��ѯ
HRESULT CPluginBase::GetCRPluginInterface(REFIID aoPluginID, REFIID aoIID, void ** appObject)
{
	VH::CComPtr<IPluginObject> ptrCRPluginObject;
	//��ȡ�ö���
	if(CRE_OK != GetCRPluginObject(aoPluginID, ptrCRPluginObject))
	{
		return CRE_FALSE;
	}

	//ȡ��ʧ��
	if(NULL == ptrCRPluginObject)
	{
		return CRE_FALSE;
	}

	//��ѯָ���ӿ�
	return ptrCRPluginObject->QueryInterface(aoIID, appObject);
}

//Ͷ����Ϣ
HRESULT CPluginBase::PostCRMessage(DWORD adwMessageID, void * apData, DWORD adwLen)
{
	VH::CComPtr<IMessageDispatcher> ptrMessageDispatcher;
	//ȡ����Ϣ�ɷ���
	GetMessageDispatcher(ptrMessageDispatcher);

	//����ȡ����Ϣ�ɷ���
	if(NULL == ptrMessageDispatcher)
	{
		return CRE_FALSE;
	}

	return ptrMessageDispatcher->PostCRMessage(m_dwSenderID, adwMessageID, apData, adwLen);
}

//�ɷ�������Ϣ
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

		//û����Ϣ��
		if(NULL == pCRMessage)
		{
			break;
		}
		//֪ͨ������Ϣ
		InstanceDealCRMessage(pCRMessage->m_dwSenderID, pCRMessage->m_dwMessageID, pCRMessage->m_pData, pCRMessage->m_dwLen);

		//��������ͷ��ڴ�
		if(NULL != pCRMessage)
		{
			delete pCRMessage;
			pCRMessage = NULL;
		}

		//�ۼӴ�����Ϣ����
		::InterlockedIncrement(&m_lCurrentDealMessage);

		//����ڴ��
		TimerCheckMemPool();
	}

	return TRUE;
}

//����ģ���UI�߳�ʹ��
DWORD CPluginBase::BeginUIThread(DWORD adwThreadIndex)
{
	VH::CComPtr<IUIThreadManager> ptrCRUIThreadManager;
	//ȡ��UI�̹߳�����
	GetUIThreadManager(ptrCRUIThreadManager);
	//����ȡ��UI�̹߳�����
	if(NULL == ptrCRUIThreadManager)
	{
		return 0;
	}

	VH::CComPtr<IUIThreadObject> ptrUIThreadObject;
	//ȡ��UI�̶߳���
	QueryInterface(IID_IUIThreadObject, ptrUIThreadObject);
	//����ȡ��UI�̶߳���
	if(NULL == ptrUIThreadObject)
	{
		return 0;
	}
	//����UI�߳�
	return ptrCRUIThreadManager->Begin(adwThreadIndex, ptrUIThreadObject);
}

//�ر�ģ���UI�߳�ʹ��
BOOL CPluginBase::EndUIThread(DWORD adwThreadIndex)
{
	VH::CComPtr<IUIThreadManager> ptrCRUIThreadManager;
	//ȡ��UI�̹߳�����
	GetUIThreadManager(ptrCRUIThreadManager);
	//����ȡ��UI�̹߳�����
	if(NULL == ptrCRUIThreadManager)
	{
		return FALSE;
	}

	VH::CComPtr<IUIThreadObject> ptrUIThreadObject;
	//ȡ��UI�̶߳���
	QueryInterface(IID_IUIThreadObject, ptrUIThreadObject);
	//����ȡ��UI�̶߳���
	if(NULL == ptrUIThreadObject)
	{
		return FALSE;
	}

	//����UI�߳�
	return (CRE_OK == ptrCRUIThreadManager->End(adwThreadIndex, ptrUIThreadObject));
}

// ע��Ԥ������Ϣ
BOOL CPluginBase::RegistUIPreMsg(BOOL abIsReg)
{
	//ȡ��UI�̹߳�����
	VH::CComPtr<IUIThreadManager> ptrCRUIThreadManager;
	if (CRE_OK != GetUIThreadManager(ptrCRUIThreadManager))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//����ȡ��UI�̹߳�����
	if(NULL == ptrCRUIThreadManager)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	//ȡ��UI�̶߳���
	VH::CComPtr<IUIPreMessageEvent> ptrUIPreMessageEvent;
	if (CRE_OK != QueryInterface(IID_IUIPreMessageEvent, ptrUIPreMessageEvent))
	{
		ASSERT(FALSE);
		return CRE_FALSE;
	}
	//����ȡ��UI�̶߳���
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

//ȡ�ò���������ӿ�:IPluginManager
HRESULT CPluginBase::GetPluginManager(void ** appCRPluginManager)
{
	CCriticalAutoLock loGuard(m_oCriticalSectionPM);

	if(NULL == m_pCRPluginManager)
	{
		return CRE_FALSE;
	}

	return m_pCRPluginManager->QueryInterface(IID_IPluginManager, appCRPluginManager);
}

//ȡ����Ϣ�ɷ����ӿ�:IMessageDispatcher
HRESULT CPluginBase::GetMessageDispatcher(void ** appMessageDispatcher)
{
	CCriticalAutoLock loGuard(m_oCriticalSectionPM);

	if(NULL == m_pMessageDispatcher)
	{
		return CRE_FALSE;
	}

	return m_pMessageDispatcher->QueryInterface(IID_IMessageDispatcher, appMessageDispatcher);
}

//ȡ���̹߳������ӿ�:IUIThreadManager
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

//��������
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
	//��ѯ��Ϣ�¼��ӿ�
	QueryInterface(IID_IMessageEvent, ptrMessageEvent);

	//����ȡ����Ϣ�¼��ӿ�
	if(NULL == ptrMessageEvent)
	{
      TRACE6("%s m_bIsPluginCreated NULL == ptrMessageEvent\n", __FUNCTION__);
		return CRE_FALSE;
	}

	CCriticalAutoLock loGuard(m_oCriticalSectionMD);

	//��Ϣ�ɷ�����
	if(NULL == m_pMessageDispatcher)
	{
      TRACE6("%s m_bIsPluginCreated NULL == m_pMessageDispatche\n", __FUNCTION__);
		return CRE_FALSE;
	}
   TRACE6("%s m_bIsPluginCreated InstanceInitRegMessagee\n", __FUNCTION__);
	//�����
	m_oCRMessageIDQueue.clear();
	//��ʼ����Ҫע�����Ϣ����
	InstanceInitRegMessage(m_oCRMessageIDQueue);

	//ע��������Ϣ
	CRMessageIDQueue::iterator itor = m_oCRMessageIDQueue.begin();
	for(; itor != m_oCRMessageIDQueue.end(); itor ++)
	{
		DWORD dwMessageID = (*itor);
		//ע��һ����Ϣ
		m_pMessageDispatcher->RegisterMessage(dwMessageID, 0, ptrMessageEvent);
	}
   TRACE6("%s m_bIsPluginCreated RegisterMessage ok\n", __FUNCTION__);
   TRACE6("%s end\n", __FUNCTION__);
	return CRE_OK;
}

//���ٶ���
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
		//��ѯ��Ϣ�¼��ӿ�
		QueryInterface(IID_IMessageEvent, ptrMessageEvent);

		//����ȡ����Ϣ�¼��ӿ�
		if(NULL == ptrMessageEvent)
		{
			break;
		}

		CCriticalAutoLock loGuard(m_oCriticalSectionMD);

		//��Ϣ�ɷ�����
		if(NULL == m_pMessageDispatcher)
		{
			break;
		}

		//��ע��������Ϣ
		CRMessageIDQueue::iterator itor = m_oCRMessageIDQueue.begin();
		for(; itor != m_oCRMessageIDQueue.end(); itor ++)
		{
			DWORD dwMessageID = (*itor);
			//��ע��һ����Ϣ
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

//ˢ�²���������Ҫ���¶�ȡ��Դ���ɲ������������ݣ�����ֻ��һ��֪ͨ��Ҳ���Բ����κβ�����
HRESULT STDMETHODCALLTYPE CPluginBase::Update()
{
	return CRE_NOTIMPL;
}

//ȡ��GUID
HRESULT STDMETHODCALLTYPE CPluginBase::GetPluginGUID(GUID& aoGuid)
{
	aoGuid = m_oPluginInfo.m_oCRPID;
	return CRE_OK;
}

//ȡ�ò����Ϣ
HRESULT STDMETHODCALLTYPE CPluginBase::GetPluginInfo(STRU_CR_PLUGIN_INFO& aoPluginInfo)
{
	aoPluginInfo = m_oPluginInfo;
	return CRE_OK;
}

//֪ͨ���ӵ����������
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

//�Ͽ����������
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
//IMessageEvent�ӿ�
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

//�� IMessageDispatcher ������
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
//�Ͽ��� IMessageDispatcher ������
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

//��������
HRESULT STDMETHODCALLTYPE CPluginBase::XMessageEvent::OnRecvMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen)
{
	METHOD_PROLOGUE_(CPluginBase, MessageEvent);

	if(FALSE == pThis->m_bIsPluginCreated)
	{
		return CRE_FALSE;
	}

	STRU_CR_MESSAGE_ITEM * pCRMessage = NULL;
	//����
	pCRMessage = new STRU_CR_MESSAGE_ITEM();

	//����ʧ��
	if(NULL == pCRMessage)
	{
		return CRE_FALSE;
	}

	//�������
	pCRMessage->m_dwSenderID = adwSenderID;
	pCRMessage->m_dwMessageID = adwMessageID;
	pCRMessage->SetData(apData, adwLen);

	pThis->m_oCriticalSectionMQ.Lock();
	pThis->m_oCRMessageQueue.push_back(pCRMessage);
	pThis->m_oCriticalSectionMQ.UnLock();

	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
//IUIThreadObject�ӿ�
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

//֪ͨ���ӵ�����̹߳�����
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

//�Ͽ�����̹߳�����
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

//��ʼ����UI�̹߳����������Beginʱ����UI�̹߳�������������UI�߳̽��棬������UI�̻߳ص���ͨ�����߼�ģ�鲻��Ҫʹ���������
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::Init(DWORD dwThreadID)
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
	return pThis->InstanceUIThreadInit(dwThreadID);
}

//����ʼ����UI�̹߳����������Endʱ����UI�̹߳�������������UI�߳̽��棬������UI�̻߳ص���ͨ�����߼�ģ�鲻��Ҫʹ���������
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::UnInit()
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
	return pThis->InstanceUIThreadUnInit();
}

//֪ͨ������Ϣ�ɷ������͵Ļ�����Ϣ�����������Beginʱָ����UI�̶߳�ʱ���ã�ͨ��������Ҫ����UI����Ĳ��ģ�����Ϣ������Ҫ���������������
HRESULT STDMETHODCALLTYPE CPluginBase::XUIThreadObject::DealCachedMessage(DWORD dwThreadID)
{
	METHOD_PROLOGUE_(CPluginBase, UIThreadObject);
	return pThis->DispatchCachedMessage() ? CRE_OK : CRE_FALSE;
}

//------------------------------------------------------------------------------------------------------------------------------
//IPluginManagerEvent�ӿ�
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

//֪ͨĳһ����������ص���������
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::OnPluginLoaded(IPluginObject * apPluginObject)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceOnPluginLoaded(apPluginObject);
}

//֪ͨĳһ������ӹ�����ж��֮ǰ
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::OnPluginUnLoadBefore(IPluginObject * apPluginObject)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceOnPluginUnLoadBefore(apPluginObject);
}

//֪ͨĳһ������Ѵӹ�����ж��
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::OnPluginUnLoaded(REFGUID aoCRPGuid)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceOnPluginUnLoaded(aoCRPGuid);
}

//�Ƿ���Լ���ĳһ�����
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::IsCanLoadPlugin(REFGUID aoCRPGuid)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceIsCanLoadPlugin(aoCRPGuid);
}

//�Ƿ����ж��
HRESULT STDMETHODCALLTYPE CPluginBase::XPluginManagerEvent::IsCanUnLoadPlugin(IPluginObject * apPluginObject)
{
	METHOD_PROLOGUE_(CPluginBase, PluginManagerEvent);
	return pThis->InstanceIsCanUnLoadPlugin(apPluginObject);
}

//------------------------------------------------------------------------------------------------------------------------------
//IPluginManagerEvent�ӿ�
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

//Ԥ����
HRESULT STDMETHODCALLTYPE CPluginBase::XUIPreMessageEvent::PreTranslateMessage(MSG* apMsg)
{
	METHOD_PROLOGUE_(CPluginBase, UIPreMessageEvent);
	return pThis->InstancePreTranslateMessage(apMsg);
}

//------------------------------------------------------------------------------------------------------------------------------

//��ѯ��չ�ӿ�
HRESULT CPluginBase::InstanceQueryInterface(REFIID riid, void ** appvObject)
{
	return CRE_NOINTERFACE;
}

//�������
HRESULT CPluginBase::InstanceCreatePlugin()
{
	return CRE_OK;
}

//���ٲ��
HRESULT CPluginBase::InstanceDestroyPlugin()
{
	return CRE_OK;
}

//��ʼ����Ҫע�����Ϣ
HRESULT CPluginBase::InstanceInitRegMessage(CRMessageIDQueue& aoCRMessageIDQueue)
{
	return CRE_OK;
}

//������Ϣ
HRESULT CPluginBase::InstanceDealCRMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen)
{
	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
//UI�̳߳�ʼ��
HRESULT CPluginBase::InstanceUIThreadInit(DWORD adwThreadID)
{
	return CRE_OK;
}

//UI�̷߳���ʼ��
HRESULT CPluginBase::InstanceUIThreadUnInit()
{
	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
//֪ͨĳһ����������ص���������
HRESULT CPluginBase::InstanceOnPluginLoaded(IPluginObject * apPluginObject)
{
	return CRE_OK;
}

//֪ͨĳһ������ӹ�����ж��֮ǰ
HRESULT CPluginBase::InstanceOnPluginUnLoadBefore(IPluginObject * apPluginObject)
{
	return CRE_OK;
}

//֪ͨĳһ������Ѵӹ�����ж��
HRESULT CPluginBase::InstanceOnPluginUnLoaded(REFGUID aoCRPGuid)
{
	return CRE_OK;
}

//�Ƿ���Լ���ĳһ�����
HRESULT CPluginBase::InstanceIsCanLoadPlugin(REFGUID aoCRPGuid)
{
	return CRE_OK;
}

//�Ƿ����ж��
HRESULT CPluginBase::InstanceIsCanUnLoadPlugin(IPluginObject * apPluginObject)
{
	return CRE_OK;
}

//------------------------------------------------------------------------------------------------------------------------------
// Ԥ����
HRESULT CPluginBase::InstancePreTranslateMessage(MSG* apMsg)
{
	return CRE_FALSE;
}

//------------------------------------------------------------------------------------------------------------------------------
//����ڴ��
BOOL CPluginBase::TimerCheckMemPool()
{
	if(m_lCurrentDealMessage > 1000)
	{
		CGlobalMemPool::Recovery();
		::InterlockedExchange(&m_lCurrentDealMessage, 0);
	}
	return TRUE;
}

// TraceLog ����awstrDllName�ǲ�����.dll��ģ�����ƣ��˷����������exe����־
void CPluginBase::InitDebugTrace(std::wstring awstrDllName, int aiTraceLevel, bool abUseSameLog/* = false*/)
{
	wchar_t lwzDllPath[MAX_PATH+1] = {0};
	//GetModuleFileName(GetModuleHandle(awstrDllName.c_str()), lwzDllPath, MAX_PATH);

   SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, lwzDllPath);
	wcscat_s(lwzDllPath, _MAX_PATH, L"\\VHallHelper");

	//std::wstring lwstrDllFullPath(lwzDllPath);
	//std::wstring lwstrDllPath = lwstrDllFullPath.substr(0, lwstrDllFullPath.rfind(L"\\"));

	//����������־��ӡѡ��
	CDebugTrace::SetTraceLevel(aiTraceLevel);

	CDebugTrace::SetTraceOptions(CDebugTrace::GetTraceOptions() \
		| CDebugTrace::Timestamp & ~CDebugTrace::LogLevel \
		& ~CDebugTrace::FileAndLine | CDebugTrace::AppendToFile\
		| CDebugTrace::PrintToConsole);

	//����TRACE�ļ���
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
