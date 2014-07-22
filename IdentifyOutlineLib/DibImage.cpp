///////////////////////////////////////////////////////////////////////
// DibImage.cpp: implementation of the CDibImage class.
//  DIB(Independent Bitmap) 函数：
//
//  PaintDIB()          - 绘制DIB对象
//  CreateDIBPalette()  - 创建DIB对象调色板
//  FindDIBBits()       - 返回DIB图像象素起始位置
//  DIBWidth()          - 返回DIB宽度
//  DIBHeight()         - 返回DIB高度
//  PaletteSize()       - 返回DIB调色板大小
//  DIBNumColors()      - 计算DIB调色板颜色数目
//  CopyHandle()        - 拷贝内存块
//
//  SaveDIB()           - 将DIB保存到指定文件中
//  ReadDIBFile()       - 重指定文件中读取DIB对象
//
//  图像点运算函数：
//
//  LinerTrans()		- 图像线性变换
//	WindowTrans()		- 图像窗口变换
//  GrayStretch()		- 图像灰度拉伸
//  InteEqualize()		－直方图均衡
//
//	图像几何变换函数
//  TranslationDIB1()   - 图像平移
//  TranslationDIB()    - 图像平移
//  MirrorDIB()		    - 图像镜像
//  TransposeDIB()		- 图像转置
//  ZoomDIB()			- 图像缩放
//  RotateDIB()			- 图像旋转
//	
//  图像正交变换函数：
//
//  FFT()				- 快速付立叶变换
//  IFFT()				- 快速付立叶反变换
//  DCT()				- 离散余弦变换
//  WALSH()				- 沃尔什－哈达玛变换
//
//  Fourier()			- 图像的付立叶变换
//  DIBDct()			- 图像的离散余弦变换
//  DIBWalsh()			- 图像的沃尔什－哈达玛变换
//
//	图像模板变换函数
//
//  Template()			- 图像模板变换，通过改变模板，可以用它实现
//						  图像的平滑、锐化、边缘识别等操作。
//  MedianFilter()		- 图像中值滤波。
//  GetMedianNum()		- 获取中值。被函数MedianFilter()调用来求中值。
//  ReplaceColorPal()	- 更换伪彩色编码表。
//
//  图像形态学变换函数：
//
//  ErosionDIB()    - 图像腐蚀
//  DilationDIB()	- 图像膨胀
//  OpenDIB()		- 图像开运算
//  CloseDIB()		- 图像闭运算
//  ThiningDIB()	- 图像细化
//
//  图像边缘与轮廓运算函数：
//
//  RobertDIB()		- robert边缘检测运算
//  SobelDIB()		- sobel边缘检测运算
//  PrewittDIB()	- prewitt边缘检测运算
//  KirschDIB()		- kirsch边缘检测运算
//  GaussDIB()		- gauss边缘检测运算
//  HoughDIB()		- 利用Hough变换检测平行直线
//  ContourDIB()	- 轮廓提取
//  TraceDIB()		- 轮廓跟踪
//  FillDIB()		- 种子填充算法1
//  Fill2DIB()		- 种子填充算法2
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
// DIB(Independent Bitmap) 函数
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * 函数名称：
 *   PaintDIB()
 * 参数:
 *   HDC hDC            - 输出设备DC
 *   LPRECT lpDCRect    - 绘制矩形区域
 *   HDIB hDIB          - 指向DIB对象的指针
 *   LPRECT lpDIBRect   - 要输出的DIB区域
 *   CPalette* pPal     - 指向DIB对象调色板的指针
 * 返回值:
 *   BOOL               - 绘制成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数主要用来绘制DIB对象。其中调用了StretchDIBits()或者
 * SetDIBitsToDevice()来绘制DIB对象。输出的设备由由参数hDC指
 * 定；绘制的矩形区域由参数lpDCRect指定；输出DIB的区域由参数
 * lpDIBRect指定。
 ************************************************************************/
BOOL CDibImage::PaintDIB(HDC     hDC,
					LPRECT  lpDCRect,
					HDIB    hDIB,
					LPRECT  lpDIBRect,
					CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // BITMAPINFOHEADER指针
	LPSTR    lpDIBBits;           // DIB象素指针
	BOOL     bSuccess=FALSE;      // 成功标志
	HPALETTE hPal=NULL;           // DIB调色板
	HPALETTE hOldPal=NULL;        // 以前的调色板

	if (hDIB == NULL)
	{
		return FALSE;
	}
	
	lpDIBHdr  = (LPSTR)::GlobalLock((HGLOBAL) hDIB);// 锁定DIB	
	lpDIBBits = FindDIBBits(lpDIBHdr);	// 找到DIB图像象素起始位置

	if (pPal != NULL)					// 获取DIB调色板，并选中它
	{
		hPal = (HPALETTE) pPal->m_hObject;	
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);	// 选中调色板
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);			// 设置显示模式

	// 判断是调用StretchDIBits()还是SetDIBitsToDevice()来绘制DIB对象
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	{
		// 原始大小，不用拉伸。
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
		// 非原始大小，拉伸。
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
	 
	::GlobalUnlock((HGLOBAL) hDIB);				// 解除锁定	
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);	// 恢复以前的调色板
	}	
	return bSuccess;
}

/*************************************************************************
 * 函数名称：
 *   CreateDIBPalette()
 * 参数:
 *   HDIB hDIB          - 指向DIB对象的指针
 *   CPalette* pPal     - 指向DIB对象调色板的指针
 * 返回值:
 *   BOOL               - 创建成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数按照DIB创建一个逻辑调色板，从DIB中读取颜色表并存到调色板中，
 * 最后按照该逻辑调色板创建一个新的调色板，并返回该调色板的句柄。这样
 * 可以用最好的颜色来显示DIB图像。
 ************************************************************************/
BOOL CDibImage::CreateDIBPalette(HDIB hDIB, CPalette* pPal)
{
	
	LPLOGPALETTE lpPal;		// 指向逻辑调色板的指针
	HANDLE hLogPal;			// 逻辑调色板的句柄
	HPALETTE hPal = NULL;	// 调色板的句柄
	int i;					// 循环变量	
	WORD wNumColors;		// 颜色表中的颜色数目	
	LPSTR lpbi;				// 指向DIB的指针	
	LPBITMAPINFO lpbmi;		// 指向BITMAPINFO结构的指针（Win3.0）	
	LPBITMAPCOREINFO lpbmc;	// 指向BITMAPCOREINFO结构的指针	
	BOOL bWinStyleDIB;		// 表明是否是Win3.0 DIB的标记	
	BOOL bResult = FALSE;	// 创建结果
	
	if (hDIB == NULL)
	{
		return FALSE;
	}
		
	lpbi = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);	// 锁定DIB
	lpbmi = (LPBITMAPINFO)lpbi;		// 获取指向BITMAPINFO结构的指针（Win3.0）
	lpbmc = (LPBITMAPCOREINFO)lpbi;	// 获取指向BITMAPCOREINFO结构的指针
	wNumColors = DIBNumColors(lpbi);// 获取DIB中颜色表中的颜色数目
	
	if (wNumColors != 0)
	{
		// 分配为逻辑调色板内存
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);	
		// 如果内存不足，退出
		if (hLogPal == 0)
		{	
			::GlobalUnlock((HGLOBAL) hDIB);	// 解除锁定
			return FALSE;
		}
		
		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);		
		lpPal->palVersion = PALVERSION;			// 设置版本号
		lpPal->palNumEntries = (WORD)wNumColors;// 设置颜色数目
		bWinStyleDIB = IS_WIN30_DIB(lpbi);		// 判断是否是WIN3.0的DIB

		// 读取调色板
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				// 读取红色绿色蓝色分量
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;				
				// 保留位
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				// 读取红色绿色蓝色分量
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;				
				// 保留位
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}
			
		bResult = pPal->CreatePalette(lpPal);// 按照逻辑调色板创建调色板，并返回指针
		::GlobalUnlock((HGLOBAL) hLogPal);	// 解除锁定
		::GlobalFree((HGLOBAL) hLogPal);	// 释放逻辑调色板
	}
	
	::GlobalUnlock((HGLOBAL) hDIB);			// 解除锁定
	return bResult;
}

/*************************************************************************
 * 函数名称：
 *   FindDIBBits()
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 * 返回值:
 *   LPSTR              - 指向DIB图像象素起始位置
 * 说明:
 *   该函数计算DIB中图像象素的起始位置，并返回指向它的指针。
 ************************************************************************/
LPSTR CDibImage::FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

/*************************************************************************
 * 函数名称：
 *   DIBWidth()
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 * 返回值:
 *   DWORD              - DIB中图像的宽度
 * 说明:
 *   该函数返回DIB中图像的宽度。对于Windows 3.0 DIB，返回BITMAPINFOHEADER
 * 中的biWidth值；对于其它返回BITMAPCOREHEADER中的bcWidth值。
 ************************************************************************/
DWORD CDibImage::DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// 返回DIB中图像的宽度
	if (IS_WIN30_DIB(lpDIB))
	{		
		return lpbmi->biWidth;		// 对于Windows 3.0 DIB，返回lpbmi->biWidth
	}
	else
	{		
		return (DWORD)lpbmc->bcWidth;	// 对于其它格式的DIB，返回lpbmc->bcWidth
	}
}

/*************************************************************************
 * 函数名称：
 *   DIBHeight()
 * 参数:
 *   LPSTR lpDIB        - 指向DIB对象的指针
 * 返回值:
 *   DWORD              - DIB中图像的高度
 * 说明:
 *   该函数返回DIB中图像的高度。对于Windows 3.0 DIB，返回BITMAPINFOHEADER
 * 中的biHeight值；对于其它返回BITMAPCOREHEADER中的bcHeight值。
 ************************************************************************/
DWORD CDibImage::DIBHeight(LPSTR lpDIB)
{	
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// 返回DIB中图像的宽度
	if (IS_WIN30_DIB(lpDIB))
	{		
		return lpbmi->biHeight;		// 对于Windows 3.0 DIB，返回lpbmi->biHeight
	}
	else
	{		
		return (DWORD)lpbmc->bcHeight;	// 对于其它格式的DIB，返回lpbmc->bcHeight
	}
}

/*************************************************************************
 * 函数名称：
 *   PaletteSize()
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 * 返回值:
 *   WORD               - DIB中调色板的大小
 * 说明:
 *   该函数返回DIB中调色板的大小。对于Windows 3.0 DIB，返回颜色数目×
 * RGBQUAD的大小；对于其它返回颜色数目×RGBTRIPLE的大小。
 ************************************************************************/
WORD CDibImage::PaletteSize(LPSTR lpbi)
{
	// 计算DIB中调色板的大小
	if (IS_WIN30_DIB (lpbi))
	{
		//返回颜色数目×RGBQUAD的大小
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
	}
	else
	{
		//返回颜色数目×RGBTRIPLE的大小
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
	}
}

/*************************************************************************
 * 函数名称：
 *   DIBNumColors()
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 * 返回值:
 *   WORD               - 返回调色板中颜色的种数
 * 说明:
 *   该函数返回DIB中调色板的颜色的种数。对于单色位图，返回2，
 * 对于16色位图，返回16，对于256色位图，返回256；对于真彩色
 * 位图（24位），没有调色板，返回0。
 ************************************************************************/
WORD CDibImage::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;

	// 对于Windows的DIB, 实际颜色的数目可以比象素的位数要少。
	// 对于这种情况，则返回一个近似的数值。
	
	// 判断是否是WIN3.0 DIB
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;	// 读取dwClrUsed值
		
		if (dwClrUsed != 0)
		{
			// 如果dwClrUsed（实际用到的颜色数）不为0，直接返回该值
			return (WORD)dwClrUsed;
		}
	}

	// 读取象素的位数
	if (IS_WIN30_DIB(lpbi))
	{		
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;	// 读取biBitCount值
	}
	else
	{		
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;	// 读取biBitCount值
	}
	
	// 按照象素的位数计算颜色数目
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
 * 函数名称：
 *   CopyHandle()
 * 参数:
 *   HGLOBAL h          - 要复制的内存区域
 * 返回值:
 *   HGLOBAL            - 复制后的新内存区域
 * 说明:
 *   该函数复制指定的内存区域。返回复制后的新内存区域，出错时返回0。
 ************************************************************************/
HGLOBAL CDibImage::CopyHandle (HGLOBAL h)
{
	if (h == NULL)
	{
		return NULL;
	}
	
	DWORD dwLen = ::GlobalSize((HGLOBAL) h);	// 获取指定内存区域大小
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);	// 分配新内存空间	
	if (hCopy != NULL)							// 判断分配是否成功
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
 * 函数名称：
 *   SaveDIB()
 * 参数:
 *   HDIB hDib          - 要保存的DIB
 *   CFile& file        - 保存文件CFile
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE或者CFileException
 * 说明:
 *   该函数将指定的DIB对象保存到指定的CFile中。该CFile由调用程序打开和关闭。
 *************************************************************************/
