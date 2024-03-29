/************************************************************************/
/* Copyright (c) 2013, 中国大恒集团北京图像视觉技术分公司视觉系统部  
/* All rights reserved.													
/*																		
/* 文件名称： DHGrabberForMER.h	
/* 摘要： 导出采集卡类（公司水星相机）

/* 当前版本：1.0.0.4
/* 修改者：郭明兵
/* 主要修改内容：实现支持裁剪输出图片的功能,提供裁剪图片大小和偏移设置,提供最小曝光时间,图片最大宽度,最大高度等参数的查询
/* 完成日期：2020年4月13日

/* 当前版本：1.0.0.3
/* 修改者：阴同
/* 主要修改内容：实现基础的初始化、连续采集、停止采集、单帧采集和参数设置界面 部分函数替换为新版sdk函数
/* 完成日期：2016年7月15日

/************************************************************************/

#if !defined(DHGrabber_h_)
#define DHGrabber_h_

/*
采集卡类型编号定义如下
0:模拟采集卡
1:DALSA X64采集卡
2:AVT相机
3:千兆网相机
4:公司CG300模拟采集卡
5:公司1394相机
6:EURESYS 采集卡
7:公司USB相机
8::公司水星系列相机*/

const int GBMaxFileNameLen = 255;
const int GBMaxTextLen = 255;

typedef struct _GBSignalInfoStruct//回调信息结构
{
	PVOID Context;								//存放拥有回调函数的对象的this指针
	int iGrabberTypeSN;							// 采集卡类型编号
	int nGrabberSN;								//采集卡序号
	int nErrorCode;								//错误代码（0：正常）
	int nFrameCount;							//已采集的帧记数
	char strDescription[GBMaxTextLen];			//描述信息

	int iReserve1;								// 备用字段1
	int iReserve2;								// 备用字段2
	
}s_GBSIGNALINFO;//回调信息结构

//回调函数指针声明
typedef void (WINAPI *PGBCALLBACK)(const s_GBSIGNALINFO* SignalInfo);

typedef struct _GBInitStruct//采集卡初始化结构
{
	int iGrabberTypeSN;						// 采集卡类型编号
	int nGrabberSN;							//采集卡序号
    char strDeviceName[GBMaxTextLen];		//设备名称
	char strDeviceMark[GBMaxTextLen];		//设备标识
	char strGrabberFile[GBMaxFileNameLen];	//用于初始化的文件
	PGBCALLBACK CallBackFunc;				//回调函数指针
	PVOID Context;							//存放拥有回调函数的对象的this指针
	
	int iReserve1;							// 备用字段1
	int iReserve2;							// 备用字段2

}s_GBINITSTRUCT;//采集卡初始化结构

typedef struct _GBErrorInfoStruct//错误信息结构
{
   	int nErrorCode;							//错误代码（0：正常）
	char strErrorDescription[GBMaxTextLen];	//错误描述
	char strErrorRemark[GBMaxTextLen];		//附加信息

}s_GBERRORINFO;//错误信息结构

//公共参数
enum GBParamID
{
	GBImageWidth = 0,		//图像宽度 (单位:字节)
	GBImageHeight,			//图像高度 (单位:字节)
	GBImagePixelSize,		//图像象素大小 (单位:字节)
	GBImageBufferSize,		//图像缓冲区大小 (单位:字节)
	GBImageBufferAddr,		//图像缓冲区地址
	GBGrabberTypeSN,		//采集卡类型编号
};

//错误代码定义
const int GBOK = 0;

class CGrabber
{
//操作
public:
	CGrabber()
	{
	}
	virtual ~CGrabber()
	{
	}
	
	//初始化
	virtual BOOL Init(const s_GBINITSTRUCT* pInitParam) = 0;

	//关闭
	virtual BOOL Close() = 0;

	//开始采集
	virtual BOOL StartGrab() = 0;

	//停止采集
	virtual BOOL StopGrab() = 0;
	
	//单帧采集
	virtual BOOL Snapshot() = 0;

	//设置参数
	virtual BOOL SetParamInt(GBParamID Param, int nInputVal) = 0;
	
	//得到参数
	virtual BOOL GetParamInt(GBParamID Param, INT_PTR &nOutputVal) = 0;
	
	//调用参数对话框
	virtual void CallParamDialog() = 0;
	
	//得到出错信息
	virtual void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo) = 0;
};

#endif// !defined(DHGrabber_h_)

#if !defined(DHGrabberMER_h_)
#define DHGrabberMER_h_

const int MaxSNLen  = 32;

// 水星相机专有参数
enum MERParamID
{
	MERSnapMode,				// 图像采集模式
	MERGain,					// 增益
	MERExposure,				// 曝光时间
	MERBlacklevel,			    // 黑电平
	MERBalanceRatio,            // 白平衡
    MERDeviceVersion,           // 版本号
    MERCarveImageWidth,			// 裁剪图片区域宽度
    MERCarveImageHeight,		// 裁剪图片区域高度
    MERImageOffsetHoriz,		// 裁剪图片图像水平偏移
    MERImageOffsetVerti,		// 裁剪图片图像竖直偏移
    MERExposurTimeMin,			// 最小曝光时间 (仅查)
    MERImageMaxWidth,			// 图片最大宽度 (仅查)
    MERImageMaxHeight,			// 图片最大高度 (仅查)
};

//采集类
class _declspec(dllexport) CDHGrabberMER : public CGrabber
{
	//操作
public:
	CDHGrabberMER();
	virtual ~CDHGrabberMER();
	
	//初始化
	BOOL Init(const s_GBINITSTRUCT* pInitParam);
	
	//关闭
	BOOL Close();
	
	//开始采集
	BOOL StartGrab();
	
	//停止采集
	BOOL StopGrab();
	
	// 单帧采集 
	BOOL Snapshot();
	
	//设置参数
	BOOL SetParamInt(GBParamID Param, int nInputVal);
	
	//得到参数
	BOOL GetParamInt(GBParamID Param, INT_PTR &nOutputVal);
	
	//调用参数对话框
	void CallParamDialog();
	
	//得到出错信息
	void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo);
	
	//////////////////////////////////////////////////////////////////////////
	// 公司相机专有接口	
	BOOL MERSetParamInt(MERParamID Param, int nInputVal1,int nInputVal2 = 0,int nInputVal3 = 0,int nInputVal4 = 0,int nInputVal5 = 0,void *sInputVal6 = NULL);
	BOOL MERGetParamInt(MERParamID Param, int &nOutputVal1,int &nOutputVal2,int &nOutputVal3);	
	BOOL MERGetParamString(MERParamID Param, char* sOutputString,int size);	
	int  MERSetOutputParamInt(int nOutputPort,int nOutputVal);//设置相机自定义输出内容 
	//返回值 0:成功 1:不支持自定义信号 2:设置自定义信号失败 3:打开端口失败 4:写输出值失败 
    //获取相机个数
	BOOL MERGetCameraCount(int &nCameraCount);
	//按序号获取相机序列号,序号从0开始
    BOOL MerGetCameraSN(int nCameraNumber,char sCameraSN[MaxSNLen]);
	
	void SaveParamToINI();		// 保存相机设置信息
	
	//
	//////////////////////////////////////////////////////////////////////////
	int CamerColorOrGray;
	//属性
protected:
	CGrabber *m_pGrabber;
};

#endif// !defined(DHGrabberMER_h_)