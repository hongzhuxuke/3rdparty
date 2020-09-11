#ifndef __VH_UNKNWN__H_INCLUDED__
#define __VH_UNKNWN__H_INCLUDED__

#pragma once

#define INITGUID

#include <windows.h>
#include <Guiddef.h>
#include <initguid.h>

#include "VH_Error.h"
#include "VHComPtr.h"

// {00000000-0000-0000-0000-000000000000}
DEFINE_GUID(IID_VHInvalid,
0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

// {00000000-0000-0000-4700-545400000054}
DEFINE_GUID(IID_VHIUnknown,
0x00000000, 0x0000, 0x0000, 0x47, 0x00, 0x54, 0x54, 0x00, 0x00, 0x00, 0x54);

// {00000001-0000-0000-4700-545400000054}
DEFINE_GUID(IID_VHIClassFactory,
0x00000001, 0x0000, 0x0000, 0x47, 0x00, 0x54, 0x54, 0x00, 0x00, 0x00, 0x54);

extern "C++"
{
	class VH_IUnknown
    {
	public:
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** appvObject) = 0;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
		virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
	};

	class CR_IClassFactory : public VH_IUnknown
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE CreateInstance(VH_IUnknown * apUnkOuter, REFIID riid, void ** appvObject) = 0;
		virtual HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) = 0;
	};
};

#endif //__VH_UNKNWN__H_INCLUDED__