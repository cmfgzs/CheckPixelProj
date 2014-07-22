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

#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x )\
	if ( NULL != x )\
{\
	x->Release( );\
	x = NULL;\
}
#endif


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
	
public:
	STDMETHODIMP_(ULONG) AddRef() { return 2;}
	STDMETHODIMP_(ULONG) Release() { return 1;}
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize );
	BOOL SaveBitmap(BYTE * pBuffer, long lBufferSize );
	HBITMAP MakeBitmap(HDC hDc,LPBYTE lpBits,BITMAPINFOHEADER& bmiheader);
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
	CComPtr<ISampleGrabber> m_pGrabber;

	CString m_strBMPFile;
protected:
	void FreeMediaType(AM_MEDIA_TYPE& mt);
	bool BindFilter(int deviceId, IBaseFilter **pFilter);
	void ResizeVideoWindow();
	HRESULT SetupVideoWindow();
	HRESULT InitCaptureGraphBuilder();
public:
	void SetMediaPlayOrPause(BOOL isPlay);
	void SetSaveBMPFileName(const CString& saveDir);
public:
	CSampleGrabberCB m_GrabberCB;
	
};
#endif // !defined(AFX_CAPTUREVIDEO_H__F5345AA4_A39F_4B07_B843_3D87C4287AA0__INCLUDED_)