BOOL CDibImage::SaveDIB(HDIB hDib, CFile& file)
{	
	BITMAPFILEHEADER bmfHdr;	// Bitmap文件头	
	LPBITMAPINFOHEADER lpBI;	// 指向BITMAPINFOHEADER的指针
	DWORD dwDIBSize;			// DIB大小

	if (hDib == NULL)
	{
		return FALSE;
	}

	// 读取BITMAPINFO结构，并锁定
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDib);	
	if (lpBI == NULL)
	{
		return FALSE;
	}
	
	// 判断是否是WIN3.0 DIB
	if (!IS_WIN30_DIB(lpBI))
	{
		// 不支持其它类型的DIB保存
		::GlobalUnlock((HGLOBAL) hDib);
		return FALSE;
	}

	////////////////////////////////////////////////////////////////////////
	// 填充文件头///////////////////////////////////////////////////////////	
	bmfHdr.bfType = DIB_HEADER_MARKER;		// 文件类型"BM"

	// 计算DIB大小时，最简单的方法是调用GlobalSize()函数。但是全局内存大小并
	// 不是DIB真正的大小，它总是多几个字节。这样就需要计算一下DIB的真实大小。
	
	// 文件头大小＋颜色表大小
	// （BITMAPINFOHEADER和BITMAPCOREHEADER结构的第一个DWORD都是该结构的大小）
	dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);
	
	// 计算图像大小
	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// 对于RLE位图，没法计算大小，只能信任biSizeImage内的值
		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{		
		DWORD dwBmBitsSize;			// 象素的大小
		dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) 
			* lpBI->biHeight;		// 大小为Width * Height
		dwDIBSize += dwBmBitsSize;	// 计算出DIB真正的大小

		// 更新biSizeImage（很多BMP文件头中biSizeImage的值是错误的）
		lpBI->biSizeImage = dwBmBitsSize;
	}

	// 计算文件大小：DIB大小＋BITMAPFILEHEADER结构大小
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	
	// 两个保留字
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	// 计算偏移量bfOffBits，它的大小为Bitmap文件头大小＋DIB头大小＋颜色表大小
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize
											  + PaletteSize((LPSTR)lpBI);

	/////////////////////////////////////////////////////////////////////////
	// 尝试写文件////////////////////////////////////////////////////////////
	TRY
	{		
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));	// 写文件头
		//file.WriteHuge(lpBI, dwDIBSize);					// 写DIB头和象素
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
 * 函数名称：
 *   ReadDIBFile()
 * 参数:
 *   CFile& file        - 要读取得文件文件CFile
 * 返回值:
 *   HDIB               - 成功返回DIB的句柄，否则返回NULL。
 * 说明:
 *   该函数将指定的文件中的DIB对象读到指定的内存区域中。除BITMAPFILEHEADER
 * 外的内容都将被读入内存。
 *************************************************************************/
HDIB CDibImage::ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	HDIB hDIB;
	LPSTR pDIB;
	DWORD dwBitsSize;

	dwBitsSize = file.GetLength();		// 获取DIB（文件）长度（字节）

	// 尝试读取DIB文件头
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		return NULL;
	}
	// 判断是否是DIB对象，检查头两个字节是否是"BM"
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
	{
		return NULL;
	}
	// 为DIB分配内存
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		return NULL;
	}
	
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )		// 读象素
	{
		::GlobalUnlock((HGLOBAL) hDIB);
		::GlobalFree((HGLOBAL) hDIB);
		return NULL;
	}

	::GlobalUnlock((HGLOBAL) hDIB);
	return hDIB;
}

//////////////////////////////////////////////////////////////////////
// 图像点运算函数
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * 函数名称：
 *   LinerTrans()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   FLOAT fA		    - 线性变换的斜率
 *   FLOAT fB           - 线性变换的截距
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行灰度的线性变换操作。
 ************************************************************************/
BOOL CDibImage::LinerTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						   FLOAT fA, FLOAT fB)
{		
	unsigned char*	lpSrc;		// 指向源图像的指针
	LONG	i;					// 循环变量
	LONG	j;		
	LONG	lLineBytes;			// 图像每行的字节数	
	FLOAT	fTemp;				// 中间变量	

	lLineBytes = WIDTHBYTES(lWidth * 8);	// 计算图像每行的字节数
		
	for(i = 0; i < lHeight; i++)			// 每行
	{		
		for(j = 0; j < lWidth; j++)			// 每列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			fTemp = fA * (*lpSrc) + fB;		// 线性变换	
			
			if (fTemp > 255)				// 判断是否超出范围
			{
				*lpSrc = 255;
			}
			else if (fTemp < 0)
			{
				*lpSrc = 0;
			}
			else
			{				
				*lpSrc = (unsigned char) (fTemp + 0.5);	// 四舍五入
			}
		}
	}
	
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   ThresholdTrans()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   BYTE  bThre	    - 阈值
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行阈值变换。对于灰度值小于阈值的象素直接设置
 * 灰度值为0；灰度值大于阈值的象素直接设置为255。
 ************************************************************************/
