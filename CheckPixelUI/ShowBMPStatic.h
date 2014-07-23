#pragma once


// CShowBMPStatic

class CShowBMPStatic : public CStatic
{
	DECLARE_DYNAMIC(CShowBMPStatic)

public:
	CShowBMPStatic();
	virtual ~CShowBMPStatic();

protected:
	DECLARE_MESSAGE_MAP()
private:
	HBITMAP m_hbmp;
	BITMAPINFO m_bmpinfo;
	BYTE* m_pData;
public:
	void SetBMPHandler(HBITMAP hbmp);
	afx_msg void OnPaint();
	void SetBytes(BYTE* pData);
	void SetBMPInfoHeader(BITMAPINFO bmpinfo);
};


