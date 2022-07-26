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

#include "DHGrabberBase.h"

#if !defined(DHGrabberMER_h_)
#define DHGrabberMER_h_


#ifdef DHGrabberMER_DLL
#define DHGrabberMER_IMPL _declspec(dllexport)
#else
#define DHGrabberMER_IMPL _declspec(dllimport)
#endif

// 水星相机专有参数
enum MERParamID
{
    MERSnapMode,				// 图像采集模式
    MERGain,					// 增益
    MERExposure,				// 曝光时间
    MERBlacklevel,			    // 黑电平
    MERBalanceRatio,            // 白平衡
    MERDeviceVersion,           // 版本号
    MER_INT_WIDTH,              // 输出图片宽
    MER_INT_HEIGHT,             // 输出图片高
    MER_INT_OFFSETX,            // 输出图片水平偏移
    MER_INT_OFFSETY,            // 输出图片竖直偏移
    MERExposurTimeMin,			// 最小曝光时间 (仅查)
};

//采集类
class DHGrabberMER_IMPL CDHGrabberMER : public CGrabber
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
    BOOL GetParamInt(GBParamID Param, int &nOutputVal);

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
    static BOOL MERGetCameraCount(int &nCameraCount);
    //按序号获取相机序列号,序号从0开始
    static BOOL MerGetCameraSN(int nCameraNumber,char sCameraSN[MaxSNLen]);
    //设置剪切框
    BOOL MERSetROI(int noffsetX,int noffsetY,int nwidth,int nheight);

    void SaveParamToINI();		// 保存相机设置信息
    
    //
    //////////////////////////////////////////////////////////////////////////
    int CamerColorOrGray;
    //属性
protected:
    CGrabber *m_pGrabber;
};

#endif// !defined(DHGrabberMER_h_)