BOOL CDibImage::ThresholdTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bThre)
{	
	unsigned char*	lpSrc;				// 指向源图像的指针	
	LONG	i;							// 循环变量
	LONG	j;	
	LONG	lLineBytes;					// 图像每行的字节数
		
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	
	for(i = 0; i < lHeight; i++)		// 每行
	{		
		for(j = 0; j < lWidth; j++)		// 每列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			
			if ((*lpSrc) < bThre)		// 判断是否小于阈值
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
 * 函数名称：
 *   WindowTrans()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   BYTE  bLow		    - 窗口下限
 *   BYTE  bUp          - 窗口上限
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行窗口变换。只有在窗口范围内的灰度保持不变，
 * 小于下限的象素直接设置灰度值为0；大于上限的象素直接设置灰度值为255。
 ************************************************************************/
BOOL CDibImage::WindowTrans(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
							BYTE bLow, BYTE bUp)
{	
	unsigned char*	lpSrc;				// 指向源图像的指针	
	LONG	i;							// 循环变量
	LONG	j;
	LONG	lLineBytes;					// 图像每行的字节数
		
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	
	for(i = 0; i < lHeight; i++)		// 每行
	{	
		for(j = 0; j < lWidth; j++)		// 每列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			if ((*lpSrc) < bLow)		// 判断是否超出范围
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
 * 函数名称：
 *   GrayStretch()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   BYTE bX1			- 灰度拉伸第一个点的X坐标
 *   BYTE bY1			- 灰度拉伸第一个点的Y坐标
 *   BYTE bX2			- 灰度拉伸第二个点的X坐标
 *   BYTE bY2			- 灰度拉伸第二个点的Y坐标
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行灰度拉伸。
 ************************************************************************/
BOOL CDibImage::GrayStretch(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
							BYTE bX1, BYTE bY1, BYTE bX2, BYTE bY2)
{	
	unsigned char*	lpSrc;				// 指向源图像的指针
	LONG	i;							// 循环变量
	LONG	j;	
	BYTE	bMap[256];					// 灰度映射表	
	LONG	lLineBytes;					// 图像每行的字节数
	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数
	
	// 计算灰度映射表
	for (i = 0; i <= bX1; i++)
	{		
		if (bX1 > 0)					// 判断bX1是否大于0（防止分母为0）
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
		if (bX2 != bX1)					// 判断bX1是否等于bX2（防止分母为0）
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
		if (bX2 != 255)					// 判断bX2是否等于255（防止分母为0）
		{
			bMap[i] = bY2 + (BYTE) ((255 - bY2) * (i - bX2) / (255 - bX2));
		}
		else
		{
			bMap[i] = 255;
		}
	}
		
	for(i = 0; i < lHeight; i++)		// 每行
	{		
		for(j = 0; j < lWidth; j++)		// 每列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			// 计算新的灰度值
			*lpSrc = bMap[*lpSrc];
		}
	}
	
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   InteEqualize()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行直方图均衡。
 ************************************************************************/
BOOL CDibImage::InteEqualize(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	unsigned char*	lpSrc;				// 指向源图像的指针	
	LONG	lTemp;						// 临时变量	
	LONG	i;							// 循环变量
	LONG	j;	
	BYTE	bMap[256];					// 灰度映射表	
	LONG	lCount[256];				// 灰度映射表
	LONG	lLineBytes;					// 图像每行的字节数
		
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数
		
	for (i = 0; i < 256; i ++)			// 重置计数为0
	{
		lCount[i] = 0;
	}
	
	// 计算各个灰度值的计数
	for (i = 0; i < lHeight; i ++)
	{
		for (j = 0; j < lWidth; j ++)
		{
			lpSrc = (unsigned char *)lpDIBBits + lLineBytes * i + j;
			lCount[*(lpSrc)]++;
		}
	}
	
	// 计算灰度映射表
	for (i = 0; i < 256; i++)
	{
		lTemp = 0;		
		for (j = 0; j <= i ; j++)
		{
			lTemp += lCount[j];
		}
		
		// 计算对应的新灰度值
		bMap[i] = (BYTE) (lTemp * 255 / lHeight / lWidth);
	}
		
	for(i = 0; i < lHeight; i++)			// 每行
	{
		for(j = 0; j < lWidth; j++)			// 每列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			// 计算新的灰度值
			*lpSrc = bMap[*lpSrc];
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// 图像几何变换函数
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * 函数名称：
 *   TranslationDIB1()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   LONG  lXOffset     - X轴平移量（象素数）
 *   LONG  lYOffset     - Y轴平移量（象素数）
 * 返回值:
 *   BOOL               - 平移成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来水平移动DIB图像。函数不会改变图像的大小，移出的部分图像
 * 将截去，空白部分用白色填充。
 ************************************************************************/
BOOL CDibImage::TranslationDIB1(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
								LONG lXOffset, LONG lYOffset)
{	
	LPSTR	lpSrc;					// 指向源图像的指针	
	LPSTR	lpDst;					// 指向要复制区域的指针
	LPSTR	lpNewDIBBits;			// 指向复制图像的指针
	HLOCAL	hNewDIBBits;
	LONG	i;						// 象素在新DIB中的坐标
	LONG	j;	
	LONG	i0;						// 象素在源DIB中的坐标
	LONG	j0;	
	LONG lLineBytes;				// 图像每行的字节数
	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数
	
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
		
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);	
		
	for(i = 0; i < lHeight; i++)					// 每行
	{		
		for(j = 0; j < lWidth; j++)					// 每列
		{
			// 指向新DIB第i行，第j个象素的指针
			// 注意由于DIB中图像第一行其实保存在最后一行的位置，因此lpDst
			// 值不是(char *)lpNewDIBBits + lLineBytes * i + j，而是
			// (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 计算该象素在源DIB中的坐标
			i0 = i - lXOffset;
			j0 = j - lYOffset;
			
			// 判断是否在源图范围内
			if( (j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// 指向源DIB第i0行，第j0个象素的指针
				// 同样要注意DIB上下倒置的问题
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;							
				*lpDst = *lpSrc;					// 复制象素
			}
			else
			{
				// 对于源图中没有的象素，直接赋值为255
				* ((unsigned char*)lpDst) = 255;
			}
		}
	}
	
	// 复制平移后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   TranslationDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   LONG  lXOffset     - X轴平移量（象素数）
 *   LONG  lYOffset     - Y轴平移量（象素数）
 * 返回值:
 *   BOOL               - 平移成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来水平移动DIB图像。函数不会改变图像的大小，移出的部分图像
 * 将截去，空白部分用白色填充。
 ************************************************************************/
BOOL CDibImage::TranslationDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
							   LONG lXOffset, LONG lYOffset)
{	
	CRect	rectSrc;	// 平移后剩余图像在源图像中的位置（矩形区域）	
	CRect	rectDst;	// 平移后剩余图像在新图像中的位置（矩形区域）	
	LPSTR	lpSrc;		// 指向源图像的指针	
	LPSTR	lpDst;		// 指向要复制区域的指针	
	LPSTR	lpNewDIBBits;	// 指向复制图像的指针
	HLOCAL	hNewDIBBits;
	BOOL	bVisible;	// 指明图像是否全部移去可视区间	
	LONG	i;			// 循环变量	
	LONG lLineBytes;	// 图像每行的字节数
		
	lLineBytes = WIDTHBYTES(lWidth * 8);	// 计算图像每行的字节数	
	bVisible = TRUE;	// 赋初值
	
	// 计算rectSrc和rectDst的X坐标
	if (lXOffset <= -lWidth)
	{		
		bVisible = FALSE;	// X轴方向全部移出可视区域
	}
	else if (lXOffset <= 0)
	{		
		rectDst.left = 0;	// 移动后，有图区域左上角X坐标为0
		
		// 移动后，有图区域右下角X坐标为lWidth - |lXOffset| = lWidth + lXOffset
		rectDst.right = lWidth + lXOffset;
	}
	else if (lXOffset < lWidth)
	{	
		rectDst.left = lXOffset;	// 移动后，有图区域左上角X坐标为lXOffset	
		rectDst.right = lWidth;		// 移动后，有图区域右下角X坐标为lWidth
	}
	else
	{		
		bVisible = FALSE;			// X轴方向全部移出可视区域
	}
	
	// 平移后剩余图像在源图像中的X坐标
	rectSrc.left = rectDst.left - lXOffset;
	rectSrc.right = rectDst.right - lXOffset;
	
	//  计算rectSrc和rectDst的Y坐标
	if (lYOffset <= -lHeight)
	{		
		bVisible = FALSE;			// Y轴方向全部移出可视区域
	}
	else if (lYOffset <= 0)
	{		
		rectDst.top = 0;			// 移动后，有图区域左上角Y坐标为0
		
		// 移动后，有图区域右下角Y坐标为lHeight - |lYOffset| = lHeight + lYOffset
		rectDst.bottom = lHeight + lYOffset;
	}
	else if (lYOffset < lHeight)
	{		
		rectDst.top = lYOffset;		// 移动后，有图区域左上角Y坐标为lYOffset		
		rectDst.bottom = lHeight;	// 移动后，有图区域右下角Y坐标为lHeight
	}
	else
	{		
		bVisible = FALSE;			// X轴方向全部移出可视区域
	}
	
	// 平移后剩余图像在源图像中的Y坐标
	rectSrc.top = rectDst.top - lYOffset;
	rectSrc.bottom = rectDst.bottom - lYOffset;
	
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);
		
	if (bVisible)		// 如果有部分图像可见
	{
		// 平移图像
		for(i = 0; i < (rectSrc.bottom - rectSrc.top); i++)
		{
			// 要复制区域的起点，注意由于DIB图像内容是上下倒置的，第一行内容是保存
			// 在最后一行，因此复制区域的起点不是lpDIBBits + lLineBytes * (i + 
			// rectSrc.top) + rectSrc.left，而是 lpDIBBits + lLineBytes * 
			// (lHeight - i - rectSrc.top - 1) + rectSrc.left。
			
			lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - i - rectSrc.top
				- 1) + rectSrc.left;
			
			// 要目标区域的起点，同样注意上下倒置的问题。
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - i - rectDst.top
				- 1) + rectDst.left;
			
			// 拷贝每一行，宽度为rectSrc.right - rectSrc.left
			memcpy(lpDst, lpSrc, rectSrc.right - rectSrc.left);
			
		}
	}

	// 复制平移后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   MirrorDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   BOOL  bDirection   - 镜像的方向，TRUE表示水平镜像，FALSE表示垂直镜像
 * 返回值:
 *   BOOL               - 镜像成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来镜像DIB图像。可以指定镜像的方式是水平还是垂直。
 ************************************************************************/
BOOL CDibImage::MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						  BOOL bDirection)
{		
	LPSTR	lpSrc;						// 指向源图像的指针	
	LPSTR	lpDst;						// 指向要复制区域的指针	
	LPSTR	lpBits;						// 指向复制图像的指针
	HLOCAL	hBits;	
	LONG	i;							// 循环变量
	LONG	j;	
	LONG lLineBytes;					// 图像每行的字节数	
	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数		
	hBits = LocalAlloc(LHND, lLineBytes);	// 暂时分配内存，以保存一行图像
	if (hBits == NULL)
	{
		return FALSE;
	}
	
	lpBits = (char * )LocalLock(hBits);
	
	// 判断镜像方式
	if (bDirection)		// 水平镜像
	{	
		// 针对图像每行进行操作
		for(i = 0; i < lHeight; i++)
		{
			// 针对每行图像左半部分进行操作
			for(j = 0; j < lWidth / 2; j++)
			{				
				// 指向倒数第i行，第j个象素的指针
				lpSrc = (char *)lpDIBBits + lLineBytes * i + j;				
				// 指向倒数第i行，倒数第j个象素的指针
				lpDst = (char *)lpDIBBits + lLineBytes * (i + 1) - j;
				
				// 备份一个象素
				*lpBits = *lpDst;
				
				// 将倒数第i行，第j个象素复制到倒数第i行，倒数第j个象素
				*lpDst = *lpSrc;				
				// 将倒数第i行，倒数第j个象素复制到倒数第i行，第j个象素
				*lpSrc = *lpBits;
			}			
		}
	}
	else				// 垂直镜像
	{
		// 针对上半图像进行操作
		for(i = 0; i < lHeight / 2; i++)
		{			
			// 指向倒数第i行象素起点的指针
			lpSrc = (char *)lpDIBBits + lLineBytes * i;			
			// 指向第i行象素起点的指针
			lpDst = (char *)lpDIBBits + lLineBytes * (lHeight - i - 1);
			
			// 备份一行，宽度为lWidth
			memcpy(lpBits, lpDst, lLineBytes);
			
			// 将倒数第i行象素复制到第i行
			memcpy(lpDst, lpSrc, lLineBytes);			
			// 将第i行象素复制到倒数第i行
			memcpy(lpSrc, lpBits, lLineBytes);			
		}
	}
	
	LocalUnlock(hBits);
	LocalFree(hBits);
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   TransposeDIB()
 * 参数:
 *   LPSTR lpDIB		- 指向源DIB的指针
 * 返回值:
 *   BOOL               - 转置成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来转置DIB图像，即图像x、y坐标互换。函数将不会改变图像的大小，
 * 但是图像的高宽将互换。
 ************************************************************************/
BOOL CDibImage::TransposeDIB(LPSTR lpDIB)
{	
	LONG	lWidth;				// 图像的宽度
	LONG	lHeight;			// 图像的高度	
	LPSTR	lpDIBBits;			// 指向源图像的指针	
	LPSTR	lpSrc;				// 指向源象素的指针	
	LPSTR	lpDst;				// 指向转置图像对应象素的指针	
	LPSTR	lpNewDIBBits;		// 指向转置图像的指针
	HLOCAL	hNewDIBBits;
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针
	LONG	i;					
	LONG	j;	
	LONG lLineBytes;			// 图像每行的字节数
	LONG lNewLineBytes;			// 新图像每行的字节数

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	
	lpDIBBits = FindDIBBits(lpDIB);		// 找到源DIB图像象素起始位置	
	lWidth = DIBWidth(lpDIB);			// 获取图像的"宽度"（4的倍数）
	lHeight = DIBHeight(lpDIB);			// 获取图像的高度
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数
	lNewLineBytes = WIDTHBYTES(lHeight * 8);	// 计算新图像每行的字节数
	
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lNewLineBytes);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
		
	for(i = 0; i < lHeight; i++)	// 针对图像每行进行操作
	{		
		for(j = 0; j < lWidth; j++)	// 针对每行图像每列进行操作
		{			
			// 指向源DIB第i行，第j个象素的指针
			lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;	
			
			// 指向转置DIB第j行，第i个象素的指针
			// 注意此处lWidth和lHeight是源DIB的宽度和高度，应该互换
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lWidth - 1 - j) + i;
			
			// 复制象素
			*lpDst = *lpSrc;			
		}		
	}
	
	// 复制转置后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lNewLineBytes);
	
	// 互换DIB中图像的高宽
	if (IS_WIN30_DIB(lpDIB))
	{
		// 对于Windows 3.0 DIB
		lpbmi->biWidth = lHeight;		
		lpbmi->biHeight = lWidth;
	}
	else
	{
		// 对于其它格式的DIB
		lpbmc->bcWidth = (unsigned short) lHeight;
		lpbmc->bcHeight = (unsigned short) lWidth;
	}
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   ZoomDIB()
 * 参数:
 *   LPSTR lpDIB		- 指向源DIB的指针
 *   float fXZoomRatio	- X轴方向缩放比率
 *   float fYZoomRatio	- Y轴方向缩放比率
 * 返回值:
 *   HGLOBAL            - 缩放成功返回新DIB句柄，否则返回NULL。
 * 说明:
 *   该函数用来缩放DIB图像，返回新生成DIB的句柄。
 ************************************************************************/
HGLOBAL CDibImage::ZoomDIB(LPSTR lpDIB, float fXZoomRatio, float fYZoomRatio)
{
	LONG	lWidth;				// 源图像的宽度
	LONG	lHeight;			// 源图像的高度	
	LONG	lNewWidth;			// 缩放后图像的宽度
	LONG	lNewHeight;			// 缩放后图像的高度	
	LONG	lNewLineBytes;		// 缩放后图像的宽度（lNewWidth'，必须是4的倍数）	
	LPSTR	lpDIBBits;			// 指向源图像的指针	
	LPSTR	lpSrc;				// 指向源象素的指针	
	HDIB	hDIB;				// 缩放后新DIB句柄	
	LPSTR	lpDst;				// 指向缩放图像对应象素的指针	
	LPSTR	lpNewDIB;			// 指向缩放图像的指针
	LPSTR	lpNewDIBBits;	
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）	
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针	

	LONG	i;					// 循环变量（象素在新DIB中的坐标）
	LONG	j;	
	LONG	i0;					// 象素在源DIB中的坐标
	LONG	j0;
	
	
	LONG lLineBytes;			// 图像每行的字节数	
	lpDIBBits = FindDIBBits(lpDIB);	// 找到源DIB图像象素起始位置		
	lWidth = DIBWidth(lpDIB);	// 获取图像的宽度
	lLineBytes = WIDTHBYTES(lWidth * 8);	// 计算图像每行的字节数
	lHeight = DIBHeight(lpDIB);	// 获取图像的高度
	
	// 计算缩放后的图像实际宽度
	// 此处直接加0.5是由于强制类型转换时不四舍五入，而是直接截去小数部分
	lNewWidth = (LONG) (DIBWidth(lpDIB) * fXZoomRatio + 0.5);
	
	// 计算新图像每行的字节数
	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	
	// 计算缩放后的图像高度
	lNewHeight = (LONG) (lHeight * fYZoomRatio + 0.5);
	
	// 分配内存，以保存新DIB
	hDIB = (HDIB) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + 
		*(LPDWORD)lpDIB + PaletteSize(lpDIB));	
	if (hDIB == NULL)
	{
		return NULL;
	}
	
	lpNewDIB =  (char * )::GlobalLock((HGLOBAL) hDIB);
	
	// 复制DIB信息头和调色板
	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + PaletteSize(lpDIB));
	
	// 找到新DIB象素起始位置
	lpNewDIBBits = FindDIBBits(lpNewDIB);
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;
	
	// 更新DIB中图像的高度和宽度
	if (IS_WIN30_DIB(lpNewDIB))
	{
		// 对于Windows 3.0 DIB
		lpbmi->biWidth = lNewWidth;
		lpbmi->biHeight = lNewHeight;
	}
	else
	{
		// 对于其它格式的DIB
		lpbmc->bcWidth = (unsigned short) lNewWidth;
		lpbmc->bcHeight = (unsigned short) lNewHeight;
	}
		
	for(i = 0; i < lNewHeight; i++)		// 针对图像每行进行操作
	{		
		for(j = 0; j < lNewWidth; j++)	// 针对图像每列进行操作
		{			
			// 指向新DIB第i行，第j个象素的指针
			// 注意此处宽度和高度是新DIB的宽度和高度
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			
			// 计算该象素在源DIB中的坐标
			i0 = (LONG) (i / fYZoomRatio + 0.5);
			j0 = (LONG) (j / fXZoomRatio + 0.5);
			
			// 判断是否在源图范围内
			if( (j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{				
				// 指向源DIB第i0行，第j0个象素的指针
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;				
				// 复制象素
				*lpDst = *lpSrc;
			}
			else
			{
				// 对于源图中没有的象素，直接赋值为255
				* ((unsigned char*)lpDst) = 255;
			}			
		}		
	}
	
	return hDIB;
}

/*************************************************************************
 * 函数名称：
 *   RotateDIB()
 * 参数:
 *   LPSTR lpDIB		- 指向源DIB的指针
 *   int iRotateAngle	- 旋转的角度（0-360度）
 * 返回值:
 *   HGLOBAL            - 旋转成功返回新DIB句柄，否则返回NULL。
 * 说明:
 *   该函数用来以图像中心为中心旋转DIB图像，返回新生成DIB的句柄。
 * 调用该函数会自动扩大图像以显示所有的象素。函数中采用最邻近插
 * 值算法进行插值。
 ************************************************************************/
HGLOBAL CDibImage::RotateDIB(LPSTR lpDIB, int iRotateAngle)
{	
	LONG	lWidth;				// 源图像的宽度
	LONG	lHeight;			// 源图像的高度	
	LONG	lNewWidth;			// 旋转后图像的宽度
	LONG	lNewHeight;			// 旋转后图像的高度	
	LONG	lLineBytes;			// 图像每行的字节数	
	LONG	lNewLineBytes;		// 旋转后图像的宽度（lNewWidth'，必须是4的倍数）	
	LPSTR	lpDIBBits;			// 指向源图像的指针	
	LPSTR	lpSrc;				// 指向源象素的指针	
	HDIB	hDIB;				// 旋转后新DIB句柄	
	LPSTR	lpDst;				// 指向旋转图像对应象素的指针	
	LPSTR	lpNewDIB;			// 指向旋转图像的指针
	LPSTR	lpNewDIBBits;	
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）	
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针
		
	LONG	i;					// 循环变量（象素在新DIB中的坐标）
	LONG	j;	
	LONG	i0;					// 象素在源DIB中的坐标
	LONG	j0;	
	
	float	fRotateAngle;		// 旋转角度（弧度）	
	float	fSina, fCosa;		// 旋转角度的正弦和余弦	
	// 源图四个角的坐标（以图像中心为坐标系原点）
	float	fSrcX1,fSrcY1,fSrcX2,fSrcY2,fSrcX3,fSrcY3,fSrcX4,fSrcY4;	
	// 旋转后四个角的坐标（以图像中心为坐标系原点）
	float	fDstX1,fDstY1,fDstX2,fDstY2,fDstX3,fDstY3,fDstX4,fDstY4;
	float	f1,f2;
		
	lpDIBBits = FindDIBBits(lpDIB);		// 找到源DIB图像象素起始位置	
	lWidth = DIBWidth(lpDIB);			// 获取图像的"宽度"（4的倍数）	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	lHeight = DIBHeight(lpDIB);			// 获取图像的高度
	
	// 将旋转角度从度转换到弧度
	fRotateAngle = (float) RADIAN(iRotateAngle);	
	fSina = (float) sin((double)fRotateAngle);	// 计算旋转角度的正弦
	fCosa = (float) cos((double)fRotateAngle);	// 计算旋转角度的余弦
	
	// 计算原图的四个角的坐标（以图像中心为坐标系原点）
	fSrcX1 = (float) (- (lWidth  - 1) / 2);
	fSrcY1 = (float) (  (lHeight - 1) / 2);
	fSrcX2 = (float) (  (lWidth  - 1) / 2);
	fSrcY2 = (float) (  (lHeight - 1) / 2);
	fSrcX3 = (float) (- (lWidth  - 1) / 2);
	fSrcY3 = (float) (- (lHeight - 1) / 2);
	fSrcX4 = (float) (  (lWidth  - 1) / 2);
	fSrcY4 = (float) (- (lHeight - 1) / 2);
	
	// 计算新图四个角的坐标（以图像中心为坐标系原点）
	fDstX1 =  fCosa * fSrcX1 + fSina * fSrcY1;
	fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	fDstX2 =  fCosa * fSrcX2 + fSina * fSrcY2;
	fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	fDstX3 =  fCosa * fSrcX3 + fSina * fSrcY3;
	fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	fDstX4 =  fCosa * fSrcX4 + fSina * fSrcY4;
	fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;
	
	// 计算旋转后的图像实际宽度
	lNewWidth  = (LONG)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	
	// 计算新图像每行的字节数
	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	
	// 计算旋转后的图像高度
	lNewHeight = (LONG)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	
	// 两个常数，这样不用以后每次都计算了
	f1 = (float) (-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina
		+ 0.5 * (lWidth  - 1));
	f2 = (float) ( 0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa
		+ 0.5 * (lHeight - 1));
	
	// 分配内存，以保存新DIB
	hDIB = (HDIB) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + 
		*(LPDWORD)lpDIB + PaletteSize(lpDIB));
	if (hDIB == NULL)
	{
		return NULL;
	}

	lpNewDIB =  (char * )::GlobalLock((HGLOBAL) hDIB);
	
	// 复制DIB信息头和调色板
	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + PaletteSize(lpDIB));
	
	// 找到新DIB象素起始位置
	lpNewDIBBits = FindDIBBits(lpNewDIB);
	
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;

	// 更新DIB中图像的高度和宽度
	if (IS_WIN30_DIB(lpNewDIB))
	{
		// 对于Windows 3.0 DIB
		lpbmi->biWidth = lNewWidth;
		lpbmi->biHeight = lNewHeight;
	}
	else
	{
		// 对于其它格式的DIB
		lpbmc->bcWidth = (unsigned short) lNewWidth;
		lpbmc->bcHeight = (unsigned short) lNewHeight;
	}
		
	for(i = 0; i < lNewHeight; i++)		// 针对图像每行进行操作
	{		
		for(j = 0; j < lNewWidth; j++)	// 针对图像每列进行操作
		{
			// 指向新DIB第i行，第j个象素的指针
			// 注意此处宽度和高度是新DIB的宽度和高度
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			
			// 计算该象素在源DIB中的坐标
			i0 = (LONG) (-((float) j) * fSina + ((float) i) * fCosa + f2 + 0.5);
			j0 = (LONG) ( ((float) j) * fCosa + ((float) i) * fSina + f1 + 0.5);
			
			// 判断是否在源图范围内
			if( (j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// 指向源DIB第i0行，第j0个象素的指针
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;
				
				// 复制象素
				*lpDst = *lpSrc;
			}
			else
			{
				// 对于源图中没有的象素，直接赋值为255
				* ((unsigned char*)lpDst) = 255;
			}			
		}		
	}
	
	return hDIB;
}

/*************************************************************************
 * 函数名称：
 *   RotateDIB2()
 * 参数:
 *   LPSTR lpDIB		- 指向源DIB的指针
 *   int iRotateAngle	- 旋转的角度（0-360度）
 * 返回值:
 *   HGLOBAL            - 旋转成功返回新DIB句柄，否则返回NULL。
 * 说明:
 *   该函数用来以图像中心为中心旋转DIB图像，返回新生成DIB的句柄。
 * 调用该函数会自动扩大图像以显示所有的象素。函数中采用双线性插
 * 值算法进行插值。
 ************************************************************************/
HGLOBAL CDibImage::RotateDIB2(LPSTR lpDIB, int iRotateAngle)
{	
	LONG	lWidth;				// 源图像的宽度
	LONG	lHeight;			// 源图像的高度	
	LONG	lNewWidth;			// 旋转后图像的宽度
	LONG	lNewHeight;			// 旋转后图像的高度
	LONG	lNewLineBytes;		// 旋转后图像的宽度（lNewWidth'，必须是4的倍数）	
	LPSTR	lpDIBBits;			// 指向源图像的指针	
	HDIB	hDIB;				// 旋转后新DIB句柄	
	LPSTR	lpDst;				// 指向旋转图像对应象素的指针	
	LPSTR	lpNewDIB;			// 指向旋转图像的指针
	LPSTR	lpNewDIBBits;	
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）	
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针
		
	LONG	i;					// 循环变量（象素在新DIB中的坐标）
	LONG	j;	
	FLOAT	i0;					// 象素在源DIB中的坐标
	FLOAT	j0;	
	
	float	fRotateAngle;		// 旋转角度（弧度）	
	float	fSina, fCosa;		// 旋转角度的正弦和余弦	
	// 源图四个角的坐标（以图像中心为坐标系原点）
	float	fSrcX1,fSrcY1,fSrcX2,fSrcY2,fSrcX3,fSrcY3,fSrcX4,fSrcY4;	
	// 旋转后四个角的坐标（以图像中心为坐标系原点）
	float	fDstX1,fDstY1,fDstX2,fDstY2,fDstX3,fDstY3,fDstX4,fDstY4;
	float	f1,f2;
	
	
	lpDIBBits = FindDIBBits(lpDIB);		// 找到源DIB图像象素起始位置	
	lWidth = DIBWidth(lpDIB);			// 获取图像的宽度
	lHeight = DIBHeight(lpDIB);			// 获取图像的高度
	
	// 将旋转角度从度转换到弧度
	fRotateAngle = (float) RADIAN(iRotateAngle);
		
	fSina = (float) sin((double)fRotateAngle);	// 计算旋转角度的正弦
	fCosa = (float) cos((double)fRotateAngle);	// 计算旋转角度的余弦
	
	// 计算原图的四个角的坐标（以图像中心为坐标系原点）
	fSrcX1 = (float) (- (lWidth  - 1) / 2);
	fSrcY1 = (float) (  (lHeight - 1) / 2);
	fSrcX2 = (float) (  (lWidth  - 1) / 2);
	fSrcY2 = (float) (  (lHeight - 1) / 2);
	fSrcX3 = (float) (- (lWidth  - 1) / 2);
	fSrcY3 = (float) (- (lHeight - 1) / 2);
	fSrcX4 = (float) (  (lWidth  - 1) / 2);
	fSrcY4 = (float) (- (lHeight - 1) / 2);
	
	// 计算新图四个角的坐标（以图像中心为坐标系原点）
	fDstX1 =  fCosa * fSrcX1 + fSina * fSrcY1;
	fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	fDstX2 =  fCosa * fSrcX2 + fSina * fSrcY2;
	fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	fDstX3 =  fCosa * fSrcX3 + fSina * fSrcY3;
	fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	fDstX4 =  fCosa * fSrcX4 + fSina * fSrcY4;
	fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;
	
	// 计算旋转后的图像实际宽度
	lNewWidth  = (LONG)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	
	// 计算旋转后的图像高度
	lNewHeight = (LONG)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	
	f1 = (float) (-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina
		+ 0.5 * (lWidth  - 1));
	f2 = (float) ( 0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa
		+ 0.5 * (lHeight - 1));
	
	// 分配内存，以保存新DIB
	hDIB = (HDIB) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + 
		*(LPDWORD)lpDIB + PaletteSize(lpDIB));	
	if (hDIB == NULL)
	{
		return NULL;
	}

	lpNewDIB =  (char * )::GlobalLock((HGLOBAL) hDIB);
	
	// 复制DIB信息头和调色板
	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + PaletteSize(lpDIB));
	
	// 找到新DIB象素起始位置
	lpNewDIBBits = FindDIBBits(lpNewDIB);
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;

	// 更新DIB中图像的高度和宽度
	if (IS_WIN30_DIB(lpNewDIB))
	{
		// 对于Windows 3.0 DIB
		lpbmi->biWidth = lNewWidth;
		lpbmi->biHeight = lNewHeight;
	}
	else
	{
		// 对于其它格式的DIB
		lpbmc->bcWidth = (unsigned short) lNewWidth;
		lpbmc->bcHeight = (unsigned short) lNewHeight;
	}
		
	for(i = 0; i < lNewHeight; i++)		// 针对图像每行进行操作
	{			
		for(j = 0; j < lNewWidth; j++)	// 针对图像每列进行操作
		{
			// 指向新DIB第i行，第j个象素的指针
			// 注意此处宽度和高度是新DIB的宽度和高度
			lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			
			// 计算该象素在源DIB中的坐标
			i0 = -((float) j) * fSina + ((float) i) * fCosa + f2;
			j0 =  ((float) j) * fCosa + ((float) i) * fSina + f1;
			
			// 利用双线性插值算法来估算象素值
			*lpDst = Interpolation (lpDIBBits, lWidth, lHeight, j0, i0);			
		}		
	}
	
	return hDIB;
}

