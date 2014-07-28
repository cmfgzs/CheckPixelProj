
// CheckPixelUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "CheckPixelUIDlg.h"
#include "ShowResultDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CCheckPixelUIDlg 对话框




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


// CCheckPixelUIDlg 消息处理程序

BOOL CCheckPixelUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_bFreeze = FALSE;

	// TODO: 在此添加额外的初始化代码
	m_cap.EnumDevices(m_DevicesList.GetSafeHwnd());//获取连接的视频设备

	m_DevicesList.SetCurSel(0);
	HWND hWnd = m_StaticVideo.GetSafeHwnd();
	if (hWnd != NULL)
	{
		m_cap.Init(0,hWnd);//初始化摄像头
	}
	m_staticBMP.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	m_cap.m_GrabberCB.SetSafeWnd(this->GetSafeHwnd());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCheckPixelUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{	

		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCheckPixelUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCheckPixelUIDlg::OnBnClickedBtnCapImg()
{
	// TODO: 在此添加控件通知处理程序代码

	KillTimer(TIMER_ID);
	SetTimer(TIMER_ID,TIMER_DELAY,NULL);
}

void CCheckPixelUIDlg::OnBnClickedBtnFreeze()
{
	// TODO: 在此添加控件通知处理程序代码
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
	sInfo.lpszTitle = _T("选择保存文件夹");
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
	// TODO: 保存图片
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CDialog::OnEraseBkgnd(pDC);
	return FALSE;
}

void CCheckPixelUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
		AfxMessageBox(_T("先设置保存路径"));
		return;
	}
	m_cap.GrabOneFrame(TRUE);//捕捉一张图片
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
					//显示结果对话框
					CShowResultDlg dlg;
					dlg.SetAllTypeBMPFile(m_AllTypeBMPFile);
					dlg.DoModal();
				}					
				else
				{
					m_savebmpcolor = (savebmpcolor)(m_savebmpcolor+1);
				}
			}

			//保存图片
			

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
