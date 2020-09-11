
// TestDeckLinkDeviceDlg.h : ͷ�ļ�
//

#pragma once

class IDeckLinkDevice;
// CTestDeckLinkDeviceDlg �Ի���
class CTestDeckLinkDeviceDlg : public CDialogEx {
   // ����
public:
   CTestDeckLinkDeviceDlg(CWnd* pParent = NULL);	// ��׼���캯��

   // �Ի�������
   enum { IDD = IDD_TESTDECKLINKDEVICE_DIALOG };

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
   afx_msg void OnBnClickedCaptureBtn();
private:
   IDeckLinkDevice* mDeckLinkDevice;

   void GetDataLoop();
   static DWORD __stdcall GetDataThread(LPVOID lpUnused);
private:
   HANDLE mCaptureThread;
   bool mStart;
};
