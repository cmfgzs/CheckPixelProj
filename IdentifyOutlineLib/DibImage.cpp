///////////////////////////////////////////////////////////////////////
// DibImage.cpp: implementation of the CDibImage class.
//  DIB(Independent Bitmap) ������
//
//  PaintDIB()          - ����DIB����
//  CreateDIBPalette()  - ����DIB�����ɫ��
//  FindDIBBits()       - ����DIBͼ��������ʼλ��
//  DIBWidth()          - ����DIB���
//  DIBHeight()         - ����DIB�߶�
//  PaletteSize()       - ����DIB��ɫ���С
//  DIBNumColors()      - ����DIB��ɫ����ɫ��Ŀ
//  CopyHandle()        - �����ڴ��
//
//  SaveDIB()           - ��DIB���浽ָ���ļ���
//  ReadDIBFile()       - ��ָ���ļ��ж�ȡDIB����
//
//  ͼ������㺯����
//
//  LinerTrans()		- ͼ�����Ա任
//	WindowTrans()		- ͼ�񴰿ڱ任
//  GrayStretch()		- ͼ��Ҷ�����
//  InteEqualize()		��ֱ��ͼ����
//
//	ͼ�񼸺α任����
//  TranslationDIB1()   - ͼ��ƽ��
//  TranslationDIB()    - ͼ��ƽ��
//  MirrorDIB()		    - ͼ����
//  TransposeDIB()		- ͼ��ת��
//  ZoomDIB()			- ͼ������
//  RotateDIB()			- ͼ����ת
//	
//  ͼ�������任������
//
//  FFT()				- ���ٸ���Ҷ�任
//  IFFT()				- ���ٸ���Ҷ���任
//  DCT()				- ��ɢ���ұ任
//  WALSH()				- �ֶ�ʲ��������任
//
//  Fourier()			- ͼ��ĸ���Ҷ�任
//  DIBDct()			- ͼ�����ɢ���ұ任
//  DIBWalsh()			- ͼ����ֶ�ʲ��������任
//
//	ͼ��ģ��任����
//
//  Template()			- ͼ��ģ��任��ͨ���ı�ģ�壬��������ʵ��
//						  ͼ���ƽ�����񻯡���Եʶ��Ȳ�����
//  MedianFilter()		- ͼ����ֵ�˲���
//  GetMedianNum()		- ��ȡ��ֵ��������MedianFilter()����������ֵ��
//  ReplaceColorPal()	- ����α��ɫ�����
//
//  ͼ����̬ѧ�任������
//
//  ErosionDIB()    - ͼ��ʴ
//  DilationDIB()	- ͼ������
//  OpenDIB()		- ͼ������
//  CloseDIB()		- ͼ�������
//  ThiningDIB()	- ͼ��ϸ��
//
//  ͼ���Ե���������㺯����
//
//  RobertDIB()		- robert��Ե�������
//  SobelDIB()		- sobel��Ե�������
//  PrewittDIB()	- prewitt��Ե�������
//  KirschDIB()		- kirsch��Ե�������
//  GaussDIB()		- gauss��Ե�������
//  HoughDIB()		- ����Hough�任���ƽ��ֱ��
//  ContourDIB()	- ������ȡ
//  TraceDIB()		- ��������
//  FillDIB()		- ��������㷨1
//  Fill2DIB()		- ��������㷨2
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Direct.h"
#include "DibImage.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDibImage::CDibImage()
{

}

CDibImage::~CDibImage()
{

}

//////////////////////////////////////////////////////////////////////
// DIB(Independent Bitmap) ����
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * �������ƣ�
 *   PaintDIB()
 * ����:
 *   HDC hDC            - ����豸DC
 *   LPRECT lpDCRect    - ���ƾ�������
 *   HDIB hDIB          - ָ��DIB�����ָ��
 *   LPRECT lpDIBRect   - Ҫ�����DIB����
 *   CPalette* pPal     - ָ��DIB�����ɫ���ָ��
 * ����ֵ:
 *   BOOL               - ���Ƴɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�����Ҫ��������DIB�������е�����StretchDIBits()����
 * SetDIBitsToDevice()������DIB����������豸���ɲ���hDCָ
 * �������Ƶľ��������ɲ���lpDCRectָ�������DIB�������ɲ���
 * lpDIBRectָ����
 ************************************************************************/
BOOL CDibImage::PaintDIB(HDC     hDC,
					LPRECT  lpDCRect,
					HDIB    hDIB,
					LPRECT  lpDIBRect,
					CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // BITMAPINFOHEADERָ��
	LPSTR    lpDIBBits;           // DIB����ָ��
	BOOL     bSuccess=FALSE;      // �ɹ���־
	HPALETTE hPal=NULL;           // DIB��ɫ��
	HPALETTE hOldPal=NULL;        // ��ǰ�ĵ�ɫ��

	if (hDIB == NULL)
	{
		return FALSE;
	}
	
	lpDIBHdr  = (LPSTR)::GlobalLock((HGLOBAL) hDIB);// ����DIB	
	lpDIBBits = FindDIBBits(lpDIBHdr);	// �ҵ�DIBͼ��������ʼλ��

	if (pPal != NULL)					// ��ȡDIB��ɫ�壬��ѡ����
	{
		hPal = (HPALETTE) pPal->m_hObject;	
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);	// ѡ�е�ɫ��
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);			// ������ʾģʽ

	// �ж��ǵ���StretchDIBits()����SetDIBitsToDevice()������DIB����
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	{
		// ԭʼ��С���������졣
		bSuccess = ::SetDIBitsToDevice(hDC,                    // hDC
								   lpDCRect->left,             // DestX
								   lpDCRect->top,              // DestY
								   RECTWIDTH(lpDCRect),        // nDestWidth
								   RECTHEIGHT(lpDCRect),       // nDestHeight
								   lpDIBRect->left,            // SrcX
								   (int)DIBHeight(lpDIBHdr) -
									  lpDIBRect->top -
									  RECTHEIGHT(lpDIBRect),   // SrcY
								   0,                          // nStartScan
								   (WORD)DIBHeight(lpDIBHdr),  // nNumScans
								   lpDIBBits,                  // lpBits
								   (LPBITMAPINFO)lpDIBHdr,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
	}
    else
	{
		// ��ԭʼ��С�����졣
		bSuccess = ::StretchDIBits(hDC,                          // hDC
							   lpDCRect->left,                 // DestX
							   lpDCRect->top,                  // DestY
							   RECTWIDTH(lpDCRect),            // nDestWidth
							   RECTHEIGHT(lpDCRect),           // nDestHeight
							   lpDIBRect->left,                // SrcX
							   lpDIBRect->top,                 // SrcY
							   RECTWIDTH(lpDIBRect),           // wSrcWidth
							   RECTHEIGHT(lpDIBRect),          // wSrcHeight
							   lpDIBBits,                      // lpBits
							   (LPBITMAPINFO)lpDIBHdr,         // lpBitsInfo
							   DIB_RGB_COLORS,                 // wUsage
							   SRCCOPY);                       // dwROP
	}
	 
	::GlobalUnlock((HGLOBAL) hDIB);				// �������	
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);	// �ָ���ǰ�ĵ�ɫ��
	}	
	return bSuccess;
}

/*************************************************************************
 * �������ƣ�
 *   CreateDIBPalette()
 * ����:
 *   HDIB hDIB          - ָ��DIB�����ָ��
 *   CPalette* pPal     - ָ��DIB�����ɫ���ָ��
 * ����ֵ:
 *   BOOL               - �����ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�������DIB����һ���߼���ɫ�壬��DIB�ж�ȡ��ɫ���浽��ɫ���У�
 * ����ո��߼���ɫ�崴��һ���µĵ�ɫ�壬�����ظõ�ɫ��ľ��������
 * ��������õ���ɫ����ʾDIBͼ��
 ************************************************************************/
BOOL CDibImage::CreateDIBPalette(HDIB hDIB, CPalette* pPal)
{
	
	LPLOGPALETTE lpPal;		// ָ���߼���ɫ���ָ��
	HANDLE hLogPal;			// �߼���ɫ��ľ��
	HPALETTE hPal = NULL;	// ��ɫ��ľ��
	int i;					// ѭ������	
	WORD wNumColors;		// ��ɫ���е���ɫ��Ŀ	
	LPSTR lpbi;				// ָ��DIB��ָ��	
	LPBITMAPINFO lpbmi;		// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��	
	LPBITMAPCOREINFO lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��	
	BOOL bWinStyleDIB;		// �����Ƿ���Win3.0 DIB�ı��	
	BOOL bResult = FALSE;	// �������
	
	if (hDIB == NULL)
	{
		return FALSE;
	}
		
	lpbi = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);	// ����DIB
	lpbmi = (LPBITMAPINFO)lpbi;		// ��ȡָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	lpbmc = (LPBITMAPCOREINFO)lpbi;	// ��ȡָ��BITMAPCOREINFO�ṹ��ָ��
	wNumColors = DIBNumColors(lpbi);// ��ȡDIB����ɫ���е���ɫ��Ŀ
	
	if (wNumColors != 0)
	{
		// ����Ϊ�߼���ɫ���ڴ�
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);	
		// ����ڴ治�㣬�˳�
		if (hLogPal == 0)
		{	
			::GlobalUnlock((HGLOBAL) hDIB);	// �������
			return FALSE;
		}
		
		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);		
		lpPal->palVersion = PALVERSION;			// ���ð汾��
		lpPal->palNumEntries = (WORD)wNumColors;// ������ɫ��Ŀ
		bWinStyleDIB = IS_WIN30_DIB(lpbi);		// �ж��Ƿ���WIN3.0��DIB

		// ��ȡ��ɫ��
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				// ��ȡ��ɫ��ɫ��ɫ����
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;				
				// ����λ
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				// ��ȡ��ɫ��ɫ��ɫ����
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;				
				// ����λ
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}
			
		bResult = pPal->CreatePalette(lpPal);// �����߼���ɫ�崴����ɫ�壬������ָ��
		::GlobalUnlock((HGLOBAL) hLogPal);	// �������
		::GlobalFree((HGLOBAL) hLogPal);	// �ͷ��߼���ɫ��
	}
	
	::GlobalUnlock((HGLOBAL) hDIB);			// �������
	return bResult;
}

/*************************************************************************
 * �������ƣ�
 *   FindDIBBits()
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 * ����ֵ:
 *   LPSTR              - ָ��DIBͼ��������ʼλ��
 * ˵��:
 *   �ú�������DIB��ͼ�����ص���ʼλ�ã�������ָ������ָ�롣
 ************************************************************************/
LPSTR CDibImage::FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

/*************************************************************************
 * �������ƣ�
 *   DIBWidth()
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 * ����ֵ:
 *   DWORD              - DIB��ͼ��Ŀ��
 * ˵��:
 *   �ú�������DIB��ͼ��Ŀ�ȡ�����Windows 3.0 DIB������BITMAPINFOHEADER
 * �е�biWidthֵ��������������BITMAPCOREHEADER�е�bcWidthֵ��
 ************************************************************************/
DWORD CDibImage::DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// ����DIB��ͼ��Ŀ��
	if (IS_WIN30_DIB(lpDIB))
	{		
		return lpbmi->biWidth;		// ����Windows 3.0 DIB������lpbmi->biWidth
	}
	else
	{		
		return (DWORD)lpbmc->bcWidth;	// ����������ʽ��DIB������lpbmc->bcWidth
	}
}

/*************************************************************************
 * �������ƣ�
 *   DIBHeight()
 * ����:
 *   LPSTR lpDIB        - ָ��DIB�����ָ��
 * ����ֵ:
 *   DWORD              - DIB��ͼ��ĸ߶�
 * ˵��:
 *   �ú�������DIB��ͼ��ĸ߶ȡ�����Windows 3.0 DIB������BITMAPINFOHEADER
 * �е�biHeightֵ��������������BITMAPCOREHEADER�е�bcHeightֵ��
 ************************************************************************/
DWORD CDibImage::DIBHeight(LPSTR lpDIB)
{	
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// ����DIB��ͼ��Ŀ��
	if (IS_WIN30_DIB(lpDIB))
	{		
		return lpbmi->biHeight;		// ����Windows 3.0 DIB������lpbmi->biHeight
	}
	else
	{		
		return (DWORD)lpbmc->bcHeight;	// ����������ʽ��DIB������lpbmc->bcHeight
	}
}

/*************************************************************************
 * �������ƣ�
 *   PaletteSize()
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 * ����ֵ:
 *   WORD               - DIB�е�ɫ��Ĵ�С
 * ˵��:
 *   �ú�������DIB�е�ɫ��Ĵ�С������Windows 3.0 DIB��������ɫ��Ŀ��
 * RGBQUAD�Ĵ�С����������������ɫ��Ŀ��RGBTRIPLE�Ĵ�С��
 ************************************************************************/
WORD CDibImage::PaletteSize(LPSTR lpbi)
{
	// ����DIB�е�ɫ��Ĵ�С
	if (IS_WIN30_DIB (lpbi))
	{
		//������ɫ��Ŀ��RGBQUAD�Ĵ�С
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
	}
	else
	{
		//������ɫ��Ŀ��RGBTRIPLE�Ĵ�С
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
	}
}

/*************************************************************************
 * �������ƣ�
 *   DIBNumColors()
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 * ����ֵ:
 *   WORD               - ���ص�ɫ������ɫ������
 * ˵��:
 *   �ú�������DIB�е�ɫ�����ɫ�����������ڵ�ɫλͼ������2��
 * ����16ɫλͼ������16������256ɫλͼ������256���������ɫ
 * λͼ��24λ����û�е�ɫ�壬����0��
 ************************************************************************/
WORD CDibImage::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;

	// ����Windows��DIB, ʵ����ɫ����Ŀ���Ա����ص�λ��Ҫ�١�
	// ��������������򷵻�һ�����Ƶ���ֵ��
	
	// �ж��Ƿ���WIN3.0 DIB
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;	// ��ȡdwClrUsedֵ
		
		if (dwClrUsed != 0)
		{
			// ���dwClrUsed��ʵ���õ�����ɫ������Ϊ0��ֱ�ӷ��ظ�ֵ
			return (WORD)dwClrUsed;
		}
	}

	// ��ȡ���ص�λ��
	if (IS_WIN30_DIB(lpbi))
	{		
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;	// ��ȡbiBitCountֵ
	}
	else
	{		
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;	// ��ȡbiBitCountֵ
	}
	
	// �������ص�λ��������ɫ��Ŀ
	switch (wBitCount)
	{
		case 1:
			return 2;
			break;
		case 4:
			return 16;
			break;
		case 8:
			return 256;
			break;
		default:
			return 0;
			break;
	}
}

/*************************************************************************
 * �������ƣ�
 *   CopyHandle()
 * ����:
 *   HGLOBAL h          - Ҫ���Ƶ��ڴ�����
 * ����ֵ:
 *   HGLOBAL            - ���ƺ�����ڴ�����
 * ˵��:
 *   �ú�������ָ�����ڴ����򡣷��ظ��ƺ�����ڴ����򣬳���ʱ����0��
 ************************************************************************/
HGLOBAL CDibImage::CopyHandle (HGLOBAL h)
{
	if (h == NULL)
	{
		return NULL;
	}
	
	DWORD dwLen = ::GlobalSize((HGLOBAL) h);	// ��ȡָ���ڴ������С
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);	// �������ڴ�ռ�	
	if (hCopy != NULL)							// �жϷ����Ƿ�ɹ�
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		
		memcpy(lpCopy, lp, dwLen);
		
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

/*************************************************************************
 * �������ƣ�
 *   SaveDIB()
 * ����:
 *   HDIB hDib          - Ҫ�����DIB
 *   CFile& file        - �����ļ�CFile
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE����CFileException
 * ˵��:
 *   �ú�����ָ����DIB���󱣴浽ָ����CFile�С���CFile�ɵ��ó���򿪺͹رա�
 *************************************************************************/
