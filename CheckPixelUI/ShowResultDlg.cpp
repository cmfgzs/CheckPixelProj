// ShowResultDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "ShowResultDlg.h"


// CShowResultDlg 对话框

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


// CShowResultDlg 消息处理程序

BOOL CShowResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
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
			
			item.strText.Format(_T("编号"),col);
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
	// 异常: OCX 属性页应返回 FALSE
}
