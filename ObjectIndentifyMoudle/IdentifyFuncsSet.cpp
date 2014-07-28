#include "IdentifyFuncsSet.h"
#include <tchar.h>
MonitorScreenProperty  BadPixelLocaResults;//模块内的全局变量，记录最终分析结果

bool FindVectorNode(Position strPos, vector<Position>vBadPixelList)
{
		vector<Position>::iterator result = vBadPixelList.begin();
		for (; result != vBadPixelList.end(); ++result)
			if(result->Xposition == strPos.Xposition && result->Yposition == strPos.Yposition)
				return true;
		return false;
}
bool WriteShareMemFunc()
{
		HANDLE hMap = NULL;
		HANDLE hAddress = NULL;
		int  iErrCode  = 0;
		hMap = ::OpenFileMapping( FILE_MAP_WRITE, false, _T("xxxxxxxx"));
		if (NULL == hMap)
		{
			iErrCode = GetLastError();			
			return false;
		}
		/*获取共享内存地址*/
		 hAddress = MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
		 if (NULL == hAddress)
		 {
			 return false;
		 }
		/*向共享内存中写数据*/
		memcpy(hAddress, &BadPixelLocaResults.vBadPixel, BadPixelLocaResults.vBadPixel.capacity() * sizeof( Position ));
		return true;
}
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
int BadPixel_Identify_BlackPic(char* pData, int nWidth, int nHight)
{
	int nRetCode = 0;		
	//数据流安排的顺序:黑、白、红、绿、蓝(暂定内部分析图像采用24位BMP 图像)
	if (NULL == pData)
	{
		nRetCode = 0x0001;
		return nRetCode;
	}
	for (int i = 0; i < nHight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			int nCount = 0;
			char* pstTemp = NULL;
			/*先分析纯黑图像下的白点*/
			if ( pData[i*nWidth + j]  ==  pData[i*nWidth + j + 1] 
			&& pData[i*nWidth + j + 1]  == pData[i*nWidth + j + 2] )
			{
				if (0 != pData[i*nWidth + j])
				{
					Position strTemp = {i, j};
					BadPixelLocaResults.vBadPixel.push_back(strTemp);
				}
			}
		}
	}	
	return nRetCode;
}

int BadPixel_Identify_WhitePic(char* pData, int nWidth, int nHight)
{
	int nRetCode = 0;	
	if (NULL == pData)
	{
		nRetCode = 0x0002;
		return nRetCode;
	}
	for (int i = 0; i < nHight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			int nCount = 0;
			char* pstTemp = NULL;
			/*先分析纯白图像下的黑点*/
			if ( pData[i*nWidth + j]  ==  pData[i*nWidth + j + 1] 
			&& pData[i*nWidth + j + 1]  == pData[i*nWidth + j + 2] )
			{
				if (0 == pData[i*nWidth + j])
				{
					Position strTemp = {i, j};
					/*如果不在原来的坏点栈中则添加*/
					if (!FindVectorNode(strTemp, BadPixelLocaResults.vBadPixel))
						BadPixelLocaResults.vBadPixel.push_back(strTemp);
				}
			}
		}
	}	
	return nRetCode;
}
int BadPixel_Identify_OtherPic(char* pData, int nWidth, int nHight)
{
	int nRetCode = 0;	
	if (NULL == pData)
	{
		nRetCode = 0x0003;
		return nRetCode;
	}
	if (!BadPixelLocaResults.vBadPixel.empty())
	{
		 for(vector<Position>::const_iterator it = BadPixelLocaResults.vBadPixel.begin(); it < BadPixelLocaResults.vBadPixel.end(); ++it)
		 {
				/*查看对应的每个位置处图像情况，若是为全黑或者全白则该处即是坏点*/
				 if ( pData[it->Yposition*nWidth + it->Xposition]  ==  pData[it->Yposition*nWidth + it->Xposition + 1] 
				 && pData[it->Yposition*nWidth + it->Xposition + 1]  == pData[it->Yposition*nWidth + it->Xposition+ 2] )
				 {
					 /*若是坏点则不对栈进行任何处理*/
				 }
				 else 
				 {
					/*若不是坏点则弹栈*/
					BadPixelLocaResults.vBadPixel.erase(it);
				 }
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
int MonitorScreenDeadPixel_Identify(DataStream* pDataStream)
{
	int nRetCode = 0;
	int nWidth = 0;//待处理图像横向分辨率
	int nHigth = 0;//待处理图像纵向分辨率
	int nPictureSize = 0;
	int nBadPixelNum = 0;
	nWidth = pDataStream->nWidth;
	nHigth = pDataStream->nHight;	
	/*坏点分析*/
	  switch (pDataStream->eColorType)
	  {
	  case Black:      nRetCode = BadPixel_Identify_BlackPic((char*)pDataStream->pBuffer, nWidth, nHigth); 
							break;
	  case White:     nRetCode = BadPixel_Identify_WhitePic((char*)pDataStream->pBuffer, nWidth, nHigth); 
							break;
	  case Complex:break;
	  default:           nRetCode = BadPixel_Identify_OtherPic((char*)pDataStream->pBuffer, nWidth, nHigth); 
							if (Blue == pDataStream->eColorType)//写共享内存
								if (!WriteShareMemFunc())
										return CAN_NOT_WRITE_SHAREMEM;
							break;
	 }
	/*其他功能后续增加*/
	return nRetCode;
}