BOOL CDibImage::SaveDIB(HDIB hDib, CFile& file)
{	
	BITMAPFILEHEADER bmfHdr;	// Bitmap�ļ�ͷ	
	LPBITMAPINFOHEADER lpBI;	// ָ��BITMAPINFOHEADER��ָ��
	DWORD dwDIBSize;			// DIB��С

	if (hDib == NULL)
	{
		return FALSE;
	}

	// ��ȡBITMAPINFO�ṹ��������
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDib);	
	if (lpBI == NULL)
	{
		return FALSE;
	}
	
	// �ж��Ƿ���WIN3.0 DIB
	if (!IS_WIN30_DIB(lpBI))
	{
		// ��֧���������͵�DIB����
		::GlobalUnlock((HGLOBAL) hDib);
		return FALSE;
	}

	////////////////////////////////////////////////////////////////////////
	// ����ļ�ͷ///////////////////////////////////////////////////////////	
	bmfHdr.bfType = DIB_HEADER_MARKER;		// �ļ�����"BM"

	// ����DIB��Сʱ����򵥵ķ����ǵ���GlobalSize()����������ȫ���ڴ��С��
	// ����DIB�����Ĵ�С�������Ƕ༸���ֽڡ���������Ҫ����һ��DIB����ʵ��С��
	
	// �ļ�ͷ��С����ɫ���С
	// ��BITMAPINFOHEADER��BITMAPCOREHEADER�ṹ�ĵ�һ��DWORD���Ǹýṹ�Ĵ�С��
	dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);
	
	// ����ͼ���С
	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// ����RLEλͼ��û�������С��ֻ������biSizeImage�ڵ�ֵ
		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{		
		DWORD dwBmBitsSize;			// ���صĴ�С
		dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) 
			* lpBI->biHeight;		// ��СΪWidth * Height
		dwDIBSize += dwBmBitsSize;	// �����DIB�����Ĵ�С

		// ����biSizeImage���ܶ�BMP�ļ�ͷ��biSizeImage��ֵ�Ǵ���ģ�
		lpBI->biSizeImage = dwBmBitsSize;
	}

	// �����ļ���С��DIB��С��BITMAPFILEHEADER�ṹ��С
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	
	// ����������
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	// ����ƫ����bfOffBits�����Ĵ�СΪBitmap�ļ�ͷ��С��DIBͷ��С����ɫ���С
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize
											  + PaletteSize((LPSTR)lpBI);

	/////////////////////////////////////////////////////////////////////////
	// ����д�ļ�////////////////////////////////////////////////////////////
	TRY
	{		
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));	// д�ļ�ͷ
		//file.WriteHuge(lpBI, dwDIBSize);					// дDIBͷ������
	}
	CATCH (CFileException, e)
	{
		::GlobalUnlock((HGLOBAL) hDib);
		THROW_LAST();
	}
	END_CATCH
	
	::GlobalUnlock((HGLOBAL) hDib);
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   ReadDIBFile()
 * ����:
 *   CFile& file        - Ҫ��ȡ���ļ��ļ�CFile
 * ����ֵ:
 *   HDIB               - �ɹ�����DIB�ľ�������򷵻�NULL��
 * ˵��:
 *   �ú�����ָ�����ļ��е�DIB�������ָ�����ڴ������С���BITMAPFILEHEADER
 * ������ݶ����������ڴ档
 *************************************************************************/
HDIB CDibImage::ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	HDIB hDIB;
	LPSTR pDIB;
	DWORD dwBitsSize;

	dwBitsSize = file.GetLength();		// ��ȡDIB���ļ������ȣ��ֽڣ�

	// ���Զ�ȡDIB�ļ�ͷ
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		return NULL;
	}
	// �ж��Ƿ���DIB���󣬼��ͷ�����ֽ��Ƿ���"BM"
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
	{
		return NULL;
	}
	// ΪDIB�����ڴ�
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		return NULL;
	}
	
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )		// ������
	{
		::GlobalUnlock((HGLOBAL) hDIB);
		::GlobalFree((HGLOBAL) hDIB);
		return NULL;
	}

	::GlobalUnlock((HGLOBAL) hDIB);
	return hDIB;
}

//////////////////////////////////////////////////////////////////////
// ͼ������㺯��
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * �������ƣ�
 *   LinerTrans()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   FLOAT fA		    - ���Ա任��б��
 *   FLOAT fB           - ���Ա任�Ľؾ�
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ����лҶȵ����Ա任������
 ************************************************************************/
BOOL CDibImage::LinerTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						   FLOAT fA, FLOAT fB)
{		
	unsigned char*	lpSrc;		// ָ��Դͼ���ָ��
	LONG	i;					// ѭ������
	LONG	j;		
	LONG	lLineBytes;			// ͼ��ÿ�е��ֽ���	
	FLOAT	fTemp;				// �м����	

	lLineBytes = WIDTHBYTES(lWidth * 8);	// ����ͼ��ÿ�е��ֽ���
		
	for(i = 0; i < lHeight; i++)			// ÿ��
	{		
		for(j = 0; j < lWidth; j++)			// ÿ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			fTemp = fA * (*lpSrc) + fB;		// ���Ա任	
			
			if (fTemp > 255)				// �ж��Ƿ񳬳���Χ
			{
				*lpSrc = 255;
			}
			else if (fTemp < 0)
			{
				*lpSrc = 0;
			}
			else
			{				
				*lpSrc = (unsigned char) (fTemp + 0.5);	// ��������
			}
		}
	}
	
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   ThresholdTrans()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   BYTE  bThre	    - ��ֵ
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ�������ֵ�任�����ڻҶ�ֵС����ֵ������ֱ������
 * �Ҷ�ֵΪ0���Ҷ�ֵ������ֵ������ֱ������Ϊ255��
 ************************************************************************/
BOOL CDibImage::ThresholdTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bThre)
{	
	unsigned char*	lpSrc;				// ָ��Դͼ���ָ��	
	LONG	i;							// ѭ������
	LONG	j;	
	LONG	lLineBytes;					// ͼ��ÿ�е��ֽ���
		
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���	
	
	for(i = 0; i < lHeight; i++)		// ÿ��
	{		
		for(j = 0; j < lWidth; j++)		// ÿ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			
			if ((*lpSrc) < bThre)		// �ж��Ƿ�С����ֵ
			{
				*lpSrc = 0;
			}
			else
			{
				*lpSrc = 255;
			}
		}
	}
	
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   WindowTrans()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   BYTE  bLow		    - ��������
 *   BYTE  bUp          - ��������
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ����д��ڱ任��ֻ���ڴ��ڷ�Χ�ڵĻҶȱ��ֲ��䣬
 * С�����޵�����ֱ�����ûҶ�ֵΪ0���������޵�����ֱ�����ûҶ�ֵΪ255��
 ************************************************************************/
BOOL CDibImage::WindowTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
							BYTE bLow, BYTE bUp)
{	
	unsigned char*	lpSrc;				// ָ��Դͼ���ָ��	
	LONG	i;							// ѭ������
	LONG	j;
	LONG	lLineBytes;					// ͼ��ÿ�е��ֽ���
		
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���	
	
	for(i = 0; i < lHeight; i++)		// ÿ��
	{	
		for(j = 0; j < lWidth; j++)		// ÿ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			if ((*lpSrc) < bLow)		// �ж��Ƿ񳬳���Χ
			{
				*lpSrc = 0;
			}
			else if ((*lpSrc) > bUp)
			{
				*lpSrc = 255;
			}
		}
	}
	
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   GrayStretch()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   BYTE bX1			- �Ҷ������һ�����X����
 *   BYTE bY1			- �Ҷ������һ�����Y����
 *   BYTE bX2			- �Ҷ�����ڶ������X����
 *   BYTE bY2			- �Ҷ�����ڶ������Y����
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ����лҶ����졣
 ************************************************************************/
BOOL CDibImage::GrayStretch(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
							BYTE bX1, BYTE bY1, BYTE bX2, BYTE bY2)
{	
	unsigned char*	lpSrc;				// ָ��Դͼ���ָ��
	LONG	i;							// ѭ������
	LONG	j;	
	BYTE	bMap[256];					// �Ҷ�ӳ���	
	LONG	lLineBytes;					// ͼ��ÿ�е��ֽ���
	
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���
	
	// ����Ҷ�ӳ���
	for (i = 0; i <= bX1; i++)
	{		
		if (bX1 > 0)					// �ж�bX1�Ƿ����0����ֹ��ĸΪ0��
		{
			bMap[i] = (BYTE) bY1 * i / bX1;
		}
		else
		{
			bMap[i] = 0;
		}
	}
	for (; i <= bX2; i++)
	{
		if (bX2 != bX1)					// �ж�bX1�Ƿ����bX2����ֹ��ĸΪ0��
		{
			bMap[i] = bY1 + (BYTE) ((bY2 - bY1) * (i - bX1) / (bX2 - bX1));
		}
		else
		{
			bMap[i] = bY1;
		}
	}
	for (; i < 256; i++)
	{	
		if (bX2 != 255)					// �ж�bX2�Ƿ����255����ֹ��ĸΪ0��
		{
			bMap[i] = bY2 + (BYTE) ((255 - bY2) * (i - bX2) / (255 - bX2));
		}
		else
		{
			bMap[i] = 255;
		}
	}
		
	for(i = 0; i < lHeight; i++)		// ÿ��
	{		
		for(j = 0; j < lWidth; j++)		// ÿ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			// �����µĻҶ�ֵ
			*lpSrc = bMap[*lpSrc];
		}
	}
	
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   InteEqualize()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ�����ֱ��ͼ���⡣
 ************************************************************************/
BOOL CDibImage::InteEqualize(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	unsigned char*	lpSrc;				// ָ��Դͼ���ָ��	
	LONG	lTemp;						// ��ʱ����	
	LONG	i;							// ѭ������
	LONG	j;	
	BYTE	bMap[256];					// �Ҷ�ӳ���	
	LONG	lCount[256];				// �Ҷ�ӳ���
	LONG	lLineBytes;					// ͼ��ÿ�е��ֽ���
		
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���
		
	for (i = 0; i < 256; i ++)			// ���ü���Ϊ0
	{
		lCount[i] = 0;
	}
	
	// ��������Ҷ�ֵ�ļ���
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			lpSrc = (unsigned char *)lpDIBBits + lLineBytes * i + j;
			lCount[*(lpSrc)]++;
		}
	}
	
	// ����Ҷ�ӳ���
	for (i = 0; i < 256; i++)
	{
		lTemp = 0;		
		for (j = 0; j <= i ; j++)
		{
			lTemp += lCount[j];
		}
		
		// �����Ӧ���»Ҷ�ֵ
		bMap[i] = (BYTE) (lTemp * 255 / lHeight / lWidth);
	}
		
	for(i = 0; i < lHeight; i++)			// ÿ��
	{
		for(j = 0; j < lWidth; j++)			// ÿ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			// �����µĻҶ�ֵ
			*lpSrc = bMap[*lpSrc];
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// ͼ�񼸺α任����
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * �������ƣ�
 *   TranslationDIB1()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   LONG  lXOffset     - X��ƽ��������������
 *   LONG  lYOffset     - Y��ƽ��������������
 * ����ֵ:
 *   BOOL               - ƽ�Ƴɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�������ˮƽ�ƶ�DIBͼ�񡣺�������ı�ͼ��Ĵ�С���Ƴ��Ĳ���ͼ��
 * ����ȥ���հײ����ð�ɫ��䡣
 ************************************************************************/
BOOL CDibImage::TranslationDIB1(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
								LONG lXOffset, LONG lYOffset)
{	
	LPSTR	lpSrc;					// ָ��Դͼ���ָ��	
	LPSTR	lpDst;					// ָ��Ҫ���������ָ��
	LPSTR	lpNewDIBBits;			// ָ����ͼ���ָ��
	HLOCAL	hNewDIBBits;
	LONG	i;						// ��������DIB�е�����
	LONG	j;	
	LONG	i0;						// ������ԴDIB�е�����
	LONG	j0;	
	LONG lLineBytes;				// ͼ��ÿ�е��ֽ���
	
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���
	
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
		
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);	
		
	for(i = 0; i < lHeight; i++)					// ÿ��
	{		
		for(j = 0; j < lWidth; j++)					// ÿ��
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			// ע������DIB��ͼ���һ����ʵ���������һ�е�λ�ã����lpDst
			// ֵ����(char *)lpNewDIBBits + lLineBytes * i + j������
			// (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// �����������ԴDIB�е�����
			i0 = i - lXOffset;
			j0 = j - lYOffset;
			
			// �ж��Ƿ���Դͼ��Χ��
			if( (j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// ָ��ԴDIB��i0�У���j0�����ص�ָ��
				// ͬ��Ҫע��DIB���µ��õ�����
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;							
				*lpDst = *lpSrc;					// ��������
			}
			else
			{
				// ����Դͼ��û�е����أ�ֱ�Ӹ�ֵΪ255
				* ((unsigned char*)lpDst) = 255;
			}
		}
	}
	
	// ����ƽ�ƺ��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   TranslationDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   LONG  lXOffset     - X��ƽ��������������
 *   LONG  lYOffset     - Y��ƽ��������������
 * ����ֵ:
 *   BOOL               - ƽ�Ƴɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�������ˮƽ�ƶ�DIBͼ�񡣺�������ı�ͼ��Ĵ�С���Ƴ��Ĳ���ͼ��
 * ����ȥ���հײ����ð�ɫ��䡣
 ************************************************************************/
BOOL CDibImage::TranslationDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
							   LONG lXOffset, LONG lYOffset)
{	
	CRect	rectSrc;	// ƽ�ƺ�ʣ��ͼ����Դͼ���е�λ�ã���������	
	CRect	rectDst;	// ƽ�ƺ�ʣ��ͼ������ͼ���е�λ�ã���������	
	LPSTR	lpSrc;		// ָ��Դͼ���ָ��	
	LPSTR	lpDst;		// ָ��Ҫ���������ָ��	
	LPSTR	lpNewDIBBits;	// ָ����ͼ���ָ��
	HLOCAL	hNewDIBBits;
	BOOL	bVisible;	// ָ��ͼ���Ƿ�ȫ����ȥ��������	
	LONG	i;			// ѭ������	
	LONG lLineBytes;	// ͼ��ÿ�е��ֽ���
		
	lLineBytes = WIDTHBYTES(lWidth * 8);	// ����ͼ��ÿ�е��ֽ���	
	bVisible = TRUE;	// ����ֵ
	
	// ����rectSrc��rectDst��X����
	if (lXOffset <= -lWidth)
	{		
		bVisible = FALSE;	// X�᷽��ȫ���Ƴ���������
	}
	else if (lXOffset <= 0)
	{		
		rectDst.left = 0;	// �ƶ�����ͼ�������Ͻ�X����Ϊ0
		
		// �ƶ�����ͼ�������½�X����ΪlWidth - |lXOffset| = lWidth + lXOffset
		rectDst.right = lWidth + lXOffset;
	}
	else if (lXOffset < lWidth)
	{	
		rectDst.left = lXOffset;	// �ƶ�����ͼ�������Ͻ�X����ΪlXOffset	
		rectDst.right = lWidth;		// �ƶ�����ͼ�������½�X����ΪlWidth
	}
	else
	{		
		bVisible = FALSE;			// X�᷽��ȫ���Ƴ���������
	}
	
	// ƽ�ƺ�ʣ��ͼ����Դͼ���е�X����
	rectSrc.left = rectDst.left - lXOffset;
	rectSrc.right = rectDst.right - lXOffset;
	
	//  ����rectSrc��rectDst��Y����
	if (lYOffset <= -lHeight)
	{		
		bVisible = FALSE;			// Y�᷽��ȫ���Ƴ���������
	}
	else if (lYOffset <= 0)
	{		
		rectDst.top = 0;			// �ƶ�����ͼ�������Ͻ�Y����Ϊ0
		
		// �ƶ�����ͼ�������½�Y����ΪlHeight - |lYOffset| = lHeight + lYOffset
		rectDst.bottom = lHeight + lYOffset;
	}
	else if (lYOffset < lHeight)
	{		
		rectDst.top = lYOffset;		// �ƶ�����ͼ�������Ͻ�Y����ΪlYOffset		
		rectDst.bottom = lHeight;	// �ƶ�����ͼ�������½�Y����ΪlHeight
	}
	else
	{		
		bVisible = FALSE;			// X�᷽��ȫ���Ƴ���������
	}
	
	// ƽ�ƺ�ʣ��ͼ����Դͼ���е�Y����
	rectSrc.top = rectDst.top - lYOffset;
	rectSrc.bottom = rectDst.bottom - lYOffset;
	
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);
		
	if (bVisible)		// ����в���ͼ��ɼ�
	{
		// ƽ��ͼ��
		for(i = 0; i < (rectSrc.bottom - rectSrc.top); i++)
		{
			// Ҫ�����������㣬ע������DIBͼ�����������µ��õģ���һ�������Ǳ���
			// �����һ�У���˸����������㲻��lpDIBBits + lLineBytes * (i + 
			// rectSrc.top) + rectSrc.left������ lpDIBBits + lLineBytes * 
			// (lHeight - i - rectSrc.top - 1) + rectSrc.left��
			
			lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - i - rectSrc.top
				- 1) + rectSrc.left;
			
			// ҪĿ���������㣬ͬ��ע�����µ��õ����⡣
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - i - rectDst.top
				- 1) + rectDst.left;
			
			// ����ÿһ�У����ΪrectSrc.right - rectSrc.left
			memcpy(lpDst, lpSrc, rectSrc.right - rectSrc.left);
			
		}
	}

	// ����ƽ�ƺ��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   MirrorDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   BOOL  bDirection   - ����ķ���TRUE��ʾˮƽ����FALSE��ʾ��ֱ����
 * ����ֵ:
 *   BOOL               - ����ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�����������DIBͼ�񡣿���ָ������ķ�ʽ��ˮƽ���Ǵ�ֱ��
 ************************************************************************/
