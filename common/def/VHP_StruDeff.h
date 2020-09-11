#ifndef __VHP_STRU_DEFF__H_INCLUDE__
#define __VHP_STRU_DEFF__H_INCLUDE__

#pragma once
#include "VH_ConstDeff.h"

//CR插件信息
struct STRU_CR_PLUGIN_INFO
{
	GUID		m_oCRPID;												//插件ID
	WCHAR		m_wzCRPName[DEF_MAX_CRP_NAME_LEN + 1];					//插件名称
	WCHAR		m_wzCRPDescrip[DEF_MAX_CRP_DESCRIP_LEN + 1];			//插件描述
	DWORD		m_dwMajorVer;											//主版本号
	DWORD		m_dwMinorVer;											//次版本号
	DWORD		m_dwBuildVer;											//编译版本
	DWORD		m_dwPatchVer;											//补丁版本
	WCHAR		m_wzCRPVerDescrip[DEF_MAX_CRP_VER_DESCRIP_LEN + 1];		//插件版本描述
	WORD		m_wCRPDependCount;										//依赖数量
	GUID		m_oCRPDepends[DEF_MAX_CRP_DEPEND_NUM];					//依赖插件ID列表
public:
	DEF_CR_DECLARE_CONSTRUCT(STRU_CR_PLUGIN_INFO);
};

#endif //__VHP_STRU_DEFF__H_INCLUDE__