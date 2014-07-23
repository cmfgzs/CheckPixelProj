// ShowBMPStatic.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CheckPixelUI.h"
#include "ShowBMPStatic.h"


// CShowBMPStatic

IMPLEMENT_DYNAMIC(CShowBMPStatic, CStatic)

CShowBMPStatic::CShowBMPStatic()
{
	 m_hbmp = NULL;
	 m_pData = NULL;
}

CShowBMPStatic::~CShowBMPStatic()
{
}


BEGIN_MESSAGE_MAP(CShowBMPStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CShowBMPStatic::SetBMPHandler( HBITMAP hbmp )
{
	if (hbmp != NULL)
	{
		m_hbmp = hbmp;
	}
}



// CShowBMPStatic ��Ϣ�������



void CShowBMPStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
	CRect rc;
	// TODO: Add your message handler code here
	if(m_pData!=NULL)
	{
		GetClientRect(&rc);
		dc.RealizePalette();
		SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);


		int nResult = ::StretchDIBits(
			dc.GetSafeHdc(),
			rc.right-rc.left,
			rc.top,
			-rc.right-rc.left,
			rc.bottom-rc.top,
			0,
			0,
			m_bmpinfo.bmiHeader.biWidth, 
			m_bmpinfo.bmiHeader.biHeight,
			m_pData, 
			&m_bmpinfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}
}

void CShowBMPStatic::SetBytes( BYTE* pData )
{
	if (pData!=NULL)
	{
		m_pData = pData;
		this->Invalidate(FALSE);
	}
}

void CShowBMPStatic::SetBMPInfoHeader( BITMAPINFO bmpinfo )
{
	m_bmpinfo = bmpinfo;
}