/*************************************************************************
 * 函数名称：
 *   Interpolation()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   FLOAT x			- 插值元素的x坐标
 *   FLOAT y		    - 插值元素的y坐标
 * 返回值:
 *   unsigned char      - 返回插值计算结果。
 * 说明:
 *   该函数利用双线性插值算法来估算象素值。对于超出图像范围的象素，
 * 直接返回255。
 ************************************************************************/
unsigned char CDibImage::Interpolation (LPSTR lpDIBBits, LONG lWidth, 
										LONG lHeight, FLOAT x, FLOAT y)
{
	// 四个最临近象素的坐标(i1, j1), (i2, j1), (i1, j2), (i2, j2)
	LONG	i1, i2;
	LONG	j1, j2;
	
	unsigned char	f1, f2, f3, f4;	// 四个最临近象素值	
	unsigned char	f12, f34;		// 二个插值中间值	

	// 定义一个值，当象素坐标相差小于改值时认为坐标相同
	FLOAT			EXP;	

	LONG lLineBytes;				// 图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	EXP = (FLOAT) 0.0001;
	
	// 计算四个最临近象素的坐标
	i1 = (LONG) x;
	i2 = i1 + 1;
	j1 = (LONG) y;
	j2 = j1 + 1;
	
	// 根据不同情况分别处理
	if( (x < 0) || (x > lWidth - 1) || (y < 0) || (y > lHeight - 1))
	{		
		return 255;		// 要计算的点不在源图范围内，直接返回255。
	}
	else
	{
		if (fabs(x - lWidth + 1) <= EXP)
		{
			// 要计算的点在图像右边缘上
			if (fabs(y - lHeight + 1) <= EXP)
			{
				// 要计算的点正好是图像最右下角那一个象素，直接返回该点象素值
				f1 = *((unsigned char *)lpDIBBits + lLineBytes * 
					(lHeight - 1 - j1) + i1);
				return f1;
			}
			else
			{
				// 在图像右边缘上且不是最后一点，直接一次插值即可
				f1 = *((unsigned char *)lpDIBBits + lLineBytes * 
					(lHeight - 1 - j1) + i1);
				f3 = *((unsigned char *)lpDIBBits + lLineBytes * 
					(lHeight - 1 - j1) + i2);
				
				// 返回插值结果
				return ((unsigned char) (f1 + (y -j1) * (f3 - f1)));
			}
		}
		else if (fabs(y - lHeight + 1) <= EXP)
		{
			// 要计算的点在图像下边缘上且不是最后一点，直接一次插值即可
			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			
			// 返回插值结果
			return ((unsigned char) (f1 + (x -i1) * (f2 - f1)));
		}
		else
		{
			// 计算四个最临近象素值
			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			f3 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
			f4 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i2);
			
			// 插值1
			f12 = (unsigned char) (f1 + (x - i1) * (f2 - f1));			
			// 插值2
			f34 = (unsigned char) (f3 + (x - i1) * (f4 - f3));			
			// 插值3
			return ((unsigned char) (f12 + (y -j1) * (f34 - f12)));
		}
	}
}