BOOL CDibImage::MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						  BOOL bDirection)
{		
	LPSTR	lpSrc;						// ָ��Դͼ���ָ��	
	LPSTR	lpDst;						// ָ��Ҫ���������ָ��	
	LPSTR	lpBits;						// ָ����ͼ���ָ��
	HLOCAL	hBits;	
	LONG	i;							// ѭ������
	LONG	j;	
	LONG lLineBytes;					// ͼ��ÿ�е��ֽ���	
	
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���		
	hBits = LocalAlloc(LHND, lLineBytes);	// ��ʱ�����ڴ棬�Ա���һ��ͼ��
	if (hBits == NULL)
	{
		return FALSE;
	}
	
	lpBits = (char * )LocalLock(hBits);
	
	// �жϾ���ʽ
	if (bDirection)		// ˮƽ����
	{	
		// ���ͼ��ÿ�н��в���
		for(i = 0; i < lHeight; i++)
		{
			// ���ÿ��ͼ����벿�ֽ��в���
			for(j = 0; j < lWidth / 2; j++)
			{				
				// ָ������i�У���j�����ص�ָ��
				lpSrc = (char *)lpDIBBits + lLineBytes * i + j;				
				// ָ������i�У�������j�����ص�ָ��
				lpDst = (char *)lpDIBBits + lLineBytes * (i + 1) - j;
				
				// ����һ������
				*lpBits = *lpDst;
				
				// ��������i�У���j�����ظ��Ƶ�������i�У�������j������
				*lpDst = *lpSrc;				
				// ��������i�У�������j�����ظ��Ƶ�������i�У���j������
				*lpSrc = *lpBits;
			}			
		}
	}
	else				// ��ֱ����
	{
		// ����ϰ�ͼ����в���
		for(i = 0; i < lHeight / 2; i++)
		{			
			// ָ������i����������ָ��
			lpSrc = (char *)lpDIBBits + lLineBytes * i;			
			// ָ���i����������ָ��
			lpDst = (char *)lpDIBBits + lLineBytes * (lHeight - i - 1);
			
			// ����һ�У����ΪlWidth
			memcpy(lpBits, lpDst, lLineBytes);
			
			// ��������i�����ظ��Ƶ���i��
			memcpy(lpDst, lpSrc, lLineBytes);			
			// ����i�����ظ��Ƶ�������i��
			memcpy(lpSrc, lpBits, lLineBytes);			
		}
	}
	
	LocalUnlock(hBits);
	LocalFree(hBits);
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   TransposeDIB()
 * ����:
 *   LPSTR lpDIB		- ָ��ԴDIB��ָ��
 * ����ֵ:
 *   BOOL               - ת�óɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�������ת��DIBͼ�񣬼�ͼ��x��y���껥��������������ı�ͼ��Ĵ�С��
 * ����ͼ��ĸ߿�������
 ************************************************************************/
BOOL CDibImage::TransposeDIB(LPSTR lpDIB)
{	
	LONG	lWidth;				// ͼ��Ŀ��
	LONG	lHeight;			// ͼ��ĸ߶�	
	LPSTR	lpDIBBits;			// ָ��Դͼ���ָ��	
	LPSTR	lpSrc;				// ָ��Դ���ص�ָ��	
	LPSTR	lpDst;				// ָ��ת��ͼ���Ӧ���ص�ָ��	
	LPSTR	lpNewDIBBits;		// ָ��ת��ͼ���ָ��
	HLOCAL	hNewDIBBits;
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
	LONG	i;					
	LONG	j;	
	LONG lLineBytes;			// ͼ��ÿ�е��ֽ���
	LONG lNewLineBytes;			// ��ͼ��ÿ�е��ֽ���

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	
	lpDIBBits = FindDIBBits(lpDIB);		// �ҵ�ԴDIBͼ��������ʼλ��	
	lWidth = DIBWidth(lpDIB);			// ��ȡͼ���"���"��4�ı�����
	lHeight = DIBHeight(lpDIB);			// ��ȡͼ��ĸ߶�
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���
	lNewLineBytes = WIDTHBYTES(lHeight * 8);	// ������ͼ��ÿ�е��ֽ���
	
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lNewLineBytes);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
		
	for(i = 0; i < lHeight; i++)	// ���ͼ��ÿ�н��в���
	{		
		for(j = 0; j < lWidth; j++)	// ���ÿ��ͼ��ÿ�н��в���
		{			
			// ָ��ԴDIB��i�У���j�����ص�ָ��
			lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;	
			
			// ָ��ת��DIB��j�У���i�����ص�ָ��
			// ע��˴�lWidth��lHeight��ԴDIB�Ŀ�Ⱥ͸߶ȣ�Ӧ�û���
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lWidth - 1 - j) + i;
			
			// ��������
			*lpDst = *lpSrc;			
		}		
	}
	
	// ����ת�ú��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lNewLineBytes);
	
	// ����DIB��ͼ��ĸ߿�
	if (IS_WIN30_DIB(lpDIB))
	{
		// ����Windows 3.0 DIB
		lpbmi->biWidth = lHeight;		
		lpbmi->biHeight = lWidth;
	}
	else
	{
		// ����������ʽ��DIB
		lpbmc->bcWidth = (unsigned short) lHeight;
		lpbmc->bcHeight = (unsigned short) lWidth;
	}
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   ZoomDIB()
 * ����:
 *   LPSTR lpDIB		- ָ��ԴDIB��ָ��
 *   float fXZoomRatio	- X�᷽�����ű���
 *   float fYZoomRatio	- Y�᷽�����ű���
 * ����ֵ:
 *   HGLOBAL            - ���ųɹ�������DIB��������򷵻�NULL��
 * ˵��:
 *   �ú�����������DIBͼ�񣬷���������DIB�ľ����
 ************************************************************************/
HGLOBAL CDibImage::ZoomDIB(LPSTR lpDIB, float fXZoomRatio, float fYZoomRatio)
{
	LONG	lWidth;				// Դͼ��Ŀ��
	LONG	lHeight;			// Դͼ��ĸ߶�	
	LONG	lNewWidth;			// ���ź�ͼ��Ŀ��
	LONG	lNewHeight;			// ���ź�ͼ��ĸ߶�	
	LONG	lNewLineBytes;		// ���ź�ͼ��Ŀ�ȣ�lNewWidth'��������4�ı�����	
	LPSTR	lpDIBBits;			// ָ��Դͼ���ָ��	
	LPSTR	lpSrc;				// ָ��Դ���ص�ָ��	
	HDIB	hDIB;				// ���ź���DIB���	
	LPSTR	lpDst;				// ָ������ͼ���Ӧ���ص�ָ��	
	LPSTR	lpNewDIB;			// ָ������ͼ���ָ��
	LPSTR	lpNewDIBBits;	
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��	
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��	

	LONG	i;					// ѭ����������������DIB�е����꣩
	LONG	j;	
	LONG	i0;					// ������ԴDIB�е�����
	LONG	j0;
	
	
	LONG lLineBytes;			// ͼ��ÿ�е��ֽ���	
	lpDIBBits = FindDIBBits(lpDIB);	// �ҵ�ԴDIBͼ��������ʼλ��		
	lWidth = DIBWidth(lpDIB);	// ��ȡͼ��Ŀ��
	lLineBytes = WIDTHBYTES(lWidth * 8);	// ����ͼ��ÿ�е��ֽ���
	lHeight = DIBHeight(lpDIB);	// ��ȡͼ��ĸ߶�
	
	// �������ź��ͼ��ʵ�ʿ��
	// �˴�ֱ�Ӽ�0.5������ǿ������ת��ʱ���������룬����ֱ�ӽ�ȥС������
	lNewWidth = (LONG) (DIBWidth(lpDIB) * fXZoomRatio + 0.5);
	
	// ������ͼ��ÿ�е��ֽ���
	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	
	// �������ź��ͼ��߶�
	lNewHeight = (LONG) (lHeight * fYZoomRatio + 0.5);
	
	// �����ڴ棬�Ա�����DIB
	hDIB = (HDIB) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + 
		*(LPDWORD)lpDIB + PaletteSize(lpDIB));	
	if (hDIB == NULL)
	{
		return NULL;
	}
	
	lpNewDIB =  (char * )::GlobalLock((HGLOBAL) hDIB);
	
	// ����DIB��Ϣͷ�͵�ɫ��
	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + PaletteSize(lpDIB));
	
	// �ҵ���DIB������ʼλ��
	lpNewDIBBits = FindDIBBits(lpNewDIB);
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;
	
	// ����DIB��ͼ��ĸ߶ȺͿ��
	if (IS_WIN30_DIB(lpNewDIB))
	{
		// ����Windows 3.0 DIB
		lpbmi->biWidth = lNewWidth;
		lpbmi->biHeight = lNewHeight;
	}
	else
	{
		// ����������ʽ��DIB
		lpbmc->bcWidth = (unsigned short) lNewWidth;
		lpbmc->bcHeight = (unsigned short) lNewHeight;
	}
		
	for(i = 0; i < lNewHeight; i++)		// ���ͼ��ÿ�н��в���
	{		
		for(j = 0; j < lNewWidth; j++)	// ���ͼ��ÿ�н��в���
		{			
			// ָ����DIB��i�У���j�����ص�ָ��
			// ע��˴���Ⱥ͸߶�����DIB�Ŀ�Ⱥ͸߶�
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			
			// �����������ԴDIB�е�����
			i0 = (LONG) (i / fYZoomRatio + 0.5);
			j0 = (LONG) (j / fXZoomRatio + 0.5);
			
			// �ж��Ƿ���Դͼ��Χ��
			if( (j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{				
				// ָ��ԴDIB��i0�У���j0�����ص�ָ��
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;				
				// ��������
				*lpDst = *lpSrc;
			}
			else
			{
				// ����Դͼ��û�е����أ�ֱ�Ӹ�ֵΪ255
				* ((unsigned char*)lpDst) = 255;
			}			
		}		
	}
	
	return hDIB;
}

/*************************************************************************
 * �������ƣ�
 *   RotateDIB()
 * ����:
 *   LPSTR lpDIB		- ָ��ԴDIB��ָ��
 *   int iRotateAngle	- ��ת�ĽǶȣ�0-360�ȣ�
 * ����ֵ:
 *   HGLOBAL            - ��ת�ɹ�������DIB��������򷵻�NULL��
 * ˵��:
 *   �ú���������ͼ������Ϊ������תDIBͼ�񣬷���������DIB�ľ����
 * ���øú������Զ�����ͼ������ʾ���е����ء������в������ڽ���
 * ֵ�㷨���в�ֵ��
 ************************************************************************/
HGLOBAL CDibImage::RotateDIB(LPSTR lpDIB, int iRotateAngle)
{	
	LONG	lWidth;				// Դͼ��Ŀ��
	LONG	lHeight;			// Դͼ��ĸ߶�	
	LONG	lNewWidth;			// ��ת��ͼ��Ŀ��
	LONG	lNewHeight;			// ��ת��ͼ��ĸ߶�	
	LONG	lLineBytes;			// ͼ��ÿ�е��ֽ���	
	LONG	lNewLineBytes;		// ��ת��ͼ��Ŀ�ȣ�lNewWidth'��������4�ı�����	
	LPSTR	lpDIBBits;			// ָ��Դͼ���ָ��	
	LPSTR	lpSrc;				// ָ��Դ���ص�ָ��	
	HDIB	hDIB;				// ��ת����DIB���	
	LPSTR	lpDst;				// ָ����תͼ���Ӧ���ص�ָ��	
	LPSTR	lpNewDIB;			// ָ����תͼ���ָ��
	LPSTR	lpNewDIBBits;	
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��	
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
		
	LONG	i;					// ѭ����������������DIB�е����꣩
	LONG	j;	
	LONG	i0;					// ������ԴDIB�е�����
	LONG	j0;	
	
	float	fRotateAngle;		// ��ת�Ƕȣ����ȣ�	
	float	fSina, fCosa;		// ��ת�Ƕȵ����Һ�����	
	// Դͼ�ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	float	fSrcX1,fSrcY1,fSrcX2,fSrcY2,fSrcX3,fSrcY3,fSrcX4,fSrcY4;	
	// ��ת���ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	float	fDstX1,fDstY1,fDstX2,fDstY2,fDstX3,fDstY3,fDstX4,fDstY4;
	float	f1,f2;
		
	lpDIBBits = FindDIBBits(lpDIB);		// �ҵ�ԴDIBͼ��������ʼλ��	
	lWidth = DIBWidth(lpDIB);			// ��ȡͼ���"���"��4�ı�����	
	lLineBytes = WIDTHBYTES(lWidth * 8);// ����ͼ��ÿ�е��ֽ���	
	lHeight = DIBHeight(lpDIB);			// ��ȡͼ��ĸ߶�
	
	// ����ת�ǶȴӶ�ת��������
	fRotateAngle = (float) RADIAN(iRotateAngle);	
	fSina = (float) sin((double)fRotateAngle);	// ������ת�Ƕȵ�����
	fCosa = (float) cos((double)fRotateAngle);	// ������ת�Ƕȵ�����
	
	// ����ԭͼ���ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	fSrcX1 = (float) (- (lWidth  - 1) / 2);
	fSrcY1 = (float) (  (lHeight - 1) / 2);
	fSrcX2 = (float) (  (lWidth  - 1) / 2);
	fSrcY2 = (float) (  (lHeight - 1) / 2);
	fSrcX3 = (float) (- (lWidth  - 1) / 2);
	fSrcY3 = (float) (- (lHeight - 1) / 2);
	fSrcX4 = (float) (  (lWidth  - 1) / 2);
	fSrcY4 = (float) (- (lHeight - 1) / 2);
	
	// ������ͼ�ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	fDstX1 =  fCosa * fSrcX1 + fSina * fSrcY1;
	fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	fDstX2 =  fCosa * fSrcX2 + fSina * fSrcY2;
	fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	fDstX3 =  fCosa * fSrcX3 + fSina * fSrcY3;
	fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	fDstX4 =  fCosa * fSrcX4 + fSina * fSrcY4;
	fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;
	
	// ������ת���ͼ��ʵ�ʿ��
	lNewWidth  = (LONG)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	
	// ������ͼ��ÿ�е��ֽ���
	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	
	// ������ת���ͼ��߶�
	lNewHeight = (LONG)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	
	// �������������������Ժ�ÿ�ζ�������
	f1 = (float) (-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina
		+ 0.5 * (lWidth  - 1));
	f2 = (float) ( 0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa
		+ 0.5 * (lHeight - 1));
	
	// �����ڴ棬�Ա�����DIB
	hDIB = (HDIB) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + 
		*(LPDWORD)lpDIB + PaletteSize(lpDIB));
	if (hDIB == NULL)
	{
		return NULL;
	}

	lpNewDIB =  (char * )::GlobalLock((HGLOBAL) hDIB);
	
	// ����DIB��Ϣͷ�͵�ɫ��
	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + PaletteSize(lpDIB));
	
	// �ҵ���DIB������ʼλ��
	lpNewDIBBits = FindDIBBits(lpNewDIB);
	
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;

	// ����DIB��ͼ��ĸ߶ȺͿ��
	if (IS_WIN30_DIB(lpNewDIB))
	{
		// ����Windows 3.0 DIB
		lpbmi->biWidth = lNewWidth;
		lpbmi->biHeight = lNewHeight;
	}
	else
	{
		// ����������ʽ��DIB
		lpbmc->bcWidth = (unsigned short) lNewWidth;
		lpbmc->bcHeight = (unsigned short) lNewHeight;
	}
		
	for(i = 0; i < lNewHeight; i++)		// ���ͼ��ÿ�н��в���
	{		
		for(j = 0; j < lNewWidth; j++)	// ���ͼ��ÿ�н��в���
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			// ע��˴���Ⱥ͸߶�����DIB�Ŀ�Ⱥ͸߶�
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			
			// �����������ԴDIB�е�����
			i0 = (LONG) (-((float) j) * fSina + ((float) i) * fCosa + f2 + 0.5);
			j0 = (LONG) ( ((float) j) * fCosa + ((float) i) * fSina + f1 + 0.5);
			
			// �ж��Ƿ���Դͼ��Χ��
			if( (j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// ָ��ԴDIB��i0�У���j0�����ص�ָ��
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;
				
				// ��������
				*lpDst = *lpSrc;
			}
			else
			{
				// ����Դͼ��û�е����أ�ֱ�Ӹ�ֵΪ255
				* ((unsigned char*)lpDst) = 255;
			}			
		}		
	}
	
	return hDIB;
}

