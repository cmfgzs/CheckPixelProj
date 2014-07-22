#include "ObjectIndentifyMoudle.h"

/*ȫ�ֻص��ṹ����������������¹��ܿ����ڴ˴���Ӷ�Ӧ���ͺʹ���������*/
DataProcessMoudleStruct pstrOperator[] = 
{
	{StartObject,     NULL},  //��ʼ������û�ж�Ӧ������
	{MonitorScreen,   MonitorScreenDeadPixel_Identify},  //��Ļ������

	{EndObject,       NULL}   //δ֪�ļ������
};

/**************************************************************************************************************************
Description :����������ͷ���ļ���������ͻ�ȡ��Ӧ�Ĵ�����
Function    :GetDataProcessFunc
Input       :DetectType DetectObjectFromCamera,���������������			
Output      :DataProcessMoudle_CalBack pstOpreaor,��Ӧ�Ĵ�����ָ��
Return      :NULL             
Author      :Qinbeizhi
Date        :2014.6.27
Copyright   :Qinbeizhi
**************************************************************************************************************************/
DataProcessMoudle_CalBack GetDataProcessFunc(DetectType DetectObjectFromCamera)
{
	DataProcessMoudle_CalBack pstOpreaor = NULL;
	for (int i = 0; EndObject != pstrOperator[i].emCurreDetectObject; i++)
	{
		if (pstrOperator[i].emCurreDetectObject == DetectObjectFromCamera)
			{
				pstOpreaor = pstrOperator[i].DataHandleFunc;
				break;
			}
	}
	return pstOpreaor;
}

/**************************************************************************************************************************
Description :����������ģ��������
Function    :CameraDataProcessEntry
Input       :LPVOID lParam, �������������
			��������������ȡ������ݣ�������߱�֤��ηǿգ�������ڴ������ⲿ�ͷţ�			 
Output      :DetectObjectProperty* pAnalyseResult,������Ľ��
Return      :0�� ����ɹ�
             others������ʧ�ܣ���������붨�壩
Author      :Qinbeizhi
Date        :2014.6.27
Copyright   :Qinbeizhi
**************************************************************************************************************************/
int CameraDataProcessEntry(LPVOID lParam, DetectObjectProperty* pAnalyseResult)
{
	int nRetCode = 0; 
	DataProcessMoudle_CalBack pDataProOperator = NULL;
	DataForFurProcess* pDataFromPre = (DataForFurProcess*)&lParam;
	pDataProOperator = GetDataProcessFunc(pDataFromPre->emObjectType);

	if (NULL == pDataProOperator)
	{
		nRetCode = UNKONWN_DETECT_TYPE;
		return nRetCode;
	}
	nRetCode = pDataProOperator(pDataFromPre->pDataStream, pAnalyseResult);
	return nRetCode;
}


