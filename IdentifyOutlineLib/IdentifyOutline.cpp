#include "IdentifyOutline.h"


DetectObjectProperty* MonitorScreen_Handle(char* cImg)
{
	return new DetectObjectProperty;
}


DetectObjectProperty* (*PM[])(char*) = 
{
	NULL,

	MonitorScreen_Handle,

	NULL,
};


IDENTIFYOUTLINELIB_DLLSPEC DetectObjectProperty* ImgAnalyze(DetectType type, char* cImg)
{
	return PM[type](cImg);
}