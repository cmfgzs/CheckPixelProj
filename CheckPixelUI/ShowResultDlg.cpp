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

BOOL CShowResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_Grid.SetEditable(FALSE);
	m_Grid.SetTextBkColor(RGB(0xFF,0xff,0xe0));
	m_Grid.SetColumnCount(4);
	m_Grid.SetFixedColumnCount(1);
	m_Grid.SetFixedRowCount(1);
	for (int col=0;col<m_Grid.GetColumnCount();col++)
	{
		GV_ITEM item;
		item.mask = GVIF_TEXT|GVIF_FORMAT;
		item.row = 0;
		item.col = col;
		m_Grid.SetColumnWidth(col,64);
		item.nFormat = DT_CENTER|DT_WORDBREAK;
		if (col==0)
		{
			
			item.strText.Format(_T("���"),col);
		}
		else if (col == 1)
		{
			item.strText.Format(_T("X"),col);
		}
		else if (col == 2)
		{
			item.strText.Format(_T("Y"),col);
		}
		else if (col == 3)
		{
			item.strText.Format(_T("X"),col);
		}
		m_Grid.SetItem(&item);		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
