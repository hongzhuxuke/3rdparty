#ifndef __VHP_STRU_DEFF__H_INCLUDE__
#define __VHP_STRU_DEFF__H_INCLUDE__

#pragma once
#include "VH_ConstDeff.h"

//CR�����Ϣ
struct STRU_CR_PLUGIN_INFO
{
	GUID		m_oCRPID;												//���ID
	WCHAR		m_wzCRPName[DEF_MAX_CRP_NAME_LEN + 1];					//�������
	WCHAR		m_wzCRPDescrip[DEF_MAX_CRP_DESCRIP_LEN + 1];			//�������
	DWORD		m_dwMajorVer;											//���汾��
	DWORD		m_dwMinorVer;											//�ΰ汾��
	DWORD		m_dwBuildVer;											//����汾
	DWORD		m_dwPatchVer;											//�����汾
	WCHAR		m_wzCRPVerDescrip[DEF_MAX_CRP_VER_DESCRIP_LEN + 1];		//����汾����
	WORD		m_wCRPDependCount;										//��������
	GUID		m_oCRPDepends[DEF_MAX_CRP_DEPEND_NUM];					//�������ID�б�
public:
	DEF_CR_DECLARE_CONSTRUCT(STRU_CR_PLUGIN_INFO);
};

#endif //__VHP_STRU_DEFF__H_INCLUDE__