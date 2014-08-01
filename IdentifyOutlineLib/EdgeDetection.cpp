#include "edgedetection.h"
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <windows.h>
#include <iostream>
#include <set>
using namespace std;

#define MIN(x,y,z)	(min(x,y) < z ? min(x,y) : z)
#define isEqual(x,y)	(abs(x - y) < 0.0000000001f)
#define LARGE 1000000000
const float PI = 3.14159265f;
float high = 0.42f;
float low = 0.07f;
float maxV = -999999.f;
float minV = 999999.f;
float rang = 1.f;
float Gaussian[5][5] = {{2.f, 4.f , 5.f , 4.f , 2.f},
						{4.f, 9.f , 12.f, 9.f , 4.f},
						{5.f, 12.f, 15.f, 12.f, 5.f},
						{4.f, 9.f , 12.f, 9.f , 4.f},
						{2.f, 4.f , 5.f , 4.f , 2.f}
					    };
int ox = 2;
int oy = 2;
void dummyConvertToGrayScale(CFloatImage &src, CFloatImage &dst) 
{	
	CShape sh = src.Shape();
	if (sh.nBands != 3)
	{
		cout<<"Error of Image Channels: We now can only handle 1, 3 or 4 channel Images."<<endl;
		return;
	}
	float R, G, B;
	Loop_Image(x,y,sh.width,sh.height)	
	{
		R = src.Pixel(x,y,0);
		G = src.Pixel(x,y,1);
		B = src.Pixel(x,y,2);
		dst.Pixel(x,y,0) = 0.2989f * R + 0.5870f * G + 0.1140f * B; 
	}	
}


void dummyPreSmooth(CFloatImage &src, CFloatImage &dst) 
{
	CShape sh = src.Shape();
	Loop_Image(x,y,sh.width,sh.height)		
	{
		float sum = 0.f;
		float sumker = 0.f;
		for(int k = -2; k <= 2; k++)
		for(int l = -2; l <= 2; l++)
		{
			if((x + k < 0) || (x + k > sh.width - 1) || (y + l < 0) || (y + l > sh.height - 1))
				continue;
			sumker += Gaussian[l + oy][k + ox];
			sum += src.Pixel(x + k, y + l, 0) * Gaussian[l + oy][k + ox];
		}
		dst.Pixel(x,y,0) = sum/sumker;
	}
}

void ThreshHold(CFloatImage &src)
{
	CShape sh = src.Shape();
	float tmin = 999999.f, tmax = -999999.f;
	for(int y = 0; y < sh.height; y++)
		for(int x = 0; x < sh.width; x++)
		{
			if(tmin > src.Pixel(x,y,0))
				tmin = src.Pixel(x,y,0);
			if(tmax < src.Pixel(x,y,0))
				tmax = src.Pixel(x,y,0);
		}

		float RatioOfNonEdgePixel = 0.93f, RatioOfHighLow = 0.2f;
		int index;
		int hist[64] = {0};
		Loop_Image(x,y,sh.width,sh.height)
		{
			index=(int)floor((src.Pixel(x,y,0)-tmin)/(tmax-tmin)*64);
			if(index == 64)
				index--;
			hist[index]++;
		}
		float cumhist = 0.f;
		for (index = 0; index < 64; index++)
		{
			cumhist += hist[index];
			if(cumhist > sh.width * sh.height * RatioOfNonEdgePixel)
				break;
		}
		high = (tmax - tmin) / 64.f * index;
		low = high * RatioOfHighLow;
}

