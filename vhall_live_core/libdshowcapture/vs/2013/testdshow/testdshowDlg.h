
// testdshowDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "dshowcapture.hpp"
using namespace  DShow;


// CtestdshowDlg �Ի���
class CtestdshowDlg : public CDialogEx {
   // ����
public:
   CtestdshowDlg(CWnd* pParent = NULL);	// ��׼���캯��

   // �Ի�������
   enum { IDD = IDD_TESTDSHOW_DIALOG };

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
   afx_msg void OnBnClickedCapture();
   CComboBox mAudioDeviceList;
   std::vector<AudioDevice> mAudioDevices;
private:
   Device* mAudioDevice;
public:
   afx_msg void OnBnClickedCancel();
};