/*************************************************************************
 * �������ƣ�
 *   RotateDIB2()
 * ����:
 *   LPSTR lpDIB		- ָ��ԴDIB��ָ��
 *   int iRotateAngle	- ��ת�ĽǶȣ�0-360�ȣ�
 * ����ֵ:
 *   HGLOBAL            - ��ת�ɹ�������DIB��������򷵻�NULL��
 * ˵��:
 *   �ú���������ͼ������Ϊ������תDIBͼ�񣬷���������DIB�ľ����
 * ���øú������Զ�����ͼ������ʾ���е����ء������в���˫���Բ�
 * ֵ�㷨���в�ֵ��
 ************************************************************************/
HGLOBAL CDibImage::RotateDIB2(LPSTR lpDIB, int iRotateAngle)
{	
	LONG	lWidth;				// Դͼ��Ŀ��
	LONG	lHeight;			// Դͼ��ĸ߶�	
	LONG	lNewWidth;			// ��ת��ͼ��Ŀ��
	LONG	lNewHeight;			// ��ת��ͼ��ĸ߶�
	LONG	lNewLineBytes;		// ��ת��ͼ��Ŀ�ȣ�lNewWidth'��������4�ı�����	
	LPSTR	lpDIBBits;			// ָ��Դͼ���ָ��	
	HDIB	hDIB;				// ��ת����DIB���	
	LPSTR	lpDst;				// ָ����תͼ���Ӧ���ص�ָ��	
	LPSTR	lpNewDIB;			// ָ����תͼ���ָ��
	LPSTR	lpNewDIBBits;	
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��	
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
		
	LONG	i;					// ѭ����������������DIB�е����꣩
	LONG	j;	
	FLOAT	i0;					// ������ԴDIB�е�����
	FLOAT	j0;	
	
	float	fRotateAngle;		// ��ת�Ƕȣ����ȣ�	
	float	fSina, fCosa;		// ��ת�Ƕȵ����Һ�����	
	// Դͼ�ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	float	fSrcX1,fSrcY1,fSrcX2,fSrcY2,fSrcX3,fSrcY3,fSrcX4,fSrcY4;	
	// ��ת���ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	float	fDstX1,fDstY1,fDstX2,fDstY2,fDstX3,fDstY3,fDstX4,fDstY4;
	float	f1,f2;
	
	
	lpDIBBits = FindDIBBits(lpDIB);		// �ҵ�ԴDIBͼ��������ʼλ��	
	lWidth = DIBWidth(lpDIB);			// ��ȡͼ��Ŀ��
	lHeight = DIBHeight(lpDIB);			// ��ȡͼ��ĸ߶�
	
	// ����ת�ǶȴӶ�ת��������
	fRotateAngle = (float) RADIAN(iRotateAngle);
		
	fSina = (float) sin((double)fRotateAngle);	// ������ת�Ƕȵ�����
	fCosa = (float) cos((double)fRotateAngle);	// ������ת�Ƕȵ�����
	
	// ����ԭͼ���ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	fSrcX1 = (float) (- (lWidth  - 1) / 2);
	fSrcY1 = (float) (  (lHeight - 1) / 2);
	fSrcX2 = (float) (  (lWidth  - 1) / 2);
	fSrcY2 = (float) (  (lHeight - 1) / 2);
	fSrcX3 = (float) (- (lWidth  - 1) / 2);
	fSrcY3 = (float) (- (lHeight - 1) / 2);
	fSrcX4 = (float) (  (lWidth  - 1) / 2);
	fSrcY4 = (float) (- (lHeight - 1) / 2);
	
	// ������ͼ�ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	fDstX1 =  fCosa * fSrcX1 + fSina * fSrcY1;
	fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	fDstX2 =  fCosa * fSrcX2 + fSina * fSrcY2;
	fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	fDstX3 =  fCosa * fSrcX3 + fSina * fSrcY3;
	fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	fDstX4 =  fCosa * fSrcX4 + fSina * fSrcY4;
	fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;
	
	// ������ת���ͼ��ʵ�ʿ��
	lNewWidth  = (LONG)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	
	// ������ת���ͼ��߶�
	lNewHeight = (LONG)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	
	f1 = (float) (-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina
		+ 0.5 * (lWidth  - 1));
	f2 = (float) ( 0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa
		+ 0.5 * (lHeight - 1));
	
	// �����ڴ棬�Ա�����DIB
	hDIB = (HDIB) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + 
		*(LPDWORD)lpDIB + PaletteSize(lpDIB));	
	if (hDIB == NULL)
	{
		return NULL;
	}

	lpNewDIB =  (char * )::GlobalLock((HGLOBAL) hDIB);
	
	// ����DIB��Ϣͷ�͵�ɫ��
	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + PaletteSize(lpDIB));
	
	// �ҵ���DIB������ʼλ��
	lpNewDIBBits = FindDIBBits(lpNewDIB);
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;

	// ����DIB��ͼ��ĸ߶ȺͿ��
	if (IS_WIN30_DIB(lpNewDIB))
	{
		// ����Windows 3.0 DIB
		lpbmi->biWidth = lNewWidth;
		lpbmi->biHeight = lNewHeight;
	}
	else
	{
		// ����������ʽ��DIB
		lpbmc->bcWidth = (unsigned short) lNewWidth;
		lpbmc->bcHeight = (unsigned short) lNewHeight;
	}
		
	for(i = 0; i < lNewHeight; i++)		// ���ͼ��ÿ�н��в���
	{			
		for(j = 0; j < lNewWidth; j++)	// ���ͼ��ÿ�н��в���
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			// ע��˴���Ⱥ͸߶�����DIB�Ŀ�Ⱥ͸߶�
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			
			// �����������ԴDIB�е�����
			i0 = -((float) j) * fSina + ((float) i) * fCosa + f2;
			j0 =  ((float) j) * fCosa + ((float) i) * fSina + f1;
			
			// ����˫���Բ�ֵ�㷨����������ֵ
			*lpDst = Interpolation (lpDIBBits, lWidth, lHeight, j0, i0);			
		}		
	}
	
	return hDIB;
}

/*************************************************************************
 * �������ƣ�
 *   Interpolation()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   FLOAT x			- ��ֵԪ�ص�x����
 *   FLOAT y		    - ��ֵԪ�ص�y����
 * ����ֵ:
 *   unsigned char      - ���ز�ֵ��������
 * ˵��:
 *   �ú�������˫���Բ�ֵ�㷨����������ֵ�����ڳ���ͼ��Χ�����أ�
 * ֱ�ӷ���255��
 ************************************************************************/
unsigned char CDibImage::Interpolation (LPSTR lpDIBBits, LONG lWidth, 
										LONG lHeight, FLOAT x, FLOAT y)
{
	// �ĸ����ٽ����ص�����(i1, j1), (i2, j1), (i1, j2), (i2, j2)
	LONG	i1, i2;
	LONG	j1, j2;
	
	unsigned char	f1, f2, f3, f4;	// �ĸ����ٽ�����ֵ	
	unsigned char	f12, f34;		// ������ֵ�м�ֵ	

	// ����һ��ֵ���������������С�ڸ�ֵʱ��Ϊ������ͬ
	FLOAT			EXP;	

	LONG lLineBytes;				// ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	EXP = (FLOAT) 0.0001;
	
	// �����ĸ����ٽ����ص�����
	i1 = (LONG) x;
	i2 = i1 + 1;
	j1 = (LONG) y;
	j2 = j1 + 1;
	
	// ���ݲ�ͬ����ֱ���
	if( (x < 0) || (x > lWidth - 1) || (y < 0) || (y > lHeight - 1))
	{		
		return 255;		// Ҫ����ĵ㲻��Դͼ��Χ�ڣ�ֱ�ӷ���255��
	}
	else
	{
		if (fabs(x - lWidth + 1) <= EXP)
		{
			// Ҫ����ĵ���ͼ���ұ�Ե��
			if (fabs(y - lHeight + 1) <= EXP)
			{
				// Ҫ����ĵ�������ͼ�������½���һ�����أ�ֱ�ӷ��ظõ�����ֵ
				f1 = *((unsigned char *)lpDIBBits + lLineBytes * 
					(lHeight - 1 - j1) + i1);
				return f1;
			}
			else
			{
				// ��ͼ���ұ�Ե���Ҳ������һ�㣬ֱ��һ�β�ֵ����
				f1 = *((unsigned char *)lpDIBBits + lLineBytes * 
					(lHeight - 1 - j1) + i1);
				f3 = *((unsigned char *)lpDIBBits + lLineBytes * 
					(lHeight - 1 - j1) + i2);
				
				// ���ز�ֵ���
				return ((unsigned char) (f1 + (y -j1) * (f3 - f1)));
			}
		}
		else if (fabs(y - lHeight + 1) <= EXP)
		{
			// Ҫ����ĵ���ͼ���±�Ե���Ҳ������һ�㣬ֱ��һ�β�ֵ����
			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			
			// ���ز�ֵ���
			return ((unsigned char) (f1 + (x -i1) * (f2 - f1)));
		}
		else
		{
			// �����ĸ����ٽ�����ֵ
			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			f3 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
			f4 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i2);
			
			// ��ֵ1
			f12 = (unsigned char) (f1 + (x - i1) * (f2 - f1));			
			// ��ֵ2
			f34 = (unsigned char) (f3 + (x - i1) * (f4 - f3));			
			// ��ֵ3
			return ((unsigned char) (f12 + (y -j1) * (f34 - f12)));
		}
	}
}

//////////////////////////////////////////////////////////////////////
//  ͼ�������任����
//////////////////////////////////////////////////////////////////////
/*************************************************************************
 * �������ƣ�
 *   FFT()
 * ����:
 *   complex<double> * TD	- ָ��ʱ�������ָ��
 *   complex<double> * FD	- ָ��Ƶ�������ָ��
 *   r						��2������������������
 * ����ֵ:
 *   �ޡ�
 * ˵��:
 *   �ú�������ʵ�ֿ��ٸ���Ҷ�任��
 ************************************************************************/
VOID CDibImage::FFT(complex<double> * TD, complex<double> * FD, int r)
{	
	LONG	count;				// ����Ҷ�任����	
	int		i,j,k;				// ѭ������
	int		bfsize,p;	
	double	angle;				// �Ƕ�	
	complex<double> *W,*X1,*X2,*X;
	
	count = 1 << r;				// ���㸶��Ҷ�任����
	
	// ������������洢��
	W  = new complex<double>[count / 2];
	X1 = new complex<double>[count];
	X2 = new complex<double>[count];
	
	// �����Ȩϵ��
	for(i = 0; i < count / 2; i++)
	{
		angle = -i * PI * 2 / count;
		W[i] = complex<double> (cos(angle), sin(angle));
	}
	
	// ��ʱ���д��X1
	memcpy(X1, TD, sizeof(complex<double>) * count);
	
	// ���õ����㷨���п��ٸ���Ҷ�任
	for(k = 0; k < r; k++)
	{
		for(j = 0; j < 1 << k; j++)
		{
			bfsize = 1 << (r-k);
			for(i = 0; i < bfsize / 2; i++)
			{
				p = j * bfsize;
				X2[i + p] = X1[i + p] + X1[i + p + bfsize / 2];
				X2[i + p + bfsize / 2] = (X1[i + p] - X1[i + p + bfsize / 2]) 
					* W[i * (1<<k)];
			}
		}
		X  = X1;
		X1 = X2;
		X2 = X;
	}
	
	// ��������
	for(j = 0; j < count; j++)
	{
		p = 0;
		for(i = 0; i < r; i++)
		{
			if (j&(1<<i))
			{
				p+=1<<(r-i-1);
			}
		}
		FD[j]=X1[p];
	}
	
	delete W;
	delete X1;
	delete X2;
}

/*************************************************************************
 * �������ƣ�
 *   IFFT()
 * ����:
 *   complex<double> * FD	- ָ��Ƶ��ֵ��ָ��
 *   complex<double> * TD	- ָ��ʱ��ֵ��ָ��
 *   r						��2������
 * ����ֵ:
 *   �ޡ�
 * ˵��:
 *   �ú�������ʵ�ֿ��ٸ���Ҷ���任��
 ************************************************************************/
VOID CDibImage::IFFT(complex<double> * FD, complex<double> * TD, int r)
{	
	LONG	count;					// ����Ҷ�任����	
	int		i;						// ѭ������	
	complex<double> *X;	
	
	count = 1 << r;					// ���㸶��Ҷ�任����
	X = new complex<double>[count];	// ������������洢��
	memcpy(X, FD, sizeof(complex<double>) * count);	// ��Ƶ���д��X
	
	// ����
	for(i = 0; i < count; i++)
	{
		X[i] = complex<double> (X[i].real(), -X[i].imag());
	}
	
	FFT(X, TD, r);					// ���ÿ��ٸ���Ҷ�任
	
	// ��ʱ���Ĺ���
	for(i = 0; i < count; i++)
	{
		TD[i] = complex<double> (TD[i].real() / count, -TD[i].imag() / count);
	}
	
	delete X;
}

/*************************************************************************
 * �������ƣ�
 *   Fourier()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ����и���Ҷ�任��
 ************************************************************************/
