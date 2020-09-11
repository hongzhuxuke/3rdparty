#ifndef __I_MESSAGEDISPATCHER__H_INCLUDED__
#define __I_MESSAGEDISPATCHER__H_INCLUDED__

#pragma once

#include "VH_IUnknown.h"

// {0D4D68F2-43EE-4633-8788-0E7A9DFAD6B3}
DEFINE_GUID(IID_IMessageEvent, 
0xd4d68f2, 0x43ee, 0x4633, 0x87, 0x88, 0xe, 0x7a, 0x9d, 0xfa, 0xd6, 0xb3);

// {EC662E37-33E1-4C91-B45C-1973E210A121}
DEFINE_GUID(IID_IMessageDispatcherEvent, 
0xec662e37, 0x33e1, 0x4c91, 0xb4, 0x5c, 0x19, 0x73, 0xe2, 0x10, 0xa1, 0x21);

// {A09EAB9C-EAD4-4F4A-A4F3-DF8AA342D578}
DEFINE_GUID(IID_IMessageDispatcher, 
0xa09eab9c, 0xead4, 0x4f4a, 0xa4, 0xf3, 0xdf, 0x8a, 0xa3, 0x42, 0xd5, 0x78);

extern "C++"
{
	class IMessageDispatcher;

	//��Ϣ�¼����ɸ����ʵ��
	class IMessageEvent : public VH_IUnknown
	{
	public:
		//�� IMessageDispatcher ������
		virtual HRESULT STDMETHODCALLTYPE Connect(IMessageDispatcher * apMessageDispatcher) = 0;
		//�Ͽ��� IMessageDispatcher ������
		virtual HRESULT STDMETHODCALLTYPE Disconnect() = 0;
		//��������
		virtual HRESULT STDMETHODCALLTYPE OnRecvMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen) = 0;
	};

	////�ַ����¼�
	//class IMessageDispatcherEvent : public VH_IUnknown
	//{
	//public:
	//	//֪ͨ���¼�ע��
	//	virtual HRESULT STDMETHODCALLTYPE OnRegisterMessage(DWORD adwMessageID, DWORD adwFlag, IMessageEvent * apMessageEvent) = 0;
	//	//֪ͨ���¼�ȡ��ע��
	//	virtual HRESULT STDMETHODCALLTYPE OnUnRegisterMessage(DWORD adwMessageID, IMessageEvent * apMessageEvent) = 0;
	//	//��Ϣ����ַ�������֮ǰ��������� CRE_FALSE �򲻼������
	//	virtual HRESULT STDMETHODCALLTYPE OnMessagePreEnqueue(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen) = 0;
	//};

	//��Ϣ�ַ����ӿ�
	class IMessageDispatcher : public VH_IUnknown
	{
	public:
		////���÷ַ����¼�
		//virtual HRESULT STDMETHODCALLTYPE SetMessageDispatcherEvent(IMessageDispatcherEvent * apMessageDispatcherEvent) = 0;
		////ȡ�÷ַ����¼�
		//virtual HRESULT STDMETHODCALLTYPE GetMessageDispatcherEvent(IMessageDispatcherEvent ** appMessageDispatcherEvent) = 0;
		//����Ϣ�ɷ���ע����Ϣ
		virtual HRESULT STDMETHODCALLTYPE RegisterMessage(DWORD adwMessageID, DWORD adwFlag, IMessageEvent * apMessageEvent) = 0;
		//����Ϣ�ɷ������ע��
		virtual HRESULT STDMETHODCALLTYPE UnRegisterMessage(DWORD adwMessageID, IMessageEvent * apMessageEvent) = 0;
		//����Ϣ�ɷ���Ͷ����Ϣ����Ϣ������У�������������
		virtual HRESULT STDMETHODCALLTYPE PostCRMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen) = 0;
	};
}

//��ȡ ��Ϣ�ɷ��� �ӿ�ָ��
BOOL GetMessageDispatcher(IMessageDispatcher ** appMessageDispatcher);

//�ر� ��Ϣ�ɷ���
void CloseMessageDispatcher();

#endif //__I_MESSAGEDISPATCHER__H_INCLUDED__
