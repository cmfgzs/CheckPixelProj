#pragma once
#include "../GridCtrl/GridCtrl_src/GridCtrl.h"

// CShowResultDlg �Ի���

class CShowResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowResultDlg)

public:
	CShowResultDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowResultDlg();

// �Ի�������
	enum { IDD = IDD_DLG_RESULT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CGridCtrl m_Grid;
	virtual BOOL OnInitDialog();
};