void dummyComputeGradient(CFloatImage &src, CFloatImage &dx, CFloatImage &dy, CFloatImage &mag, CIntImage &theta) 
{
	CShape sh = src.Shape();
	maxV = -999999999.f, minV = 999999999.f, rang = 1.f;
	for(int y = 1; y < sh.height -1; y++)
	for(int x = 1; x < sh.width - 1; x++)
	{
		dx.Pixel(x,y,0) = src.Pixel(x + 1, y - 1, 0) - src.Pixel(x - 1, y - 1, 0) +
							2 * (src.Pixel(x + 1, y, 0) - src.Pixel(x - 1, y, 0)) +
							src.Pixel(x + 1, y + 1, 0) - src.Pixel(x - 1, y + 1, 0);
		
		dy.Pixel(x,y,0) = src.Pixel(x - 1, y + 1, 0) - src.Pixel(x - 1, y - 1, 0) +
							2 * (src.Pixel(x, y + 1, 0) - src.Pixel(x, y - 1, 0)) +
							src.Pixel(x + 1, y + 1, 0) - src.Pixel(x + 1, y - 1, 0);
		
		mag.Pixel(x,y,0) = sqrt(dx.Pixel(x,y,0) * dx.Pixel(x,y,0) + dy.Pixel(x,y,0) * dy.Pixel(x,y,0));
		if(maxV < mag.Pixel(x,y,0))
			maxV = mag.Pixel(x,y,0);
		if(minV > mag.Pixel(x,y,0))
			minV = mag.Pixel(x,y,0);
		if(isEqual(dx.Pixel(x,y,0), 0.f))
			if(isEqual(dy.Pixel(x,y,0), 0.f))
				theta.Pixel(x,y,0) = 0;
			else
				theta.Pixel(x,y,0) = 2;
		else
		{
			float tanV = dy.Pixel(x,y,0) / dx.Pixel(x,y,0);
			if(tanV > -0.4142f && tanV <= 0.4142f)
				theta.Pixel(x,y,0) = 0;
			else if(tanV > 0.4142f && tanV < 2.4142f)
				theta.Pixel(x,y,0) = 1;
			else if(abs(tanV) >= 2.4142f)
				theta.Pixel(x,y,0) = 2;
			else if(tanV > -2.4142f && tanV <= -0.4142f)
				theta.Pixel(x,y,0) = 3;
		}
	}

	for(int y = 0; y< sh.height; y++)
	{
		mag.Pixel(0, y, 0) = mag.Pixel(1, y, 0);
		mag.Pixel(sh.width - 1, y, 0) = mag.Pixel(sh.width - 2, y, 0);
	}

	for (int x = 0; x < sh.width; x++)
	{
		mag.Pixel(x, 0, 0) = mag.Pixel(x, 1, 0);
		mag.Pixel(x, sh.height - 1, 0) =mag.Pixel(x, sh.height - 2, 0);
	}
	rang = (maxV - minV > 0.f) ? (maxV - minV) : 1.0f;	
}

struct Position 
{
	int x, y;
};
void CompareNeighbors(Position p0, Position p1, Position p2, CFloatImage& mag, CFloatImage& dst)
{
	if (mag.Pixel(p1.x,p1.y,0) <= mag.Pixel(p0.x,p0.y,0) ||
		mag.Pixel(p1.x,p1.y,0) <= mag.Pixel(p2.x,p2.y,0))
	{
		dst.Pixel(p1.x,p1.y,0) = 0.f;
	}
	else
		dst.Pixel(p1.x,p1.y,0) = mag.Pixel(p1.x,p1.y,0);
}

void dummyNonMaximumSupp(CFloatImage &mag, CIntImage &theta, CFloatImage &dst) 
{
	CShape sh = mag.Shape();
	Position p0, p1, p2;
	for(int y = 1; y < sh.height - 1; y++)
	for(int x = 1; x < sh.width - 1; x++)
	{
		switch(theta.Pixel(x,y,0))
		{
		case 0:
			p0.x = x - 1;	p0.y = y;
			p1.x = x;		p1.y = y;
			p2.x = x + 1;	p2.y = y;
			CompareNeighbors(p0, p1, p2, mag, dst);
			break;
		case 1:
			p0.x = x - 1;	p0.y = y - 1;
			p1.x = x;		p1.y = y;
			p2.x = x + 1;	p2.y = y + 1;
			CompareNeighbors(p0, p1, p2, mag, dst);
			break;
		case 2:
			p0.x = x;	p0.y = y - 1;
			p1.x = x;	p1.y = y;
			p2.x = x;	p2.y = y + 1;
			CompareNeighbors(p0, p1, p2, mag, dst);
			break;
		case 3:
			p0.x = x + 1;	p0.y = y - 1;
			p1.x = x;		p1.y = y;
			p2.x = x - 1;	p2.y = y + 1;
			CompareNeighbors(p0, p1, p2, mag, dst);
			break;
		default:
			cout<<"error: there must be sth. wrong when computing mag and theta images!"<<endl;
			exit(0);
		}
	}	

	for(int y = 0; y< sh.height; y++)
	{
		dst.Pixel(0, y, 0) = dst.Pixel(1, y, 0);
		dst.Pixel(sh.width - 1, y, 0) = dst.Pixel(sh.width - 2, y, 0);
	}
	for (int x = 0; x < sh.width; x++)
	{
		dst.Pixel(x, 0, 0) = dst.Pixel(x, 1, 0);
		dst.Pixel(x, sh.height - 1, 0) = dst.Pixel(x, sh.height - 2, 0);
	}

	maxV = -999999999.f, minV = 999999999.f, rang = 1.f;
	Loop_Image(x,y,sh.width,sh.height)
	{
		if(maxV < dst.Pixel(x,y,0))
			maxV = dst.Pixel(x,y,0);
		if(minV > dst.Pixel(x,y,0))
			minV = dst.Pixel(x,y,0);
	}
	rang = (maxV - minV > 0.f) ? (maxV - minV) : 1.0f;	
}

