#include "stdafx.h"
#include "IdentifyOutlineLib.h"
#include "DibImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
class CIdentifyOutlineLibExt
{
	friend class CIdentifyOutlineLib;
private:
	CDibImage* m_pDibImage;
	HDIB m_hDIB;
	CPalette* m_palDIB;
	CSize m_sizeDoc;

public:
	CIdentifyOutlineLibExt();
	virtual ~CIdentifyOutlineLibExt();

	void InitDIBData();
	BOOL ReadFileData(LPCTSTR lpszPathName);
	void OnEdgeTrace();
};


CIdentifyOutlineLib::CIdentifyOutlineLib(void)
{
	m_pObjExt = new CIdentifyOutlineLibExt;
}

CIdentifyOutlineLibExt::CIdentifyOutlineLibExt()
{
	m_pDibImage = NULL;
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1,1);
}


CIdentifyOutlineLibExt::~CIdentifyOutlineLibExt()
{
	if (m_pDibImage != NULL)
	{
		delete m_pDibImage;
	}

	if (m_palDIB != NULL)
	{
		delete m_palDIB;
	}
}

void CIdentifyOutlineLibExt::InitDIBData()
{
	// 初始化DIB对象

	if (m_palDIB != NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;
	}	
	if (m_hDIB == NULL)
	{
		return;
	}

	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);

	// 判断图像是否过大
	if (m_pDibImage->DIBWidth(lpDIB) > INT_MAX ||m_pDibImage->DIBHeight(lpDIB) 
		> INT_MAX)
	{
		::GlobalUnlock((HGLOBAL) m_hDIB);
		::GlobalFree((HGLOBAL) m_hDIB);

		m_hDIB = NULL;

		CString strMsg;
		strMsg = "BMP图像太大！";
		MessageBox(NULL, strMsg, "系统提示", MB_ICONINFORMATION | MB_OK);

		return;
	}

	m_sizeDoc = CSize((int) m_pDibImage->DIBWidth(lpDIB), 
		(int) m_pDibImage->DIBHeight(lpDIB));

	::GlobalUnlock((HGLOBAL) m_hDIB);

	// 创建新调色板
	m_palDIB = new CPalette;
	if (m_palDIB == NULL)
	{
		::GlobalFree((HGLOBAL) m_hDIB);
		m_hDIB = NULL;

		return;
	}

	if (m_pDibImage->CreateDIBPalette(m_hDIB, m_palDIB) == NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;

		return;
	}
}


// 读取图片文件
BOOL CIdentifyOutlineLibExt::ReadFileData(LPCTSTR lpszPathName) 
{
	CFile file;
	CFileException fe;

	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		return FALSE;
	}

	if(m_pDibImage != NULL)
	{
		delete m_pDibImage;
		m_pDibImage = NULL;
	}
	m_pDibImage = new CDibImage;

	TRY
	{
		m_hDIB = m_pDibImage->ReadDIBFile(file);
	}
	CATCH (CFileException, eLoad)
	{
		file.Abort();

		m_hDIB = NULL;

		if(m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return FALSE;
	}
	END_CATCH

	InitDIBData();

	// 判断读取文件是否成功
	if (m_hDIB == NULL)
	{
		CString strMsg;
		strMsg = "读取图像时出错！可能是不支持该类型的图像文件！";
		MessageBox(NULL, strMsg, "系统提示", MB_ICONINFORMATION | MB_OK);

		if(m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return FALSE;
	}

	return TRUE;
}


//轮廓跟踪运算-wangtan 2014-07-21
void CIdentifyOutlineLibExt::OnEdgeTrace() 
{
	LPSTR	lpDIB;				// 指向DIB的指针	
	LPSTR   lpDIBBits;			// 指向DIB象素指针

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);

	// 判断是否是8-bpp位图（这里为了方便，只处理8-bpp位图的轮廓跟踪，其它的可以类推）
	if (m_pDibImage->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(NULL, "目前只支持256色位图的运算！", "系统提示" , 
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL) m_hDIB);

		return;
	}

	// 找到DIB图像象素起始位置
	lpDIBBits = m_pDibImage->FindDIBBits(lpDIB);

	// 调用TraceDIB()函数对DIB进行轮廓跟踪
	if (m_pDibImage->TraceDIB(lpDIBBits,m_pDibImage->DIBWidth(lpDIB), 
		m_pDibImage->DIBHeight(lpDIB)))
	{
	}
	else
	{
		MessageBox(NULL, "分配内存失败！", "系统提示" , MB_ICONINFORMATION | MB_OK);
	}

	::GlobalUnlock((HGLOBAL) m_hDIB);


	CFile file;
	CFileException fe;

	if (!file.Open("d:\\adf.bmp", CFile::modeCreate |CFile::modeReadWrite 
		| CFile::shareExclusive, &fe))
	{
		return;
	}

	BOOL bSuccess = FALSE;
	TRY
	{
		bSuccess = m_pDibImage->SaveDIB(m_hDIB, file);
		file.Close();
	}
	CATCH (CException, eSave)
	{
		file.Abort();
		return;
	}
	END_CATCH

}

BOOL CIdentifyOutlineLib::ImgAnalyze(LPCTSTR lpszPathName)
{
	BOOL rslt  = m_pObjExt->ReadFileData(lpszPathName);
	m_pObjExt->OnEdgeTrace();

	delete m_pObjExt;
	return rslt;
}