#include "IdentifyFuncsSet.h"
#include <tchar.h>
MonitorScreenProperty  BadPixelLocaResults;//ģ���ڵ�ȫ�ֱ�������¼���շ������

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
		/*��ȡ�����ڴ��ַ*/
		 hAddress = MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
		 if (NULL == hAddress)
		 {
			 return false;
		 }
		/*�����ڴ���д����*/
		memcpy(hAddress, &BadPixelLocaResults.vBadPixel, BadPixelLocaResults.vBadPixel.capacity() * sizeof( Position ));
		return true;
}
/**************************************************************************************************************************
Description :��ʾ����Ļ������
Function     :BadPixel_Identify
Input          :char* pData, ���������ŵ�˳��:�ڡ��ס��졢�̡���(�ݶ��ڲ�����ͼ�����24λBMP ͼ��)
				   int nWidth, ͼ�����ֱ���
				   int nHight, ͼ������ֱ���
Output       :int& nBadPixelNum���������
Return        :0�� ����ɹ�
			       others������ʧ�ܣ���������붨�壩          
Author       :Qinbeizhi
Date           :2014.6.27
Copyright   :Qinbeizhi
**************************************************************************************************************************/
int BadPixel_Identify_BlackPic(char* pData, int nWidth, int nHight)
{
	int nRetCode = 0;		
	//���������ŵ�˳��:�ڡ��ס��졢�̡���(�ݶ��ڲ�����ͼ�����24λBMP ͼ��)
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
			/*�ȷ�������ͼ���µİ׵�*/
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
			/*�ȷ�������ͼ���µĺڵ�*/
			if ( pData[i*nWidth + j]  ==  pData[i*nWidth + j + 1] 
			&& pData[i*nWidth + j + 1]  == pData[i*nWidth + j + 2] )
			{
				if (0 == pData[i*nWidth + j])
				{
					Position strTemp = {i, j};
					/*�������ԭ���Ļ���ջ�������*/
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
				/*�鿴��Ӧ��ÿ��λ�ô�ͼ�����������Ϊȫ�ڻ���ȫ����ô����ǻ���*/
				 if ( pData[it->Yposition*nWidth + it->Xposition]  ==  pData[it->Yposition*nWidth + it->Xposition + 1] 
				 && pData[it->Yposition*nWidth + it->Xposition + 1]  == pData[it->Yposition*nWidth + it->Xposition+ 2] )
				 {
					 /*���ǻ����򲻶�ջ�����κδ���*/
				 }
				 else 
				 {
					/*�����ǻ�����ջ*/
					BadPixelLocaResults.vBadPixel.erase(it);
				 }
		 }
	}
	return nRetCode;
}

/**************************************************************************************************************************
Description :��ʾ����Ļ�������ģ��
Function     :MonitorScreenDeadPixel_Identify
Input          :DataStream pDataStream,�������������
Output       :DetectObjectProperty* pAnalyseResult,���շ�����Ľ��
Return        :0�� ����ɹ�
				   others������ʧ�ܣ���������붨�壩          
Author       :Qinbeizhi
Date           :2014.6.27
Copyright   :Qinbeizhi
**************************************************************************************************************************/
int MonitorScreenDeadPixel_Identify(DataStream* pDataStream)
{
	int nRetCode = 0;
	int nWidth = 0;//������ͼ�����ֱ���
	int nHigth = 0;//������ͼ������ֱ���
	int nPictureSize = 0;
	int nBadPixelNum = 0;
	nWidth = pDataStream->nWidth;
	nHigth = pDataStream->nHight;	
	/*�������*/
	  switch (pDataStream->eColorType)
	  {
	  case Black:      nRetCode = BadPixel_Identify_BlackPic((char*)pDataStream->pBuffer, nWidth, nHigth); 
							break;
	  case White:     nRetCode = BadPixel_Identify_WhitePic((char*)pDataStream->pBuffer, nWidth, nHigth); 
							break;
	  case Complex:break;
	  default:           nRetCode = BadPixel_Identify_OtherPic((char*)pDataStream->pBuffer, nWidth, nHigth); 
							if (Blue == pDataStream->eColorType)//д�����ڴ�
								if (!WriteShareMemFunc())
										return CAN_NOT_WRITE_SHAREMEM;
							break;
	 }
	/*�������ܺ�������*/
	return nRetCode;
}
