/************************************************************************/
/* Copyright (c) 2006, 中国大恒集团北京图像视觉技术分公司   
/* All rights reserved.													
/*																		
/* 文件名称： GrabberExport.cpp												
/* 文件标识： 见配置管理计划书
/* 摘要： 导出采集卡类
/*
/* 当前版本： 1.0
/* 作者： 阴同
/* 完成日期： 2013年10月12日
/************************************************************************/

#include "stdafx.h"
#include "DHGrabberForMER.h"
#include "CameraBase.H"

CDHGrabberMER::CDHGrabberMER()
{
	//定义代理类的方法
	m_pGrabber = new CameraBase;
}

CDHGrabberMER::~CDHGrabberMER()
{
	if(m_pGrabber != NULL)
	{
		delete m_pGrabber;
		m_pGrabber = NULL;
	}
}
	
//初始化
BOOL CDHGrabberMER::Init(const s_GBINITSTRUCT* pInitParam)
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->Init(pInitParam));
}

//关闭
BOOL CDHGrabberMER::Close()
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->Close());
}

//开始采集
BOOL CDHGrabberMER::StartGrab()
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->StartGrab());
}

//停止采集
BOOL CDHGrabberMER::StopGrab()
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->StopGrab());
}

// 单帧采集
BOOL CDHGrabberMER::Snapshot()
{
	ASSERT(m_pGrabber != NULL);
	return(((CameraBase *)m_pGrabber)->Snapshot());
}
	
//得到参数
BOOL CDHGrabberMER::GetParamInt(GBParamID Param, int &nOutputVal)
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->GetParamInt(Param, nOutputVal));
}
	
//设置参数
BOOL CDHGrabberMER::SetParamInt(GBParamID Param, int nInputVal)
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->SetParamInt(Param, nInputVal));
}

//调用参数对话框
void CDHGrabberMER::CallParamDialog()
{
	ASSERT(m_pGrabber != NULL);
	m_pGrabber->CallParamDialog();
}

//得到出错信息
void CDHGrabberMER::GetLastErrorInfo(s_GBERRORINFO *pErrorInfo)
{
	ASSERT(m_pGrabber != NULL);
	m_pGrabber->GetLastErrorInfo(pErrorInfo);
}		

BOOL CDHGrabberMER::MERSetParamInt(MERParamID Param, int nInputVal1,int nInputVal2,int nInputVal3,int nInputVal4,int nInputVal5,void *sInputVal6)
{
	ASSERT(m_pGrabber != NULL);

	BOOL bRet = FALSE;	

	bRet = ((CameraBase *)m_pGrabber)->MERSetParamInt(Param, nInputVal1,nInputVal2,nInputVal3,nInputVal4,nInputVal5,sInputVal6);

	return bRet;
}

BOOL CDHGrabberMER::MERGetParamInt(MERParamID Param, int &nOutputVal1,int &nOutputVal2,int &nOutputVal3)
{
	ASSERT(m_pGrabber != NULL);
	
	BOOL bRet = FALSE;
	bRet = ((CameraBase *)m_pGrabber)->MERGetParamInt(Param, nOutputVal1,nOutputVal2,nOutputVal3);


	return bRet;
}

BOOL CDHGrabberMER::MERGetParamString(MERParamID Param, char* sOutputString,int size)
{
	ASSERT(m_pGrabber != NULL);
	BOOL bRet = FALSE;
	bRet = ((CameraBase *)m_pGrabber)->MERGetParamString(Param, sOutputString,size);
	return bRet;
}

int CDHGrabberMER::MERSetOutputParamInt(int nOutputPort,int nOutputVal)
{
	ASSERT(m_pGrabber != NULL);
	BOOL bRet = FALSE;

	if(((CameraBase *)m_pGrabber)->SetOutputValue(nOutputPort,nOutputVal))
	{
		return 0;
	}//发送输出内容
	else
	{
		return 1;
	}
	

//     if(((CHVCamDH *)m_pGrabber)->GetOutputHaveUserSignal(nOutputPort))
// 	{
// 		if(((CHVCamDH *)m_pGrabber)->SetOutputModel(nOutputPort,2))//设置自定义信号模式
// 		{
// 			if(((CHVCamDH *)m_pGrabber)->SetOutputOpen(nOutputPort,1))//打开端口
// 			{
// 				((CHVCamDH *)m_pGrabber)->SetOutputValue(nOutputPort,nOutputVal);//发送输出内容
// 			}
// 			else
// 			{
// 				return 3;
// 			}
// 		}
// 		else
// 		{
// 			return 2;
// 		}
// 	}
// 	else
// 	{
// 		return 1;
// 	}
// 
//     return 0;
}


// 保存相机设置信息
void CDHGrabberMER::SaveParamToINI()
{
	((CameraBase *)m_pGrabber)->SaveParamToINI();
}

//
//////////////////////////////////////////////////////////////////////////
BOOL CDHGrabberMER::MERGetCameraCount(int &nCameraCount)
{
	int nCount;
	bool bRet;
	bRet = ((CameraBase *)m_pGrabber)->GetCameraCount(nCount);
	nCameraCount = nCount;
	return bRet;
}

BOOL CDHGrabberMER::MerGetCameraSN(int nCameraNumber,char sCameraSN[MaxSNLen])
{
	return ((CameraBase *)m_pGrabber)->GetCameraSN(nCameraNumber,sCameraSN);
}