void Recurse(int x, int y, set<int>& posRecord, CFloatImage& src, CFloatImage& dst)
{
	int xx, yy;
	CShape sh = src.Shape();
	for (int iy = -1; iy <= 1; iy++)
	for (int ix = -1; ix <= 1; ix++)
	{
		xx = x + ix;	yy = y + iy;
		if((xx < 0) || (yy < 0) || (xx > sh.width -1) || (yy > sh.height - 1))
			continue;
		if((ix == 0) && (iy == 0))
			continue;
		if ((src.Pixel(xx,yy,0) > low) && (src.Pixel(xx,yy,0) < high) &&
			(posRecord.find(yy * LARGE + xx) == posRecord.end()))
		{
			dst.Pixel(xx, yy, 0) = 0.5f;
			posRecord.insert(yy * LARGE + xx);
			Recurse(xx, yy, posRecord, src, dst);
		}
	}
}

void dummyHysteresis(CFloatImage &src, CFloatImage &dst) 
{
	CShape sh = src.Shape();
	for(int y = 1; y < sh.height - 1; y++)
	for(int x = 1; x < sh.width - 1; x++)
	{
		if(src.Pixel(x,y,0) > high)
		{
			//dst.Pixel(x,y,0) = 1.f; wangtan 2014-07-25
			dst.Pixel(x,y,0) = 0.5f;
			set<int> posRecord;
			posRecord.clear();
			Recurse(x,y,posRecord,src,dst);
		}
	}

	for(int y = 0; y< sh.height; y++)
	{
		dst.Pixel(0, y, 0) = dst.Pixel(1, y, 0);
		dst.Pixel(sh.width - 1, y, 0) = dst.Pixel(sh.width - 2, y, 0);
	}

	for (int x = 0; x < sh.width; x++)
	{
		dst.Pixel(x, 0, 0) = dst.Pixel(x, 1, 0);
		dst.Pixel(x, sh.height - 1, 0) = dst.Pixel(x, sh.height - 2, 0);
	}
}

void performEdgeDetection(CFloatImage &image, CFloatImage &result, int step) 
{

	CShape sh = image.Shape();
	int width = sh.width;
	int height = sh.height;
	int band = sh.nBands;

	CFloatImage grayimg1(width,height,1), grayimg2(width,height,1);
	dummyConvertToGrayScale(image,grayimg1);// 产生灰色图 wangtan

	if(step>=1)
		dummyPreSmooth(grayimg1,grayimg2);
	
	CFloatImage dx(width,height,1), dy(width,height,1), mag(width,height,1);
	CIntImage theta(width,height,1);
	if(step>=2)
		dummyComputeGradient(grayimg2,dx,dy,mag,theta);// 产生梯度

	CFloatImage nonmax(width,height,1);
	if(step>=3)
		dummyNonMaximumSupp(mag,theta,nonmax);
	
	CFloatImage edge(width,height,1);
	if(step>=4)
	{
		//ThreshHold(mag);wangtan 2014-07-25
		dummyHysteresis(nonmax,edge); 
	}
	

	result.ReAllocate(sh,false);
	for (int ih=0 ; ih<height;ih++ ) 
	{
		for (int iw=0 ; iw<width;iw++) 
		{
			for (int ib=0; ib<band;ib++) 
			{
				switch(step)
				{
				case 1: 
					result.Pixel(iw,ih,ib) = grayimg2.Pixel(iw,ih,0);				
					break;
				case 2:
					if (ib==0)
						result.Pixel(iw,ih,ib) = (mag.Pixel(iw,ih,0) - minV) / rang * 1.0f + 0.f;
					else
					{
						result.Pixel(iw,ih,ib)=result.Pixel(iw,ih,0);
					}			
					break;
				case 3:
					if (ib==0)
						result.Pixel(iw,ih,ib) = (nonmax.Pixel(iw,ih,0) - minV) / rang * 1.0f + 0.f;
					else
					{
						result.Pixel(iw,ih,ib)=result.Pixel(iw,ih,0);
					}	 			
					break;
				case 4:
					result.Pixel(iw,ih,ib) = edge.Pixel(iw,ih,0);				
					break;
				default:
					result.Pixel(iw,ih,ib) = edge.Pixel(iw,ih,0);	
				}
			}
		}
	}
}