//////////////////////////////////////////////////////////////////////
//  图像正交变换函数
//////////////////////////////////////////////////////////////////////
/*************************************************************************
 * 函数名称：
 *   FFT()
 * 参数:
 *   complex<double> * TD	- 指向时域数组的指针
 *   complex<double> * FD	- 指向频域数组的指针
 *   r						－2的幂数，即迭代次数
 * 返回值:
 *   无。
 * 说明:
 *   该函数用来实现快速付立叶变换。
 ************************************************************************/
VOID CDibImage::FFT(complex<double> * TD, complex<double> * FD, int r)
{	
	LONG	count;				// 付立叶变换点数	
	int		i,j,k;				// 循环变量
	int		bfsize,p;	
	double	angle;				// 角度	
	complex<double> *W,*X1,*X2,*X;
	
	count = 1 << r;				// 计算付立叶变换点数
	
	// 分配运算所需存储器
	W  = new complex<double>[count / 2];
	X1 = new complex<double>[count];
	X2 = new complex<double>[count];
	
	// 计算加权系数
	for(i = 0; i < count / 2; i++)
	{
		angle = -i * PI * 2 / count;
		W[i] = complex<double> (cos(angle), sin(angle));
	}
	
	// 将时域点写入X1
	memcpy(X1, TD, sizeof(complex<double>) * count);
	
	// 采用蝶形算法进行快速付立叶变换
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
	
	// 重新排序
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
 * 函数名称：
 *   IFFT()
 * 参数:
 *   complex<double> * FD	- 指向频域值的指针
 *   complex<double> * TD	- 指向时域值的指针
 *   r						－2的幂数
 * 返回值:
 *   无。
 * 说明:
 *   该函数用来实现快速付立叶反变换。
 ************************************************************************/
VOID CDibImage::IFFT(complex<double> * FD, complex<double> * TD, int r)
{	
	LONG	count;					// 付立叶变换点数	
	int		i;						// 循环变量	
	complex<double> *X;	
	
	count = 1 << r;					// 计算付立叶变换点数
	X = new complex<double>[count];	// 分配运算所需存储器
	memcpy(X, FD, sizeof(complex<double>) * count);	// 将频域点写入X
	
	// 求共轭
	for(i = 0; i < count; i++)
	{
		X[i] = complex<double> (X[i].real(), -X[i].imag());
	}
	
	FFT(X, TD, r);					// 调用快速付立叶变换
	
	// 求时域点的共轭
	for(i = 0; i < count; i++)
	{
		TD[i] = complex<double> (TD[i].real() / count, -TD[i].imag() / count);
	}
	
	delete X;
}

/*************************************************************************
 * 函数名称：
 *   Fourier()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行付立叶变换。
 ************************************************************************/
BOOL CDibImage::Fourier(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	unsigned char*	lpSrc;	// 指向源图像的指针
	double	dTemp;	
	LONG	i;				// 循环变量
	LONG	j;	
	LONG	w;				// 进行付立叶变换的宽度（2的整数次方）
	LONG	h;				// 进行付立叶变换的高度（2的整数次方）	
	int		wp;
	int		hp;
		
	LONG	lLineBytes;		// 图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);	// 计算图像每行的字节数
	
	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
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
		
	for(i = 0; i < h; i++)			// 行
	{		
		for(j = 0; j < w; j++)		// 列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 给时域赋值
			TD[j + w * i] = complex<double>(*(lpSrc), 0);
		}
	}
	
	for(i = 0; i < h; i++)
	{
		// 对y方向进行快速付立叶变换
		FFT(&TD[w * i], &FD[w * i], wp);
	}
	
	// 保存变换结果
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			TD[i + h * j] = FD[j + w * i];
		}
	}
	
	for(i = 0; i < w; i++)
	{
		// 对x方向进行快速付立叶变换
		FFT(&TD[i * h], &FD[i * h], hp);
	}
		
	for(i = 0; i < h; i++)			// 行
	{		
		for(j = 0; j < w; j++)		// 列
		{
			// 计算频谱
			dTemp = sqrt(FD[j * h + i].real() * FD[j * h + i].real() + 
				         FD[j * h + i].imag() * FD[j * h + i].imag()) / 100;
			if (dTemp > 255)
			{
				dTemp = 255;
			}
			
			// 指向DIB第(i<h/2 ? i+h/2 : i-h/2)行，第(j<w/2 ? j+w/2 : j-w/2)个
			// 象素的指针，此处不直接取i和j，是为了将变换后的原点移到中心
			// lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight-1-i) + j;
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * 
				(lHeight - 1 - (i<h/2 ? i+h/2 : i-h/2)) + (j<w/2 ? j+w/2 : j-w/2);
			
			// 更新源图像
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete TD;
	delete FD;
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   DCT()
 * 参数:
 *   double * f				- 指向时域值的指针
 *   double * F				- 指向频域值的指针
 *   r						－2的幂数
 * 返回值:
 *   无。
 * 说明:
 *   该函数用来实现快速离散余弦变换。该函数利用2N点的快速付立叶变换
 * 来实现离散余弦变换。
 ************************************************************************/
VOID CDibImage::DCT(double *f, double *F, int r)
{	
	LONG	count;			// 离散余弦变换点数	
	int		i;				// 循环变量	
	double	dTemp;	
	complex<double> *X;
		
	count = 1<<r;			// 计算离散余弦变换点数	

	X = new complex<double>[count*2];	
	memset(X, 0, sizeof(complex<double>) * count * 2);	// 赋初值为0
	
	// 将时域点写入数组X
	for(i=0;i<count;i++)
	{
		X[i] = complex<double> (f[i], 0);
	}
		
	FFT(X,X,r+1);				// 调用快速付立叶变换		
	dTemp = 1/sqrt((float)count);		// 调整系数		
	F[0] = X[0].real() * dTemp;	// 求F[0]	
	dTemp *= sqrt((float)2);
	
	// 求F[u]	
	for(i = 1; i < count; i++)
	{
		F[i]=(X[i].real() * cos(i*PI/(count*2)) + X[i].imag() * 
			sin(i*PI/(count*2))) * dTemp;
	}
	
	delete X;
}

/*************************************************************************
 * 函数名称：
 *   IDCT()
 * 参数:
 *   double * F				- 指向频域值的指针
 *   double * f				- 指向时域值的指针
 *   r						－2的幂数
 * 返回值:
 *   无。
 * 说明:
 *   该函数用来实现快速离散余弦反变换。该函数也利用2N点的快速付立叶变换
 * 来实现离散余弦反变换。
 ************************************************************************/
VOID CDibImage::IDCT(double *F, double *f, int r)
{
	LONG	count;			// 离散余弦反变换点数
	int		i;				// 循环变量
	double	dTemp, d0;	
	complex<double> *X;
		
	count = 1<<r;			// 计算离散余弦变换点数

	X = new complex<double>[count*2];	
	memset(X, 0, sizeof(complex<double>) * count * 2);	// 赋初值为0
	
	// 将频域变换后点写入数组X
	for(i=0;i<count;i++)
	{
		X[i] = complex<double> (F[i] * cos(i*PI/(count*2)), F[i] * 
			sin(i*PI/(count*2)));
	}
	
	IFFT(X,X,r+1);		// 调用快速付立叶反变换
	
	// 调整系数
	dTemp = sqrt(2.0/count);
	d0 = (sqrt(1.0/count) - dTemp) * F[0];
	
	for(i = 0; i < count; i++)
	{
		f[i] = d0 + X[i].real()* dTemp * 2 * count;
	}
	
	delete X;
}

/*************************************************************************
 * 函数名称：
 *   DIBDct()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行离散余弦变换。
 ************************************************************************/
BOOL CDibImage::DIBDct(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	unsigned char*	lpSrc;	// 指向源图像的指针	
	LONG	i;				// 循环变量
	LONG	j;	
	LONG	w;				// 进行付立叶变换的宽度（2的整数次方）
	LONG	h;				// 进行付立叶变换的高度（2的整数次方）
	double	dTemp;	
	int		wp;
	int		hp;
		
	LONG	lLineBytes;		// 图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行离散余弦变换的宽度和高度（2的整数次方）
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
		
	for(i = 0; i < h; i++)			// 行
	{		
		for(j = 0; j < w; j++)		// 列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 给时域赋值
			f[j + i * w] = *(lpSrc);
		}
	}
	
	for(i = 0; i < h; i++)
	{
		// 对y方向进行离散余弦变换
		DCT(&f[w * i], &F[w * i], wp);
	}
	
	// 保存计算结果
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			f[j * h + i] = F[j + w * i];
		}
	}
	
	for(j = 0; j < w; j++)
	{
		// 对x方向进行离散余弦变换
		DCT(&f[j * h], &F[j * h], hp);
	}	
	
	for(i = 0; i < h; i++)			// 行
	{		
		for(j = 0; j < w; j++)		// 列
		{
			dTemp = fabs(F[j*h+i]);	// 计算频谱
			
			// 判断是否超过255
			if (dTemp > 255)
			{
				dTemp = 255;
			}
			
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 更新源图像
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete f;
	delete F;

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   WALSH()
 * 参数:
 *   double * f				- 指向时域值的指针
 *   double * F				- 指向频域值的指针
 *   r						－2的幂数
 * 返回值:
 *   无。
 * 说明:
 *   该函数用来实现快速沃尔什-哈达玛变换。
 ************************************************************************/
VOID CDibImage::WALSH(double *f, double *F, int r)
{	
	LONG	count;				// 沃尔什-哈达玛变换点数	
	int		i,j,k;				// 循环变量
	int		bfsize,p;	
	double *X1,*X2,*X;
		
	count = 1 << r;				// 计算快速沃尔什变换点数	
	X1 = new double[count];		// 分配运算所需的数组
	X2 = new double[count];		// 分配运算所需的数组
		
	memcpy(X1, f, sizeof(double) * count);	// 将时域点写入数组X1
	
	// 蝶形运算
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
		
		// 互换X1和X2  
		X = X1;
		X1 = X2;
		X2 = X;
	}
	
	// 调整系数
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
 * 函数名称：
 *   IWALSH()
 * 参数:
 *   double * F				- 指向频域值的指针
 *   double * f				- 指向时域值的指针
 *   r						－2的幂数
 * 返回值:
 *   无。
 * 说明:
 *   该函数用来实现快速沃尔什-哈达玛反变换。
 ************************************************************************/
VOID CDibImage::IWALSH(double *F, double *f, int r)
{	
	LONG	count;				// 变换点数	
	int		i;					// 循环变量
		
	count = 1 << r;				// 计算变换点数	
	WALSH(F, f, r);				// 调用快速沃尔什－哈达玛变换进行反变换
		
	for(i = 0; i < count; i++)	// 调整系数
	{
		f[i] *= count;
	}
}

/*************************************************************************
 * 函数名称：
 *   DIBWalsh()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行沃尔什-哈达玛变换。函数首先对图像每列进行一维
 * 沃尔什－哈达玛变换，然后对变换结果的每行进行一维沃尔什－哈达玛变换。
 ************************************************************************/
BOOL CDibImage::DIBWalsh(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	unsigned char*	lpSrc;		// 指向源图像的指针	
	LONG	i,j;				// 循环变量
	LONG	w;					// 进行付立叶变换的宽度（2的整数次方）
	LONG	h;					// 进行付立叶变换的高度（2的整数次方）
	double	dTemp;	
	int		wp;
	int		hp;
	
	LONG	lLineBytes;			// 图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行离散余弦变换的宽度和高度（2的整数次方）
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
		
	for(i = 0; i < h; i++)			// 行
	{		
		for(j = 0; j < w; j++)		// 列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 给时域赋值
			f[j + i * w] = *(lpSrc);
		}
	}
	
	for(i = 0; i < h; i++)
	{
		// 对y方向进行沃尔什-哈达玛变换
		WALSH(f + w * i, F + w * i, wp);
	}
	
	// 保存计算结果
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w; j++)
		{
			f[j * h + i] = F[j + w * i];
		}
	}
	
	for(j = 0; j < w; j++)
	{
		// 对x方向进行沃尔什-哈达玛变换
		WALSH(f + j * h, F + j * h, hp);
	}
		
	for(i = 0; i < h; i++)			// 行
	{		
		for(j = 0; j < w; j++)		// 列
		{			
			dTemp = fabs(F[j * h + i] * 1000);	// 计算频谱			
			if (dTemp > 255)			
			{					
				dTemp = 255;				
			}
			
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 更新源图像
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete f;
	delete F;

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   DIBWalsh1()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行沃尔什-哈达玛变换。于上面不同的是，此处是将二维
 * 矩阵转换成一个列向量，然后对该列向量进行一次一维沃尔什-哈达玛变换。
 ************************************************************************/
BOOL CDibImage::DIBWalsh1(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	unsigned char*	lpSrc;		// 指向源图像的指针
	LONG	i,j;				// 循环变量	
	LONG	w;					// 进行付立叶变换的宽度（2的整数次方）
	LONG	h;					// 进行付立叶变换的高度（2的整数次方）
	double	dTemp;	
	int		wp;
	int		hp;
		
	LONG	lLineBytes;			// 图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行离散余弦变换的宽度和高度（2的整数次方）
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
		
	for(i = 0; i < w; i++)			// 列
	{
		for(j = 0; j < h; j++)		// 行
		{
			// 指向DIB第j行，第i个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j) + i;
			
			// 给时域赋值
			f[j + i * w] = *(lpSrc);
		}
	}
	
	// 调用快速沃尔什－哈达玛变换
	WALSH(f, F, wp + hp);
		
	for(i = 0; i < w; i++)			// 列
	{		
		for(j = 0; j < h; j++)		// 行
		{			
			dTemp = fabs(F[i * w + j] * 1000);	// 计算频谱
			if (dTemp > 255)
			{
				dTemp = 255;
			}
			
			// 指向DIB第j行，第i个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j) + i;
			
			// 更新源图像
			* (lpSrc) = (BYTE)(dTemp);
		}
	}
	
	delete f;
	delete F;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//	图像模板变换函数
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * 函数名称：
 *   Template()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   int   iTempH		- 模板的高度
 *   int   iTempW		- 模板的宽度
 *   int   iTempMX		- 模板的中心元素X坐标 ( < iTempW - 1)
 *   int   iTempMY		- 模板的中心元素Y坐标 ( < iTempH - 1)
 *	 FLOAT * fpArray	- 指向模板数组的指针
 *	 FLOAT fCoef		- 模板系数
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用指定的模板（任意大小）来对图像进行操作，参数iTempH指定模板
 * 的高度，参数iTempW指定模板的宽度，参数iTempMX和iTempMY指定模板的中心
 * 元素坐标，参数fpArray指定模板元素，fCoef指定系数。
 ************************************************************************/
BOOL CDibImage::Template(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
					 int iTempH, int iTempW, int iTempMX, int iTempMY,
					 FLOAT * fpArray, FLOAT fCoef)
{	
	LPSTR	lpNewDIBBits;		// 指向复制图像的指针
	HLOCAL	hNewDIBBits;	
	unsigned char*	lpSrc;		// 指向源图像的指针	
	unsigned char*	lpDst;		// 指向要复制区域的指针	
	LONG	i,j,k,l;			// 循环变量	
	FLOAT	fResult;			// 计算结果
		
	LONG lLineBytes;			// 图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// 初始化图像为原始图像
	memcpy(lpNewDIBBits, lpDIBBits, lLineBytes * lHeight);
		
	for(i = iTempMY; i < lHeight - iTempH + iTempMY + 1; i++)	// 行(除去边缘几行)
	{		
		for(j = iTempMX; j < lWidth - iTempW + iTempMX + 1; j++)// 列(除去边缘几列)
		{
			// 指向新DIB第i行，第j个象素的指针
			lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			fResult = 0;

			for (k = 0; k < iTempH; k++)
			{
				for (l = 0; l < iTempW; l++)
				{
					// 指向DIB第i - iTempMY + k行，第j - iTempMX + l个象素的指针
					lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 
						- i + iTempMY - k) + j - iTempMX + l;
					
					// 保存象素值
					fResult += (* lpSrc) * fpArray[k * iTempW + l];
				}
			}
						
			fResult *= fCoef;					// 乘上系数			
			fResult = (FLOAT ) fabs(fResult);	// 取绝对值
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
	
	// 复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   MedianFilter()
 * 参数:
 *   LPSTR lpDIBBits		- 指向源DIB图像指针
 *   LONG  lWidth			- 源图像宽度（象素数）
 *   LONG  lHeight			- 源图像高度（象素数）
 *   int   iFilterH			- 滤波器的高度
 *   int   iFilterW			- 滤波器的宽度
 *   int   iFilterMX		- 滤波器的中心元素X坐标
 *   int   iFilterMY		- 滤波器的中心元素Y坐标
 * 返回值:
 *   BOOL					- 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数对DIB图像进行中值滤波。
 ************************************************************************/
BOOL CDibImage::MedianFilter(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						 int iFilterH, int iFilterW, int iFilterMX, int iFilterMY)
{		
	unsigned char*	lpSrc;			// 指向源图像的指针	
	unsigned char*	lpDst;			// 指向要复制区域的指针	
	LPSTR			lpNewDIBBits;	// 指向复制图像的指针
	HLOCAL			hNewDIBBits;	
	unsigned char	* aValue;		// 指向滤波器数组的指针
	HLOCAL			hArray;	
	LONG			i,j,k,l;		// 循环变量	
	
	LONG			lLineBytes;		// 图像每行的字节数	
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// 初始化图像为原始图像
	memcpy(lpNewDIBBits, lpDIBBits, lLineBytes * lHeight);
	
	// 暂时分配内存，以保存滤波器数组
	hArray = LocalAlloc(LHND, iFilterH * iFilterW);
	if (hArray == NULL)
	{
		LocalUnlock(hNewDIBBits);
		LocalFree(hNewDIBBits);

		return FALSE;
	}
	
	aValue = (unsigned char * )LocalLock(hArray);
	
	// 开始中值滤波
	// 行(除去边缘几行)
	for(i = iFilterMY; i < lHeight - iFilterH + iFilterMY + 1; i++)
	{
		// 列(除去边缘几列)
		for(j = iFilterMX; j < lWidth - iFilterW + iFilterMX + 1; j++)
		{
			// 指向新DIB第i行，第j个象素的指针
			lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 读取滤波器数组
			for (k = 0; k < iFilterH; k++)
			{
				for (l = 0; l < iFilterW; l++)
				{
					// 指向DIB第i - iFilterMY + k行，第j - iFilterMX + l个象素的指针
					lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 
						- i + iFilterMY - k) + j - iFilterMX + l;
					
					// 保存象素值
					aValue[k * iFilterW + l] = *lpSrc;
				}
			}
			
			// 获取中值
			* lpDst = GetMedianNum(aValue, iFilterH * iFilterW);
		}
	}
	
	// 复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	LocalUnlock(hArray);
	LocalFree(hArray);
	
	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   GetMedianNum()
 * 参数:
 *   unsigned char * bpArray	- 指向要获取中值的数组指针
 *   int   iFilterLen			- 数组长度
 * 返回值:
 *   unsigned char      - 返回指定数组的中值。
 * 说明:
 *   该函数用冒泡法对一维数组进行排序，并返回数组元素的中值。
 ************************************************************************/
unsigned char CDibImage::GetMedianNum(unsigned char * bArray, int iFilterLen)
{
	int		i,j;			// 循环变量
	unsigned char bTemp;
	
	// 用冒泡法对数组进行排序
	for (j = 0; j < iFilterLen - 1; j ++)
	{
		for (i = 0; i < iFilterLen - j - 1; i ++)
		{
			if (bArray[i] > bArray[i + 1])
			{
				// 互换
				bTemp = bArray[i];
				bArray[i] = bArray[i + 1];
				bArray[i + 1] = bTemp;
			}
		}
	}
	
	// 计算中值
	if ((iFilterLen & 1) > 0)
	{
		// 数组有奇数个元素，返回中间一个元素
		bTemp = bArray[(iFilterLen + 1) / 2];
	}
	else
	{
		// 数组有偶数个元素，返回中间两个元素平均值
		bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
	}
	
	return bTemp;
}

/*************************************************************************
 * 函数名称：
 *   GradSharp()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   BYTE  bThre		- 阈值
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用来对图像进行梯度锐化。
 ************************************************************************/
BOOL CDibImage::GradSharp(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BYTE bThre)
{	
			
	unsigned char*	lpSrc;		// 指向源图像的指针
	unsigned char*	lpSrc1;
	unsigned char*	lpSrc2;	
	LONG	i,j;				// 循环变量
	LONG	lLineBytes;			// 图像每行的字节数
	BYTE	bTemp;
	
	lLineBytes = WIDTHBYTES(lWidth * 8);
		
	for(i = 0; i < lHeight; i++)		// 每行
	{		
		for(j = 0; j < lWidth; j++)		// 每列
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc  = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;			
			// 指向DIB第i+1行，第j个象素的指针
			lpSrc1 = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 2 - i) + j;			
			// 指向DIB第i行，第j+1个象素的指针
			lpSrc2 = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) 
				+ j + 1;
			
			bTemp = abs((*lpSrc)-(*lpSrc1)) + abs((*lpSrc)-(*lpSrc2));
			
			// 判断是否小于阈值
			if (bTemp < 255)
			{
				// 判断是否大于阈值，对于小于情况，灰度值不变。
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
 * 函数名称：
 *   ReplaceColorPal()
 * 参数:
 *   LPSTR lpDIB			- 指向源DIB图像指针
 *   BYTE * bpColorsTable	- 伪彩色编码表
 * 返回值:
 *   BOOL					- 成功返回TRUE，否则返回FALSE。
 * 说明:
 *   该函数用指定的伪彩色编码表来替换图像的调试板，参数bpColorsTable
 * 指向要替换的伪彩色编码表。
 ************************************************************************/
BOOL CDibImage::ReplaceColorPal(LPSTR lpDIB, BYTE * bpColorsTable)
{	
	int i;					// 循环变量	
	WORD wNumColors;		// 颜色表中的颜色数目	
	LPBITMAPINFO lpbmi;		// 指向BITMAPINFO结构的指针（Win3.0）		
	LPBITMAPCOREINFO lpbmc;	// 指向BITMAPCOREINFO结构的指针
	BOOL bWinStyleDIB;		// 表明是否是Win3.0 DIB的标记	
	BOOL bResult = FALSE;	// 创建结果	

	lpbmi = (LPBITMAPINFO)lpDIB;		// 获取指向BITMAPINFO结构的指针（Win3.0）	
	lpbmc = (LPBITMAPCOREINFO)lpDIB;	// 获取指向BITMAPCOREINFO结构的指针
	wNumColors = DIBNumColors(lpDIB);	// 获取DIB中颜色表中的颜色数目
		
	if (wNumColors == 256)				// 判断颜色数目是否是256色
	{		
		bWinStyleDIB = IS_WIN30_DIB(lpDIB);	// 判断是否是WIN3.0的DIB
		
		// 读取伪彩色编码，更新DIB调色板
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				// 更新DIB调色板红色绿色蓝色分量
				lpbmi->bmiColors[i].rgbRed = bpColorsTable[i * 4];
				lpbmi->bmiColors[i].rgbGreen = bpColorsTable[i * 4 + 1];
				lpbmi->bmiColors[i].rgbBlue = bpColorsTable[i * 4 + 2];
				
				// 更新DIB调色板保留位
				lpbmi->bmiColors[i].rgbReserved = 0;
			}
			else
			{
				// 更新DIB调色板红色绿色蓝色分量
				lpbmc->bmciColors[i].rgbtRed = bpColorsTable[i * 4];
				lpbmc->bmciColors[i].rgbtGreen = bpColorsTable[i * 4 + 1];
				lpbmc->bmciColors[i].rgbtBlue = bpColorsTable[i * 4 + 2];
			}
		}
	}
	
	return bResult;	
}

//////////////////////////////////////////////////////////////////////
//  图像形态学变换函数
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * 函数名称：
 *   ErosiontionDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   int   nMode		- 腐蚀方式：0表示水平方向，1垂直方向，2自定义结构元素。
 *	 int   structure[3][3]	- 自定义的3×3结构元素。
 * 返回值:
 *   BOOL               - 腐蚀成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行腐蚀运算。结构元素为水平方向或垂直方向的三个点，
 * 中间点位于原点；或者由用户自己定义3×3的结构元素。
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::ErosionDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, 
						   int nMode , int structure[3][3])
{	
	LPSTR	lpSrc;			// 指向源图像的指针	
	LPSTR	lpDst;			// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;	
	long i,j,n,m;			//循环变量
	unsigned char pixel;	//像素值

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//使用水平方向的结构元素进行腐蚀
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}
				
				//目标图像中的当前点先赋成黑色
				*lpDst = (unsigned char)0;

				//如果源图像中当前点自身或者左右有一个点不是黑色，
				//则将目标图像中的当前点赋成白色
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
		//使用垂直方向的结构元素进行腐蚀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最上边和最下边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成黑色
				*lpDst = (unsigned char)0;

				//如果源图像中当前点自身或者上下有一个点不是黑色，
				//则将目标图像中的当前点赋成白色
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
		//使用自定义的结构元素进行腐蚀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素和最上边和最下边的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成黑色
				*lpDst = (unsigned char)0;

				//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
				//则将目标图像中的当前点赋成白色
				//注意在DIB图像中内容是上下倒置的
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
	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   DilationDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   int   nMode		- 膨胀方式：0表示水平方向，1垂直方向，2自定义结构元素。
 *	 int   structure[3][3]	- 自定义的3×3结构元素。
 * 返回值:
 *   BOOL               - 膨胀成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行膨胀运算。结构元素为水平方向或垂直方向的三个点，
 * 中间点位于原点；或者由用户自己定义3×3的结构元素。
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::DilationDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, int nMode, 
							int structure[3][3])
{	
	LPSTR	lpSrc;			// 指向源图像的指针	
	LPSTR	lpDst;			// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;	
	long i,j,m,n;			//循环变量	
	unsigned char pixel;	//像素值

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//使用水平方向的结构元素进行膨胀
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && pixel != 0)
				{
					return FALSE;
				}
				
				//目标图像中的当前点先赋成白色
				*lpDst = (unsigned char)255;

				//源图像中当前点自身或者左右只要有一个点是黑色，
				//则将目标图像中的当前点赋成黑色
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
		//使用垂直方向的结构元素进行膨胀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最上边和最下边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成白色
				*lpDst = (unsigned char)255;

				//源图像中当前点自身或者上下只要有一个点是黑色，
				//则将目标图像中的当前点赋成黑色
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
		//使用自定义的结构元素进行膨胀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素和最上边和最下边的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成白色
				*lpDst = (unsigned char)255;

				//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
				//则将目标图像中的当前点赋成黑色
				//注意在DIB图像中内容是上下倒置的
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
	// 复制膨胀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   OpenDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   int   nMode		- 开运算方式：0表示水平方向，1垂直方向，2自定义结构元素。
 *	 int   structure[3][3]	- 自定义的3×3结构元素。
 * 返回值:
 *   BOOL               - 开运算成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行开运算。结构元素为水平方向或垂直方向的三个点，
 * 中间点位于原点；或者由用户自己定义3×3的结构元素。
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::OpenDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, int nMode , 
						int structure[3][3])
{		
	LPSTR	lpSrc;			// 指向源图像的指针	
	LPSTR	lpDst;			// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	long i,j,m,n;			//循环变量	
	unsigned char pixel;	//像素值

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//使用水平方向的结构元素进行腐蚀
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}
				
				//目标图像中的当前点先赋成黑色
				*lpDst = (unsigned char)0;

				//如果源图像中当前点自身或者左右有一个点不是黑色，
				//则将目标图像中的当前点赋成白色
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
		//使用垂直方向的结构元素进行腐蚀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最上边和最下边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成黑色
				*lpDst = (unsigned char)0;

				//如果源图像中当前点自身或者上下有一个点不是黑色，
				//则将目标图像中的当前点赋成白色
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
		//使用自定义的结构元素进行腐蚀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素和最上边和最下边的两列像素
				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成黑色
				*lpDst = (unsigned char)0;

				//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
				//则将目标图像中的当前点赋成白色
				//注意在DIB图像中内容是上下倒置的
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
	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	// 重新初始化新分配的内存，设定初始值为255
	//lpDst = (char *)lpNewDIBBits;
	//memset(lpDst, (BYTE)255, lWidth * lHeight);

	if(nMode == 0)
	{
		//使用水平方向的结构元素进行膨胀
		for(j = 0; j <lHeight; j++)
		{
			for(i = 1;i <lWidth-1; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}
				
				//目标图像中的当前点先赋成白色
				*lpDst = (unsigned char)255;

				//源图像中当前点自身或者左右只要有一个点是黑色，
				//则将目标图像中的当前点赋成黑色
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
		//使用垂直方向的结构元素进行膨胀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用1×3的结构元素，为防止越界，所以不处理最上边和最下边
				// 的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成白色
				*lpDst = (unsigned char)255;

				//源图像中当前点自身或者上下只要有一个点是黑色，
				//则将目标图像中的当前点赋成黑色
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
		//使用自定义的结构元素进行膨胀
		for(j = 1; j <lHeight-1; j++)
		{
			for(i = 0;i <lWidth; i++)
			{
				// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边
				// 的两列像素和最上边和最下边的两列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					return FALSE;
				}

				//目标图像中的当前点先赋成白色
				*lpDst = (unsigned char)255;

				//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
				//则将目标图像中的当前点赋成黑色
				//注意在DIB图像中内容是上下倒置的
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
	// 复制膨胀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   CloseDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 *   int   nMode		- 闭运算方式：0表示水平方向，1垂直方向，2自定义结构元素。
 *	 int   structure[3][3]	- 自定义的3×3结构元素。
 * 返回值:
 *   BOOL               - 闭运算成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行开运算。结构元素为水平方向或垂直方向的三个点，
 * 中间点位于原点；或者由用户自己定义3×3的结构元素。
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
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
 * 函数名称：
 *   ThinDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 闭运算成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行细化运算。要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::ThiningDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;			// 指向源图像的指针	
	LPSTR	lpDst;			// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;	
	BOOL bModified;			//脏标记	
	long i,j,m,n;			//循环变量

	//四个条件
	BOOL bCondition1;
	BOOL bCondition2;
	BOOL bCondition3;
	BOOL bCondition4;
	
	unsigned char nCount;	//计数器	
	unsigned char pixel;	//像素值	
	unsigned char neighbour[5][5];	//5×5相邻区域像素值

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	bModified = TRUE;
	while(bModified)
	{
		bModified = FALSE;
		// 初始化新分配的内存，设定初始值为255
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

				//由于使用5×5的结构元素，为防止越界，所以不处理外围的几行和几列像素

				// 指向源图像倒数第j行，第i个象素的指针			
				lpSrc = (char *)lpDIBBits + lWidth * j + i;
				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lWidth * j + i;
				//取得当前指针处的像素值，注意要转换为unsigned char型
				pixel = (unsigned char)*lpSrc;

				//目标图像中含有0和255外的其它灰度值
				if(pixel != 255 && *lpSrc != 0)
				{
					continue;
				}
				//如果源图像中当前点为白色，则跳过
				else if(pixel == 255)
				{
					continue;
				}

				//获得当前点相邻的5×5区域内像素值，白色用0代表，黑色用1代表
				for (m = 0;m < 5;m++ )
				{
					for (n = 0;n < 5;n++)
					{
						neighbour[m][n] =(255 - (unsigned char)*(lpSrc + 
							((4 - m) - 2)*lWidth + n - 2 )) / 255;
					}
				}

				//逐个判断条件。
				//判断2<=NZ(P1)<=6
				nCount =  neighbour[1][1] + neighbour[1][2] + neighbour[1][3] \
						+ neighbour[2][1] + neighbour[2][3] + \
						+ neighbour[3][1] + neighbour[3][2] + neighbour[3][3];
				if ( nCount >= 2 && nCount <=6)
				{
					bCondition1 = TRUE;
				}

				//判断Z0(P1)=1
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

				//判断P2*P4*P8=0 or Z0(p2)!=1
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

				//判断P2*P4*P6=0 or Z0(p4)!=1
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
		// 复制腐蚀后的图像
		memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);
	}
	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// 图像边缘与轮廓运算函数
//////////////////////////////////////////////////////////////////////

/*************************************************************************
 * 函数名称：
 *   RobertDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 边缘检测成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用Robert边缘检测算子对图像进行边缘检测运算。
 * 要求目标图像为灰度图像。
 ************************************************************************/
BOOL CDibImage::RobertDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	LPSTR	lpSrc;			// 指向源图像的指针	
	LPSTR	lpDst;			// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;	
	long i,j;				//循环变量	
	double result;			//像素值
	unsigned char pixel[4];

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	//使用水平方向的结构元素进行腐蚀
	for(j = lHeight-1; j > 0; j--)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			//由于使用2×2的模板，为防止越界，所以不处理最下边和最右边的两列像素

			// 指向源图像第j行，第i个象素的指针			
			lpSrc = (char *)lpDIBBits + lWidth * j + i;
			// 指向目标图像第j行，第i个象素的指针			
			lpDst = (char *)lpNewDIBBits + lWidth * j + i;

			//取得当前指针处2*2区域的像素值，注意要转换为unsigned char型
			pixel[0] = (unsigned char)*lpSrc;
			pixel[1] = (unsigned char)*(lpSrc + 1);
			pixel[2] = (unsigned char)*(lpSrc - lWidth);
			pixel[3] = (unsigned char)*(lpSrc - lWidth + 1);

			//计算目标图像中的当前点
			 result = sqrt((float)(( pixel[0] - pixel[3] )*( pixel[0] - pixel[3] ) + \
				  ( pixel[1] - pixel[2] )*( pixel[1] - pixel[2] )));
			*lpDst = (unsigned char)result;		
		}
	}

	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   SobelDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 边缘检测成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用Sobel边缘检测算子对图像进行边缘检测运算。
 * 要求目标图像为灰度图像。
 ************************************************************************/
BOOL CDibImage::SobelDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{	
	LPSTR	lpDst1;				// 指向缓存图像的指针
	LPSTR	lpDst2;		
	LPSTR	lpNewDIBBits1;		// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;
	
	long i,j;					// 循环变量	
	int		iTempH;				// 模板高度	
	int		iTempW;				// 模板宽度	
	FLOAT	fTempC;				// 模板系数	
	int		iTempMX;			// 模板中心元素X坐标	
	int		iTempMY;			// 模板中心元素Y坐标	
	FLOAT aTemplate[9];			// 模板数组

	// 暂时分配内存，以保存新图像
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// 暂时分配内存，以保存新图像
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// 拷贝源图像到缓存图像中
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// 设置Sobel模板参数
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

	// 设置Sobel模板参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 复制经过模板运算后的图像到源图像
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   PrewittDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 边缘检测成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用Prewitt边缘检测算子对图像进行边缘检测运算。
 * 要求目标图像为灰度图像。
 ************************************************************************/
BOOL CDibImage::PrewittDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpDst1;				// 指向缓存图像的指针
	LPSTR	lpDst2;	
	LPSTR	lpNewDIBBits1;		// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;
	
	long i,j;					// 循环变量	
	int		iTempH;				// 模板高度	
	int		iTempW;				// 模板宽度	
	FLOAT	fTempC;				// 模板系数	
	int		iTempMX;			// 模板中心元素X坐标
	int		iTempMY;			// 模板中心元素Y坐标
	FLOAT aTemplate[9];			// 模板数组

	// 暂时分配内存，以保存新图像
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// 暂时分配内存，以保存新图像
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// 拷贝源图像到缓存图像中
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);


	// 设置Prewitt模板参数
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

	// 设置Prewitt模板参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 复制经过模板运算后的图像到源图像
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   KirschDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 边缘检测成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用kirsch边缘检测算子对图像进行边缘检测运算。
 * 要求目标图像为灰度图像。
 ************************************************************************/
BOOL CDibImage::KirschDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpDst1;			// 指向缓存图像的指针
	LPSTR	lpDst2;
	LPSTR	lpNewDIBBits1;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;

	long i,j;				// 循环变量	
	int		iTempH;			// 模板高度	
	int		iTempW;			// 模板宽度	
	FLOAT	fTempC;			// 模板系数	
	int		iTempMX;		// 模板中心元素X坐标	
	int		iTempMY;		// 模板中心元素Y坐标	
	FLOAT aTemplate[9];		// 模板数组

	// 暂时分配内存，以保存新图像
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// 暂时分配内存，以保存新图像
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// 拷贝源图像到缓存图像中
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// 设置Kirsch模板1参数
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

	// 设置Kirsch模板2参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 拷贝源图像到缓存图像中
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// 设置Kirsch模板3参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 拷贝源图像到缓存图像中
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// 设置Kirsch模板4参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 拷贝源图像到缓存图像中
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// 设置Kirsch模板5参数
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

	// 拷贝源图像到缓存图像中
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 拷贝源图像到缓存图像中
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// 设置Kirsch模板6参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 拷贝源图像到缓存图像中
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// 设置Kirsch模板7参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 拷贝源图像到缓存图像中
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);
	
	// 设置Kirsch模板8参数
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

	//求两幅缓存图像的最大值
	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth-1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (char *)lpNewDIBBits1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (char *)lpNewDIBBits2 + lWidth * j + i;
			
			if(*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 复制经过模板运算后的图像到源图像
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *  GaussDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 边缘检测成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用高斯拉普拉斯边缘检测算子对图像进行边缘检测运算。 
 * 要求目标图像为灰度图像。
 ************************************************************************/
BOOL CDibImage::GaussDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpDst1;				// 指向缓存图像的指针
	LPSTR	lpDst2;	
	LPSTR	lpNewDIBBits1;		// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits1;
	LPSTR	lpNewDIBBits2;
	HLOCAL	hNewDIBBits2;
	
	int		iTempH;				// 模板高度	
	int		iTempW;				// 模板宽度	
	FLOAT	fTempC;				// 模板系数	
	int		iTempMX;			// 模板中心元素X坐标	
	int		iTempMY;			// 模板中心元素Y坐标
	FLOAT aTemplate[25];		//模板数组

	// 暂时分配内存，以保存新图像
	hNewDIBBits1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits1 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits1 = (char * )LocalLock(hNewDIBBits1);

	// 暂时分配内存，以保存新图像
	hNewDIBBits2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits2 == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits2 = (char * )LocalLock(hNewDIBBits2);

	// 拷贝源图像到缓存图像中
	lpDst1 = (char *)lpNewDIBBits1;
	memcpy(lpNewDIBBits1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (char *)lpNewDIBBits2;
	memcpy(lpNewDIBBits2, lpDIBBits, lWidth * lHeight);

	// 设置Gauss模板参数
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

	// 复制经过模板运算后的图像到源图像
	memcpy(lpDIBBits, lpNewDIBBits1, lWidth * lHeight);

	LocalUnlock(hNewDIBBits1);
	LocalFree(hNewDIBBits1);
	LocalUnlock(hNewDIBBits2);
	LocalFree(hNewDIBBits2);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   HoughDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 运算成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对检测图像中的平行直线。如果图像中有两条平行的直线，
 * 则将这两条平行直线提取出来。 
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::HoughDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{			
	LPSTR	lpSrc;				// 指向源图像的指针	
	LPSTR	lpDst;				// 指向缓存图像的指针	
	LPSTR   lpTrans;			// 指向变换域的指针	
	LONG	lLineBytes;			// 图像每行的字节数		
	LPSTR	lpNewDIBBits;		// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;	
	LPSTR	lpTransArea;		// 指向变换域的指针
	HLOCAL	hTransArea;
	
	int		iMaxDist;			// 变换域的尺寸
	int		iMaxAngleNumber;	
	int		iDist;				// 变换域的坐标
	int		iAngleNumber;	
	long	i,j;				// 循环变量	
	unsigned char pixel;		// 像素值
	MaxValue MaxValue1;			// 存储变换域中的两个最大值
	MaxValue MaxValue2;

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);

	//计算变换域的尺寸	最大距离
	iMaxDist = (int) sqrt((double)(lWidth*lWidth + lHeight*lHeight));	
	//角度从0－180，每格2度
	iMaxAngleNumber = 90;						

	//为变换域分配内存
	hTransArea = LocalAlloc(LHND, lWidth * lHeight * sizeof(int));
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpTransArea = (char * )LocalLock(hTransArea);
		
	// 初始化新分配的内存，设定初始值为0
	lpTrans = (char *)lpTransArea;
	memset(lpTrans, 0, lWidth * lHeight * sizeof(int));

	// 计算图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);

	for(j = 0; j <lHeight; j++)
	{
		for(i = 0;i <lWidth; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (char *)lpDIBBits + lLineBytes * j + i;

			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;
			//目标图像中含有0和255外的其它灰度值
			if(pixel != 255 && *lpSrc != 0)
			{
				return FALSE;
			}

			//如果是黑点，则在变换域的对应各点上加1
			if(pixel == 0)
			{
				//注意步长是2度
				for(iAngleNumber=0; iAngleNumber<iMaxAngleNumber; iAngleNumber++)
				{
					iDist = (int) fabs(i*cos(iAngleNumber*2*PI/180.0) + \
						j*sin(iAngleNumber*2*PI/180.0));
				
					//变换域的对应点上加1
					*(lpTransArea+iDist*iMaxAngleNumber+iAngleNumber) = \
						*(lpTransArea+iDist*iMaxAngleNumber+iAngleNumber) +1;
				}
			}	
		}
	}
				
	//找到变换域中的两个最大值点
	MaxValue1.Value=0;
	MaxValue2.Value=0;
	
	//找到第一个最大值点
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

	//将第一个最大值点附近清零
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

	//找到第二个最大值点
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

	//判断两直线是否平行
	if(abs(MaxValue1.AngleNumber-MaxValue2.AngleNumber)<=2)
	{
		//两直线平行，在缓存图像中重绘这两条直线
		for(j = 0; j <lHeight; j++)
		{
			for(i = 0;i <lWidth; i++)
			{	
				// 指向缓存图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lLineBytes * j + i;	

				//如果该点在某一条平行直线上，则在缓存图像上将该点赋为黑

				//在第一条直线上
				iDist = (int) fabs(i*cos(MaxValue1.AngleNumber*2*PI/180.0) + \
							j*sin(MaxValue1.AngleNumber*2*PI/180.0));
				if (iDist == MaxValue1.Dist)
				{
					*lpDst = (unsigned char)0;
				}
				//在第二条直线上
				iDist = (int) fabs(i*cos(MaxValue2.AngleNumber*2*PI/180.0) + \
							j*sin(MaxValue2.AngleNumber*2*PI/180.0));
				if (iDist == MaxValue2.Dist)
				{
					*lpDst = (unsigned char)0;
				}
			}
		}
	}

	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	LocalUnlock(hTransArea);
	LocalFree(hTransArea);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   Fill2DIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 种子填充成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行种子填充运算。 
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::Fill2DIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;			// 指向源图像的指针	
	long i;					// 循环变量	
	unsigned char pixel;	// 像素值	
	int xl,xr;				// 左右边界像素位置
	BOOL bFilll,bFillr;		// 是否已填充至边界	
	Seed Seeds[10];			// 种子堆栈及指针
	int StackPoint;	
	int iCurrentPixelx,iCurrentPixely;	// 当前像素位置
	int iBufferPixelx,iBufferPixely;

	Seeds[1].Height = lHeight / 2;		// 初始化种子
	Seeds[1].Width = lWidth / 2;
	StackPoint = 1;

	while( StackPoint != 0)
	{
		//取出种子
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
		//填充种子所在的行，保存种子像素的位置
		iBufferPixelx = iCurrentPixelx;
		iBufferPixely = iCurrentPixely;
		//先向左填充
		while(bFilll)
		{
			lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + iCurrentPixelx;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;

			//目标图像中含有0和255外的其它灰度值
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
			//遇到边界
			if(pixel == 0)
			{
				bFilll = false;
				xl=iCurrentPixelx+1;
			}
			else
			{
				*lpSrc = (unsigned char)0;
				iCurrentPixelx--;
				//防止越界
				if(iCurrentPixelx<0)
				{	
					bFilll = false;
					iCurrentPixelx = 0;
					xl = 0; 
				}
			}		
		}
		//再向右填充，取回种子像素的位置
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
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;

			//目标图像中含有0和255外的其它灰度值
			if(pixel != 255 && pixel != 0)
				return FALSE;
			//遇到边界
			if(pixel == 0)
			{
				bFillr = false;
				xr=iCurrentPixelx-1;
			}
			else
			{
				*lpSrc = (unsigned char)0;
				iCurrentPixelx++;
				//防止越界
				if(iCurrentPixelx>lWidth)
				{	
					bFillr = false;
					iCurrentPixelx = lWidth;
					xr = lWidth; 
				}
			}		
		}
		//上、下两条扫描线是否全为边界象素或已填充过
		//先看上面的扫描线
		iCurrentPixely--;
		if(iCurrentPixely < 0)
		{
			iCurrentPixely = 0;
		}
		for (i = xr; i>= xl;i--)
		{
			lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + i;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;

			//有未填充的像素，将新的种子压入堆栈
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely;
				Seeds[StackPoint].Width = i;
				break;
			}				
		}
		//再看下面的扫描线
		iCurrentPixely+=2;
		if(iCurrentPixely > lHeight)
		{
			iCurrentPixely = lHeight;
		}
		for (i = xr; i>= xl;i--)
		{
			lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + i;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;

			//有未填充的像素，将新的种子压入堆栈
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
 * 函数名称：
 *   FillDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 种子填充成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行种子填充运算。
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::FillDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;				// 指向源图像的指针	
	unsigned char pixel;		// 像素值	
	Seed *Seeds;				// 种子堆栈及指针
	int StackPoint;
	int iCurrentPixelx,iCurrentPixely;	// 当前像素位置
 
	Seeds = new Seed[lWidth*lHeight];	// 初始化种子
	Seeds[1].Height = lHeight / 2;
	Seeds[1].Width = lWidth / 2;
	StackPoint = 1;

	while( StackPoint != 0)
	{
		//取出种子
		iCurrentPixelx = Seeds[StackPoint].Width;
		iCurrentPixely = Seeds[StackPoint].Height;
		StackPoint--;

		lpSrc = (char *)lpDIBBits + lWidth * iCurrentPixely + iCurrentPixelx;
		//取得当前指针处的像素值，注意要转换为unsigned char型
		pixel = (unsigned char)*lpSrc;

		//目标图像中含有0和255外的其它灰度值
		if(pixel != 255 && pixel != 0)
		{
			return FALSE;
		}

		//将当前点涂黑
		*lpSrc = (unsigned char)0;

		//判断左面的点，如果为白，则压入堆栈，注意防止越界
		if(iCurrentPixelx > 0)
		{
			lpSrc = (char *)lpDIBBits+lWidth*iCurrentPixely+iCurrentPixelx-1;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely;
				Seeds[StackPoint].Width = iCurrentPixelx - 1;
			}
			//目标图像中含有0和255外的其它灰度值
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
		}

		//判断上面的点，如果为白，则压入堆栈，注意防止越界
		if(iCurrentPixely < lHeight - 1)
		{
			lpSrc = (char *)lpDIBBits+lWidth*(iCurrentPixely+1)+iCurrentPixelx;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely + 1;
				Seeds[StackPoint].Width = iCurrentPixelx;
			}
			//目标图像中含有0和255外的其它灰度值
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
		}

		//判断右面的点，如果为白，则压入堆栈，注意防止越界
		if(iCurrentPixelx < lWidth - 1)
		{
			lpSrc = (char *)lpDIBBits+lWidth*iCurrentPixely+iCurrentPixelx+1;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely;
				Seeds[StackPoint].Width = iCurrentPixelx + 1;
			}
			//目标图像中含有0和255外的其它灰度值
			if(pixel != 255 && pixel != 0)
			{
				return FALSE;
			}
		}

		//判断下面的点，如果为白，则压入堆栈，注意防止越界
		if(iCurrentPixely > 0)
		{
			lpSrc = (char *)lpDIBBits+lWidth*(iCurrentPixely-1)+iCurrentPixelx;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;
			if (pixel == 255)
			{
				StackPoint++;
				Seeds[StackPoint].Height = iCurrentPixely - 1;
				Seeds[StackPoint].Width = iCurrentPixelx;
			}
			//目标图像中含有0和255外的其它灰度值
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
 * 函数名称：
 *   ContourDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 运算成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行轮廓提取运算。
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::ContourDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;				// 指向源图像的指针	
	LPSTR	lpDst;				// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;		// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	long i,j;					//循环变量
	unsigned char n,e,s,w,ne,se,nw,sw;
	unsigned char pixel;		//像素值

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lWidth * lHeight);
	for(j = 1; j <lHeight-1; j++)
	{
		for(i = 1;i <lWidth-1; i++)
		{			
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (char *)lpDIBBits + lWidth * j + i;			
			// 指向目标图像倒数第j行，第i个象素的指针			
			lpDst = (char *)lpNewDIBBits + lWidth * j + i;			
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;

			//目标图像中含有0和255外的其它灰度值
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
				//如果相邻的八个点都是黑点
				if(nw+n+ne+w+e+sw+s+se==0)
				{
					*lpDst = (unsigned char)255;
				}
			}
		}
	}

	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

