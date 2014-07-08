
// CheckPixelUIDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "CaptureVideo.h"


// CCheckPixelUIDlg �Ի���
class CCheckPixelUIDlg : public CDialog
{
// ����
public:
	CCheckPixelUIDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHECKPIXELUI_DIALOG };

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
	afx_msg void OnBnClickedBtnCapImg();
	afx_msg void OnBnClickedBtnFreeze();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_StaticVideo;
	CCaptureVideo m_cap;
	BOOL m_bFreeze;
	
	
	CComboBox m_DevicesList;
	CString m_strSaveDir;
	afx_msg void OnBnClickedBtnSavePath();
};
