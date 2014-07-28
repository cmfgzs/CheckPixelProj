#pragma once
#include "../GridCtrl/GridCtrl_src/GridCtrl.h"
#include "PictureCtrl.h"
#include <vector>
#include "afxwin.h"
using namespace std;

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
private:
	vector<CString> m_AllBMPFile;
	void InitBMPFiles();
	void AnalyseBMP();//分析5张图片
public:
	CGridCtrl m_Grid;
	virtual BOOL OnInitDialog();
	void SetAllTypeBMPFile(vector<CString>& names);
	void CreateShareMemory(TCHAR* pName);
	CPictureCtrl m_BlackBMP;
	CPictureCtrl m_WhiteBMP;
	CPictureCtrl m_RedBMP;
	CPictureCtrl m_GreenBMP;
	CPictureCtrl m_BlueBMP;
};
