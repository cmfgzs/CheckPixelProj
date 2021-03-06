//-------------------------------------------------------------------
// CCaptureVideo视频捕捉类实现文件CaptureVideo.cpp
//-------------------------------------------------------------------
// CaptureVideo.cpp: implementation of the CCaptureVideo class.
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CaptureVideo.h"
#include <io.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
BOOL bOneShot=FALSE;//全局变量
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCaptureVideo::CCaptureVideo()
{
	//COM Library Intialization
	if(FAILED(CoInitialize(NULL)))
	{
		AfxMessageBox(_T("CoInitialize Failed!\r\n"));
		return;
	}
	m_hWnd = NULL;
	m_pVW = NULL;
	m_pMC = NULL;
	m_pGB = NULL;
	m_pBF = NULL;
	m_pCapture = NULL;
}
CCaptureVideo::~CCaptureVideo()
{
	// Stop media playback
	if(m_pMC)m_pMC->Stop();
	if(m_pVW){
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
	}
	SAFE_RELEASE(m_pCapture);
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	CoUninitialize( );
}
int CCaptureVideo::EnumDevices(HWND hList)
{
	if (!hList)
		return -1;
	int id = 0;
	
	//枚举视频扑捉设备
	ICreateDevEnum *pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	
	if (hr != NOERROR)
		return -1;
	CComPtr<IEnumMoniker> pEm;

	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	
	if (hr != NOERROR)
		return -1;
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				char str[2048];			
				TCHAR tStr[2048];
				id++;
				WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
				MultiByteToWideChar(CP_ACP,0,str,-1,tStr,2048);
				::SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)tStr);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
	}
	return id;
}

HRESULT CCaptureVideo::Init(int iDeviceID, HWND hWnd)
{
	HRESULT hr;
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr)){
		AfxMessageBox(_T("Failed to get video interfaces!"));
		return hr;
	}
	// Bind Device Filter. We know the device because the id was passed in
	if(!BindFilter(iDeviceID, &m_pBF))return S_FALSE;
	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
	// hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
	// m_pBF, NULL, NULL);
	// create a sample grabber
	hr = m_pGrabber.CoCreateInstance( CLSID_SampleGrabber );
	if( !m_pGrabber ){
		AfxMessageBox(_T("Fail to create SampleGrabber, maybe qedit.dll is not registered?"));
		return hr;
	}
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabBase( m_pGrabber );
	
	//设置视频格式
		AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	hr = m_pGrabber->SetMediaType(&mt);
	
	if( FAILED( hr ) ){
		AfxMessageBox(_T("Fail to set media type!"));
		return hr;
	}
	hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
	if( FAILED( hr ) ){
		AfxMessageBox(_T("Fail to put sample grabber in graph"));
		return hr;
	}
	
	// try to render preview/capture pin
		hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
	if( FAILED( hr ) )
		hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
	
		if( FAILED( hr ) ){
		AfxMessageBox(_T("can't build graph"));
		return hr;
	}
	
	hr = m_pGrabber->GetConnectedMediaType( &mt );
	if ( FAILED( hr) ){
		AfxMessageBox(_T("Failt to read the connected media type"));
		return hr;
	}
	
	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;
	m_GrabberCB.lWidth = vih->bmiHeader.biWidth;
	m_GrabberCB.lHeight = vih->bmiHeader.biHeight;
	FreeMediaType(mt);
	hr = m_pGrabber->SetBufferSamples( TRUE );
	hr = m_pGrabber->SetOneShot( FALSE );
	hr = m_pGrabber->SetCallback( &m_GrabberCB, 1 );


	
	//设置视频捕捉窗口
	m_hWnd = hWnd ;
	//m_GrabberCB.SetSafeWnd(m_hWnd);
	SetupVideoWindow();
	hr = m_pMC->Run();//开始视频捕捉
	if(FAILED(hr)){AfxMessageBox(_T("不能捕捉图像"));return hr;}
	return S_OK;
}
bool CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
		return false;
	
	//enumerate all video capture devices
		CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	{
		return false;
	}
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR)
	{
		return false;
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				if (index == deviceId)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}
	return true;
}

HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
	HRESULT hr;
	
	// 创建IGraphBuilder接口
		hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
	// 创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
	if (FAILED(hr))return hr;
	m_pCapture->SetFiltergraph(m_pGB);
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	if (FAILED(hr))return hr;
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);
	if (FAILED(hr))return hr;
	return hr;
}
HRESULT CCaptureVideo::SetupVideoWindow()
{
	HRESULT hr;
	hr = m_pVW->put_Owner((OAHWND)m_hWnd);
	if (FAILED(hr))return hr;
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))return hr;
	ResizeVideoWindow();
	hr = m_pVW->put_Visible(OATRUE);
	return hr;
}
void CCaptureVideo::ResizeVideoWindow()
{
	if (m_pVW){
		//让图像充满整个窗口
		CRect rc;
		::GetClientRect(m_hWnd,&rc);
		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	}
}
void CCaptureVideo::GrabOneFrame(BOOL bGrab)
{
	bOneShot = bGrab;
}
void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) {
		CoTaskMemFree((PVOID)mt.pbFormat);
		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) {
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

void CCaptureVideo::SetMediaPlayOrPause( BOOL isPlay )
{
	if (m_pMC != NULL)
	{
		if (isPlay)
		{
			m_pMC->Run();
		}
		else
		{
			m_pMC->Pause();
		}
	}
	else
	{
		AfxMessageBox(_T("摄像头未启动!"));
	}
}

void CCaptureVideo::SetSaveBMPFileDir( const CString& saveDir )
{//截屏功能
	CString tempFileName = saveDir;
	int i=1;
	CString strTemp;
	while(true)
	{
		strTemp.Format(_T("\\SaveBMP%d.bmp"),i);
		tempFileName += strTemp;
		char* filename = (LPSTR)(LPCTSTR)tempFileName;
		if (_access(filename,0)==-1)
		{
			break;
		}
	}
	m_GrabberCB.m_szFileName = tempFileName;
}

void CCaptureVideo::SetSaveBMPFileName( const CString& name )
{
	m_GrabberCB.m_szFileName = name;
}


CSampleGrabberCB::CSampleGrabberCB()
{
	m_szFileName = _T("");
	m_SnapBMPHandle = NULL;
	m_pImgFileData = NULL;
	m_pFileHeader = new BITMAPFILEHEADER;
	m_pBitmapHeader = new BYTE[sizeof(BITMAPINFOHEADER)];
}


STDMETHODIMP CSampleGrabberCB::QueryInterface( REFIID riid, void ** ppv )
{
	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ){
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP CSampleGrabberCB::SampleCB( double SampleTime, IMediaSample * pSample )
{
	return 0;
}

STDMETHODIMP CSampleGrabberCB::BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
{
	if( !bOneShot )return 0;

	if (!pBuffer)return E_POINTER;
	if (m_capbmp.lBufferSize<lBufferSize&&m_capbmp.pBuffer!= NULL)
	{
		delete[] m_capbmp.pBuffer;
		m_capbmp.pBuffer = NULL;
		m_capbmp.lBufferSize = 0;
	}
	m_capbmp.dbSampleTime = dblSampleTime;
	if (!m_capbmp.pBuffer)
	{
		m_capbmp.lBufferSize = lBufferSize;
		m_capbmp.pBuffer = new BYTE[lBufferSize];
	}
	if (!m_capbmp.pBuffer)
	{
		m_capbmp.lBufferSize = 0;
		return E_OUTOFMEMORY;
	}

	BITMAPINFOHEADER bih;
	memset( &bih, 0, sizeof( bih ) );
	bih.biSize = sizeof( bih );
	bih.biWidth = lWidth;
	bih.biHeight = lHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	memcpy(&(m_capbmp.bih),&bih,sizeof(bih));

	memcpy(m_capbmp.pBuffer,pBuffer,sizeof(BYTE)*lBufferSize);

	//BYTE* pImageData = new BYTE[m_capbmp.lBufferSize];
	//CopyMemory(pImageData,m_capbmp.pBuffer,m_capbmp.lBufferSize);

	SendMessage(m_hWnd,WM_CAPTURE_BITMAP,0,0);

	//SaveBitmap(pBuffer, lBufferSize);
	bOneShot = FALSE;
	return 0;
}

BOOL CSampleGrabberCB::SaveBitmap( BYTE * pBuffer, long lBufferSize )
{
	if (m_szFileName.IsEmpty())
	{
		AfxMessageBox(_T("图片名称为空!"));
		return FALSE;
	}
	HANDLE hf = CreateFile(
		m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, NULL, NULL );
	if( hf == INVALID_HANDLE_VALUE )return 0;
	// 写文件头
	BITMAPFILEHEADER bfh;
	memset( &bfh, 0, sizeof( bfh ) );
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof( bfh ) + lBufferSize + sizeof( BITMAPINFOHEADER );
	bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
	DWORD dwWritten = 0;
	WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );
	// 写位图格式
	BITMAPINFOHEADER bih;
	memset( &bih, 0, sizeof( bih ) );
	bih.biSize = sizeof( bih );
	bih.biWidth = lWidth;
	bih.biHeight = lHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	WriteFile( hf, &bih, sizeof( bih ), &dwWritten, NULL );
	// 写位图数据
	WriteFile( hf, pBuffer, lBufferSize, &dwWritten, NULL );
	m_SnapBMPHandle = MakeBitmap(::GetWindowDC(NULL),pBuffer,bih);
	CloseHandle( hf );
	AfxMessageBox(_T("保存图片成功!"));
	return 0;
}

HBITMAP CSampleGrabberCB::MakeBitmap( HDC hDc,LPBYTE lpBits,BITMAPINFOHEADER& bmiheader )
{
	BITMAPINFO bitinfo;
	memset(&bitinfo, 0, sizeof(BITMAPINFO));
	bitinfo.bmiHeader = bmiheader;
	HBITMAP hBmp = CreateDIBitmap(hDc,&bitinfo.bmiHeader,CBM_INIT,lpBits,&bitinfo,DIB_RGB_COLORS);
	return hBmp;
}

CSampleGrabberCB::~CSampleGrabberCB()
{
	if (m_SnapBMPHandle != NULL)
	{
		DeleteObject(m_SnapBMPHandle);
	}
	if (m_pBitmapHeader!= NULL)
	{
		delete m_pBitmapHeader;
		m_pBitmapHeader = NULL;
	}
	if (m_pFileHeader != NULL)
	{
		delete m_pFileHeader;
		m_pFileHeader = NULL;
	}
	if (m_capbmp.pBuffer != NULL)
	{
		delete[] m_capbmp.pBuffer;
	}
	if (m_pImgFileData != NULL)
	{
		delete[] m_pImgFileData;
	}
}

BOOL CSampleGrabberCB::FormatImage( BYTE* lpImageData,int nBitcount,int nWidth,int nHeight )
{
	m_bValidBMP = FALSE;
	int nKlsBmpBitCount;
	int nImgWidth = nWidth;
	int nImgHeight = nHeight;
	if (nBitcount==8||nBitcount==24||nBitcount==32)
	{
		nKlsBmpBitCount = nBitcount;
	}
	else
	{
		return m_bValidBMP;
	}
	int nDataWidth = nKlsBmpBitCount/8*nWidth;

	nDataWidth = ( nDataWidth % 4 == 0 ) ? nDataWidth : ( ( nDataWidth / 4 + 1 ) * 4 ) ;

	m_pFileHeader->bfType = 0x4d42;
	m_pFileHeader->bfReserved1 = m_pFileHeader->bfReserved2 = 0;
	m_pFileHeader->bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+ nDataWidth*nImgHeight;
	if (nBitcount == 8)
	{
		int nBmpInfoSize = sizeof(BITMAPFILEHEADER)   
			+ sizeof(BITMAPINFOHEADER)  
			+ 256 * 4 ;  
		m_pFileHeader->bfOffBits = nBmpInfoSize;
		m_pBitmapHeader = new BYTE[nBmpInfoSize];
		m_pBmpInfo = (LPBITMAPINFOHEADER)m_pBitmapHeader;
		m_pvColorTable = m_pBitmapHeader + sizeof(BITMAPINFOHEADER);
		LPRGBQUAD pDibQuad =  (LPRGBQUAD)(m_pvColorTable);

		for (int c=0;c<256;c++)
		{
			pDibQuad[c].rgbRed = c;
			pDibQuad[c].rgbGreen = c;
			pDibQuad[c].rgbBlue = c;
			pDibQuad[c].rgbReserved = 0;
		}
	}
	else
	{
		m_pFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		m_pBmpInfo = (LPBITMAPINFOHEADER)m_pBitmapHeader;  
		m_pvColorTable = NULL; 
	}
	m_pBmpInfo->biBitCount      = nKlsBmpBitCount;   
	m_pBmpInfo->biWidth         = nImgWidth;  
	m_pBmpInfo->biHeight        = nImgHeight;  
	m_pBmpInfo->biPlanes        = 1;  
	m_pBmpInfo->biSize          = sizeof(BITMAPINFOHEADER);  
	m_pBmpInfo->biSizeImage     = nImgWidth * nImgHeight * nKlsBmpBitCount / 8;   
	m_pBmpInfo->biClrImportant  = 0;  
	m_pBmpInfo->biClrUsed       = 0;  
	m_pBmpInfo->biCompression   = 0;  
	m_pBmpInfo->biXPelsPerMeter = 0;  
	m_pBmpInfo->biYPelsPerMeter = 0; 
	SetImgFileData(nDataWidth,nImgHeight);
	if (nBitcount == 8)
	{
		if (nImgWidth%4 == 0)
		{
			memset( m_pImgFileData, 0, nDataWidth*nImgHeight ) ;  
			memcpy( m_pImgFileData, lpImageData, m_capbmp.lBufferSize);
		}
		else
		{
			memset( m_pImgFileData, 0, nDataWidth*nImgHeight);
			for (int i=0;i<nImgHeight;i++)
			{
				memcpy( m_pImgFileData + i*nDataWidth, lpImageData + i*nImgWidth, nImgWidth );
			}
		}
	}
	else if (nBitcount == 24)
	{
		if ( nImgWidth % 4 == 0 )  
		{  
			memset( m_pImgFileData, 0, nDataWidth*nImgHeight ) ;  
			memcpy( m_pImgFileData, lpImageData, m_capbmp.lBufferSize ) ;  
		}
		else
		{
			memset( m_pImgFileData, 0, nDataWidth*nImgHeight ) ;  
			for ( int i=0; i<nImgHeight; i++ )  
			{  
				memcpy( m_pImgFileData + i*nDataWidth, lpImageData + i*3*nImgWidth, 3*nImgWidth ) ;  
			}  
		}
	}
	else if (nBitcount == 32)
	{
		memcpy( m_pImgFileData, lpImageData,m_capbmp.lBufferSize );
	}
	m_bValidBMP = TRUE;
	return m_bValidBMP;

}

void CSampleGrabberCB::SetImgFileData( int datawidth,int dataheight )
{
	if (!m_pImgFileData)
	{
		m_pImgFileData = new BYTE[datawidth*dataheight];
	}
	else
	{
		delete[] m_pImgFileData;
		m_pImgFileData = new BYTE[datawidth*dataheight];
	}
}

BYTE* CSampleGrabberCB::GetImgFileData()
{
	return m_pImgFileData;
}

void CSampleGrabberCB::SetSafeWnd( HWND hWnd )
{
	m_hWnd = hWnd;
}

LPBITMAPINFOHEADER CSampleGrabberCB::GetBMPInforHeader()
{
	return m_pBmpInfo;
}

LPBITMAPFILEHEADER CSampleGrabberCB::GetBMPFileHeader()
{
	return m_pFileHeader;
}
