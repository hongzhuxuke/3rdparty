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

	//UI线程工作对象
	class IUIThreadObject : public VH_IUnknown
	{
	public:
		//线程管理器主动调用，通知连接到线程管理器
		virtual HRESULT STDMETHODCALLTYPE Connect(IUIThreadManager * apCRUIThreadManager) = 0;
		//线程管理器主动调用，通知断开线程管理器，对象保存的管理器指针不再保证有效
		virtual HRESULT STDMETHODCALLTYPE Disconnect() = 0;
		//初始化，UI线程工作对象调用Begin时，由UI线程管理器与期望的UI线程交涉，由期望UI线程回调，通常纯逻辑模块不需要使用这个功能
		virtual HRESULT STDMETHODCALLTYPE Init(DWORD dwThreadID) = 0;
		//反初始化，UI线程工作对象调用End时，由UI线程管理器与期望的UI线程交涉，由期望UI线程回调，通常纯逻辑模块不需要使用这个功能
		virtual HRESULT STDMETHODCALLTYPE UnInit() = 0;
		//通知处理消息派发器推送的缓存消息，这个函数由Begin时指定的UI线程定时调用，通常所有需要操作UI界面的插件模块的消息处理，都要在这个函数调用中
		virtual HRESULT STDMETHODCALLTYPE DealCachedMessage(DWORD dwThreadID) = 0;
	};

	//UI线程消息事件
	class IUIThreadMessageEvent : public VH_IUnknown
	{
	public:
		//通知注册的UI线程工作对象，你所期望的UI线程有消息预处理
		virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage(DWORD dwThreadID, HANDLE ahWnd, DWORD dwMessage, DWORD dwWParam, DWORD dwLParam) = 0;
	};

	// UI预处理事件
	class IUIPreMessageEvent : public VH_IUnknown
	{
	public:
		//通知注册的UI线程工作对象，你所期望的UI线程有消息预处理
		virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage(MSG* apMsg) = 0;
	};

	//UI线程管理器
	class IUIThreadManager : public VH_IUnknown
	{
	public:
		//请求指定的UI线程启动工作，返回目标线程ID,0表示失败
		virtual DWORD STDMETHODCALLTYPE Begin(DWORD dwThreadIndex, IUIThreadObject * apCRUIThreadObject) = 0;
		//请求指定的UI线程结束工作
		virtual HRESULT STDMETHODCALLTYPE End(DWORD dwThreadIndex, IUIThreadObject * apCRUIThreadObject) = 0;
		//向指定的UI线程注册UI消息回调
		virtual HRESULT STDMETHODCALLTYPE RegisterUIMessageEvent(DWORD dwThreadIndex, IUIThreadMessageEvent * apEvent) = 0;
		//向指定的UI线程取消注册UI消息回调
		virtual HRESULT STDMETHODCALLTYPE UnRegisterUIMessageEvent(DWORD dwThreadIndex, IUIThreadMessageEvent * pEvent) = 0;

		// 向指定的UI线程注册UI预处理消息回调
		virtual HRESULT STDMETHODCALLTYPE RegisterUIPreMessageEvent(IUIPreMessageEvent * apEvent) = 0;
		// 向指定的UI线程取消UI预处理消息回调
		virtual HRESULT STDMETHODCALLTYPE UnRegisterUIPreMessageEvent(IUIPreMessageEvent * apEvent) = 0;
	};
}

#endif // __UI_THREAD_MANAGER__H_INCLUDE__