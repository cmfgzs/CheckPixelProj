#include "LibExport.h"

#include <fstream>
#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>
#include "EdgeDetection.h"


IDENTIFYOUTLINELIB_API bool CheckPiexl(char* pFile)
{
	// 使用image时需要注册 wangtan 2014-07-26
	fl_register_images();

	int step = 4;
	Fl_Shared_Image *queryImage = Fl_Shared_Image::get(pFile);

	if (queryImage == NULL) 
	{
		printf("couldn't load query image\n");
		return false;
	}

	CFloatImage srcimg, dstimg;
	if (!convertImage(queryImage, srcimg)) 
	{
		printf("couldn't convert image to RGB format\n");
		return false;
	}
	performEdgeDetection(srcimg,dstimg,step);

	CByteImage udstimg;
	convertToByteImage(dstimg, udstimg);
	writeByteImageToBMP(udstimg,"temp.bmp");

	return true;
}