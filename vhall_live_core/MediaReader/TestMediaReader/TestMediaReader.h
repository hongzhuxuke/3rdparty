
// TestMediaReader.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestMediaReaderApp: 
// �йش����ʵ�֣������ TestMediaReader.cpp
//

class CTestMediaReaderApp : public CWinApp
{
public:
	CTestMediaReaderApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestMediaReaderApp theApp;