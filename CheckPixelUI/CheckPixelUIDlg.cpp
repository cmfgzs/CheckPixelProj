
// CheckPixelUIDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "CheckPixelUIDlg.h"
#include "ShowResultDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCheckPixelUIDlg �Ի���




CCheckPixelUIDlg::CCheckPixelUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckPixelUIDlg::IDD, pParent)
	, m_strSaveDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hBitmap = NULL;
	m_lWidth = m_lHeight = 0;
	m_savebmpcolor = color_black;
}

void CCheckPixelUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_StaticVideo);
	DDX_Control(pDX, IDC_COMBO1, m_DevicesList);
	DDX_Text(pDX, IDC_SAVE_PATH, m_strSaveDir);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_staticBMP);
}

BEGIN_MESSAGE_MAP(CCheckPixelUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CAP_IMG, &CCheckPixelUIDlg::OnBnClickedBtnCapImg)
	ON_BN_CLICKED(IDC_BTN_FREEZE, &CCheckPixelUIDlg::OnBnClickedBtnFreeze)
	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, &CCheckPixelUIDlg::OnBnClickedBtnSavePath)
	ON_BN_CLICKED(IDC_BTN_SAVEBMP, &CCheckPixelUIDlg::OnBnClickedBtnSavebmp)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_MESSAGE(WM_CAPTURE_BITMAP,&CCheckPixelUIDlg::OnCapMessage)
END_MESSAGE_MAP()


// CCheckPixelUIDlg ��Ϣ�������

BOOL CCheckPixelUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_bFreeze = FALSE;

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_cap.EnumDevices(m_DevicesList.GetSafeHwnd());//��ȡ���ӵ���Ƶ�豸

	m_DevicesList.SetCurSel(0);
	HWND hWnd = m_StaticVideo.GetSafeHwnd();
	if (hWnd != NULL)
	{
		m_cap.Init(0,hWnd);//��ʼ������ͷ
	}
	m_staticBMP.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	m_cap.m_GrabberCB.SetSafeWnd(this->GetSafeHwnd());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCheckPixelUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCheckPixelUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{	

		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCheckPixelUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCheckPixelUIDlg::OnBnClickedBtnCapImg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	KillTimer(TIMER_ID);
	SetTimer(TIMER_ID,TIMER_DELAY,NULL);
}

void CCheckPixelUIDlg::OnBnClickedBtnFreeze()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bFreeze)
	{
		//capPreview(m_StaticVideo.GetSafeHwnd(),FALSE);
		m_cap.SetMediaPlayOrPause(FALSE);
		m_bFreeze = TRUE;
	}
	else
	{
		m_cap.SetMediaPlayOrPause(TRUE);
		m_bFreeze = FALSE;
	}
}

void CCheckPixelUIDlg::OnBnClickedBtnSavePath()
{
	TCHAR szFolderPath[2048] = {0};
	CString strFolder = TEXT("");
	BROWSEINFO sInfo;
	::ZeroMemory(&sInfo,sizeof(sInfo));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("ѡ�񱣴��ļ���");
	sInfo.ulFlags =  BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn = NULL;
	LPITEMIDLIST lpidBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidBrowse!=NULL)
	{
		if (::SHGetPathFromIDList(lpidBrowse,szFolderPath))    
		{  
			strFolder = szFolderPath;  
		} 
	}
	if (lpidBrowse != NULL)
	{
		::CoTaskMemFree(lpidBrowse);
	}
	m_strSaveDir = strFolder;
	m_cap.SetSaveBMPFileDir(m_strSaveDir);
	
	UpdateData(FALSE);

}

