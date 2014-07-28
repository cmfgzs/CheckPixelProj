#ifndef _CHECKPIXELUI_BASICDEF_H_
#define _CHECKPIXELUI_BASICDEF_H_
#include <windows.h>
#include<vector> 
using namespace std;
/**********************************************宏定义区域Start*************************************************/

/****************错误码定义区Start***************************/
#define UNKONWN_DETECT_TYPE 0x0001 //未知的检测类型
#define CAN_NOT_WRITE_SHAREMEM 0xFFFF //写共享内存失败
/****************错误码定义区End*****************************/

/**********************************************宏定义区域End***************************************************/




/**********************************************数据结构定义区域Start*************************************************/
#pragma pack(push,1)        //这里采用一字节对齐是方便后续从数据流中去数据
/*分析对象的枚举定义*/
typedef enum
{
	StartObject = 0x0000,   //初始分析对象
	MonitorScreen = 0x0001, //显示屏幕分析

	EndObject = 0x0010
}DetectType;

typedef enum
{
	Black ,
	White,
	Red,
	Green,
	Blue	,
	Complex //非单色图
}Color;

struct DataStream
{
	int        nWidth;       //代分析的数据的宽度
	int        nHight;		 ////代分析的数据的高度
	void*     pBuffer;      //该对象的数据	
	Color    eColorType; //色彩类型 
};

struct DataForFurProcess
{
	DetectType  emObjectType; //需要进一步处理的对象
	DataStream* pDataStream;  //待处理的数据流
};
#pragma pack(pop)

struct Position
{
	int Xposition;
	int Yposition;
};
/*处理对象基础数据结构*/
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
	/*属性部分*/

public : 
	DetectType emCurDetectType; //当前的检测对象
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
	/*属性部分*/
public:
	vector<Position> vBadPixel; //坏点集合
	int GetDetectType() {return MonitorScreen;}
};
/**********************************************数据结构定义区域End***************************************************/
#endif


