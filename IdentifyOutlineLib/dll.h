// Copyright (c) 2012 David Muse
// See the COPYING file for more information.
// _SOL_PROJ_FILE_H_

#ifndef _CHECKPIXELPROJ_IDENTIFYOUTLINELIB_DLL_H
#define _CHECKPIXELPROJ_IDENTIFYOUTLINELIB_DLL_H

#ifdef _WIN32
	#ifdef IDENTIFYOUTLINELIB_EXPORTS
		#define IDENTIFYOUTLINELIB_DLLSPEC __declspec(dllexport)
	#else
		#define IDENTIFYOUTLINELIB_DLLSPEC __declspec(dllimport)
	#endif
#else
	#define IDENTIFYOUTLINELIB_DLLSPEC
#endif

#endif
