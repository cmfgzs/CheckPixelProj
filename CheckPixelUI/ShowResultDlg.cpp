// ShowResultDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "ShowResultDlg.h"


// CShowResultDlg �Ի���

IMPLEMENT_DYNAMIC(CShowResultDlg, CDialog)

CShowResultDlg::CShowResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowResultDlg::IDD, pParent)
{

}

CShowResultDlg::~CShowResultDlg()
{
}

void CShowResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CShowResultDlg, CDialog)
END_MESSAGE_MAP()


// CShowResultDlg ��Ϣ�������
