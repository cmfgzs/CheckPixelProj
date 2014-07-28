// CCaptureVideo视频捕捉类头文件
/////////////////////////////////////////////////////////////////////
#if !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_
/////////////////////////////////////////////////////////////////////
// CaptureVideo.h : header file
/////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#define __IDxtCompositor_INTERFACE_DEFINED__  
#define __IDxtAlphaSetter_INTERFACE_DEFINED__  
#define __IDxtJpeg_INTERFACE_DEFINED__  
#define __IDxtKey_INTERFACE_DEFINED__  
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
#include <qedit.h>
#define WM_CAPTURE_BITMAP WM_USER+100

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x )\
	if ( NULL != x )\
{\
	x->Release( );\
	x = NULL;\
}
#endif

typedef struct tag_bmpinfo
{
	tag_bmpinfo()
	{
		lBufferSize = 0;
		pBuffer = NULL;
	}
	double dbSampleTime;
	long lBufferSize;
	BYTE* pBuffer;
	BITMAPINFOHEADER bih;
} CapBmpInfo;


class CSampleGrabberCB : public ISampleGrabberCB
{
public:
	CSampleGrabberCB();
	~CSampleGrabberCB();
	long lWidth;
	long lHeight;
	//char m_szFileName[MAX_PATH];// 位图文件名称
	CString m_szFileName;
	HBITMAP m_SnapBMPHandle;
private:
	LPBITMAPFILEHEADER m_pFileHeader;//bmp文件头
	LPBITMAPINFOHEADER m_pBmpInfo;//bmp信息头

	BYTE* m_pImgFileData;//BMP文件数据
	BOOL m_bValidBMP;

	LPBYTE m_pBitmapHeader;
	LPVOID m_pvColorTable;//调色板

	

	HWND m_hWnd;
public:
	CapBmpInfo m_capbmp;
	
public:
	STDMETHODIMP_(ULONG) AddRef() { return 2;}
	STDMETHODIMP_(ULONG) Release() { return 1;}
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize );
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize );
	HBITMAP MakeBitmap(HDC hDc,LPBYTE lpBits,BITMAPINFOHEADER& bmiheader);
	BOOL FormatImage(BYTE* lpImageData,int nBitcount,int nWidth,int nHeight);
	void SetImgFileData(int datawidth,int dataheight);
	BYTE* GetImgFileData();
	LPBITMAPINFOHEADER GetBMPInforHeader();
	LPBITMAPFILEHEADER GetBMPFileHeader();
	void SetSafeWnd(HWND hWnd);
};

class CCaptureVideo : public CWnd
{
public:
	void GrabOneFrame(BOOL bGrab);
	HRESULT Init(int iDeviceID,HWND hWnd);
	int EnumDevices(HWND hList);
	CCaptureVideo();
	virtual ~CCaptureVideo();
private:
	HWND m_hWnd;
	IGraphBuilder *m_pGB;
	ICaptureGraphBuilder2* m_pCapture;
	IBaseFilter* m_pBF;
	IMediaControl* m_pMC;
	IVideoWindow* m_pVW;
	

	CString m_strBMPFile;
protected:
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
	void ResizeVideoWindow();
	HRESULT SetupVideoWindow();
	HRESULT InitCaptureGraphBuilder();
public:
	void SetMediaPlayOrPause(BOOL isPlay);
	void SetSaveBMPFileDir(const CString& saveDir);
	void SetSaveBMPFileName(const CString& name);
	void FreeMediaType(AM_MEDIA_TYPE& mt);
public:
	CSampleGrabberCB m_GrabberCB;
	CComPtr<ISampleGrabber> m_pGrabber;
	
};
#endif // !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
