// ShowResultDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "ShowResultDlg.h"
#include "../Include/BasicDef.h"
#include "../IdentifyOutlineLib/LibExport.h"
#include <vector>
using namespace std;


// CShowResultDlg 对话框

IMPLEMENT_DYNAMIC(CShowResultDlg, CDialog)

CShowResultDlg::CShowResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowResultDlg::IDD, pParent)
{
	m_pShareMemData = NULL;
	m_hMapping = NULL;
}

CShowResultDlg::~CShowResultDlg()
{
	if (m_hMapping != NULL)
	{
		CloseHandle(m_hMapping);
	}
	if (m_pShareMemData != NULL)
	{
		UnmapViewOfFile(m_pShareMemData);
	}
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
	ON_BN_CLICKED(IDC_BUTTON1, &CShowResultDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_EXPORT, &CShowResultDlg::OnBnClickedBtnExport)
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
			item.strText = _T("编号");
		}
		else if (col == 1)
		{
			item.strText = _T("X");
		}
		else if (col == 2)
		{
			item.strText = _T("Y");
		}
		else if (col == 3)
		{
			item.strText=_T("X");
		}
		m_Grid.SetItem(&item);		
	}

	return TRUE;
}

void CShowResultDlg::CreateShareMemory(TCHAR* pName)
{
	if (m_hMapping != NULL)
	{
		CloseHandle(m_hMapping);
	}
	m_hMapping = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,SHAREMEMSIZE,pName);
	if (m_hMapping != NULL)
	{
		m_pShareMemData = (char*)MapViewOfFile(m_hMapping,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
		if(m_pShareMemData == NULL)
		{
			CloseHandle(m_hMapping);
			m_hMapping = NULL;
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
	//分析5张BMP图
	CreateShareMemory(TEXT(MAPPINGFILENAME));//申请共享内存	
	if (m_pShareMemData != NULL)
	{//进行分析操作
		//先初始化
		memset(m_pShareMemData,-1,SHAREMEMSIZE*sizeof(char));

		bool bFailed = false;
		for (int i=0;i<m_AllBMPFile.size();i++)
		{
			CStringA tempFile(m_AllBMPFile[i]);
			bool bTemp = CheckPiexl(tempFile.GetBuffer());//5张检测图片
			tempFile.ReleaseBuffer();
			if (!bTemp)
			{
				bFailed = true;
				break;
			}
		}
		if (bFailed)
		{
			AfxMessageBox(_T("检测图片边界失败!"));
			return;
		}
		vector<Position> AllBadPosition;
		char* pTempData = m_pShareMemData;
		int structPosSize = sizeof(Position);
		int count = 0;
		while((*pTempData)!=-1)
		{
			count++;
			pTempData++;
		}
		memcpy(&AllBadPosition,m_pShareMemData,count);//将所有数据拷贝到vector中
		for (size_t i=0;i<AllBadPosition.size();i++)
		{
			Position tempP = AllBadPosition[i];
			int x = tempP.Xposition;
			int y = tempP.Yposition;
			for (int col=0;col<m_Grid.GetColumnCount();col++)
			{
				GV_ITEM item;
				item.nFormat =  DT_CENTER|DT_VCENTER
					|DT_SINGLELINE|DT_END_ELLIPSIS
					|DT_NOPREFIX;
				item.row = i+1;
				item.col = col;
				if (col == 0)
				{
					item.strText.Format(_T("%d"),i+1);
				}
				else if (col == 1)
				{
					item.strText.Format(_T("%d"),x);
				}
				else if (col == 2)
				{
					item.strText.Format(_T("%d"),y);
				}				
				m_Grid.SetItem(&item);
			}			
		}
	}
}


void CShowResultDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	AnalyseBMP();
}

void CShowResultDlg::OnBnClickedBtnExport()
{
	// TODO: 导出结果为CSV
	CFileDialog dlg(FALSE,_T("CSV"),_T("SavePic.csv"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	if (IDOK == dlg.DoModal())
	{
		CStdioFile file;
		CString savefile = dlg.GetPathName();
		file.Open(savefile,CFile::modeWrite);
		for (int row=1;row<m_Grid.GetRowCount();row++)
		{
			Position tempp;
			for (int col=1;col<m_Grid.GetColumnCount();col++)
			{
				GV_ITEM item;
				item.col = col;
				item.row = row;
				m_Grid.GetItem(&item);
				if (col == 1)
				{
					tempp.Xposition = _ttoi(item.strText);
				}
				else if (col == 2)
				{
					tempp.Yposition = _ttoi(item.strText);
				}
			}
			//写文件			
			CString strtemp;
			strtemp.Format(_T("%d,%d,%d\n"),row,tempp.Xposition,tempp.Yposition);
			file.WriteString(strtemp);		
		}
		file.Flush();
		file.Close();
	}
}