void CCheckPixelUIDlg::OnBnClickedBtnSavebmp()
{
	// TODO: ����ͼƬ
	if (m_strSaveDir.IsEmpty())
	{
		CFileDialog dlg(FALSE,_T("BMP"),_T("SavePic.bmp"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
		if (IDOK == dlg.DoModal())
		{
			m_cap.m_GrabberCB.m_szFileName = dlg.GetPathName();

			BYTE* pImageData = new BYTE[m_cap.m_GrabberCB.m_capbmp.lBufferSize];  
			CopyMemory(pImageData, m_cap.m_GrabberCB.m_capbmp.pBuffer, m_cap.m_GrabberCB.m_capbmp.lBufferSize);
			m_cap.m_GrabberCB.SaveBitmap(pImageData, m_cap.m_GrabberCB.m_capbmp.lBufferSize);
			delete[] pImageData;
		}

	}
	
}

BOOL CCheckPixelUIDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//return CDialog::OnEraseBkgnd(pDC);
	return FALSE;
}

void CCheckPixelUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (TIMER_ID == nIDEvent)
	{
		KillTimer(nIDEvent);
		OnStartCap();
		//SetTimer (nIDEvent,TIMER_DELAY,NULL);
		
	}

	CDialog::OnTimer(nIDEvent);
}

void CCheckPixelUIDlg::OnStartCap()
{
	if (m_strSaveDir.IsEmpty())
	{
		AfxMessageBox(_T("�����ñ���·��"));
		return;
	}
	m_cap.GrabOneFrame(TRUE);//��׽һ��ͼƬ
	return;
}

LRESULT CCheckPixelUIDlg::OnCapMessage( WPARAM wparam,LPARAM lparam )
{
	int wstatus = (int)wparam;
	int lstatus = (int)lparam;
	if (wparam == 0)
	{
		BYTE* pImageData = new BYTE[m_cap.m_GrabberCB.m_capbmp.lBufferSize];  
		CopyMemory(pImageData, m_cap.m_GrabberCB.m_capbmp.pBuffer, m_cap.m_GrabberCB.m_capbmp.lBufferSize);
		if (m_cap.m_GrabberCB.FormatImage(pImageData, 24, m_cap.m_GrabberCB.m_capbmp.bih.biWidth, m_cap.m_GrabberCB.m_capbmp.bih.biHeight))  
		{  
			BYTE* byte = m_cap.m_GrabberCB.GetImgFileData();
			BITMAPINFO BitmapInfo;
			ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
			CopyMemory(&BitmapInfo.bmiHeader,m_cap.m_GrabberCB.GetBMPInforHeader(),sizeof(BITMAPINFOHEADER));
			m_staticBMP.SetBMPInfoHeader(BitmapInfo);
			m_staticBMP.SetBytes(byte);
			m_staticBMP.Invalidate(FALSE);

			if (!m_strSaveDir.IsEmpty())
			{
				CString strtemp;
				FormatBMPFileName(m_savebmpcolor,strtemp);
				strtemp = m_strSaveDir+_T("/")+strtemp;
				m_cap.SetSaveBMPFileName(strtemp);
				m_cap.m_GrabberCB.SaveBitmap(pImageData,m_cap.m_GrabberCB.m_capbmp.lBufferSize);
				if (m_savebmpcolor == color_black)
				{
					m_AllTypeBMPFile.clear();
				}
				m_AllTypeBMPFile.push_back(strtemp);
				if(m_savebmpcolor == color_blue)
				{
					m_savebmpcolor = color_black;
					//��ʾ����Ի���
					CShowResultDlg dlg;
					dlg.SetAllTypeBMPFile(m_AllTypeBMPFile);
					dlg.DoModal();
				}					
				else
				{
					m_savebmpcolor = (savebmpcolor)(m_savebmpcolor+1);
				}
			}

			//����ͼƬ
			

		}  
		//to free memory  
		delete[] pImageData;
	}
	return 0;
}

void CCheckPixelUIDlg::FormatBMPFileName( savebmpcolor colortype,CString& filename )
{
	CString strTemp;
	SYSTEMTIME st;
	GetLocalTime(&st);
	strTemp.Format(_T("%04d%02d%02d %02d%02d%02d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	switch(colortype)
	{
	case color_black:
		strTemp += "_black.bmp";
		break;
	case color_white:
		strTemp += "_white.bmp";
		break;
	case color_red:
		strTemp += "_red.bmp";
		break;
	case color_green:
		strTemp += "_green.bmp";
		break;
	case color_blue:
		strTemp += "_blue.bmp";
		break;
	}
	filename = strTemp;
}
