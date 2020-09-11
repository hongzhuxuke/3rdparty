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

//CR�������
class CPluginBase : public IPluginObject
{
public:
	//CR��Ϣ��
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
	//����CR��Ϣ����
	typedef std::list<STRU_CR_MESSAGE_ITEM*>	CRMessageQueue;

public:
	CPluginBase(const GUID& aoPLuginGuid, DWORD adwSenderID, DWORD adwCycleMsgDealNum);
	virtual ~CPluginBase(void);

public:
	//----------�ӿڿ��Ʋ���------------------------------------------------------------
	//ȡ��ĳ���������ӿڣ�IPluginObject
	HRESULT GetCRPluginObject(REFIID aoPluginID, void ** appObject);
	//��ȡĳ���������Ľӿڣ���IPluginObjectȥ��ѯ
	HRESULT GetCRPluginInterface(REFIID aoPluginID, REFIID aoIID, void ** appObject);

	//----------��Ϣ���Ʋ���------------------------------------------------------------
	//Ͷ����Ϣ
	HRESULT PostCRMessage(DWORD adwMessageID, void * apData, DWORD adwLen);
	//Ͷ����Ϣ����ҪPack������
	template<typename T>
	HRESULT PostCRMessage(DWORD adwMessageID, T& aoPackData);
	//�ɷ�������Ϣ������Ƿ�UIģ�飬����Ҫ��ʱ���øú��������� InstanceDealCRMessage �ص�
	BOOL DispatchCachedMessage();

	//----------�߳̿��Ʋ���------------------------------------------------------------
	//����ģ���UI�߳�ʹ��
	DWORD BeginUIThread(DWORD adwThreadIndex);
	//�ر�ģ���UI�߳�ʹ��
	BOOL EndUIThread(DWORD adwThreadIndex);

	//--------------ע��Ԥ������Ϣ---------------------------------------------
	// ע��Ԥ������Ϣ
	BOOL RegistUIPreMsg(BOOL abIsReg);
	// TraceLog ����awstrDllName�ǲ�����.dll��ģ�����ƣ��˷����������exe����־
   void InitDebugTrace(std::wstring awstrDllName, int aiTraceLevel, bool abUseSameLog = false);
protected:
	//ȡ�ò���������ӿ�:IPluginManager
	HRESULT GetPluginManager(void ** appCRPluginManager);
	//ȡ����Ϣ�ɷ����ӿ�:IMessageDispatcher
	HRESULT GetMessageDispatcher(void ** appMessageDispatcher);
	//ȡ���̹߳������ӿ�:IUIThreadManager
	HRESULT GetUIThreadManager(void ** appCRUIThreadManager);

public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** appvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

protected:
	//��������
	virtual HRESULT STDMETHODCALLTYPE Create();
	//���ٶ���
	virtual HRESULT STDMETHODCALLTYPE Destroy();
	//ˢ�²���������Ҫ���¶�ȡ��Դ���ɲ������������ݣ�����ֻ��һ��֪ͨ��Ҳ���Բ����κβ�����
	virtual HRESULT STDMETHODCALLTYPE Update();
	//ȡ��GUID
	virtual HRESULT STDMETHODCALLTYPE GetPluginGUID(GUID& aoGuid);
	//ȡ�ò����Ϣ
	virtual HRESULT STDMETHODCALLTYPE GetPluginInfo(STRU_CR_PLUGIN_INFO& aoPluginInfo);
	//֪ͨ���ӵ����������
	virtual HRESULT STDMETHODCALLTYPE ConnectPluginManager(IPluginManager * apCRPluginManager);
	//�Ͽ����������
	virtual HRESULT STDMETHODCALLTYPE DisconnectPluginManager();

protected:
	//----------------------------------------------------------------------------------------------------------------
	//IMessageEvent�ӿ�
	BEGIN_INTERFACE_PART(MessageEvent, IMessageEvent)
		INIT_INTERFACE_PART(CPluginBase, MessageEvent)

