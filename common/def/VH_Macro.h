#ifndef __VH_MACRO__H_INCLUDE__
#define __VH_MACRO__H_INCLUDE__

#pragma once

#if defined(WINNT) || !defined(WIN32)
	typedef long			HRESULT;
	typedef unsigned long	ULONG;
#endif

#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif

#ifndef __RPC_FAR
#define __RPC_FAR
#endif

#ifdef __DEFINE_NESTED_MACRO__
	#ifndef offsetof
	#define offsetof(s,m)   (size_t)&(((s *)0)->m)
	#endif
#endif
#ifdef _WIN32
#include <combaseapi.h>
#else
//*************************************************************************************************************
#ifndef STDMETHOD
#define STDMETHOD(method)			virtual HRESULT STDMETHODCALLTYPE method
#endif

#ifndef STDMETHOD_
#define STDMETHOD_(type, method)	virtual type STDMETHODCALLTYPE method
#endif

#ifndef STDMETHODV
#define STDMETHODV(method)			virtual HRESULT STDMETHODVCALLTYPE method
#endif

#ifndef STDMETHODV_
#define STDMETHODV_(type, method)	virtual type STDMETHODVCALLTYPE method
#endif
#endif

//*************************************************************************************************************
#ifndef BEGIN_ONLY_INTERFACE_PART
#define BEGIN_ONLY_INTERFACE_PART(localClass, baseClass) \
	class X##localClass : public baseClass \
	{ \
	public:
#endif

#ifndef BEGIN_INTERFACE_PART

	#define BEGIN_INTERFACE_PART(localClass, baseClass) \
		class X##localClass : public baseClass \
		{ \
		public: \
			STDMETHOD_(ULONG, AddRef)(); \
			STDMETHOD_(ULONG, Release)(); \
			STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject); \

	#define INIT_INTERFACE_PART_DERIVE(theClass, localClass) \
			X##localClass() \
				{ m_nOffset = offsetof(theClass, m_x##localClass); } \

	#define INIT_INTERFACE_PART(theClass, localClass) \
			size_t m_nOffset; \
			INIT_INTERFACE_PART_DERIVE(theClass, localClass) \

	#define INIT_INTERFACE_PART_DERIVE_INIT(theClass, localClass) \
			X##localClass() \
				{ m_nOffset = offsetof(theClass, m_x##localClass);Initialize();} \
			void Initialize(); \

	#define INIT_INTERFACE_PART_INIT(theClass, localClass) \
			size_t m_nOffset; \
			INIT_INTERFACE_PART_DERIVE_INIT(theClass, localClass) \

	#define END_INTERFACE_PART(localClass) \
		} m_x##localClass; \
		friend class X##localClass; \

	#define METHOD_PROLOGUE_(theClass, localClass) \
		theClass* pThis = \
			((theClass*)((BYTE*)this - offsetof(theClass, m_x##localClass)));
#endif

//*************************************************************************************************************
#ifndef SafeRelease
#define SafeRelease(x) \
	if((x)) \
	{ \
		(x)->Release(); \
		(x) = NULL; \
	}
#endif

//*************************************************************************************************************
#ifdef UNICODE
	#define QStringToTCHAR(x)		(wchar_t*) x.utf16()
	#define PQStringToTCHAR(x)		(wchar_t*) x->utf16()
	#define TCHARToQString(x)		QString::fromUtf16((x))
	#define TCHARToQStringN(x,y)	QString::fromUtf16((x),(y))
#else
	#define QStringToTCHAR(x)		x.local8Bit().constData()
	#define PQStringToTCHAR(x)		x->local8Bit().constData()
	#define TCHARToQString(x)		QString::fromLocal8Bit((x))
	#define TCHARToQStringN(x,y)	QString::fromLocal8Bit((x),(y))
#endif

//*************************************************************************************************************
//集合中是否有某个掩码，注意，这里考虑了集合为0时表示所有的情况
#ifndef DEF_IS_HAS_MASK
#define DEF_IS_HAS_MASK(__MASK_CAP__, __MASK__) \
	((0 == (__MASK_CAP__)) || ((__MASK_CAP__) & (__MASK__)))
#endif

//*************************************************************************************************************
//直接定义的缺省构造函数
#ifndef DEF_CR_DECLARE_CONSTRUCT
#define DEF_CR_DECLARE_CONSTRUCT(__STRUCT_NAME__) \
	__STRUCT_NAME__() \
	{ \
		memset(this, 0, sizeof(__STRUCT_NAME__)); \
	}
#endif

//*************************************************************************************************************
//-------------------------------------------------------------------------------------------------------------
//CR消息参数检查，对象定义宏
#ifndef DEF_CR_MESSAGE_DATA_DECLARE
#define DEF_CR_MESSAGE_DATA_DECLARE(StructName, ObjectName, ReturnValue) \
	if(NULL == apData || adwLen < sizeof(StructName)) \
	{ \
		return (ReturnValue); \
	} \
	StructName& ObjectName = *(StructName*)apData
#endif
//-------------------------------------------------------------------------------------------------------------
//CR消息参数检查，对象定义，自定义动作宏
#ifndef DEF_CR_MESSAGE_DATA_DECLARE_CA
#define DEF_CR_MESSAGE_DATA_DECLARE_CA(StructName, ObjectName, CustomAction) \
	if(NULL == apData || adwLen < sizeof(StructName)) \
	{ \
		CustomAction; \
	} \
	StructName& ObjectName = *(StructName*)apData
#endif
//-------------------------------------------------------------------------------------------------------------
//CR消息参数检查，对象解包宏
#ifndef DEF_CR_MESSAGE_DATA_UNPACK
#define DEF_CR_MESSAGE_DATA_UNPACK(StructName, ObjectName, ReturnValue) \
	if(NULL == apData || adwLen < sizeof(StructName)) \
	{ \
		return (ReturnValue); \
	} \
	StructName ObjectName; \
	if(FALSE == ObjectName.UnPack((char*)apData, (int)adwLen)) \
	{ \
		return (ReturnValue); \
	}
#endif
//-------------------------------------------------------------------------------------------------------------
//CR消息参数检查，对象解包自定义动作宏
#ifndef DEF_CR_MESSAGE_DATA_UNPACK_CA
#define DEF_CR_MESSAGE_DATA_UNPACK_CA(StructName, ObjectName, CustomAction) \
	if(NULL == apData || adwLen < sizeof(StructName)) \
	{ \
		CustomAction; \
	} \
	StructName ObjectName; \
	if(FALSE == ObjectName.UnPack((char*)apData, (int)adwLen)) \
	{ \
		CustomAction; \
	}
#endif
//-------------------------------------------------------------------------------------------------------------
//*************************************************************************************************************

//主路径,路径转换使用
#ifndef DEF_MAIN_PATH_KEY_DEF
	#define DEF_MAIN_PATH_KEY_DEF
	#define DEF_MAIN_PATH_KEY				"$MAIN_PATH"
	#define DEF_MAIN_PATH_KEY_U				L"$MAIN_PATH"
	#ifdef UNICODE
		#define DEF_MAIN_PATH_KEY_T			DEF_MAIN_PATH_KEY_U
	#else
		#define DEF_MAIN_PATH_KEY_T			DEF_MAIN_PATH_KEY
	#endif
#endif
//-------------------------------------------------------------------------------------------------------------
//获取智能指针
#ifndef DEF_GET_INTERFACE_PTR
#define DEF_GET_INTERFACE_PTR(plug, pid, iid, pComPtr, reVal, waring) \
	plug::Instance().GetCRPluginInterface(pid, iid, pComPtr);\
	if(NULL == pComPtr)\
	{\
	waring;\
	reVal;\
	}
#pragma warning(disable:4003)
#endif

#endif //__VH_MACRO__H_INCLUDE__