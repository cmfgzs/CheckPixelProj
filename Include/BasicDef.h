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

/*�������������ݽṹ*/
struct DetectObjectProperty
{
	DetectObjectProperty()
	{

	}

	~DetectObjectProperty()
	{

	}
	/*���Բ���*/

protected : 
	DetectType emCurDetectType; //��ǰ�ļ�����		
};

struct MonitorScreenProperty :DetectObjectProperty
{
	MonitorScreenProperty()
	{

	}

	~MonitorScreenProperty()
	{

	}
	/*���Բ���*/

};
/**********************************************���ݽṹ��������End***************************************************/
#endif

