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
	//插件对象，所有插件必需实现
	class IPluginObject : public VH_IUnknown
	{
	public:
		//创建对象
		virtual HRESULT STDMETHODCALLTYPE Create() = 0;
		//销毁对象
		virtual HRESULT STDMETHODCALLTYPE Destroy() = 0;
		//刷新插件，插件需要重新读取资源（由插件定义更新内容，这里只是一个通知，也可以不作任何操作）
		virtual HRESULT STDMETHODCALLTYPE Update() = 0;
		//取得GUID
		virtual HRESULT STDMETHODCALLTYPE GetPluginGUID(GUID& aoGuid) = 0;
		//取得插件信息
		virtual HRESULT STDMETHODCALLTYPE GetPluginInfo(STRU_CR_PLUGIN_INFO& aoPluginInfo) = 0;
		//通知连接到插件管理器
		virtual HRESULT STDMETHODCALLTYPE ConnectPluginManager(IPluginManager * apCRPluginManager) = 0;
		//断开插件管理器
		virtual HRESULT STDMETHODCALLTYPE DisconnectPluginManager() = 0;
		//设置兼容查询接口，这个接口在插件系统完整后删除
	};

	//插件管理器事件
	class IPluginManagerEvent : public VH_IUnknown
	{
	public:
		//通知某一个插件被加载到管理器中
		virtual HRESULT STDMETHODCALLTYPE OnPluginLoaded(IPluginObject * apPluginObject) = 0;
		//通知某一个插件从管理器卸载之前
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoadBefore(IPluginObject * apPluginObject) = 0;
		//通知某一个插件已从管理器卸载
		virtual HRESULT STDMETHODCALLTYPE OnPluginUnLoaded(REFGUID aoCRPGuid) = 0;
		//是否可以加载某一个插件
		virtual HRESULT STDMETHODCALLTYPE IsCanLoadPlugin(REFGUID aoCRPGuid) = 0;
		//是否可以卸载
		virtual HRESULT STDMETHODCALLTYPE IsCanUnLoadPlugin(IPluginObject * apPluginObject) = 0;
	};

	//插件管理器
	class IPluginManager : public VH_IUnknown
	{
	public:
		//注册管理器事件
		virtual HRESULT STDMETHODCALLTYPE RegisterEvent(IPluginManagerEvent * apPluginManagerEvent) = 0;
		//取消注册管理器事件
		virtual HRESULT STDMETHODCALLTYPE UnRegisterEvent(IPluginManagerEvent * apPluginManagerEvent) = 0;
		//加载一个插件到管理器，如果 appPluginObject 不为 NULL，则返回插件对象接口
		virtual HRESULT STDMETHODCALLTYPE LoadPlugin(const WCHAR * apwzFileName, IPluginObject ** appPluginObject) = 0;
		//卸载一个插件从管理器
		virtual HRESULT STDMETHODCALLTYPE UnLoadPlugin(REFGUID aoCRPGuid) = 0;
		//是否可以加载某一个插件
		virtual HRESULT STDMETHODCALLTYPE IsCanLoadPlugin(REFGUID aoCRPGuid) = 0;
		//询问是否可以卸载插件
		virtual HRESULT STDMETHODCALLTYPE IsCanUnLoadPlugin(REFGUID aoCRPGuid) = 0;
		//获取插件对象接口
		virtual HRESULT STDMETHODCALLTYPE GetPlugin(REFGUID pluginGuid, IPluginObject ** appPluginObject) = 0;
	};
}

//取得插件管理器接口，只允许管理器的宿主模块调用
HRESULT GetPluginManager(IPluginManager ** appPluginManager);

#endif // __I_PLUGIN_MANAGER__H_INCLUDE__