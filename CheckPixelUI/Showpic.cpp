//////////////////////////////////////////////////////////////////////
//
//  This class is designed to provide simple interface for 
//  simultaneous Video Capture & Preview using DirectShow
//
//////////////////////////////////////////////////////////////////////
//
//	References: MS DirectShow Samples
//
//		
//////////////////////////////////////////////////////////////////////
//
//	This class was written by Sagar K.R . 
//  Use of this class is not restricted in any
//	way whatsoever.Please report the bugs to krssagar@firsteccom.co.kr
//
//	Special thanks to all the members at The Code Project! 
//	(www.codeproject.com)
//
//////////////////////////////////////////////////////////////////////
// Showpic.cpp : implementation file
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "Showpic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowpic

CShowpic::CShowpic()
{
	m_pImageBytes=NULL;
	m_un32ImageBits = 24;
	MakeBMPHeader();
}

CShowpic::~CShowpic()
{
	if(m_pBmp)
	delete []m_pBmp;

	m_pBmp=NULL;
}


BEGIN_MESSAGE_MAP(CShowpic, CStatic)
	//{{AFX_MSG_MAP(CShowpic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowpic message handlers

void CShowpic::MakeBMPHeader()
{	

	DWORD  dwBitmapInfoSize;
	dwBitmapInfoSize = sizeof(BITMAPINFO);
	m_pBmp    = (BITMAPINFO *)new BYTE [dwBitmapInfoSize];


	m_pBmp->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_pBmp->bmiHeader.biWidth = m_un32ImageWidth;
    m_pBmp->bmiHeader.biHeight = m_un32ImageHeight*-1;	
    m_pBmp->bmiHeader.biPlanes = 1;
    m_pBmp->bmiHeader.biBitCount =(unsigned short) m_un32ImageBits;
    m_pBmp->bmiHeader.biCompression = BI_RGB;
    m_pBmp->bmiHeader.biSizeImage = 0;
    m_pBmp->bmiHeader.biXPelsPerMeter = 0;
    m_pBmp->bmiHeader.biYPelsPerMeter = 0;
    m_pBmp->bmiHeader.biClrUsed = 0;
    m_pBmp->bmiHeader.biClrImportant = 0;


		
		
}

void CShowpic::OnPaint() 
{
	CRect rc;
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	if(m_pImageBytes!=NULL)
	{
	GetClientRect(&rc);
	dc.RealizePalette();
	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);

	
	//int m_un32ImageHeight;
	
	
	int nResult = ::StretchDIBits(
								dc.GetSafeHdc(),
								rc.left,
								rc.top,
								rc.Width(),
								rc.Height(),
								0,
								0,
								m_un32ImageWidth, 
								m_un32ImageHeight,
								m_pImageBytes, 
								m_pBmp,
								DIB_RGB_COLORS,
								SRCCOPY);

//	LocalFree((HLOCAL)m_pImageBytes);
	}
}

void CShowpic::ShowImage(BYTE *pImageByte)
{
	if(pImageByte!=NULL)
	{
		m_pImageBytes=pImageByte;
		this->Invalidate (FALSE);
	}
}
