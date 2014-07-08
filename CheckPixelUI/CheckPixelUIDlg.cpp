
// CheckPixelUIDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "CheckPixelUIDlg.h"

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
	UpdateData(FALSE);

}
