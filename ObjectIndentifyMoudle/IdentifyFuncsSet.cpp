#include "IdentifyFuncsSet.h"
/**************************************************************************************************************************
Description :显示器屏幕坏点检测
Function     :BadPixel_Identify
Input          :char* pData, 数据流安排的顺序:黑、白、红、绿、蓝(暂定内部分析图像采用24位BMP 图像)
				   int nWidth, 图像横向分辨率
				   int nHight, 图像纵向分辨率
Output       :int& nBadPixelNum，坏点个数
Return        :0： 处理成功
			       others：处理失败（详见错误码定义）          
Author       :Qinbeizhi
Date           :2014.6.27
Copyright   :Qinbeizhi
**************************************************************************************************************************/
int BadPixel_Identify(char* pData, int nWidth, int nHight, int& nBadPixelNum)
{
	int nRetCode = 0;
	int nPictureSize = 0;	
	//数据流安排的顺序:黑、白、红、绿、蓝(暂定内部分析图像采用24位BMP 图像)
	nPictureSize = 3 * nWidth * nHight;//24 位图像一个像素点是3个字节的大小
	if (NULL == pData)
	{
		nRetCode = 0X0001;
		return nRetCode;
	}	

	for (int i = 0; i < nHight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			int nCount = 0;
			char* pstTemp = NULL;
			/*1.先分析纯黑图像下的白点*/
			if (255 == pData[i*nWidth + j] 
			&& 255 == pData[i*nWidth + j + 1] 
			&& 255 == pData[i*nWidth + j + 2])
				nCount++;
			/*2.纯白图像的指针位置*/
			pstTemp = pData + nPictureSize;
			if (0 == pstTemp[i*nWidth + j]
			&& 0 == pstTemp[i*nWidth + j + 1]
			&& 0 == pstTemp[i*nWidth + j + 2])
				nCount++;
			/*3.纯红图像的指针位置*/
			pstTemp = pData + nPictureSize*2;
			if ((0 == pstTemp[i*nWidth + j]
			&& 0 == pstTemp[i*nWidth + j + 1]
			&& 0 == pstTemp[i*nWidth + j + 2]) 
				|| 
				(255 == pstTemp[i*nWidth + j]
			&& 255 == pstTemp[i*nWidth + j + 1]
			&& 255 == pstTemp[i*nWidth + j + 2]))
				nCount++;
			/*3.纯绿图像的指针位置*/
			pstTemp = pData + nPictureSize*3;
			if ((0 == pstTemp[i*nWidth + j]
			&& 0 == pstTemp[i*nWidth + j + 1]
			&& 0 == pstTemp[i*nWidth + j + 2]) 
				|| 
				(255 == pstTemp[i*nWidth + j]
			&& 255 == pstTemp[i*nWidth + j + 1]
			&& 255 == pstTemp[i*nWidth + j + 2]))
				nCount++;
			/*4.纯蓝图像的指针位置*/
			pstTemp = pData + nPictureSize*4;
			if ((0 == pstTemp[i*nWidth + j]
			&& 0 == pstTemp[i*nWidth + j + 1]
			&& 0 == pstTemp[i*nWidth + j + 2]) 
				|| 
				(255 == pstTemp[i*nWidth + j]
			&& 255 == pstTemp[i*nWidth + j + 1]
			&& 255 == pstTemp[i*nWidth + j + 2]))
				nCount++;
			if (BAD_PIXEL_LEVEL == nCount)
				nBadPixelNum++;
		}
	}	
	return nRetCode;
}
/**************************************************************************************************************************
Description :显示器屏幕质量检测模块
Function     :MonitorScreenDeadPixel_Identify
Input          :DataStream pDataStream,待处理的数据流
Output       :DetectObjectProperty* pAnalyseResult,最终分析后的结果
Return        :0： 处理成功
				   others：处理失败（详见错误码定义）          
Author       :Qinbeizhi
Date           :2014.6.27
Copyright   :Qinbeizhi
**************************************************************************************************************************/
int MonitorScreenDeadPixel_Identify(DataStream* pDataStream, DetectObjectProperty* pAnalyseResult)
{
	int nRetCode = 0;
	int nWidth = 0;//待处理图像横向分辨率
	int nHigth = 0;//待处理图像纵向分辨率
	int nPictureSize = 0;
	int nBadPixelNum = 0;
	nWidth = pDataStream->nWidth;
	nHigth = pDataStream->nHight;
	nPictureSize = nWidth * nHigth;
	/*坏点分析*/
	nRetCode = BadPixel_Identify((char*)pDataStream->pBuffer, nWidth, nHigth, nBadPixelNum);
	/*结果输出*/
	MonitorScreenProperty strMonitorForDetect;	
	strMonitorForDetect.nBadPixelNum = nBadPixelNum;	
	pAnalyseResult = &strMonitorForDetect;
	pAnalyseResult->emCurDetectType = MonitorScreen;
	/*其他功能后续增加*/
	return nRetCode;
}
