#ifndef _CHECKPIXELUI_BASICDEF_H_
#define _CHECKPIXELUI_BASICDEF_H_
#include <windows.h>
/**********************************************宏定义区域Start*************************************************/

/****************错误码定义区Start***************************/
#define UNKONWN_DETECT_TYPE 0x0001 //未知的检测类型
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

struct DataStream
{
	int        nWidth;       //代分析的数据的宽度
	int        nHight;		 ////代分析的数据的高度
	void*      pBuffer;      //该对象的数据
};

struct DataForFurProcess
{
	DetectType  emObjectType; //需要进一步处理的对象
	DataStream* pDataStream;  //待处理的数据流
};
#pragma pack(pop)

/*处理对象基础数据结构*/
struct DetectObjectProperty
{
	DetectObjectProperty()
	{

	}

	~DetectObjectProperty()
	{

	}
	/*属性部分*/

protected : 
	DetectType emCurDetectType; //当前的检测对象		
};

struct MonitorScreenProperty :DetectObjectProperty
{
	MonitorScreenProperty()
	{

	}

	~MonitorScreenProperty()
	{

	}
	/*属性部分*/

};
/**********************************************数据结构定义区域End***************************************************/
#endif

