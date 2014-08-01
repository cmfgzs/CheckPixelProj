#ifndef EDGEDETECTION_HEAD_H
#define EDGEDETECTION_HEAD_H

#include <ImageLib/ImageLib.h>

class Fl_Image;

#define Loop_Image(x,y,width,height) for(int y = 0; y < height; y++) for(int x = 0; x < width; x++) 

void performEdgeDetection(CFloatImage &image, CFloatImage &result, int step);
bool convertImage(const Fl_Image *image, CFloatImage &convertedImage);
void convertToByteImage(CFloatImage &floatImage, CByteImage &byteImage);
int	writeByteImageToBMP(CByteImage &byteImage, char* fileName);

#endif