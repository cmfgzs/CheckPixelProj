#pragma once
#include "../GridCtrl/GridCtrl_src/GridCtrl.h"

// CShowResultDlg 对话框

class CShowResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowResultDlg)

public:
	CShowResultDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowResultDlg();

// 对话框数据
	enum { IDD = IDD_DLG_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl m_Grid;
	virtual BOOL OnInitDialog();
};