		//�� IMessageDispatcher ������
		virtual HRESULT STDMETHODCALLTYPE Connect(IMessageDispatcher * apMessageDispatcher);
		//�Ͽ��� IMessageDispatcher ������
		virtual HRESULT STDMETHODCALLTYPE Disconnect();
		//��������
		virtual HRESULT STDMETHODCALLTYPE OnRecvMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen);

	END_INTERFACE_PART(MessageEvent);
	//----------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------
	//IUIThreadObject
	BEGIN_INTERFACE_PART(UIThreadObject, IUIThreadObject)
		INIT_INTERFACE_PART(CPluginBase, UIThreadObject)

		//�̹߳������������ã�֪ͨ���ӵ��̹߳�����
		virtual HRESULT STDMETHODCALLTYPE Connect(IUIThreadManager * apCRUIThreadManager);
		//�̹߳������������ã�֪ͨ�Ͽ��̹߳����������󱣴�Ĺ�����ָ�벻�ٱ�֤��Ч
		virtual HRESULT STDMETHODCALLTYPE Disconnect();
		//��ʼ����UI�̹߳����������Beginʱ����UI�̹߳�������������UI�߳̽��棬������UI�̻߳ص���ͨ�����߼�ģ�鲻��Ҫʹ���������
		virtual HRESULT STDMETHODCALLTYPE Init(DWORD dwThreadID);
		//����ʼ����UI�̹߳����������Endʱ����UI�̹߳�������������UI�߳̽��棬������UI�̻߳ص���ͨ�����߼�ģ�鲻��Ҫʹ���������
		virtual HRESULT STDMETHODCALLTYPE UnInit();
		//֪ͨ������Ϣ�ɷ������͵Ļ�����Ϣ�����������Beginʱָ����UI�̶߳�ʱ���ã�ͨ��������Ҫ����UI����Ĳ��ģ�����Ϣ������Ҫ���������������
		virtual HRESULT STDMETHODCALLTYPE DealCachedMessage(DWORD dwThreadID);

	END_INTERFACE_PART(UIThreadObject);
	//----------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------
	//IPluginManagerEvent�ӿ�
	BEGIN_INTERFACE_PART(PluginManagerEvent, IPluginManagerEvent)
		INIT_INTERFACE_PART(CPluginBase, PluginManagerEvent)

		//֪ͨĳһ����������ص���������
		virtual HRESULT STDMETHODCALLTYPE OnPluginLoaded(IPluginObject * apPluginObject);
		//֪ͨĳһ������ӹ�����ж��֮ǰ
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoadBefore(IPluginObject * apPluginObject);
		//֪ͨĳһ������Ѵӹ�����ж��
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoaded(REFGUID aoCRPGuid);
		//�Ƿ���Լ���ĳһ�����
		virtual HRESULT STDMETHODCALLTYPE IsCanLoadPlugin(REFGUID aoCRPGuid);
		//�Ƿ����ж��
		virtual HRESULT STDMETHODCALLTYPE IsCanUnLoadPlugin(IPluginObject * apPluginObject);

	END_INTERFACE_PART(PluginManagerEvent);
	//----------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------
	//IPluginManagerEvent�ӿ�
	BEGIN_INTERFACE_PART(UIPreMessageEvent, IUIPreMessageEvent)
		INIT_INTERFACE_PART(CPluginBase, UIPreMessageEvent)

		//֪ͨע���UI�̹߳�����������������UI�߳�����ϢԤ����
		virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage(MSG* apMsg);

	END_INTERFACE_PART(UIPreMessageEvent);
	//----------------------------------------------------------------------------------------------------------------

