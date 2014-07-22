#include "IdentifyFuncsSet.h"
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
int BadPixel_Identify(char* pData, int nWidth, int nHight, int& nBadPixelNum)
{
	int nRetCode = 0;
	int nPictureSize = 0;	
	//���������ŵ�˳��:�ڡ��ס��졢�̡���(�ݶ��ڲ�����ͼ�����24λBMP ͼ��)
	nPictureSize = 3 * nWidth * nHight;//24 λͼ��һ�����ص���3���ֽڵĴ�С
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
			/*1.�ȷ�������ͼ���µİ׵�*/
			if (255 == pData[i*nWidth + j] 
			&& 255 == pData[i*nWidth + j + 1] 
			&& 255 == pData[i*nWidth + j + 2])
				nCount++;
			/*2.����ͼ���ָ��λ��*/
			pstTemp = pData + nPictureSize;
			if (0 == pstTemp[i*nWidth + j]
			&& 0 == pstTemp[i*nWidth + j + 1]
			&& 0 == pstTemp[i*nWidth + j + 2])
				nCount++;
			/*3.����ͼ���ָ��λ��*/
			pstTemp = pData + nPictureSize*2;
			if ((0 == pstTemp[i*nWidth + j]
			&& 0 == pstTemp[i*nWidth + j + 1]
			&& 0 == pstTemp[i*nWidth + j + 2]) 
				|| 
				(255 == pstTemp[i*nWidth + j]
			&& 255 == pstTemp[i*nWidth + j + 1]
			&& 255 == pstTemp[i*nWidth + j + 2]))
				nCount++;
			/*3.����ͼ���ָ��λ��*/
			pstTemp = pData + nPictureSize*3;
			if ((0 == pstTemp[i*nWidth + j]
			&& 0 == pstTemp[i*nWidth + j + 1]
			&& 0 == pstTemp[i*nWidth + j + 2]) 
				|| 
				(255 == pstTemp[i*nWidth + j]
			&& 255 == pstTemp[i*nWidth + j + 1]
			&& 255 == pstTemp[i*nWidth + j + 2]))
				nCount++;
			/*4.����ͼ���ָ��λ��*/
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
int MonitorScreenDeadPixel_Identify(DataStream* pDataStream, DetectObjectProperty* pAnalyseResult)
{
	int nRetCode = 0;
	int nWidth = 0;//������ͼ�����ֱ���
	int nHigth = 0;//������ͼ������ֱ���
	int nPictureSize = 0;
	int nBadPixelNum = 0;
	nWidth = pDataStream->nWidth;
	nHigth = pDataStream->nHight;
	nPictureSize = nWidth * nHigth;
	/*�������*/
	nRetCode = BadPixel_Identify((char*)pDataStream->pBuffer, nWidth, nHigth, nBadPixelNum);
	/*������*/
	MonitorScreenProperty strMonitorForDetect;	
	strMonitorForDetect.nBadPixelNum = nBadPixelNum;	
	pAnalyseResult = &strMonitorForDetect;
	pAnalyseResult->emCurDetectType = MonitorScreen;
	/*�������ܺ�������*/
	return nRetCode;
}
