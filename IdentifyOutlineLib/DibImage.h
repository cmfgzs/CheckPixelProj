// DibImage.h: interface for the CDibImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIBIMAGE_H__254F3D1E_BB20_40DA_AE07_E8E0219DFA8C__INCLUDED_)
#define AFX_DIBIMAGE_H__254F3D1E_BB20_40DA_AE07_E8E0219DFA8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DECLARE_HANDLE(HDIB);		// DIB���
#define PALVERSION   0x300	// DIB����

/* DIB�� */
// �ж��Ƿ���Win 3.0��DIB
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
// �����������Ŀ��
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
// �����������ĸ߶�
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// �ڼ���ͼ���Сʱ�����ù�ʽ��biSizeImage = biWidth' �� biHeight��
// ��biWidth'��������biWidth�������biWidth'������4������������ʾ
// ���ڻ����biWidth�ģ���4�������������WIDTHBYTES������������
// biWidth'
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

// Dib�ļ�ͷ��־���ַ���"BM"��дDIBʱ�õ��ó�����
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

// ͼ�񼸺α任����//////////////////////////////////////////
// ������
#define PI 3.1415926535
//�Ƕȵ�����ת���ĺ�
#define RADIAN(angle) ((angle)*PI/180.0) 
/////////////////////////////////////////////////////////////

// ͼ�������任����//////////////////////////////////////////
#include <complex>
using namespace std;
/////////////////////////////////////////////////////////////

//  ͼ���Ե���������㺯��///////////////////////////////////
typedef struct{
	int Value;
	int Dist;
	int AngleNumber;
}	MaxValue;

typedef struct{
	int Height;
	int Width;
}	Seed;

typedef struct{
	int Height;
	int Width;
}	Point;
/////////////////////////////////////////////////////////////

class CDibImage  
{
	// Constructor and Destructor ///////////////////////////////
public:
	CDibImage();
	virtual ~CDibImage();

	// function /////////////////////////////////////////////////
public:
	//  DIB(Independent Bitmap) ����
	BOOL    PaintDIB (HDC, LPRECT, HDIB, LPRECT, CPalette* pPal);
	BOOL    CreateDIBPalette(HDIB hDIB, CPalette* cPal);
	LPSTR   FindDIBBits (LPSTR lpbi);
	DWORD   DIBWidth (LPSTR lpDIB);
	DWORD   DIBHeight (LPSTR lpDIB);
	WORD    PaletteSize (LPSTR lpbi);
	WORD    DIBNumColors (LPSTR lpbi);
	HGLOBAL CopyHandle (HGLOBAL h);
	BOOL    SaveDIB (HDIB hDib, CFile& file);
	HDIB    ReadDIBFile(CFile& file);

	//  ͼ������㺯��
	BOOL	LinerTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, FLOAT fA, 
				FLOAT fB);
	BOOL	ThresholdTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bThre);
	BOOL	WindowTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bLow, 
				BYTE bUp);
	BOOL	GrayStretch(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bX1, 
				BYTE bY1, BYTE bX2, BYTE bY2);
	BOOL	InteEqualize(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);

	// ͼ�񼸺α任����
	BOOL	TranslationDIB1 (LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
				LONG lXOffset, LONG lYOffset);
	BOOL	TranslationDIB (LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
				LONG lXOffset, LONG lYOffset);
	BOOL	MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bDirection);
	BOOL	TransposeDIB(LPSTR lpbi);
	HGLOBAL ZoomDIB(LPSTR lpbi, float fXZoomRatio, float fYZoomRatio);
	HGLOBAL RotateDIB(LPSTR lpbi, int iRotateAngle);
	HGLOBAL RotateDIB2(LPSTR lpbi, int iRotateAngle);
	unsigned char Interpolation (LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
				FLOAT x, FLOAT y);

	// ͼ�������任����
	VOID FFT(complex<double> * TD, complex<double> * FD, int r);
	VOID IFFT(complex<double> * FD, complex<double> * TD, int r);
	VOID DCT(double *f, double *F, int power);
	VOID IDCT(double *F, double *f, int power);
	VOID WALSH(double *f, double *F, int r);
	VOID IWALSH(double *F, double *f, int r);
	
	BOOL Fourier(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL DIBDct(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL DIBWalsh(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL DIBWalsh1(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);

	// ͼ��ģ��任����
	BOOL Template(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
		int iTempH, int iTempW, 
		int iTempMX, int iTempMY,
		FLOAT * fpArray, FLOAT fCoef);
	BOOL MedianFilter(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
		int iFilterH, int iFilterW, 
		int iFilterMX, int iFilterMY);
	unsigned char GetMedianNum(unsigned char * bArray, int iFilterLen);
	BOOL GradSharp(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bThre);
	BOOL ReplaceColorPal(LPSTR lpDIB, BYTE * bpColorsTable);

	//  ͼ����̬ѧ�任����
	BOOL ErosionDIB (LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bHori , 
		int structure[3][3]);
	BOOL DilationDIB (LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bHori , 
		int structure[3][3]);
	BOOL OpenDIB (LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bHori , 
		int structure[3][3]);
	BOOL CloseDIB (LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bHori , 
		int structure[3][3]);
	BOOL ThiningDIB (LPSTR lpDIBBits, LONG lWidth, LONG lHeight);

	//  ͼ���Ե���������㺯��
	BOOL RobertDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL SobelDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL PrewittDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL KirschDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL GaussDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL HoughDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL FillDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL Fill2DIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL ContourDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
	BOOL TraceDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);
};

#endif // !defined(AFX_DIBIMAGE_H__254F3D1E_BB20_40DA_AE07_E8E0219DFA8C__INCLUDED_)
