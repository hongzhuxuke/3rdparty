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

	//消息事件，由各插件实现
	class IMessageEvent : public VH_IUnknown
	{
	public:
		//与 IMessageDispatcher 的连接
		virtual HRESULT STDMETHODCALLTYPE Connect(IMessageDispatcher * apMessageDispatcher) = 0;
		//断开与 IMessageDispatcher 的连接
		virtual HRESULT STDMETHODCALLTYPE Disconnect() = 0;
		//接收数据
		virtual HRESULT STDMETHODCALLTYPE OnRecvMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen) = 0;
	};

	////分发器事件
	//class IMessageDispatcherEvent : public VH_IUnknown
	//{
	//public:
	//	//通知有事件注册
	//	virtual HRESULT STDMETHODCALLTYPE OnRegisterMessage(DWORD adwMessageID, DWORD adwFlag, IMessageEvent * apMessageEvent) = 0;
	//	//通知有事件取消注册
	//	virtual HRESULT STDMETHODCALLTYPE OnUnRegisterMessage(DWORD adwMessageID, IMessageEvent * apMessageEvent) = 0;
	//	//消息进入分发器队列之前。如果返回 CRE_FALSE 则不加入队列
	//	virtual HRESULT STDMETHODCALLTYPE OnMessagePreEnqueue(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen) = 0;
	//};

	//消息分发器接口
	class IMessageDispatcher : public VH_IUnknown
	{
	public:
		////设置分发器事件
		//virtual HRESULT STDMETHODCALLTYPE SetMessageDispatcherEvent(IMessageDispatcherEvent * apMessageDispatcherEvent) = 0;
		////取得分发器事件
		//virtual HRESULT STDMETHODCALLTYPE GetMessageDispatcherEvent(IMessageDispatcherEvent ** appMessageDispatcherEvent) = 0;
		//向消息派发器注册消息
		virtual HRESULT STDMETHODCALLTYPE RegisterMessage(DWORD adwMessageID, DWORD adwFlag, IMessageEvent * apMessageEvent) = 0;
		//向消息派发器解除注册
		virtual HRESULT STDMETHODCALLTYPE UnRegisterMessage(DWORD adwMessageID, IMessageEvent * apMessageEvent) = 0;
		//向消息派发器投递消息，消息进入队列，函数立即返回
		virtual HRESULT STDMETHODCALLTYPE PostCRMessage(DWORD adwSenderID, DWORD adwMessageID, void * apData, DWORD adwLen) = 0;
	};
}

//获取 消息派发器 接口指针
BOOL GetMessageDispatcher(IMessageDispatcher ** appMessageDispatcher);

//关闭 消息派发器
void CloseMessageDispatcher();

#endif //__I_MESSAGEDISPATCHER__H_INCLUDED__
