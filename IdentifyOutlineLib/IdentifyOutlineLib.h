#ifndef _CHECKPIXELPROJ_IDENTIFYOUTLINELIB_IDENTIFYOUTLINE_H
#define _CHECKPIXELPROJ_IDENTIFYOUTLINELIB_IDENTIFYOUTLINE_H

#ifdef IDENTIFYOUTLINELIB_EXPORTS
#define IDENTIFYOUTLINELIB_API __declspec(dllexport)
#else
#define IDENTIFYOUTLINELIB_API __declspec(dllimport)
#endif

class CIdentifyOutlineLibExt;
class IDENTIFYOUTLINELIB_API CIdentifyOutlineLib 
{
public:
	CIdentifyOutlineLib(void);

	// TODO: 在此添加您的方法。
	BOOL ImgAnalyze(LPCTSTR lpszPathName);

private:
	CIdentifyOutlineLibExt* m_pObjExt;

};

#endif
