
// TestDeckLinkDeviceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestDeckLinkDevice.h"
#include "TestDeckLinkDeviceDlg.h"
#include "afxdialogex.h"

#include "IDeckLinkDevice.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx {
public:
   CAboutDlg();

   // �Ի�������
   enum { IDD = IDD_ABOUTBOX };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

   // ʵ��
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
   CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestDeckLinkDeviceDlg �Ի���



CTestDeckLinkDeviceDlg::CTestDeckLinkDeviceDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CTestDeckLinkDeviceDlg::IDD, pParent) {
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   mDeckLinkDevice = NULL;
}

void CTestDeckLinkDeviceDlg::DoDataExchange(CDataExchange* pDX) {
   CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDeckLinkDeviceDlg, CDialogEx)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(ID_CAPTURE_BTN, &CTestDeckLinkDeviceDlg::OnBnClickedCaptureBtn)
END_MESSAGE_MAP()


// CTestDeckLinkDeviceDlg ��Ϣ�������

BOOL CTestDeckLinkDeviceDlg::OnInitDialog() {
   CDialogEx::OnInitDialog();

   // ��������...���˵�����ӵ�ϵͳ�˵��С�

   // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL) {
      BOOL bNameValid;
      CString strAboutMenu;
      bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
      ASSERT(bNameValid);
      if (!strAboutMenu.IsEmpty()) {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   // ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
   //  ִ�д˲���
   SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
   SetIcon(m_hIcon, FALSE);		// ����Сͼ��

   // TODO:  �ڴ���Ӷ���ĳ�ʼ������


   InitDeckLinkDeviceManager();

   return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTestDeckLinkDeviceDlg::OnSysCommand(UINT nID, LPARAM lParam) {
   if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   } else {
      CDialogEx::OnSysCommand(nID, lParam);
   }
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestDeckLinkDeviceDlg::OnPaint() {
   if (IsIconic()) {
      CPaintDC dc(this); // ���ڻ��Ƶ��豸������

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // ʹͼ���ڹ����������о���
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // ����ͼ��
      dc.DrawIcon(x, y, m_hIcon);
   } else {
      CDialogEx::OnPaint();
   }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestDeckLinkDeviceDlg::OnQueryDragIcon() {
   return static_cast<HCURSOR>(m_hIcon);
}



void CTestDeckLinkDeviceDlg::OnBnClickedCaptureBtn() {
   // TODO:  �ڴ���ӿؼ�֪ͨ����������
   if (mDeckLinkDevice == NULL) {
      const wchar_t* deckLinkName = GetDeckLinkDeviceName(0);
      if (deckLinkName)
         mDeckLinkDevice = GetDeckLinkDevice(deckLinkName);
      if (mDeckLinkDevice) {
         mDeckLinkDevice->EnableAudio(true);
         mDeckLinkDevice->EnableVideo(true,NULL);
         mDeckLinkDevice->StartCapture();
         mStart = true;
         mCaptureThread = CreateThread(NULL, 0, CTestDeckLinkDeviceDlg::GetDataThread, this, 0, NULL);
         if (!mCaptureThread) {
            DWORD error = GetLastError();
            /* gLogger->logError("ProcessMonitor::Start: CreateThread failed, error = 0x%08x.",
            error);*/
            return;
         }


      }
   } else {
      mStart = false;
      mDeckLinkDevice->StopCapture();
      mDeckLinkDevice = NULL;
   }

}


void CTestDeckLinkDeviceDlg::GetDataLoop() {
   FILE* pcmFile = NULL;
   fopen_s(&pcmFile, "d:\\decklink.pcm", "wb");
   FILE* yuvFile = NULL;
   fopen_s(&yuvFile, "d:\\decklink.yuv", "wb");
   bool ret = false;
   unsigned long long audioTime = 0;
   unsigned long long videoTime = 0;
   void *buffer = NULL;
   unsigned int numFrames;   
   unsigned int channels;
   unsigned int samplesPerSec;
   unsigned int bitsPerSample;
   unsigned long videoWidth;
   unsigned long videoHeight;
    long long frameDur;
    long long framTimescale;
   mDeckLinkDevice->GetAudioParam(channels, samplesPerSec, bitsPerSample);
   mDeckLinkDevice->GetVideoParam(videoWidth, videoHeight, frameDur, framTimescale);

   while (mStart == true) {
      if (audioTime <= videoTime) {
         ret = mDeckLinkDevice->GetNextAudioBuffer(&buffer, &numFrames, &audioTime);
         if (ret) {
            fwrite(buffer, 1, numFrames* bitsPerSample / 8 * channels, pcmFile);
            fflush(pcmFile);
         }
      } else {
         unsigned long long bufferSize;
         ret = mDeckLinkDevice->GetNextVideoBuffer(&buffer, &bufferSize, &videoTime);
         if (ret) {
            fwrite(buffer, 1, bufferSize, yuvFile);
            fflush(yuvFile);
         }
      }
   }
   fclose(pcmFile);
   fclose(yuvFile);
}
DWORD __stdcall CTestDeckLinkDeviceDlg::GetDataThread(LPVOID lpUnused) {
   CTestDeckLinkDeviceDlg* thisObj = (CTestDeckLinkDeviceDlg*)lpUnused;
   CoInitialize(0);
   thisObj->GetDataLoop();
   CoUninitialize();
   return 0;

}