/*************************************************************************
 * 函数名称：
 *   TraceDIB()
 * 参数:
 *   LPSTR lpDIBBits    - 指向源DIB图像指针
 *   LONG  lWidth       - 源图像宽度（象素数，必须是4的倍数）
 *   LONG  lHeight      - 源图像高度（象素数）
 * 返回值:
 *   BOOL               - 运算成功返回TRUE，否则返回FALSE。
 * 说明:
 * 该函数用于对图像进行轮廓跟踪运算。
 * 要求目标图像为只有0和255两个灰度值的灰度图像。
 ************************************************************************/
BOOL CDibImage::TraceDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{		
	LPSTR	lpSrc;				// 指向源图像的指针	
	LPSTR	lpDst;				// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;		// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	LONG	lLineBytes;			// 图像每行的字节数
	long i,j;					// 循环变量
	unsigned char pixel;		// 像素值
	bool bFindStartPoint;		// 是否找到起始点及回到起始点
	bool bFindPoint;			// 是否扫描到一个边界点	
	Point StartPoint,CurrentPoint;	// 起始边界点与当前边界点

	//八个方向和起始扫描方向
	int Direction[8][2]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
	int BeginDirect;

	// 计算图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	lpDst = (char *)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	//先找到最左上方的边界点
	bFindStartPoint = false;
	for (j = 0;j < lHeight && !bFindStartPoint;j++)
	{
		for(i = 0;i < lWidth && !bFindStartPoint;i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc = (char *)lpDIBBits + lLineBytes * j + i;			
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel = (unsigned char)*lpSrc;
			
			if(pixel == 0)
			{
				bFindStartPoint = true;
				StartPoint.Height = j;
				StartPoint.Width = i;

				// 指向目标图像倒数第j行，第i个象素的指针			
				lpDst = (char *)lpNewDIBBits + lLineBytes * j + i;	
				*lpDst = (unsigned char)0;
			}		
		}
	}

	//由于起始点是在左下方，故起始扫描沿左上方向
	BeginDirect = 0;
	//跟踪边界
	bFindStartPoint = false;
	//从初始点开始扫描
	CurrentPoint.Height = StartPoint.Height;
	CurrentPoint.Width = StartPoint.Width;
	while(!bFindStartPoint)
	{
		bFindPoint = false;
		while(!bFindPoint)
		{
			//沿扫描方向查看一个像素
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
				//扫描的方向逆时针旋转两格
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
				//扫描方向顺时针旋转一格
				BeginDirect++;
				if(BeginDirect == 8)
				{
					BeginDirect = 0;
				}
			}
		}
	}

	// 复制腐蚀后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