bool convertImage(const Fl_Image *image, CFloatImage &convertedImage) 
{
	if (image == NULL) 
	{
		return false;
	}

	if (image->count() != 1) 
	{
		return false;
	}

	int w = image->w();
	int h = image->h();
	int d = image->d();

	convertedImage.ReAllocate(CShape(w,h,d));
	// Get the image data.
	const char *const *data = image->data();

	int index = 0;

	for (int y=0; y<h; y++) 
	{
		for (int x=0; x<w; x++) 
		{
			if (d < 3) 
			{
				convertedImage.Pixel(x,y,0) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,1) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,2) = ((uchar) data[0][index]) / 255.0f;
			}
			else 
			{
				convertedImage.Pixel(x,y,0) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,1) = ((uchar) data[0][index+1]) / 255.0f;
				convertedImage.Pixel(x,y,2) = ((uchar) data[0][index+2]) / 255.0f;
			}

			index += d;
		}
	}
	return true;
}

void convertToByteImage(CFloatImage &floatImage, CByteImage &byteImage) 
{
	CShape sh = floatImage.Shape();
	byteImage.ReAllocate(sh);

	for (int y=0; y<sh.height; y++) 
	{
		for (int x=0; x<sh.width; x++) 
		{
			for (int c=0; c<3; c++) 
			{
				float value = floor(255*floatImage.Pixel(x,y,c) + 0.5f);

				if (value < byteImage.MinVal()) 
				{
					value = byteImage.MinVal();
				}
				else if (value > byteImage.MaxVal()) 
				{
					value = byteImage.MaxVal();
				}

				byteImage.Pixel(x,sh.height-y-1,2-c) = (uchar) value;
			}
		}
	}
}

int	writeByteImageToBMP(CByteImage &byteImage, char* fileName) 
{

	FILE	*fp;
	BITMAPFILEHEADER	BmpFileHdr;
	BITMAPINFOHEADER	BmpInfoHdr;
	unsigned char *pBmpData, *pBmpRowData;
	unsigned char	*pMxData, *pMxRowData;
	unsigned int	nRows, nCols, nBmpDataSize;
	unsigned int 	i,j;
	int	Rest;

	CShape sh = byteImage.Shape();

	if(fileName==NULL) 
	{
		return -1;
	}
	
	fp = NULL;
	pBmpData = NULL;

	fp = fopen(fileName, "wb");
	if (!fp)
	{
		if (fp)
		{
			fclose(fp);
		}
		if (pBmpData)
		{
			free(pBmpData);
		}
		return -1;
	}

	nRows = sh.height;
	nCols = sh.width;
	Rest = (nCols*3) % sizeof(int);
	if (Rest!=0)
		Rest = sizeof(int) - Rest;
	nBmpDataSize = nRows * (nCols*3 + Rest);

	BmpFileHdr.bfType = 0x4d42;
	BmpFileHdr.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nBmpDataSize;
	BmpFileHdr.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	BmpFileHdr.bfReserved1 = 0;
	BmpFileHdr.bfReserved2 = 0;

	BmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	BmpInfoHdr.biWidth = nCols;
	BmpInfoHdr.biHeight = nRows;
	BmpInfoHdr.biPlanes = 1;
	BmpInfoHdr.biBitCount = 24;
	BmpInfoHdr.biCompression = BI_RGB;
	BmpInfoHdr.biSizeImage = nBmpDataSize;
	BmpInfoHdr.biXPelsPerMeter = 0;
	BmpInfoHdr.biYPelsPerMeter = 0;
	BmpInfoHdr.biClrUsed = 0;
	BmpInfoHdr.biClrImportant = 0;

	pBmpData = (unsigned char*)malloc(nBmpDataSize);
	if (!pBmpData)
	{
		if (fp)
		{
			fclose(fp);
		}
		if (pBmpData)
		{
			free(pBmpData);
		}
		return -1;
	}

	for (i=0; i<nRows; ++i)
	{
		pBmpRowData = pBmpData+(nRows-1-i)*(nCols*3+Rest);
		for (j=0; j<nCols; ++j)
		{
			pBmpRowData[0] = byteImage.Pixel(j,nRows-1-i,0);
			pBmpRowData[1] = byteImage.Pixel(j,nRows-1-i,1);
			pBmpRowData[2] = byteImage.Pixel(j,nRows-1-i,2);	
			pBmpRowData += 3;
		}
	}

	fwrite(&BmpFileHdr, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&BmpInfoHdr, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(pBmpData, nRows*(nCols*3+Rest), 1, fp);
	fclose(fp);

	return 0;
}


