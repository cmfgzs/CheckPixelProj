
// CheckPixelUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "CheckPixelUIDlg.h"

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
}

void CCheckPixelUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_StaticVideo);
	DDX_Control(pDX, IDC_COMBO1, m_DevicesList);
	DDX_Text(pDX, IDC_SAVE_PATH, m_strSaveDir);
}

BEGIN_MESSAGE_MAP(CCheckPixelUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CAP_IMG, &CCheckPixelUIDlg::OnBnClickedBtnCapImg)
	ON_BN_CLICKED(IDC_BTN_FREEZE, &CCheckPixelUIDlg::OnBnClickedBtnFreeze)
	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, &CCheckPixelUIDlg::OnBnClickedBtnSavePath)
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
	UpdateData(FALSE);

}
