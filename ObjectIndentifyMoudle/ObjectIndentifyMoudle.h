#ifndef _OBJECT_INDENTIFY_MOUDLE_H_INCLUDED
#define _OBJECT_INDENTIFY_MOUDLE_H_INCLUDED
#include "DataAnalysMoudleBasicDef.h"
#include "IdentifyFuncsSet.h"         //ʵ�����ݴ����ļ�


/*���ݴ������Ķ���*/
typedef int (*DataProcessMoudle_CalBack)(DataStream* pDataStream, DetectObjectProperty& strAnalyseResult);


/*�ص����ݽṹ����*/
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
Description :����������ģ��������
Function    :CameraDataProcessEntry
Input       :LPVOID lParam, �������������
			��������������ȡ������ݣ�������߱�֤��ηǿգ�������ڴ������ⲿ�ͷţ�			 
Output      :DetectObjectProperty& strAnalyseResult,������Ľ��
Return      :0�� ����ɹ�
             others������ʧ�ܣ���������붨�壩
Author      :Qinbeizhi
Date        :2014.6.27
Copyright   :Qinbeizhi
**************************************************************************************************************************/
EXTERN_C OBJECTINDENTIFYMOUDLE_API int CameraDataProcessEntry(LPVOID lParam, DetectObjectProperty& strAnalyseResult);

#endif


