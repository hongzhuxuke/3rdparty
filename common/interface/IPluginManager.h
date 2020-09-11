#ifndef __I_PLUGIN_MANAGER__H_INCLUDE__
#define __I_PLUGIN_MANAGER__H_INCLUDE__

#pragma once

#include "VH_IUnknown.h"
#include "VHP_StruDeff.h"

// {37529FCD-DCD0-43C3-82F6-C4FD8D48EE69}
DEFINE_GUID(IID_IPluginObject, 
	0x37529fcd, 0xdcd0, 0x43c3, 0x82, 0xf6, 0xc4, 0xfd, 0x8d, 0x48, 0xee, 0x69);

// {176E891A-3D11-49C7-A8E7-541564823DAD}
DEFINE_GUID(IID_IPluginManagerEvent, 
	0x176e891a, 0x3d11, 0x49c7, 0xa8, 0xe7, 0x54, 0x15, 0x64, 0x82, 0x3d, 0xad);

// {F062E656-C75B-428C-9B6A-ED4710A4BDAE}
DEFINE_GUID(IID_IPluginManager, 
	0xf062e656, 0xc75b, 0x428c, 0x9b, 0x6a, 0xed, 0x47, 0x10, 0xa4, 0xbd, 0xae);

extern "C++"
{
	class IPluginManager;
	//����������в������ʵ��
	class IPluginObject : public VH_IUnknown
	{
	public:
		//��������
		virtual HRESULT STDMETHODCALLTYPE Create() = 0;
		//���ٶ���
		virtual HRESULT STDMETHODCALLTYPE Destroy() = 0;
		//ˢ�²���������Ҫ���¶�ȡ��Դ���ɲ������������ݣ�����ֻ��һ��֪ͨ��Ҳ���Բ����κβ�����
		virtual HRESULT STDMETHODCALLTYPE Update() = 0;
		//ȡ��GUID
		virtual HRESULT STDMETHODCALLTYPE GetPluginGUID(GUID& aoGuid) = 0;
		//ȡ�ò����Ϣ
		virtual HRESULT STDMETHODCALLTYPE GetPluginInfo(STRU_CR_PLUGIN_INFO& aoPluginInfo) = 0;
		//֪ͨ���ӵ����������
		virtual HRESULT STDMETHODCALLTYPE ConnectPluginManager(IPluginManager * apCRPluginManager) = 0;
		//�Ͽ����������
		virtual HRESULT STDMETHODCALLTYPE DisconnectPluginManager() = 0;
		//���ü��ݲ�ѯ�ӿڣ�����ӿ��ڲ��ϵͳ������ɾ��
	};

	//����������¼�
	class IPluginManagerEvent : public VH_IUnknown
	{
	public:
		//֪ͨĳһ����������ص���������
		virtual HRESULT STDMETHODCALLTYPE OnPluginLoaded(IPluginObject * apPluginObject) = 0;
		//֪ͨĳһ������ӹ�����ж��֮ǰ
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoadBefore(IPluginObject * apPluginObject) = 0;
		//֪ͨĳһ������Ѵӹ�����ж��
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoaded(REFGUID aoCRPGuid) = 0;
		//�Ƿ���Լ���ĳһ�����
		virtual HRESULT STDMETHODCALLTYPE IsCanLoadPlugin(REFGUID aoCRPGuid) = 0;
		//�Ƿ����ж��
		virtual HRESULT STDMETHODCALLTYPE IsCanUnLoadPlugin(IPluginObject * apPluginObject) = 0;
	};

	//���������
	class IPluginManager : public VH_IUnknown
	{
	public:
		//ע��������¼�
		virtual HRESULT STDMETHODCALLTYPE RegisterEvent(IPluginManagerEvent * apPluginManagerEvent) = 0;
		//ȡ��ע��������¼�
		virtual HRESULT STDMETHODCALLTYPE UnRegisterEvent(IPluginManagerEvent * apPluginManagerEvent) = 0;
		//����һ������������������ appPluginObject ��Ϊ NULL���򷵻ز������ӿ�
		virtual HRESULT STDMETHODCALLTYPE LoadPlugin(const WCHAR * apwzFileName, IPluginObject ** appPluginObject) = 0;
		//ж��һ������ӹ�����
		virtual HRESULT STDMETHODCALLTYPE UnLoadPlugin(REFGUID aoCRPGuid) = 0;
		//�Ƿ���Լ���ĳһ�����
		virtual HRESULT STDMETHODCALLTYPE IsCanLoadPlugin(REFGUID aoCRPGuid) = 0;
		//ѯ���Ƿ����ж�ز��
		virtual HRESULT STDMETHODCALLTYPE IsCanUnLoadPlugin(REFGUID aoCRPGuid) = 0;
		//��ȡ�������ӿ�
		virtual HRESULT STDMETHODCALLTYPE GetPlugin(REFGUID pluginGuid, IPluginObject ** appPluginObject) = 0;
	};
}

//ȡ�ò���������ӿڣ�ֻ���������������ģ�����
HRESULT GetPluginManager(IPluginManager ** appPluginManager);

#endif // __I_PLUGIN_MANAGER__H_INCLUDE__