BOOL CDibImage::Fourier(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	unsigned char*	lpSrc;	// ָ��Դͼ���ָ��
	double	dTemp;	
	LONG	i;				// ѭ������
	LONG	j;	
	LONG	w;				// ���и���Ҷ�任�Ŀ�ȣ�2�������η���
	LONG	h;				// ���и���Ҷ�任�ĸ߶ȣ�2�������η���	
	int		wp;
	int		hp;
		
	LONG	lLineBytes;		// ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);	// ����ͼ��ÿ�е��ֽ���
	
	// ����ֵ
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// ������и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ�2�������η���
	while(w * 2 <= lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h * 2 <= lHeight)
	{
		h *= 2;
		hp++;
	}
	
	complex<double> *TD = new complex<double>[w * h];
	complex<double> *FD = new complex<double>[w * h];
		
	for(i = 0; i < h; i++)			// ��
	{		
		for(j = 0; j < w; j++)		// ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// ��ʱ��ֵ
			TD[j + w * i] = complex<double>(*(lpSrc), 0);
		}
	}
	
	for(i = 0; i < h; i++)
	{
		// ��y������п��ٸ���Ҷ�任
		FFT(&TD[w * i], &FD[w * i], wp);
	}
	
	// ����任���
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			TD[i + h * j] = FD[j + w * i];
		}
	}
	
	for(i = 0; i < w; i++)
	{
		// ��x������п��ٸ���Ҷ�任
		FFT(&TD[i * h], &FD[i * h], hp);
	}
		
	for(i = 0; i < h; i++)			// ��
	{		
		for(j = 0; j < w; j++)		// ��
		{
			// ����Ƶ��
			dTemp = sqrt(FD[j * h + i].real() * FD[j * h + i].real() + 
				         FD[j * h + i].imag() * FD[j * h + i].imag()) / 100;
			if (dTemp > 255)
			{
				dTemp = 255;
			}
			
			// ָ��DIB��(i<h/2 ? i+h/2 : i-h/2)�У���(j<w/2 ? j+w/2 : j-w/2)��
			// ���ص�ָ�룬�˴���ֱ��ȡi��j����Ϊ�˽��任���ԭ���Ƶ�����
			// lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight-1-i) + j;
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * 
				(lHeight - 1 - (i<h/2 ? i+h/2 : i-h/2)) + (j<w/2 ? j+w/2 : j-w/2);
			
			// ����Դͼ��
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete TD;
	delete FD;
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   DCT()
 * ����:
 *   double * f				- ָ��ʱ��ֵ��ָ��
 *   double * F				- ָ��Ƶ��ֵ��ָ��
 *   r						��2������
 * ����ֵ:
 *   �ޡ�
 * ˵��:
 *   �ú�������ʵ�ֿ�����ɢ���ұ任���ú�������2N��Ŀ��ٸ���Ҷ�任
 * ��ʵ����ɢ���ұ任��
 ************************************************************************/
VOID CDibImage::DCT(double *f, double *F, int r)
{	
	LONG	count;			// ��ɢ���ұ任����	
	int		i;				// ѭ������	
	double	dTemp;	
	complex<double> *X;
		
	count = 1<<r;			// ������ɢ���ұ任����	

	X = new complex<double>[count*2];	
	memset(X, 0, sizeof(complex<double>) * count * 2);	// ����ֵΪ0
	
	// ��ʱ���д������X
	for(i=0;i<count;i++)
	{
		X[i] = complex<double> (f[i], 0);
	}
		
	FFT(X,X,r+1);				// ���ÿ��ٸ���Ҷ�任		
	dTemp = 1/sqrt((float)count);		// ����ϵ��		
	F[0] = X[0].real() * dTemp;	// ��F[0]	
	dTemp *= sqrt((float)2);
	
	// ��F[u]	
	for(i = 1; i < count; i++)
	{
		F[i]=(X[i].real() * cos(i*PI/(count*2)) + X[i].imag() * 
			sin(i*PI/(count*2))) * dTemp;
	}
	
	delete X;
}

/*************************************************************************
 * �������ƣ�
 *   IDCT()
 * ����:
 *   double * F				- ָ��Ƶ��ֵ��ָ��
 *   double * f				- ָ��ʱ��ֵ��ָ��
 *   r						��2������
 * ����ֵ:
 *   �ޡ�
 * ˵��:
 *   �ú�������ʵ�ֿ�����ɢ���ҷ��任���ú���Ҳ����2N��Ŀ��ٸ���Ҷ�任
 * ��ʵ����ɢ���ҷ��任��
 ************************************************************************/
VOID CDibImage::IDCT(double *F, double *f, int r)
{
	LONG	count;			// ��ɢ���ҷ��任����
	int		i;				// ѭ������
	double	dTemp, d0;	
	complex<double> *X;
		
	count = 1<<r;			// ������ɢ���ұ任����

	X = new complex<double>[count*2];	
	memset(X, 0, sizeof(complex<double>) * count * 2);	// ����ֵΪ0
	
	// ��Ƶ��任���д������X
	for(i=0;i<count;i++)
	{
		X[i] = complex<double> (F[i] * cos(i*PI/(count*2)), F[i] * 
			sin(i*PI/(count*2)));
	}
	
	IFFT(X,X,r+1);		// ���ÿ��ٸ���Ҷ���任
	
	// ����ϵ��
	dTemp = sqrt(2.0/count);
	d0 = (sqrt(1.0/count) - dTemp) * F[0];
	
	for(i = 0; i < count; i++)
	{
		f[i] = d0 + X[i].real()* dTemp * 2 * count;
	}
	
	delete X;
}

/*************************************************************************
 * �������ƣ�
 *   DIBDct()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ�������ɢ���ұ任��
 ************************************************************************/
BOOL CDibImage::DIBDct(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	unsigned char*	lpSrc;	// ָ��Դͼ���ָ��	
	LONG	i;				// ѭ������
	LONG	j;	
	LONG	w;				// ���и���Ҷ�任�Ŀ�ȣ�2�������η���
	LONG	h;				// ���и���Ҷ�任�ĸ߶ȣ�2�������η���
	double	dTemp;	
	int		wp;
	int		hp;
		
	LONG	lLineBytes;		// ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// ����ֵ
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// ���������ɢ���ұ任�Ŀ�Ⱥ͸߶ȣ�2�������η���
	while(w * 2 <= lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h * 2 <= lHeight)
	{
		h *= 2;
		hp++;
	}
	
	double *f = new double[w * h];
	double *F = new double[w * h];
		
	for(i = 0; i < h; i++)			// ��
	{		
		for(j = 0; j < w; j++)		// ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// ��ʱ��ֵ
			f[j + i * w] = *(lpSrc);
		}
	}
	
	for(i = 0; i < h; i++)
	{
		// ��y���������ɢ���ұ任
		DCT(&f[w * i], &F[w * i], wp);
	}
	
	// ���������
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			f[j * h + i] = F[j + w * i];
		}
	}
	
	for(j = 0; j < w; j++)
	{
		// ��x���������ɢ���ұ任
		DCT(&f[j * h], &F[j * h], hp);
	}	
	
	for(i = 0; i < h; i++)			// ��
	{		
		for(j = 0; j < w; j++)		// ��
		{
			dTemp = fabs(F[j*h+i]);	// ����Ƶ��
			
			// �ж��Ƿ񳬹�255
			if (dTemp > 255)
			{
				dTemp = 255;
			}
			
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// ����Դͼ��
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete f;
	delete F;

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   WALSH()
 * ����:
 *   double * f				- ָ��ʱ��ֵ��ָ��
 *   double * F				- ָ��Ƶ��ֵ��ָ��
 *   r						��2������
 * ����ֵ:
 *   �ޡ�
 * ˵��:
 *   �ú�������ʵ�ֿ����ֶ�ʲ-������任��
 ************************************************************************/
VOID CDibImage::WALSH(double *f, double *F, int r)
{	
	LONG	count;				// �ֶ�ʲ-������任����	
	int		i,j,k;				// ѭ������
	int		bfsize,p;	
	double *X1,*X2,*X;
		
	count = 1 << r;				// ��������ֶ�ʲ�任����	
	X1 = new double[count];		// �����������������
	X2 = new double[count];		// �����������������
		
	memcpy(X1, f, sizeof(double) * count);	// ��ʱ���д������X1
	
	// ��������
	for(k = 0; k < r; k++)
	{
		for(j = 0; j < 1<<k; j++)
		{
			bfsize = 1 << (r-k);
			for(i = 0; i < bfsize / 2; i++)
			{
				p = j * bfsize;
				X2[i + p] = X1[i + p] + X1[i + p + bfsize / 2];
				X2[i + p + bfsize / 2] = X1[i + p] - X1[i + p + bfsize / 2];
			}
		}
		
		// ����X1��X2  
		X = X1;
		X1 = X2;
		X2 = X;
	}
	
	// ����ϵ��
	for(j = 0; j < count; j++)
	{
		p = 0;
		for(i = 0; i < r; i++)
		{
			if (j & (1<<i))
			{
				p += 1 << (r-i-1);
			}
		}

		F[j] = X1[p] / count;
	}
	
	delete X1;
	delete X2;
}

/*************************************************************************
 * �������ƣ�
 *   IWALSH()
 * ����:
 *   double * F				- ָ��Ƶ��ֵ��ָ��
 *   double * f				- ָ��ʱ��ֵ��ָ��
 *   r						��2������
 * ����ֵ:
 *   �ޡ�
 * ˵��:
 *   �ú�������ʵ�ֿ����ֶ�ʲ-�����귴�任��
 ************************************************************************/
VOID CDibImage::IWALSH(double *F, double *f, int r)
{	
	LONG	count;				// �任����	
	int		i;					// ѭ������
		
	count = 1 << r;				// ����任����	
	WALSH(F, f, r);				// ���ÿ����ֶ�ʲ��������任���з��任
		
	for(i = 0; i < count; i++)	// ����ϵ��
	{
		f[i] *= count;
	}
}

/*************************************************************************
 * �������ƣ�
 *   DIBWalsh()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ������ֶ�ʲ-������任���������ȶ�ͼ��ÿ�н���һά
 * �ֶ�ʲ��������任��Ȼ��Ա任�����ÿ�н���һά�ֶ�ʲ��������任��
 ************************************************************************/
BOOL CDibImage::DIBWalsh(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	unsigned char*	lpSrc;		// ָ��Դͼ���ָ��	
	LONG	i,j;				// ѭ������
	LONG	w;					// ���и���Ҷ�任�Ŀ�ȣ�2�������η���
	LONG	h;					// ���и���Ҷ�任�ĸ߶ȣ�2�������η���
	double	dTemp;	
	int		wp;
	int		hp;
	
	LONG	lLineBytes;			// ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// ����ֵ
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// ���������ɢ���ұ任�Ŀ�Ⱥ͸߶ȣ�2�������η���
	while(w * 2 <= lWidth)
	{
		w *= 2;
		wp++;
	}	
	while(h * 2 <= lHeight)
	{
		h *= 2;
		hp++;
	}
	
	double *f = new double[w * h];
	double *F = new double[w * h];
		
	for(i = 0; i < h; i++)			// ��
	{		
		for(j = 0; j < w; j++)		// ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// ��ʱ��ֵ
			f[j + i * w] = *(lpSrc);
		}
	}
	
	for(i = 0; i < h; i++)
	{
		// ��y��������ֶ�ʲ-������任
		WALSH(f + w * i, F + w * i, wp);
	}
	
	// ���������
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			f[j * h + i] = F[j + w * i];
		}
	}
	
	for(j = 0; j < w; j++)
	{
		// ��x��������ֶ�ʲ-������任
		WALSH(f + j * h, F + j * h, hp);
	}
		
	for(i = 0; i < h; i++)			// ��
	{		
		for(j = 0; j < w; j++)		// ��
		{			
			dTemp = fabs(F[j * h + i] * 1000);	// ����Ƶ��			
			if (dTemp > 255)			
			{					
				dTemp = 255;				
			}
			
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// ����Դͼ��
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete f;
	delete F;

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   DIBWalsh1()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ������ֶ�ʲ-������任�������治ͬ���ǣ��˴��ǽ���ά
 * ����ת����һ����������Ȼ��Ը�����������һ��һά�ֶ�ʲ-������任��
 ************************************************************************/
BOOL CDibImage::DIBWalsh1(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	unsigned char*	lpSrc;		// ָ��Դͼ���ָ��
	LONG	i,j;				// ѭ������	
	LONG	w;					// ���и���Ҷ�任�Ŀ�ȣ�2�������η���
	LONG	h;					// ���и���Ҷ�任�ĸ߶ȣ�2�������η���
	double	dTemp;	
	int		wp;
	int		hp;
		
	LONG	lLineBytes;			// ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// ����ֵ
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// ���������ɢ���ұ任�Ŀ�Ⱥ͸߶ȣ�2�������η���
	while(w * 2 <= lWidth)
	{
		w *= 2;
		wp++;
	}	
	while(h * 2 <= lHeight)
	{
		h *= 2;
		hp++;
	}
	
	double *f = new double[w * h];
	double *F = new double[w * h];
		
	for(i = 0; i < w; i++)			// ��
	{
		for(j = 0; j < h; j++)		// ��
		{
			// ָ��DIB��j�У���i�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j) + i;
			
			// ��ʱ��ֵ
			f[j + i * w] = *(lpSrc);
		}
	}
	
	// ���ÿ����ֶ�ʲ��������任
	WALSH(f, F, wp + hp);
		
	for(i = 0; i < w; i++)			// ��
	{		
		for(j = 0; j < h; j++)		// ��
		{			
			dTemp = fabs(F[i * w + j] * 1000);	// ����Ƶ��
			if (dTemp > 255)
			{
				dTemp = 255;
			}
			
			// ָ��DIB��j�У���i�����ص�ָ��
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j) + i;
			
			// ����Դͼ��
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete f;
	delete F;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//	ͼ��ģ��任����
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * �������ƣ�
 *   Template()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   int   iTempH		- ģ��ĸ߶�
 *   int   iTempW		- ģ��Ŀ��
 *   int   iTempMX		- ģ�������Ԫ��X���� ( < iTempW - 1)
 *   int   iTempMY		- ģ�������Ԫ��Y���� ( < iTempH - 1)
 *	 FLOAT * fpArray	- ָ��ģ�������ָ��
 *	 FLOAT fCoef		- ģ��ϵ��
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�����ָ����ģ�壨�����С������ͼ����в���������iTempHָ��ģ��
 * �ĸ߶ȣ�����iTempWָ��ģ��Ŀ�ȣ�����iTempMX��iTempMYָ��ģ�������
 * Ԫ�����꣬����fpArrayָ��ģ��Ԫ�أ�fCoefָ��ϵ����
 ************************************************************************/
BOOL CDibImage::Template(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
					 int iTempH, int iTempW, int iTempMX, int iTempMY,
					 FLOAT * fpArray, FLOAT fCoef)
{	
	LPSTR	lpNewDIBBits;		// ָ����ͼ���ָ��
	HLOCAL	hNewDIBBits;	
	unsigned char*	lpSrc;		// ָ��Դͼ���ָ��	
	unsigned char*	lpDst;		// ָ��Ҫ���������ָ��	
	LONG	i,j,k,l;			// ѭ������	
	FLOAT	fResult;			// ������
		
	LONG lLineBytes;			// ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// ��ʼ��ͼ��Ϊԭʼͼ��
	memcpy(lpNewDIBBits, lpDIBBits, lLineBytes * lHeight);
		
	for(i = iTempMY; i < lHeight - iTempH + iTempMY + 1; i++)	// ��(��ȥ��Ե����)
	{		
		for(j = iTempMX; j < lWidth - iTempW + iTempMX + 1; j++)// ��(��ȥ��Ե����)
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			fResult = 0;

			for (k = 0; k < iTempH; k++)
			{
				for (l = 0; l < iTempW; l++)
				{
					// ָ��DIB��i - iTempMY + k�У���j - iTempMX + l�����ص�ָ��
					lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 
						- i + iTempMY - k) + j - iTempMX + l;
					
					// ��������ֵ
					fResult += (* lpSrc) * fpArray[k * iTempW + l];
				}
			}
						
			fResult *= fCoef;					// ����ϵ��			
			fResult = (FLOAT ) fabs(fResult);	// ȡ����ֵ
			if(fResult > 255)
			{
				* lpDst = 255;
			}
			else
			{
				* lpDst = (unsigned char) (fResult + 0.5);
			}			
		}
	}
	
	// ���Ʊ任���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   MedianFilter()
 * ����:
 *   LPSTR lpDIBBits		- ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth			- Դͼ���ȣ���������
 *   LONG  lHeight			- Դͼ��߶ȣ���������
 *   int   iFilterH			- �˲����ĸ߶�
 *   int   iFilterW			- �˲����Ŀ��
 *   int   iFilterMX		- �˲���������Ԫ��X����
 *   int   iFilterMY		- �˲���������Ԫ��Y����
 * ����ֵ:
 *   BOOL					- �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�����DIBͼ�������ֵ�˲���
 ************************************************************************/
BOOL CDibImage::MedianFilter(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						 int iFilterH, int iFilterW, int iFilterMX, int iFilterMY)
{		
	unsigned char*	lpSrc;			// ָ��Դͼ���ָ��	
	unsigned char*	lpDst;			// ָ��Ҫ���������ָ��	
	LPSTR			lpNewDIBBits;	// ָ����ͼ���ָ��
	HLOCAL			hNewDIBBits;	
	unsigned char	* aValue;		// ָ���˲��������ָ��
	HLOCAL			hArray;	
	LONG			i,j,k,l;		// ѭ������	
	
	LONG			lLineBytes;		// ͼ��ÿ�е��ֽ���	
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// ��ʼ��ͼ��Ϊԭʼͼ��
	memcpy(lpNewDIBBits, lpDIBBits, lLineBytes * lHeight);
	
	// ��ʱ�����ڴ棬�Ա����˲�������
	hArray = LocalAlloc(LHND, iFilterH * iFilterW);
	if (hArray == NULL)
	{
		LocalUnlock(hNewDIBBits);
		LocalFree(hNewDIBBits);

		return FALSE;
	}
	
	aValue = (unsigned char * )LocalLock(hArray);
	
	// ��ʼ��ֵ�˲�
	// ��(��ȥ��Ե����)
	for(i = iFilterMY; i < lHeight - iFilterH + iFilterMY + 1; i++)
	{
		// ��(��ȥ��Ե����)
		for(j = iFilterMX; j < lWidth - iFilterW + iFilterMX + 1; j++)
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// ��ȡ�˲�������
			for (k = 0; k < iFilterH; k++)
			{
				for (l = 0; l < iFilterW; l++)
				{
					// ָ��DIB��i - iFilterMY + k�У���j - iFilterMX + l�����ص�ָ��
					lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 
						- i + iFilterMY - k) + j - iFilterMX + l;
					
					// ��������ֵ
					aValue[k * iFilterW + l] = *lpSrc;
				}
			}
			
			// ��ȡ��ֵ
			* lpDst = GetMedianNum(aValue, iFilterH * iFilterW);
		}
	}
	
	// ���Ʊ任���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	LocalUnlock(hArray);
	LocalFree(hArray);
	
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   GetMedianNum()
 * ����:
 *   unsigned char * bpArray	- ָ��Ҫ��ȡ��ֵ������ָ��
 *   int   iFilterLen			- ���鳤��
 * ����ֵ:
 *   unsigned char      - ����ָ���������ֵ��
 * ˵��:
 *   �ú�����ð�ݷ���һά����������򣬲���������Ԫ�ص���ֵ��
 ************************************************************************/
unsigned char CDibImage::GetMedianNum(unsigned char * bArray, int iFilterLen)
{
	int		i,j;			// ѭ������
	unsigned char bTemp;
	
	// ��ð�ݷ��������������
	for (j = 0; j < iFilterLen - 1; j ++)
	{
		for (i = 0; i < iFilterLen - j - 1; i ++)
		{
			if (bArray[i] > bArray[i + 1])
			{
				// ����
				bTemp = bArray[i];
				bArray[i] = bArray[i + 1];
				bArray[i + 1] = bTemp;
			}
		}
	}
	
	// ������ֵ
	if ((iFilterLen & 1) > 0)
	{
		// ������������Ԫ�أ������м�һ��Ԫ��
		bTemp = bArray[(iFilterLen + 1) / 2];
	}
	else
	{
		// ������ż����Ԫ�أ������м�����Ԫ��ƽ��ֵ
		bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
	}
	
	return bTemp;
}

/*************************************************************************
 * �������ƣ�
 *   GradSharp()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   BYTE  bThre		- ��ֵ
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú���������ͼ������ݶ��񻯡�
 ************************************************************************/
BOOL CDibImage::GradSharp(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bThre)
{	
			
	unsigned char*	lpSrc;		// ָ��Դͼ���ָ��
	unsigned char*	lpSrc1;
	unsigned char*	lpSrc2;	
	LONG	i,j;				// ѭ������
	LONG	lLineBytes;			// ͼ��ÿ�е��ֽ���
	BYTE	bTemp;
	
	lLineBytes = WIDTHBYTES(lWidth * 8);
		
	for(i = 0; i < lHeight; i++)		// ÿ��
	{		
		for(j = 0; j < lWidth; j++)		// ÿ��
		{
			// ָ��DIB��i�У���j�����ص�ָ��
			lpSrc  = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			// ָ��DIB��i+1�У���j�����ص�ָ��
			lpSrc1 = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 2 - i) + j;			
			// ָ��DIB��i�У���j+1�����ص�ָ��
			lpSrc2 = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) 
				+ j + 1;
			
			bTemp = abs((*lpSrc)-(*lpSrc1)) + abs((*lpSrc)-(*lpSrc2));
			
			// �ж��Ƿ�С����ֵ
			if (bTemp < 255)
			{
				// �ж��Ƿ������ֵ������С��������Ҷ�ֵ���䡣
				if (bTemp >= bThre)
				{
					*lpSrc = bTemp;
				}
			}
			else
			{
				*lpSrc = 255;
			}
		}
	}
	
	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   ReplaceColorPal()
 * ����:
 *   LPSTR lpDIB			- ָ��ԴDIBͼ��ָ��
 *   BYTE * bpColorsTable	- α��ɫ�����
 * ����ֵ:
 *   BOOL					- �ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 *   �ú�����ָ����α��ɫ��������滻ͼ��ĵ��԰壬����bpColorsTable
 * ָ��Ҫ�滻��α��ɫ�����
 ************************************************************************/
BOOL CDibImage::ReplaceColorPal(LPSTR lpDIB, BYTE * bpColorsTable)
{	
	int i;					// ѭ������	
	WORD wNumColors;		// ��ɫ���е���ɫ��Ŀ	
	LPBITMAPINFO lpbmi;		// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��		
	LPBITMAPCOREINFO lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
	BOOL bWinStyleDIB;		// �����Ƿ���Win3.0 DIB�ı��	
	BOOL bResult = FALSE;	// �������	

	lpbmi = (LPBITMAPINFO)lpDIB;		// ��ȡָ��BITMAPINFO�ṹ��ָ�루Win3.0��	
	lpbmc = (LPBITMAPCOREINFO)lpDIB;	// ��ȡָ��BITMAPCOREINFO�ṹ��ָ��
	wNumColors = DIBNumColors(lpDIB);	// ��ȡDIB����ɫ���е���ɫ��Ŀ
		
	if (wNumColors == 256)				// �ж���ɫ��Ŀ�Ƿ���256ɫ
	{		
		bWinStyleDIB = IS_WIN30_DIB(lpDIB);	// �ж��Ƿ���WIN3.0��DIB
		
		// ��ȡα��ɫ���룬����DIB��ɫ��
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				// ����DIB��ɫ���ɫ��ɫ��ɫ����
				lpbmi->bmiColors[i].rgbRed = bpColorsTable[i * 4];
				lpbmi->bmiColors[i].rgbGreen = bpColorsTable[i * 4 + 1];
				lpbmi->bmiColors[i].rgbBlue = bpColorsTable[i * 4 + 2];
				
				// ����DIB��ɫ�屣��λ
				lpbmi->bmiColors[i].rgbReserved = 0;
			}
			else
			{
				// ����DIB��ɫ���ɫ��ɫ��ɫ����
				lpbmc->bmciColors[i].rgbtRed = bpColorsTable[i * 4];
				lpbmc->bmciColors[i].rgbtGreen = bpColorsTable[i * 4 + 1];
				lpbmc->bmciColors[i].rgbtBlue = bpColorsTable[i * 4 + 2];
			}
		}
	}
	
	return bResult;	
}

