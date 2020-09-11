
// testdshow.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"  
#include "Logging.h"
#include "testdshow.h"
#include "testdshowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
Logger *gLogger = NULL;

// CtestdshowApp

BEGIN_MESSAGE_MAP(CtestdshowApp, CWinApp)
   ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



// CtestdshowApp ����

CtestdshowApp::CtestdshowApp() {
   // ֧����������������
   m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

   // TODO:  �ڴ˴���ӹ�����룬
   // ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CtestdshowApp ����

CtestdshowApp theApp;
void dslog(DShow::LogType type, const wchar_t *msg,
           void *param) {
   gLogger->logInfo(msg);
}


// CtestdshowApp ��ʼ��

BOOL CtestdshowApp::InitInstance() {
   // ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
   // ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
   //����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
   INITCOMMONCONTROLSEX InitCtrls;
   InitCtrls.dwSize = sizeof(InitCtrls);
   // ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
   // �����ؼ��ࡣ
   InitCtrls.dwICC = ICC_WIN95_CLASSES;
   InitCommonControlsEx(&InitCtrls);

   CWinApp::InitInstance();
   SetLogCallback(dslog, this);
   gLogger = new Logger(L"test.log", CURRENT);
   CoInitialize(nullptr);
   AfxEnableControlContainer();

   // ���� shell ���������Է��Ի������
   // �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
   CShellManager *pShellManager = new CShellManager;

   // ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
   CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

   // ��׼��ʼ��
   // ���δʹ����Щ���ܲ�ϣ����С
   // ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
   // ����Ҫ���ض���ʼ������
   // �������ڴ洢���õ�ע�����
   // TODO:  Ӧ�ʵ��޸ĸ��ַ�����
   // �����޸�Ϊ��˾����֯��
   SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

   CtestdshowDlg dlg;
   m_pMainWnd = &dlg;
   INT_PTR nResponse = dlg.DoModal();
   if (nResponse == IDOK) {
      // TODO:  �ڴ˷��ô����ʱ��
      //  ��ȷ�������رնԻ���Ĵ���
   } else if (nResponse == IDCANCEL) {
      // TODO:  �ڴ˷��ô����ʱ��
      //  ��ȡ�������رնԻ���Ĵ���
   } else if (nResponse == -1) {
      TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
      TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
   }

   // ɾ�����洴���� shell ��������
   if (pShellManager != NULL) {
      delete pShellManager;
   }

   // ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
   //  ����������Ӧ�ó������Ϣ�á�
   CoUninitialize();
   return FALSE;
}