protected:
	//----------------------------------------------------------------------------------------------------------------
	//ʵ���ӿڲ�ѯ
	virtual HRESULT InstanceQueryInterface(REFIID riid, void ** appvObject);
	//�������
	virtual HRESULT InstanceCreatePlugin();
	//���ٲ��
	virtual HRESULT InstanceDestroyPlugin();
	//��ʼ����Ҫע�����Ϣ
	virtual HRESULT InstanceInitRegMessage(CRMessageIDQueue& aoCRMessageIDQueue);
	//������Ϣ
	virtual HRESULT InstanceDealCRMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen);

	//----------------------------------------------------------------------------------------------------------------
	//UI�̳߳�ʼ��
	virtual HRESULT InstanceUIThreadInit(DWORD adwThreadID);
	//UI�̷߳���ʼ��
	virtual HRESULT InstanceUIThreadUnInit();

	//----------------------------------------------------------------------------------------------------------------
	//֪ͨĳһ����������ص���������
	virtual HRESULT InstanceOnPluginLoaded(IPluginObject * apPluginObject);
	//֪ͨĳһ������ӹ�����ж��֮ǰ
	virtual HRESULT InstanceOnPluginUnLoadBefore(IPluginObject * apPluginObject);
	//֪ͨĳһ������Ѵӹ�����ж��
	virtual HRESULT InstanceOnPluginUnLoaded(REFGUID aoCRPGuid);
	//�Ƿ���Լ���ĳһ�����
	virtual HRESULT InstanceIsCanLoadPlugin(REFGUID aoCRPGuid);
	//�Ƿ����ж��
	virtual HRESULT InstanceIsCanUnLoadPlugin(IPluginObject * apPluginObject);

	//----------------------------------------------------------------------------------------------------------------
	// Ԥ����
	virtual HRESULT InstancePreTranslateMessage(MSG* apMsg);
	//----------------------------------------------------------------------------------------------------------------

protected:
	//����ڴ��
	BOOL TimerCheckMemPool();
private:
	IMessageDispatcher *		m_pMessageDispatcher;	//��Ϣ�ɷ���
	IPluginManager *			m_pCRPluginManager;		//���������
	IUIThreadManager *		m_pCRUIThreadMgr;		//UI�̹߳�����

	CCriticalSection			m_oCriticalSectionMD;	//�ٽ��� ��Ϣ�ɷ���
	CCriticalSection			m_oCriticalSectionPM;	//�ٽ��� ���������
	CCriticalSection			m_oCriticalSectionUTM;	//�ٽ��� UI�̹߳�����
    CCriticalSection			m_oCriticalSectionMQ;	//�ٽ��� CR��Ϣ����

	DWORD						m_dwSenderID;			//��Ϣ����ģ��ID
	DWORD						m_dwCycleMsgDealNum;	//���ڴ�����Ϣ����

	CRMessageIDQueue			m_oCRMessageIDQueue;	//��Ҫע�����ϢI�б�
    CRMessageQueue				m_oCRMessageQueue;		//CR��Ϣ����

	BOOL						m_bIsPluginCreated;		//����Ƿ񴴽�
	long						m_lRefCount;			//���ü�����
	long						m_lCurrentDealMessage;	//��ǰ������Ϣ����

protected:
	STRU_CR_PLUGIN_INFO			m_oPluginInfo;			//�����Ϣ
};

//Ͷ����Ϣ����ҪPack������
template<typename T>
HRESULT CPluginBase::PostCRMessage(DWORD adwMessageID, T& aoPackData)
{
	HRESULT hResult = CRE_FALSE;

	char * pMem = NULL;

	do
	{
		//���㳤��
		int iLen = aoPackData.Pack(NULL, 0);
		//û�г���
		if(iLen < 1)
		{
			break;
		}

		//�����ڴ�
		pMem = (char*)CGlobalMemPool::Malloc(iLen);
		//����ʧ��
		if(NULL == pMem)
		{
			break;
		}

		//���
		iLen = aoPackData.Pack(pMem, iLen);
		//���ʧ��
		if(iLen < 1)
		{
			break;
		}

		//�ɹ���Ͷ�ݳ�ȥ
		hResult = PostCRMessage(adwMessageID, pMem, iLen);
	}
	while(0);

	//��Ҫ�ͷ��ڴ�
	if(pMem)
	{
		CGlobalMemPool::Free(pMem);
		pMem = NULL;
	}

	return hResult;
}

#endif //__PLUGIN_BASE__H_INCLUDE__