//////////////////////////////////////////////////////////////////////
//  ͼ����̬ѧ�任����
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * �������ƣ�
 *   ErosiontionDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   int   nMode		- ��ʴ��ʽ��0��ʾˮƽ����1��ֱ����2�Զ���ṹԪ�ء�
 *	 int   structure[3][3]	- �Զ����3��3�ṹԪ�ء�
 * ����ֵ:
 *   BOOL               - ��ʴ�ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ����и�ʴ���㡣�ṹԪ��Ϊˮƽ�����ֱ����������㣬
 * �м��λ��ԭ�㣻�������û��Լ�����3��3�ĽṹԪ�ء�
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::ErosionDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						   int nMode , int structure[3][3])
{	
	LPSTR	lpSrc;			// ָ��Դͼ���ָ��	
	LPSTR	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;	
	long i,j,n,m;			//ѭ������
	unsigned char pixel;	//����ֵ

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//ʹ��ˮƽ����ĽṹԪ�ؽ��и�ʴ
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}
				
				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɺ�ɫ
				*lpDst = (unsigned char)0;

				//���Դͼ���е�ǰ���������������һ���㲻�Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɰ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+n-1);
					if (pixel == 255 )
					{
						*lpDst = (unsigned char)255;
						break;
					}
				}				
			}
		}
	}
	else if(nMode == 1)
	{
		//ʹ�ô�ֱ����ĽṹԪ�ؽ��и�ʴ
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ��������ϱߺ����±�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɺ�ɫ
				*lpDst = (unsigned char)0;

				//���Դͼ���е�ǰ���������������һ���㲻�Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɰ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+(n-1)*lWidth);
					if (pixel == 255 )
					{
						*lpDst = (unsigned char)255;
						break;
					}
				}			
			}
		}
	}
	else
	{
		//ʹ���Զ���ĽṹԪ�ؽ��и�ʴ
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��3��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ���������غ����ϱߺ����±ߵ���������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɺ�ɫ
				*lpDst = (unsigned char)0;

				//���ԭͼ���ж�Ӧ�ṹԪ����Ϊ��ɫ����Щ������һ�����Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɰ�ɫ
				//ע����DIBͼ�������������µ��õ�
				for (m = 0;m < 3;m++ )
				{
					for (n = 0;n < 3;n++)
					{
						if( structure[m][n] == -1)
							continue;
						pixel = *(lpSrc + ((2-m)-1)*lWidth + (n-1));
						if (pixel == 255 )
						{	
							*lpDst = (unsigned char)255;
							break;
						}
					}
				}			
			}
		}
	}
	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   DilationDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   int   nMode		- ���ͷ�ʽ��0��ʾˮƽ����1��ֱ����2�Զ���ṹԪ�ء�
 *	 int   structure[3][3]	- �Զ����3��3�ṹԪ�ء�
 * ����ֵ:
 *   BOOL               - ���ͳɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ������������㡣�ṹԪ��Ϊˮƽ�����ֱ����������㣬
 * �м��λ��ԭ�㣻�������û��Լ�����3��3�ĽṹԪ�ء�
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::DilationDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, int nMode, 
							int structure[3][3])
{	
	LPSTR	lpSrc;			// ָ��Դͼ���ָ��	
	LPSTR	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;	
	long i,j,m,n;			//ѭ������	
	unsigned char pixel;	//����ֵ

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//ʹ��ˮƽ����ĽṹԪ�ؽ�������
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && pixel != 0)
				{
					return FALSE;
				}
				
				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɰ�ɫ
				*lpDst = (unsigned char)255;

				//Դͼ���е�ǰ�������������ֻҪ��һ�����Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɺ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+n-1);
					if (pixel == 0 )
					{
						*lpDst = (unsigned char)0;
						break;
					}
				}				
			}
		}
	}
	else if(nMode == 1)
	{
		//ʹ�ô�ֱ����ĽṹԪ�ؽ�������
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ��������ϱߺ����±�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɰ�ɫ
				*lpDst = (unsigned char)255;

				//Դͼ���е�ǰ�������������ֻҪ��һ�����Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɺ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+(n-1)*lWidth);
					if (pixel == 0 )
					{
						*lpDst = (unsigned char)0;
						break;
					}
				}			
			}
		}
	}
	else
	{
		//ʹ���Զ���ĽṹԪ�ؽ�������
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��3��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ���������غ����ϱߺ����±ߵ���������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɰ�ɫ
				*lpDst = (unsigned char)255;

				//ԭͼ���ж�Ӧ�ṹԪ����Ϊ��ɫ����Щ����ֻҪ��һ���Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɺ�ɫ
				//ע����DIBͼ�������������µ��õ�
				for (m = 0;m < 3;m++ )
				{
					for (n = 0;n < 3;n++)
					{
						if( structure[m][n] == -1)
							continue;
						pixel = *(lpSrc + ((2-m)-1)*lWidth + (n-1));
						if (pixel == 0 )
						{	
							*lpDst = (unsigned char)0;
							break;
						}
					}
				}				
			}
		}
	}
	// �������ͺ��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   OpenDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   int   nMode		- �����㷽ʽ��0��ʾˮƽ����1��ֱ����2�Զ���ṹԪ�ء�
 *	 int   structure[3][3]	- �Զ����3��3�ṹԪ�ء�
 * ����ֵ:
 *   BOOL               - ������ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ����п����㡣�ṹԪ��Ϊˮƽ�����ֱ����������㣬
 * �м��λ��ԭ�㣻�������û��Լ�����3��3�ĽṹԪ�ء�
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::OpenDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, int nMode , 
						int structure[3][3])
{		
	LPSTR	lpSrc;			// ָ��Դͼ���ָ��	
	LPSTR	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;
	long i,j,m,n;			//ѭ������	
	unsigned char pixel;	//����ֵ

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//ʹ��ˮƽ����ĽṹԪ�ؽ��и�ʴ
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}
				
				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɺ�ɫ
				*lpDst = (unsigned char)0;

				//���Դͼ���е�ǰ���������������һ���㲻�Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɰ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+n-1);
					if (pixel == 255 )
					{
						*lpDst = (unsigned char)255;
						break;
					}
				}				
			}
		}
	}
	else if(nMode == 1)
	{
		//ʹ�ô�ֱ����ĽṹԪ�ؽ��и�ʴ
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ��������ϱߺ����±�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɺ�ɫ
				*lpDst = (unsigned char)0;

				//���Դͼ���е�ǰ���������������һ���㲻�Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɰ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+(n-1)*lWidth);
					if (pixel == 255 )
					{
						*lpDst = (unsigned char)255;
						break;
					}
				}				
			}
		}
	}
	else
	{
		//ʹ���Զ���ĽṹԪ�ؽ��и�ʴ
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��3��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ���������غ����ϱߺ����±ߵ���������
				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɺ�ɫ
				*lpDst = (unsigned char)0;

				//���ԭͼ���ж�Ӧ�ṹԪ����Ϊ��ɫ����Щ������һ�����Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɰ�ɫ
				//ע����DIBͼ�������������µ��õ�
				for (m = 0;m < 3;m++ )
				{
					for (n = 0;n < 3;n++)
					{
						if( structure[m][n] == -1)
							continue;
						pixel = *(lpSrc + ((2-m)-1)*lWidth + (n-1));
						if (pixel == 255 )
						{	
							*lpDst = (unsigned char)255;
							break;
						}
					}
				}			
			}
		}
	}
	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	// ���³�ʼ���·�����ڴ棬�趨��ʼֵΪ255
	//lpDst = (char *)lpNewDIBBits;
	//memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//ʹ��ˮƽ����ĽṹԪ�ؽ�������
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}
				
				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɰ�ɫ
				*lpDst = (unsigned char)255;

				//Դͼ���е�ǰ�������������ֻҪ��һ�����Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɺ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+n-1);
					if (pixel == 0 )
					{
						*lpDst = (unsigned char)0;
						break;
					}
				}			
			}
		}
	}
	else if(nMode == 1)
	{
		//ʹ�ô�ֱ����ĽṹԪ�ؽ�������
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��1��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ��������ϱߺ����±�
				// ����������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɰ�ɫ
				*lpDst = (unsigned char)255;

				//Դͼ���е�ǰ�������������ֻҪ��һ�����Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɺ�ɫ
				for (n = 0;n < 3;n++ )
				{
					pixel = *(lpSrc+(n-1)*lWidth);
					if (pixel == 0 )
					{
						*lpDst = (unsigned char)0;
						break;
					}
				}				
			}
		}
	}
	else
	{
		//ʹ���Զ���ĽṹԪ�ؽ�������
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// ����ʹ��3��3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ���������ߺ����ұ�
				// ���������غ����ϱߺ����±ߵ���������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//Ŀ��ͼ���еĵ�ǰ���ȸ��ɰ�ɫ
				*lpDst = (unsigned char)255;

				//ԭͼ���ж�Ӧ�ṹԪ����Ϊ��ɫ����Щ����ֻҪ��һ���Ǻ�ɫ��
				//��Ŀ��ͼ���еĵ�ǰ�㸳�ɺ�ɫ
				//ע����DIBͼ�������������µ��õ�
				for (m = 0;m < 3;m++ )
				{
					for (n = 0;n < 3;n++)
					{
						if( structure[m][n] == -1)
							continue;
						pixel = *(lpSrc + ((2-m)-1)*lWidth + (n-1));
						if (pixel == 0 )
						{	
							*lpDst = (unsigned char)0;
							break;
						}
					}
				}			
			}
		}
	}
	// �������ͺ��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   CloseDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 *   int   nMode		- �����㷽ʽ��0��ʾˮƽ����1��ֱ����2�Զ���ṹԪ�ء�
 *	 int   structure[3][3]	- �Զ����3��3�ṹԪ�ء�
 * ����ֵ:
 *   BOOL               - ������ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ����п����㡣�ṹԪ��Ϊˮƽ�����ֱ����������㣬
 * �м��λ��ԭ�㣻�������û��Լ�����3��3�ĽṹԪ�ء�
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::CloseDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, int nMode , 
						 int structure[3][3])
{
	if (DilationDIB(lpDIBBits, lWidth, lHeight, nMode , structure))
	{
		if (ErosionDIB(lpDIBBits, lWidth, lHeight, nMode , structure))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*************************************************************************
 * �������ƣ�
 *   ThinDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ������ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ�����ϸ�����㡣Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::ThiningDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;			// ָ��Դͼ���ָ��	
	LPSTR	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;	
	BOOL bModified;			//����	
	long i,j,m,n;			//ѭ������

	//�ĸ�����
	BOOL bCondition1;
	BOOL bCondition2;
	BOOL bCondition3;
	BOOL bCondition4;
	
	unsigned char nCount;	//������	
	unsigned char pixel;	//����ֵ	
	unsigned char neighbour[5][5];	//5��5������������ֵ

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	bModified = TRUE;
	while(bModified)
	{
		bModified = FALSE;
		// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
		lpDst = (char *)lpNewDIBBits;
		memset(lpDst, (BYTE)255, lWidth * lHeight);

		for(j = 2; j <lHeight-2; j++)
		{
			for(i = 2;i <lWidth-2; i++)
			{
				bCondition1 = FALSE;
				bCondition2 = FALSE;
				bCondition3 = FALSE;
				bCondition4 = FALSE;

				//����ʹ��5��5�ĽṹԪ�أ�Ϊ��ֹԽ�磬���Բ�������Χ�ļ��кͼ�������

				// ָ��Դͼ������j�У���i�����ص�ָ��			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
				pixel = (unsigned char)*lpSrc;

				//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
				if(pixel != 255 && *lpSrc != 0)
				{
					continue;
				}
				//���Դͼ���е�ǰ��Ϊ��ɫ��������
				else if(pixel == 255)
				{
					continue;
				}

				//��õ�ǰ�����ڵ�5��5����������ֵ����ɫ��0������ɫ��1����
				for (m = 0;m < 5;m++ )
				{
					for (n = 0;n < 5;n++)
					{
						neighbour[m][n] =(255 - (unsigned char)*(lpSrc + 
							((4 - m) - 2)*lWidth + n - 2 )) / 255;
					}
				}

				//����ж�������
				//�ж�2<=NZ(P1)<=6
				nCount =  neighbour[1][1] + neighbour[1][2] + neighbour[1][3] \
						+ neighbour[2][1] + neighbour[2][3] + \
						+ neighbour[3][1] + neighbour[3][2] + neighbour[3][3];
				if ( nCount >= 2 && nCount <=6)
				{
					bCondition1 = TRUE;
				}

				//�ж�Z0(P1)=1
				nCount = 0;
				if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
					nCount++;
				if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
					nCount++;
				if (neighbour[2][1] == 0 && neighbour[3][1] == 1)
					nCount++;
				if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
					nCount++;
				if (neighbour[3][2] == 0 && neighbour[3][3] == 1)
					nCount++;
				if (neighbour[3][3] == 0 && neighbour[2][3] == 1)
					nCount++;
				if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
					nCount++;
				if (neighbour[1][3] == 0 && neighbour[1][2] == 1)
					nCount++;
				if (nCount == 1)
					bCondition2 = TRUE;

				//�ж�P2*P4*P8=0 or Z0(p2)!=1
				if (neighbour[1][2]*neighbour[2][1]*neighbour[2][3] == 0)
				{
					bCondition3 = TRUE;
				}
				else
				{
					nCount = 0;
					if (neighbour[0][2] == 0 && neighbour[0][1] == 1)
						nCount++;
					if (neighbour[0][1] == 0 && neighbour[1][1] == 1)
						nCount++;
					if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
						nCount++;
					if (neighbour[2][1] == 0 && neighbour[2][2] == 1)
						nCount++;
					if (neighbour[2][2] == 0 && neighbour[2][3] == 1)
						nCount++;
					if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
						nCount++;
					if (neighbour[1][3] == 0 && neighbour[0][3] == 1)
						nCount++;
					if (neighbour[0][3] == 0 && neighbour[0][2] == 1)
						nCount++;
					if (nCount != 1)
						bCondition3 = TRUE;
				}

				//�ж�P2*P4*P6=0 or Z0(p4)!=1
				if (neighbour[1][2]*neighbour[2][1]*neighbour[3][2] == 0)
				{
					bCondition4 = TRUE;
				}
				else
				{
					nCount = 0;
					if (neighbour[1][1] == 0 && neighbour[1][0] == 1)
						nCount++;
					if (neighbour[1][0] == 0 && neighbour[2][0] == 1)
						nCount++;
					if (neighbour[2][0] == 0 && neighbour[3][0] == 1)
						nCount++;
					if (neighbour[3][0] == 0 && neighbour[3][1] == 1)
						nCount++;
					if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
						nCount++;
					if (neighbour[3][2] == 0 && neighbour[2][2] == 1)
						nCount++;
					if (neighbour[2][2] == 0 && neighbour[1][2] == 1)
						nCount++;
					if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
						nCount++;
					if (nCount != 1)
						bCondition4 = TRUE;
				}
				if(bCondition1 && bCondition2 && bCondition3 && bCondition4)
				{
					*lpDst = (unsigned char)255;
					bModified = TRUE;
				}
				else
				{
					*lpDst = (unsigned char)0;
				}
			}
		}
		// ���Ƹ�ʴ���ͼ��
		memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);
	}
	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// ͼ���Ե���������㺯��
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * �������ƣ�
 *   RobertDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ��Ե���ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú�����Robert��Ե������Ӷ�ͼ����б�Ե������㡣
 * Ҫ��Ŀ��ͼ��Ϊ�Ҷ�ͼ��
 ************************************************************************/
BOOL CDibImage::RobertDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	LPSTR	lpSrc;			// ָ��Դͼ���ָ��	
	LPSTR	lpDst;			// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;	
	long i,j;				//ѭ������	
	double result;			//����ֵ
	unsigned char pixel[4];

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	//ʹ��ˮƽ����ĽṹԪ�ؽ��и�ʴ
	for(j = lHeight-1; j > 0; j--)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			//����ʹ��2��2��ģ�壬Ϊ��ֹԽ�磬���Բ��������±ߺ����ұߵ���������

			// ָ��Դͼ���j�У���i�����ص�ָ��			
			lpSrc = (char *)lpDIBBits + lWidth * j + i;
			// ָ��Ŀ��ͼ���j�У���i�����ص�ָ��			
			lpDst = (char *)lpNewDIBBits + lWidth * j + i;

			//ȡ�õ�ǰָ�봦2*2���������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel[0] = (unsigned char)*lpSrc;
			pixel[1] = (unsigned char)*(lpSrc + 1);
			pixel[2] = (unsigned char)*(lpSrc - lWidth);
			pixel[3] = (unsigned char)*(lpSrc - lWidth + 1);

			//����Ŀ��ͼ���еĵ�ǰ��
			 result = sqrt((float)(( pixel[0] - pixel[3] )*( pixel[0] - pixel[3] ) + \
				  ( pixel[1] - pixel[2] )*( pixel[1] - pixel[2] )));
			*lpDst = (unsigned char)result;		
		}
	}

	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   SobelDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ��Ե���ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú�����Sobel��Ե������Ӷ�ͼ����б�Ե������㡣
 * Ҫ��Ŀ��ͼ��Ϊ�Ҷ�ͼ��
 ************************************************************************/
