
// TestMediaReaderDlg.h : ͷ�ļ�
//

#pragma once

#include "IMediaReader.h"
#include <vector>

class CAVPlayer;
// CTestMediaReaderDlg �Ի���
class CTestMediaReaderDlg : public CDialogEx
{
// ����
public:
	CTestMediaReaderDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTMEDIAREADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

   

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedOk();
   private:
   IMediaReader* mMediaReader;
   IMediaOutput* mMediaOutput;

   void GetDataLoop();
   static DWORD __stdcall GetDataThread(LPVOID lpUnused);
private:
   HANDLE mCaptureThread;
   bool mStart;
   CAVPlayer*        mAvPlayer;
   std::vector<void*> mMediaList;
public:
   afx_msg void OnBnClickedVideoNext100s();
   afx_msg void OnBnClickedNextMedia();
};
