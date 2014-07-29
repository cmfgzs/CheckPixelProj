// ShowResultDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "ShowResultDlg.h"
#include "../Include/BasicDef.h"
#include <vector>
using namespace std;


// CShowResultDlg �Ի���

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
END_MESSAGE_MAP()


// CShowResultDlg ��Ϣ��������

BOOL CShowResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ����Ӷ���ĳ�ʼ��
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
	//����5��BMPͼ
	CreateShareMemory(TEXT(MAPPINGFILENAME));//���빲���ڴ�	
	if (m_pShareMemData != NULL)
	{//���з�������
		//�ȳ�ʼ��
		memset(m_pShareMemData,-1,SHAREMEMSIZE*sizeof(char));

		/************************************************************************/
		/* ��������                                                                     */
		/************************************************************************/

		vector<Position> AllBadPosition;
		char* pTempData = m_pShareMemData;
		int structPosSize = sizeof(Position);
		int count = 0;
		while((*pTempData)!=-1)
		{
			count++;
			pTempData++;
		}
		memcpy(&AllBadPosition,m_pShareMemData,count);//���������ݿ�����vector��
	}
}


void CShowResultDlg::OnBnClickedButton1()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	AnalyseBMP();
}