
// CheckPixelUIDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "CaptureVideo.h"
#include "ShowBMPStatic.h"
#include "PictureCtrl.h"
#include <vector>
using namespace std;


#define TIMER_DELAY 1000 //������ʱ
#define TIMER_ID 1

// CCheckPixelUIDlg �Ի���

enum savebmpcolor
{
	color_black,
	color_white,
	color_red,
	color_green,
	color_blue
};

class CCheckPixelUIDlg : public CDialog
{
// ����
public:
	CCheckPixelUIDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHECKPIXELUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnCapImg();
	afx_msg void OnBnClickedBtnFreeze();
	afx_msg void OnBnClickedBtnSavePath();
	afx_msg void OnBnClickedBtnSavebmp();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnCapMessage(WPARAM wparam,LPARAM lparam);

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_StaticVideo;
	CCaptureVideo m_cap;
	BOOL m_bFreeze;
	HBITMAP m_hBitmap;
	long m_lWidth;
	long m_lHeight;
	
	CComboBox m_DevicesList;
	CString m_strSaveDir;
	CShowBMPStatic m_staticBMP;
private:
	void OnStartCap();//��ʼץ��
	void FormatBMPFileName(savebmpcolor colortype,CString& filename);

private:
	savebmpcolor m_savebmpcolor;//��¼ץ�ĵ�ͼƬ���ͣ����紿�ڣ����׵ȵ�

	vector<CString> m_AllTypeBMPFile;
	
};
