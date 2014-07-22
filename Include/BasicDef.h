#ifndef _CHECKPIXELUI_BASICDEF_H_
#define _CHECKPIXELUI_BASICDEF_H_
#include <windows.h>
/**********************************************�궨������Start*************************************************/

/****************�����붨����Start***************************/
#define UNKONWN_DETECT_TYPE 0x0001 //δ֪�ļ������
/****************�����붨����End*****************************/

/**********************************************�궨������End***************************************************/




/**********************************************���ݽṹ��������Start*************************************************/
#pragma pack(push,1)        //�������һ�ֽڶ����Ƿ����������������ȥ����
/*���������ö�ٶ���*/
typedef enum
{
	StartObject = 0x0000,   //��ʼ��������
	MonitorScreen = 0x0001, //��ʾ��Ļ����

	EndObject = 0x0010
}DetectType;

struct DataStream
{
	int        nWidth;       //�����������ݵĿ��
	int        nHight;		 ////�����������ݵĸ߶�
	void*      pBuffer;      //�ö��������
};

struct DataForFurProcess
{
	DetectType  emObjectType; //��Ҫ��һ������Ķ���
	DataStream* pDataStream;  //�������������
};
#pragma pack(pop)

struct Position
{
	int Xposition;
	int Yposition;
};
/*�������������ݽṹ*/
struct DetectObjectProperty
{
	DetectObjectProperty()
	{
		emCurDetectType = StartObject;
	}

	~DetectObjectProperty()
	{

	}
	virtual  int GetResults() = 0;
	/*���Բ���*/

public : 
	DetectType emCurDetectType; //��ǰ�ļ�����		
};

struct MonitorScreenProperty :DetectObjectProperty
{
	MonitorScreenProperty()
	{
		nBadPixelNum = 0;
	}

	~MonitorScreenProperty()
	{

	}
	/*���Բ���*/
public:	
	int nBadPixelNum; //�������	
	int GetResults()	{ return nBadPixelNum;}	
};
/**********************************************���ݽṹ��������End***************************************************/
#endif


