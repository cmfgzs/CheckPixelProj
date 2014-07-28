#ifndef _CHECKPIXELUI_BASICDEF_H_
#define _CHECKPIXELUI_BASICDEF_H_
#include <windows.h>
#include<vector> 
using namespace std;
/**********************************************�궨������Start*************************************************/

/****************�����붨����Start***************************/
#define UNKONWN_DETECT_TYPE 0x0001 //δ֪�ļ������
#define CAN_NOT_WRITE_SHAREMEM 0xFFFF //д�����ڴ�ʧ��
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

typedef enum
{
	Black ,
	White,
	Red,
	Green,
	Blue	,
	Complex //�ǵ�ɫͼ
}Color;

struct DataStream
{
	int        nWidth;       //�����������ݵĿ��
	int        nHight;		 ////�����������ݵĸ߶�
	void*     pBuffer;      //�ö��������	
	Color    eColorType; //ɫ������ 
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
	virtual  int GetDetectType() = 0;
	/*���Բ���*/

public : 
	DetectType emCurDetectType; //��ǰ�ļ�����
};

struct MonitorScreenProperty :DetectObjectProperty
{
	MonitorScreenProperty()
	{
		emCurDetectType = MonitorScreen;
	}

	~MonitorScreenProperty()
	{
		vector<Position>(vBadPixel).swap(vBadPixel);
	}
	/*���Բ���*/
public:
	vector<Position> vBadPixel; //���㼯��
	int GetDetectType() {return MonitorScreen;}
};
/**********************************************���ݽṹ��������End***************************************************/
#endif


