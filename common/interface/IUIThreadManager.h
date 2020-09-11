#ifndef __UI_THREAD_MANAGER__H_INCLUDE__
#define __UI_THREAD_MANAGER__H_INCLUDE__

#pragma once

#include "VH_IUnknown.h"

// {8D097EA7-2E5D-4C05-93AC-2D140240AE86}
DEFINE_GUID(IID_IUIThreadObject, 
0x8d097ea7, 0x2e5d, 0x4c05, 0x93, 0xac, 0x2d, 0x14, 0x2, 0x40, 0xae, 0x86);

// {76E93B3A-EFF6-402F-B021-A9C28F43608B}
DEFINE_GUID(IID_IUIThreadMessageEvent, 
0x76e93b3a, 0xeff6, 0x402f, 0xb0, 0x21, 0xa9, 0xc2, 0x8f, 0x43, 0x60, 0x8b);

// {0795F91A-328B-485B-A8CD-AD74589A2E66}
DEFINE_GUID(IID_IUIThreadManager, 
0x795f91a, 0x328b, 0x485b, 0xa8, 0xcd, 0xad, 0x74, 0x58, 0x9a, 0x2e, 0x66);

// {2E1E7E8B-7592-42E9-B967-8E571B544DDF}
DEFINE_GUID(IID_IUIPreMessageEvent, 
	0x2e1e7e8b, 0x7592, 0x42e9, 0xb9, 0x67, 0x8e, 0x57, 0x1b, 0x54, 0x4d, 0xdf);


extern "C++"
{
	class IUIThreadManager;

	//UI�̹߳�������
	class IUIThreadObject : public VH_IUnknown
	{
	public:
		//�̹߳������������ã�֪ͨ���ӵ��̹߳�����
		virtual HRESULT STDMETHODCALLTYPE Connect(IUIThreadManager * apCRUIThreadManager) = 0;
		//�̹߳������������ã�֪ͨ�Ͽ��̹߳����������󱣴�Ĺ�����ָ�벻�ٱ�֤��Ч
		virtual HRESULT STDMETHODCALLTYPE Disconnect() = 0;
		//��ʼ����UI�̹߳����������Beginʱ����UI�̹߳�������������UI�߳̽��棬������UI�̻߳ص���ͨ�����߼�ģ�鲻��Ҫʹ���������
		virtual HRESULT STDMETHODCALLTYPE Init(DWORD dwThreadID) = 0;
		//����ʼ����UI�̹߳����������Endʱ����UI�̹߳�������������UI�߳̽��棬������UI�̻߳ص���ͨ�����߼�ģ�鲻��Ҫʹ���������
		virtual HRESULT STDMETHODCALLTYPE UnInit() = 0;
		//֪ͨ������Ϣ�ɷ������͵Ļ�����Ϣ�����������Beginʱָ����UI�̶߳�ʱ���ã�ͨ��������Ҫ����UI����Ĳ��ģ�����Ϣ������Ҫ���������������
		virtual HRESULT STDMETHODCALLTYPE DealCachedMessage(DWORD dwThreadID) = 0;
	};

	//UI�߳���Ϣ�¼�
	class IUIThreadMessageEvent : public VH_IUnknown
	{
	public:
		//֪ͨע���UI�̹߳�����������������UI�߳�����ϢԤ����
		virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage(DWORD dwThreadID, HANDLE ahWnd, DWORD dwMessage, DWORD dwWParam, DWORD dwLParam) = 0;
	};

	// UIԤ�����¼�
	class IUIPreMessageEvent : public VH_IUnknown
	{
	public:
		//֪ͨע���UI�̹߳�����������������UI�߳�����ϢԤ����
		virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage(MSG* apMsg) = 0;
	};

	//UI�̹߳�����
	class IUIThreadManager : public VH_IUnknown
	{
	public:
		//����ָ����UI�߳���������������Ŀ���߳�ID,0��ʾʧ��
		virtual DWORD STDMETHODCALLTYPE Begin(DWORD dwThreadIndex, IUIThreadObject * apCRUIThreadObject) = 0;
		//����ָ����UI�߳̽�������
		virtual HRESULT STDMETHODCALLTYPE End(DWORD dwThreadIndex, IUIThreadObject * apCRUIThreadObject) = 0;
		//��ָ����UI�߳�ע��UI��Ϣ�ص�
		virtual HRESULT STDMETHODCALLTYPE RegisterUIMessageEvent(DWORD dwThreadIndex, IUIThreadMessageEvent * apEvent) = 0;
		//��ָ����UI�߳�ȡ��ע��UI��Ϣ�ص�
		virtual HRESULT STDMETHODCALLTYPE UnRegisterUIMessageEvent(DWORD dwThreadIndex, IUIThreadMessageEvent * pEvent) = 0;

		// ��ָ����UI�߳�ע��UIԤ������Ϣ�ص�
		virtual HRESULT STDMETHODCALLTYPE RegisterUIPreMessageEvent(IUIPreMessageEvent * apEvent) = 0;
		// ��ָ����UI�߳�ȡ��UIԤ������Ϣ�ص�
		virtual HRESULT STDMETHODCALLTYPE UnRegisterUIPreMessageEvent(IUIPreMessageEvent * apEvent) = 0;
	};
}

#endif // __UI_THREAD_MANAGER__H_INCLUDE__