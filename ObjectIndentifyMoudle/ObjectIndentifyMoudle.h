#ifndef _OBJECT_INDENTIFY_MOUDLE_H_INCLUDED
#define _OBJECT_INDENTIFY_MOUDLE_H_INCLUDED
#include "IdentifyFuncsSet.h"         //实际数据处理文件


/*数据处理函数的定义*/
typedef int (*DataProcessMoudle_CalBack)(DataStream* pDataStream);


/*回调数据结构定义*/
typedef struct 
{
	DetectType emCurreDetectObject;
	DataProcessMoudle_CalBack DataHandleFunc;
}DataProcessMoudleStruct;



#ifdef  OBJECTINDENTIFYMOUDLE_EXPORTS
#define OBJECTINDENTIFYMOUDLE_API __declspec(dllexport)
#else
#define OBJECTINDENTIFYMOUDLE_API __declspec(dllimport)
#endif

#ifdef __cplusplus
#define EXTERN_C     extern "C"
#else
#define EXTERN_C
#endif 

/**************************************************************************************************************************
Description :检测分析功能模块的总入口
Function     :CameraDataProcessEntry
Input          :LPVOID lParam, 带处理的数据流
			       （经过轮廓线提取后的数据，请调用者保证入参非空，申请的内存请在外部释放）			 
Output       :DetectObjectProperty* pAnalyseResult,分析后的结果
Return       :0： 处理成功
                  others：处理失败（详见错误码定义）
Author      :Qinbeizhi
Date          :2014.6.27
Copyright  :Qinbeizhi
**************************************************************************************************************************/
EXTERN_C OBJECTINDENTIFYMOUDLE_API int CameraDataProcessEntry(LPVOID lParam);
#endif



