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
	DDX_Control(pDX, IDC_STATIC_BLACK_BMP, m_BlackBMP);
	DDX_Control(pDX, IDC_STATIC_WHITE_BMP, m_WhiteBMP);
	DDX_Control(pDX, IDC_STATIC_RED_BMP, m_RedBMP);
	DDX_Control(pDX, IDC_STATIC_GREEN_BMP, m_GreenBMP);
	DDX_Control(pDX, IDC_STATIC_BLUE_BMP, m_BlueBMP);
}


BEGIN_MESSAGE_MAP(CShowResultDlg, CDialog)
END_MESSAGE_MAP()


// CShowResultDlg 消息处理程序

BOOL CShowResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitBMPFiles();
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

void CShowResultDlg::CreateShareMemory(TCHAR* pName)
{
	HANDLE hMapping;
	long memsize = 10*1024*1024;//开10M空间
	hMapping = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,memsize,pName);
	if (hMapping != NULL)
	{
		char* pbFile = (char*)MapViewOfFile(hMapping,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
		if(pbFile == NULL)
		{
			CloseHandle(hMapping);
			hMapping = NULL;
			return;
		}
	}
}

void CShowResultDlg::SetAllTypeBMPFile( vector<CString>& names )
{
	m_AllBMPFile.clear();
	for (size_t i=0;i<names.size();i++)
	{
		m_AllBMPFile.push_back(names[i]);
	}
}

void CShowResultDlg::InitBMPFiles()
{
	if (m_AllBMPFile.size()<5)
	{
		return;
	}
	m_BlackBMP.Load(m_AllBMPFile[0]);
	m_WhiteBMP.Load(m_AllBMPFile[1]);
	m_RedBMP.Load(m_AllBMPFile[2]);
	m_GreenBMP.Load(m_AllBMPFile[3]);
	m_BlueBMP.Load(m_AllBMPFile[4]);

}

void CShowResultDlg::AnalyseBMP()
{

}
