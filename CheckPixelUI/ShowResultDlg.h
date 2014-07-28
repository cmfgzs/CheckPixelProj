#pragma once
#include "../GridCtrl/GridCtrl_src/GridCtrl.h"
#include "PictureCtrl.h"
#include <vector>
#include "afxwin.h"
using namespace std;

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
private:
	vector<CString> m_AllBMPFile;
	void InitBMPFiles();
	void AnalyseBMP();//����5��ͼƬ
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