BOOL CDibImage::SobelDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	LPSTR	lpDst1;				// ָ�򻺴�ͼ���ָ��
	LPSTR	lpDst2;		
	LPSTR	lpNewDIBBits1;		// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;
	
	long i,j;					// ѭ������	
	int		iTempH;				// ģ��߶�	
	int		iTempW;				// ģ����	
	FLOAT	fTempC;				// ģ��ϵ��	
	int		iTempMX;			// ģ������Ԫ��X����	
	int		iTempMY;			// ģ������Ԫ��Y����	
	FLOAT aTemplate[9];			// ģ������

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// ����Դͼ�񵽻���ͼ����
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// ����Sobelģ�����
	iTempW = 3;
	iTempH = 3;
	fTempC = 1.0;
	iTempMX = 1;
	iTempMY = 1;
	aTemplate[0] = -1.0;
	aTemplate[1] = -2.0;
	aTemplate[2] = -1.0;
	aTemplate[3] = 0.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = 0.0;
	aTemplate[6] = 1.0;
	aTemplate[7] = 2.0;
	aTemplate[8] = 1.0;

	if (!Template(lpNewDIBBits1, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	// ����Sobelģ�����
	aTemplate[0] = -1.0;
	aTemplate[1] = 0.0;
	aTemplate[2] = 1.0;
	aTemplate[3] = -2.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = 2.0;
	aTemplate[6] = -1.0;
	aTemplate[7] = 0.0;
	aTemplate[8] = 1.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ���ƾ���ģ��������ͼ��Դͼ��
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   PrewittDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ��Ե���ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú�����Prewitt��Ե������Ӷ�ͼ����б�Ե������㡣
 * Ҫ��Ŀ��ͼ��Ϊ�Ҷ�ͼ��
 ************************************************************************/
BOOL CDibImage::PrewittDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpDst1;				// ָ�򻺴�ͼ���ָ��
	LPSTR	lpDst2;	
	LPSTR	lpNewDIBBits1;		// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;
	
	long i,j;					// ѭ������	
	int		iTempH;				// ģ��߶�	
	int		iTempW;				// ģ����	
	FLOAT	fTempC;				// ģ��ϵ��	
	int		iTempMX;			// ģ������Ԫ��X����
	int		iTempMY;			// ģ������Ԫ��Y����
	FLOAT aTemplate[9];			// ģ������

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// ����Դͼ�񵽻���ͼ����
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);


	// ����Prewittģ�����
	iTempW = 3;
	iTempH = 3;
	fTempC = 1.0;
	iTempMX = 1;
	iTempMY = 1;
	aTemplate[0] = -1.0;
	aTemplate[1] = -1.0;
	aTemplate[2] = -1.0;
	aTemplate[3] = 0.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = 0.0;
	aTemplate[6] = 1.0;
	aTemplate[7] = 1.0;
	aTemplate[8] = 1.0;

	if (!Template(lpNewDIBBits1, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	// ����Prewittģ�����
	aTemplate[0] = 1.0;
	aTemplate[1] = 0.0;
	aTemplate[2] = -1.0;
	aTemplate[3] = 1.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = -1.0;
	aTemplate[6] = 1.0;
	aTemplate[7] = 0.0;
	aTemplate[8] = -1.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ���ƾ���ģ��������ͼ��Դͼ��
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   KirschDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ��Ե���ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú�����kirsch��Ե������Ӷ�ͼ����б�Ե������㡣
 * Ҫ��Ŀ��ͼ��Ϊ�Ҷ�ͼ��
 ************************************************************************/
BOOL CDibImage::KirschDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpDst1;			// ָ�򻺴�ͼ���ָ��
	LPSTR	lpDst2;
	LPSTR	lpNewDIBBits1;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;

	long i,j;				// ѭ������	
	int		iTempH;			// ģ��߶�	
	int		iTempW;			// ģ����	
	FLOAT	fTempC;			// ģ��ϵ��	
	int		iTempMX;		// ģ������Ԫ��X����	
	int		iTempMY;		// ģ������Ԫ��Y����	
	FLOAT aTemplate[9];		// ģ������

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// ����Դͼ�񵽻���ͼ����
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// ����Kirschģ��1����
	iTempW = 3;
	iTempH = 3;
	fTempC = 1.0;
	iTempMX = 1;
	iTempMY = 1;
	aTemplate[0] = 5.0;
	aTemplate[1] = 5.0;
	aTemplate[2] = 5.0;
	aTemplate[3] = -3.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = -3.0;
	aTemplate[6] = -3.0;
	aTemplate[7] = -3.0;
	aTemplate[8] = -3.0;

	if (!Template(lpNewDIBBits1, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	// ����Kirschģ��2����
	aTemplate[0] = -3.0;
	aTemplate[1] = 5.0;
	aTemplate[2] = 5.0;
	aTemplate[3] = -3.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = 5.0;
	aTemplate[6] = -3.0;
	aTemplate[7] = -3.0;
	aTemplate[8] = -3.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ����Դͼ�񵽻���ͼ����
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// ����Kirschģ��3����
	aTemplate[0] = -3.0;
	aTemplate[1] = -3.0;
	aTemplate[2] = 5.0;
	aTemplate[3] = -3.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = 5.0;
	aTemplate[6] = -3.0;
	aTemplate[7] = -3.0;
	aTemplate[8] = 5.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ����Դͼ�񵽻���ͼ����
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// ����Kirschģ��4����
	aTemplate[0] = -3.0;
	aTemplate[1] = -3.0;
	aTemplate[2] = -3.0;
	aTemplate[3] = -3.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = 5.0;
	aTemplate[6] = -3.0;
	aTemplate[7] = 5.0;
	aTemplate[8] = 5.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ����Դͼ�񵽻���ͼ����
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// ����Kirschģ��5����
	aTemplate[0] = -3.0;
	aTemplate[1] = -3.0;
	aTemplate[2] = -3.0;
	aTemplate[3] = -3.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = -3.0;
	aTemplate[6] = 5.0;
	aTemplate[7] = 5.0;
	aTemplate[8] = 5.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	// ����Դͼ�񵽻���ͼ����
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ����Դͼ�񵽻���ͼ����
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// ����Kirschģ��6����
	aTemplate[0] = -3.0;
	aTemplate[1] = -3.0;
	aTemplate[2] = -3.0;
	aTemplate[3] = 5.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = -3.0;
	aTemplate[6] = 5.0;
	aTemplate[7] = 5.0;
	aTemplate[8] = -3.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ����Դͼ�񵽻���ͼ����
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// ����Kirschģ��7����
	aTemplate[0] = 5.0;
	aTemplate[1] = -3.0;
	aTemplate[2] = -3.0;
	aTemplate[3] = 5.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = -3.0;
	aTemplate[6] = 5.0;
	aTemplate[7] = -3.0;
	aTemplate[8] = -3.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ����Դͼ�񵽻���ͼ����
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// ����Kirschģ��8����
	aTemplate[0] = 5.0;
	aTemplate[1] = 5.0;
	aTemplate[2] = -3.0;
	aTemplate[3] = 5.0;
	aTemplate[4] = 0.0;
	aTemplate[5] = -3.0;
	aTemplate[6] = -3.0;
	aTemplate[7] = -3.0;
	aTemplate[8] = -3.0;

	if (!Template(lpNewDIBBits2, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ���ƾ���ģ��������ͼ��Դͼ��
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *  GaussDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ��Ե���ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú����ø�˹������˹��Ե������Ӷ�ͼ����б�Ե������㡣 
 * Ҫ��Ŀ��ͼ��Ϊ�Ҷ�ͼ��
 ************************************************************************/
BOOL CDibImage::GaussDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpDst1;				// ָ�򻺴�ͼ���ָ��
	LPSTR	lpDst2;	
	LPSTR	lpNewDIBBits1;		// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;
	
	int		iTempH;				// ģ��߶�	
	int		iTempW;				// ģ����	
	FLOAT	fTempC;				// ģ��ϵ��	
	int		iTempMX;			// ģ������Ԫ��X����	
	int		iTempMY;			// ģ������Ԫ��Y����
	FLOAT aTemplate[25];		//ģ������

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// ����Դͼ�񵽻���ͼ����
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// ����Gaussģ�����
	iTempW = 5;
	iTempH = 5;
	fTempC = 1.0;
	iTempMX = 3;
	iTempMY = 3;
	aTemplate[0] = -2.0;
	aTemplate[1] = -4.0;
	aTemplate[2] = -4.0;
	aTemplate[3] = -4.0;
	aTemplate[4] = -2.0;
	aTemplate[5] = -4.0;
	aTemplate[6] = 0.0;
	aTemplate[7] = 8.0;
	aTemplate[8] = 0.0;
	aTemplate[9] = -4.0;
	aTemplate[10] = -4.0;
	aTemplate[11] = 8.0;
	aTemplate[12] = 24.0;
	aTemplate[13] = 8.0;
	aTemplate[14] = -4.0;
	aTemplate[15] = -4.0;
	aTemplate[16] = 0.0;
	aTemplate[17] = 8.0;
	aTemplate[18] = 0.0;
	aTemplate[19] = -4.0;
	aTemplate[20] = -2.0;
	aTemplate[21] = -4.0;
	aTemplate[22] = -4.0;
	aTemplate[23] = -4.0;
	aTemplate[24] = -2.0;

	if (!Template(lpNewDIBBits1, lWidth, lHeight, 
		iTempH, iTempW, iTempMX, iTempMY, aTemplate, fTempC))
	{
		return FALSE;
	}

	// ���ƾ���ģ��������ͼ��Դͼ��
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   HoughDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ����ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶԼ��ͼ���е�ƽ��ֱ�ߡ����ͼ����������ƽ�е�ֱ�ߣ�
 * ��������ƽ��ֱ����ȡ������ 
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::HoughDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{			
	LPSTR	lpSrc;				// ָ��Դͼ���ָ��	
	LPSTR	lpDst;				// ָ�򻺴�ͼ���ָ��	
	LPSTR   lpTrans;			// ָ��任���ָ��	
	LONG	lLineBytes;			// ͼ��ÿ�е��ֽ���		
	LPSTR	lpNewDIBBits;		// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;	
	LPSTR	lpTransArea;		// ָ��任���ָ��
	HLOCAL	hTransArea;
	
	int		iMaxDist;			// �任��ĳߴ�
	int		iMaxAngleNumber;	
	int		iDist;				// �任�������
	int		iAngleNumber;	
	long	i,j;				// ѭ������	
	unsigned char pixel;		// ����ֵ
	MaxValue MaxValue1;			// �洢�任���е��������ֵ
	MaxValue MaxValue2;

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	//����任��ĳߴ�	������
	iMaxDist = (int) sqrt((double)(lWidth*lWidth + lHeight*lHeight));	
	//�Ƕȴ�0��180��ÿ��2��
	iMaxAngleNumber = 90;						

	//Ϊ�任������ڴ�
	hTransArea = LocalAlloc(LHND, lWidth * lHeight * sizeof(int));
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpTransArea = (char * )LocalLock(hTransArea);
		
	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ0
	lpTrans = (char *)lpTransArea;
	memset(lpTrans, 0, lWidth * lHeight * sizeof(int));

	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);

	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth; i++)
		{
			// ָ��Դͼ������j�У���i�����ص�ָ��			
			lpSrc = (char *)lpDIBBits + lLineBytes * j + i;

			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;
			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
			if(pixel != 255 && *lpSrc != 0)
			{
				return FALSE;
			}

			//����Ǻڵ㣬���ڱ任��Ķ�Ӧ�����ϼ�1
			if(pixel == 0)
			{
				//ע�ⲽ����2��
				for(iAngleNumber=0; iAngleNumber<iMaxAngleNumber; iAngleNumber++)
				{
					iDist = (int) fabs(i*cos(iAngleNumber*2*PI/180.0) + \
						j*sin(iAngleNumber*2*PI/180.0));
				
					//�任��Ķ�Ӧ���ϼ�1
					*(lpTransArea+iDist*iMaxAngleNumber+iAngleNumber) = \
						*(lpTransArea+iDist*iMaxAngleNumber+iAngleNumber) +1;
				}
			}	
		}
	}
				
	//�ҵ��任���е��������ֵ��
	MaxValue1.Value=0;
	MaxValue2.Value=0;
	
	//�ҵ���һ�����ֵ��
	for (iDist=0; iDist<iMaxDist;iDist++)
	{
		for(iAngleNumber=0; iAngleNumber<iMaxAngleNumber; iAngleNumber++)
		{
			if((int)*(lpTransArea+iDist*iMaxAngleNumber+iAngleNumber)>MaxValue1.Value)
			{
				MaxValue1.Value = (int)*(lpTransArea+iDist*iMaxAngleNumber
					+iAngleNumber);
				MaxValue1.Dist = iDist;
				MaxValue1.AngleNumber = iAngleNumber;
			}
		}
	}

	//����һ�����ֵ�㸽������
	for (iDist = -9;iDist < 10;iDist++)
	{
		for(iAngleNumber=-1; iAngleNumber<2; iAngleNumber++)
		{
			if(iDist+MaxValue1.Dist>=0 && iDist+MaxValue1.Dist<iMaxDist \
				&& iAngleNumber+MaxValue1.AngleNumber>=0 && iAngleNumber
				+MaxValue1.AngleNumber<=iMaxAngleNumber)
			{
				*(lpTransArea+(iDist+MaxValue1.Dist)*iMaxAngleNumber+\
					(iAngleNumber+MaxValue1.AngleNumber))=0;
			}
		}
	}

	//�ҵ��ڶ������ֵ��
	for (iDist=0; iDist<iMaxDist;iDist++)
	{
		for(iAngleNumber=0; iAngleNumber<iMaxAngleNumber; iAngleNumber++)
		{
			if((int)*(lpTransArea+iDist*iMaxAngleNumber+iAngleNumber)>MaxValue2.Value)
			{
				MaxValue2.Value = (int)*(lpTransArea+iDist*iMaxAngleNumber
					+iAngleNumber);
				MaxValue2.Dist = iDist;
				MaxValue2.AngleNumber = iAngleNumber;
			}
		}
	}

	//�ж���ֱ���Ƿ�ƽ��
	if(abs(MaxValue1.AngleNumber-MaxValue2.AngleNumber)<=2)
	{
		//��ֱ��ƽ�У��ڻ���ͼ�����ػ�������ֱ��
		for(j = 0; j <lHeight; j++)
		{
			for(i = 0;i <lWidth; i++)
			{	
				// ָ�򻺴�ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lLineBytes * j + i;	

				//����õ���ĳһ��ƽ��ֱ���ϣ����ڻ���ͼ���Ͻ��õ㸳Ϊ��

				//�ڵ�һ��ֱ����
				iDist = (int) fabs(i*cos(MaxValue1.AngleNumber*2*PI/180.0) + \
							j*sin(MaxValue1.AngleNumber*2*PI/180.0));
				if (iDist == MaxValue1.Dist)
				{
					*lpDst = (unsigned char)0;
				}
				//�ڵڶ���ֱ����
				iDist = (int) fabs(i*cos(MaxValue2.AngleNumber*2*PI/180.0) + \
							j*sin(MaxValue2.AngleNumber*2*PI/180.0));
				if (iDist == MaxValue2.Dist)
				{
					*lpDst = (unsigned char)0;
				}
			}
		}
	}

	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	LocalUnlock(hTransArea);
	LocalFree(hTransArea);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   Fill2DIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - �������ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ���������������㡣 
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::Fill2DIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;			// ָ��Դͼ���ָ��	
	long i;					// ѭ������	
	unsigned char pixel;	// ����ֵ	
	int xl,xr;				// ���ұ߽�����λ��
	BOOL bFilll,bFillr;		// �Ƿ���������߽�	
	Seed Seeds[10];			// ���Ӷ�ջ��ָ��
	int StackPoint;	
	int iCurrentPixelx,iCurrentPixely;	// ��ǰ����λ��
	int iBufferPixelx,iBufferPixely;

	Seeds[1].Height = lHeight / 2;		// ��ʼ������
	Seeds[1].Width = lWidth / 2;
	StackPoint = 1;

	while( StackPoint != 0)
	{
		//ȡ������
		iCurrentPixelx = Seeds[StackPoint].Width;
		iCurrentPixely = Seeds[StackPoint].Height;
		StackPoint--;
//		if(Seed2.Height== 75)
//		{
//			return true;
//			i++;
//		}
		bFilll = true;
		bFillr = true;
		//����������ڵ��У������������ص�λ��
		iBufferPixelx = iCurrentPixelx;
		iBufferPixely = iCurrentPixely;
		//���������
		while(bFilll)
		{
			lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + iCurrentPixelx;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;

			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
			//�����߽�
			if(pixel == 0)
			{
				bFilll = false;
				xl=iCurrentPixelx+1;
			}
			else
			{
				*lpSrc = (unsigned char)0;
				iCurrentPixelx--;
				//��ֹԽ��
				if(iCurrentPixelx<0)
				{	
					bFilll = false;
					iCurrentPixelx = 0;
					xl = 0; 
				}
			}		
		}
		//��������䣬ȡ���������ص�λ��
		iCurrentPixelx = iBufferPixelx+1;
		if(iCurrentPixelx>lWidth)
		{	
			bFillr = false;
			iCurrentPixelx = lWidth;
			xr = lWidth; 
		}
		iCurrentPixely = iBufferPixely;
		while(bFillr)
		{
			lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + iCurrentPixelx;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;

			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
			if(pixel != 255 && pixel != 0)
				return FALSE;
			//�����߽�
			if(pixel == 0)
			{
				bFillr = false;
				xr=iCurrentPixelx-1;
			}
			else
			{
				*lpSrc = (unsigned char)0;
				iCurrentPixelx++;
				//��ֹԽ��
				if(iCurrentPixelx>lWidth)
				{	
					bFillr = false;
					iCurrentPixelx = lWidth;
					xr = lWidth; 
				}
			}		
		}
		//�ϡ�������ɨ�����Ƿ�ȫΪ�߽����ػ�������
		//�ȿ������ɨ����
		iCurrentPixely--;
		if(iCurrentPixely < 0)
		{
			iCurrentPixely = 0;
		}
		for (i = xr; i>= xl;i--)
		{
			lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + i;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;

			//��δ�������أ����µ�����ѹ���ջ
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely;
				Seeds[StackPoint].Width = i;
				break;
			}				
		}
		//�ٿ������ɨ����
		iCurrentPixely+=2;
		if(iCurrentPixely > lHeight)
		{
			iCurrentPixely = lHeight;
		}
		for (i = xr; i>= xl;i--)
		{
			lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + i;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;

			//��δ�������أ����µ�����ѹ���ջ
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely;
				Seeds[StackPoint].Width = i;
				break;
			}				
		}
	}

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   FillDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - �������ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ���������������㡣
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::FillDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;				// ָ��Դͼ���ָ��	
	unsigned char pixel;		// ����ֵ	
	Seed *Seeds;				// ���Ӷ�ջ��ָ��
	int StackPoint;
	int iCurrentPixelx,iCurrentPixely;	// ��ǰ����λ��
 
	Seeds = new Seed[lWidth*lHeight];	// ��ʼ������
	Seeds[1].Height = lHeight / 2;
	Seeds[1].Width = lWidth / 2;
	StackPoint = 1;

	while( StackPoint != 0)
	{
		//ȡ������
		iCurrentPixelx = Seeds[StackPoint].Width;
		iCurrentPixely = Seeds[StackPoint].Height;
		StackPoint--;

		lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + iCurrentPixelx;
		//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
		pixel = (unsigned char)*lpSrc;

		//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
		if(pixel != 255 && pixel != 0)
		{
			return FALSE;
		}

		//����ǰ��Ϳ��
		*lpSrc = (unsigned char)0;

		//�ж�����ĵ㣬���Ϊ�ף���ѹ���ջ��ע���ֹԽ��
		if(iCurrentPixelx > 0)
		{
			lpSrc = (char *)lpDIBBits+lWidth*iCurrentPixely+iCurrentPixelx-1;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely;
				Seeds[StackPoint].Width = iCurrentPixelx - 1;
			}
			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
		}

		//�ж�����ĵ㣬���Ϊ�ף���ѹ���ջ��ע���ֹԽ��
		if(iCurrentPixely < lHeight - 1)
		{
			lpSrc = (char *)lpDIBBits+lWidth*(iCurrentPixely+1)+iCurrentPixelx;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely + 1;
				Seeds[StackPoint].Width = iCurrentPixelx;
			}
			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
		}

		//�ж�����ĵ㣬���Ϊ�ף���ѹ���ջ��ע���ֹԽ��
		if(iCurrentPixelx < lWidth - 1)
		{
			lpSrc = (char *)lpDIBBits+lWidth*iCurrentPixely+iCurrentPixelx+1;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely;
				Seeds[StackPoint].Width = iCurrentPixelx + 1;
			}
			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
		}

		//�ж�����ĵ㣬���Ϊ�ף���ѹ���ջ��ע���ֹԽ��
		if(iCurrentPixely > 0)
		{
			lpSrc = (char *)lpDIBBits+lWidth*(iCurrentPixely-1)+iCurrentPixelx;
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely - 1;
				Seeds[StackPoint].Width = iCurrentPixelx;
			}
			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
		}
	}

	delete Seeds;

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   ContourDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ����ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ�����������ȡ���㡣
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::ContourDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;				// ָ��Դͼ���ָ��	
	LPSTR	lpDst;				// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;		// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;
	long i,j;					//ѭ������
	unsigned char n,e,s,w,ne,se,nw,sw;
	unsigned char pixel;		//����ֵ

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);
	for(j = 1; j <lHeight-1; j++)
	{
		for(i = 1;i <lWidth-1; i++)
		{			
			// ָ��Դͼ������j�У���i�����ص�ָ��			
			lpSrc = (char *)lpDIBBits + lWidth * j + i;			
			// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
			lpDst = (char *)lpNewDIBBits + lWidth * j + i;			
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;

			//Ŀ��ͼ���к���0��255��������Ҷ�ֵ
//			if(pixel != 255 && pixel != 0)
//				return FALSE;
			if(pixel == 0)
			{
				*lpDst = (unsigned char)0;
				nw = (unsigned char)*(lpSrc + lWidth -1);
				n  = (unsigned char)*(lpSrc + lWidth );
				ne = (unsigned char)*(lpSrc + lWidth +1);
				w = (unsigned char)*(lpSrc -1);
				e = (unsigned char)*(lpSrc +1);
				sw = (unsigned char)*(lpSrc - lWidth -1);
				s  = (unsigned char)*(lpSrc - lWidth );
				se = (unsigned char)*(lpSrc - lWidth +1);
				//������ڵİ˸��㶼�Ǻڵ�
				if(nw+n+ne+w+e+sw+s+se==0)
				{
					*lpDst = (unsigned char)255;
				}
			}
		}
	}

	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * �������ƣ�
 *   TraceDIB()
 * ����:
 *   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth       - Դͼ���ȣ���������������4�ı�����
 *   LONG  lHeight      - Դͼ��߶ȣ���������
 * ����ֵ:
 *   BOOL               - ����ɹ�����TRUE�����򷵻�FALSE��
 * ˵��:
 * �ú������ڶ�ͼ����������������㡣
 * Ҫ��Ŀ��ͼ��Ϊֻ��0��255�����Ҷ�ֵ�ĻҶ�ͼ��
 ************************************************************************/
