/************************************************************************/
/* Copyright (c) 2006, �й���㼯�ű���ͼ���Ӿ������ֹ�˾   
/* All rights reserved.													
/*																		
/* �ļ����ƣ� GrabberExport.cpp												
/* �ļ���ʶ�� �����ù���ƻ���
/* ժҪ�� �����ɼ�����
/*
/* ��ǰ�汾�� 1.0
/* ���ߣ� ��ͬ
/* ������ڣ� 2013��10��12��
/************************************************************************/

#include "stdafx.h"
#include "DHGrabberForMER.h"
#include "CameraBase.H"

CDHGrabberMER::CDHGrabberMER()
{
	//���������ķ���
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
	
//��ʼ��
BOOL CDHGrabberMER::Init(const s_GBINITSTRUCT* pInitParam)
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->Init(pInitParam));
}

//�ر�
BOOL CDHGrabberMER::Close()
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->Close());
}

//��ʼ�ɼ�
BOOL CDHGrabberMER::StartGrab()
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->StartGrab());
}

//ֹͣ�ɼ�
BOOL CDHGrabberMER::StopGrab()
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->StopGrab());
}

// ��֡�ɼ�
BOOL CDHGrabberMER::Snapshot()
{
	ASSERT(m_pGrabber != NULL);
	return(((CameraBase *)m_pGrabber)->Snapshot());
}
	
//�õ�����
BOOL CDHGrabberMER::GetParamInt(GBParamID Param, int &nOutputVal)
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->GetParamInt(Param, nOutputVal));
}
	
//���ò���
BOOL CDHGrabberMER::SetParamInt(GBParamID Param, int nInputVal)
{
	ASSERT(m_pGrabber != NULL);
	return(m_pGrabber->SetParamInt(Param, nInputVal));
}

//���ò����Ի���
void CDHGrabberMER::CallParamDialog()
{
	ASSERT(m_pGrabber != NULL);
	m_pGrabber->CallParamDialog();
}

//�õ�������Ϣ
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
	}//�����������
	else
	{
		return 1;
	}
	

//     if(((CHVCamDH *)m_pGrabber)->GetOutputHaveUserSignal(nOutputPort))
// 	{
// 		if(((CHVCamDH *)m_pGrabber)->SetOutputModel(nOutputPort,2))//�����Զ����ź�ģʽ
// 		{
// 			if(((CHVCamDH *)m_pGrabber)->SetOutputOpen(nOutputPort,1))//�򿪶˿�
// 			{
// 				((CHVCamDH *)m_pGrabber)->SetOutputValue(nOutputPort,nOutputVal);//�����������
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


// �������������Ϣ
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