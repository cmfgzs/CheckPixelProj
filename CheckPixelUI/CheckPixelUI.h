
// CheckPixelUI.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCheckPixelUIApp:
// �йش����ʵ�֣������ CheckPixelUI.cpp
//

class CCheckPixelUIApp : public CWinAppEx
{
public:
	CCheckPixelUIApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCheckPixelUIApp theApp;