
// CheckPixelUIDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "CaptureVideo.h"


// CCheckPixelUIDlg 对话框
class CCheckPixelUIDlg : public CDialog
{
// 构造
public:
	CCheckPixelUIDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CHECKPIXELUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