BOOL CDibImage::TraceDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;				// ָ��Դͼ���ָ��	
	LPSTR	lpDst;				// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;		// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;
	LONG	lLineBytes;			// ͼ��ÿ�е��ֽ���
	long i,j;					// ѭ������
	unsigned char pixel;		// ����ֵ
	bool bFindStartPoint;		// �Ƿ��ҵ���ʼ�㼰�ص���ʼ��
	bool bFindPoint;			// �Ƿ�ɨ�赽һ���߽��	
	Point StartPoint,CurrentPoint;	// ��ʼ�߽���뵱ǰ�߽��

	//�˸��������ʼɨ�跽��
	int Direction[8][2]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
	int BeginDirect;

	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	//���ҵ������Ϸ��ı߽��
	bFindStartPoint = false;
	for (j = 0;j < lHeight && !bFindStartPoint;j++)
	{
		for(i = 0;i < lWidth && !bFindStartPoint;i++)
		{
			// ָ��Դͼ������j�У���i�����ص�ָ��			
			lpSrc = (char *)lpDIBBits + lLineBytes * j + i;			
			//ȡ�õ�ǰָ�봦������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel = (unsigned char)*lpSrc;
			
			if(pixel == 0)
			{
				bFindStartPoint = true;
				StartPoint.Height = j;
				StartPoint.Width = i;

				// ָ��Ŀ��ͼ������j�У���i�����ص�ָ��			
				lpDst = (char *)lpNewDIBBits + lLineBytes * j + i;	
				*lpDst = (unsigned char)0;
			}		
		}
	}

	//������ʼ���������·�������ʼɨ�������Ϸ���
	BeginDirect = 0;
	//���ٱ߽�
	bFindStartPoint = false;
	//�ӳ�ʼ�㿪ʼɨ��
	CurrentPoint.Height = StartPoint.Height;
	CurrentPoint.Width = StartPoint.Width;
	while(!bFindStartPoint)
	{
		bFindPoint = false;
		while(!bFindPoint)
		{
			//��ɨ�跽��鿴һ������
			lpSrc = (char *)lpDIBBits + lLineBytes * ( CurrentPoint.Height + 
				Direction[BeginDirect][1]) + (CurrentPoint.Width + 
				Direction[BeginDirect][0]);
			pixel = (unsigned char)*lpSrc;
			if(pixel == 0)
			{
				bFindPoint = true;
				CurrentPoint.Height = CurrentPoint.Height+Direction[BeginDirect][1];
				CurrentPoint.Width = CurrentPoint.Width + Direction[BeginDirect][0];
				if(CurrentPoint.Height == StartPoint.Height && CurrentPoint.Width 
					== StartPoint.Width)
				{
					bFindStartPoint = true;
				}
				lpDst = (char *)lpNewDIBBits + lLineBytes * CurrentPoint.Height 
					+ CurrentPoint.Width;
				*lpDst = (unsigned char)0;
				//ɨ��ķ�����ʱ����ת����
				BeginDirect--;
				if(BeginDirect == -1)
				{
					BeginDirect = 7;
				}
				BeginDirect--;
				if(BeginDirect == -1)
				{
					BeginDirect = 7;
				}
			}
			else
			{
				//ɨ�跽��˳ʱ����תһ��
				BeginDirect++;
				if(BeginDirect == 8)
				{
					BeginDirect = 0;
				}
			}
		}
	}

	// ���Ƹ�ʴ���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

