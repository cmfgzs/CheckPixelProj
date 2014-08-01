#ifndef IDENTIFYOUTLINELIB_API_HEAD_H
#define IDENTIFYOUTLINELIB_API_HEAD_H

#ifdef IDENTIFYOUTLINELIB_EXPORTS
#define IDENTIFYOUTLINELIB_API __declspec(dllexport)
#else
#define IDENTIFYOUTLINELIB_API __declspec(dllimport)
#endif


IDENTIFYOUTLINELIB_API bool CheckPiexl(char* pFile);


#endif