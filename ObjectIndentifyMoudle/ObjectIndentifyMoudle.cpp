#include "ObjectIndentifyMoudle.h"

/*全局回调结构体变量，后续增加新功能可以在此处添加对应类型和处理函数即可*/
DataProcessMoudleStruct pstrOperator[] = 
{
	{StartObject,     NULL},  //初始检测对象没有对应处理函数
	{MonitorScreen,   MonitorScreenDeadPixel_Identify},  //屏幕坏点检测

	{EndObject,       NULL}   //未知的检测类型
};

/**************************************************************************************************************************
Description :根据数据流头部的检测数据类型获取相应的处理函数
Function    :GetDataProcessFunc
Input       :DetectType DetectObjectFromCamera,待处理的数据类型			
Output      :DataProcessMoudle_CalBack pstOpreaor,对应的处理函数指针
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
Description :检测分析功能模块的总入口
Function    :CameraDataProcessEntry
Input       :LPVOID lParam, 带处理的数据流
			（经过轮廓线提取后的数据，请调用者保证入参非空，申请的内存请在外部释放）			 
Output      :DetectObjectProperty* pAnalyseResult,分析后的结果
Return      :0： 处理成功
             others：处理失败（详见错误码定义）
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


