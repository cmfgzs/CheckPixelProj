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
	// ��ʼ��DIB����

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

	// �ж�ͼ���Ƿ����
	if (m_pDibImage->DIBWidth(lpDIB) > INT_MAX ||m_pDibImage->DIBHeight(lpDIB) 
		> INT_MAX)
	{
		::GlobalUnlock((HGLOBAL) m_hDIB);
		::GlobalFree((HGLOBAL) m_hDIB);

		m_hDIB = NULL;

		CString strMsg;
		strMsg = "BMPͼ��̫��";
		MessageBox(NULL, strMsg, "ϵͳ��ʾ", MB_ICONINFORMATION | MB_OK);

		return;
	}

	m_sizeDoc = CSize((int) m_pDibImage->DIBWidth(lpDIB), 
		(int) m_pDibImage->DIBHeight(lpDIB));

	::GlobalUnlock((HGLOBAL) m_hDIB);

	// �����µ�ɫ��
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


// ��ȡͼƬ�ļ�
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

	// �ж϶�ȡ�ļ��Ƿ�ɹ�
	if (m_hDIB == NULL)
	{
		CString strMsg;
		strMsg = "��ȡͼ��ʱ���������ǲ�֧�ָ����͵�ͼ���ļ���";
		MessageBox(NULL, strMsg, "ϵͳ��ʾ", MB_ICONINFORMATION | MB_OK);

		if(m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return FALSE;
	}

	return TRUE;
}


//������������-wangtan 2014-07-21
void CIdentifyOutlineLibExt::OnEdgeTrace() 
{
	LPSTR	lpDIB;				// ָ��DIB��ָ��	
	LPSTR   lpDIBBits;			// ָ��DIB����ָ��

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);

	// �ж��Ƿ���8-bppλͼ������Ϊ�˷��㣬ֻ����8-bppλͼ���������٣������Ŀ������ƣ�
	if (m_pDibImage->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(NULL, "Ŀǰֻ֧��256ɫλͼ�����㣡", "ϵͳ��ʾ" , 
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL) m_hDIB);

		return;
	}

	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = m_pDibImage->FindDIBBits(lpDIB);

	// ����TraceDIB()������DIB������������
	if (m_pDibImage->TraceDIB(lpDIBBits,m_pDibImage->DIBWidth(lpDIB), 
		m_pDibImage->DIBHeight(lpDIB)))
	{
	}
	else
	{
		MessageBox(NULL, "�����ڴ�ʧ�ܣ�", "ϵͳ��ʾ" , MB_ICONINFORMATION | MB_OK);
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