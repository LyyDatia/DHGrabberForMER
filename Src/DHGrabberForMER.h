/************************************************************************/
/* Copyright (c) 2013, �й���㼯�ű���ͼ���Ӿ������ֹ�˾�Ӿ�ϵͳ��  
/* All rights reserved.													
/*																		
/* �ļ����ƣ� DHGrabberForMER.h	
/* ժҪ�� �����ɼ����ࣨ��˾ˮ�������

/* ��ǰ�汾��1.0.0.4
/* �޸��ߣ�������
/* ��Ҫ�޸����ݣ�ʵ��֧�ֲü����ͼƬ�Ĺ���,�ṩ�ü�ͼƬ��С��ƫ������,�ṩ��С�ع�ʱ��,ͼƬ�����,���߶ȵȲ����Ĳ�ѯ
/* ������ڣ�2020��4��13��

/* ��ǰ�汾��1.0.0.3
/* �޸��ߣ���ͬ
/* ��Ҫ�޸����ݣ�ʵ�ֻ����ĳ�ʼ���������ɼ���ֹͣ�ɼ�����֡�ɼ��Ͳ������ý��� ���ֺ����滻Ϊ�°�sdk����
/* ������ڣ�2016��7��15��

/************************************************************************/

#include "DHGrabberBase.h"

#if !defined(DHGrabberMER_h_)
#define DHGrabberMER_h_


#ifdef DHGrabberMER_DLL
#define DHGrabberMER_IMPL _declspec(dllexport)
#else
#define DHGrabberMER_IMPL _declspec(dllimport)
#endif

// ˮ�����ר�в���
enum MERParamID
{
    MERGain,					// ����
    MERBlacklevel,			    // �ڵ�ƽ
    MERBalanceRatio,            // ��ƽ�� (δʹ�ã�
    MERDeviceVersion,           // �汾�� (����)
    MERExposurTimeMin,			// ��С�ع�ʱ�� (����)
    MERExposureDelay,           // �ع���ʱ
    MERTriggerDelay,            // ������ʱ
};

//�ɼ���
class DHGrabberMER_IMPL CDHGrabberMER : public CGrabber
{
    //����
public:
    CDHGrabberMER();
    virtual ~CDHGrabberMER();

    //��ʼ��
    BOOL Init(const s_GBINITSTRUCT* pInitParam);

    //�ر�
    BOOL Close();

    //��ʼ�ɼ�
    BOOL StartGrab();

    //ֹͣ�ɼ�
    BOOL StopGrab();

    // ��֡�ɼ� 
    BOOL Snapshot();

    //���ò���
    BOOL SetParamInt(GBParamID Param, int nInputVal);

    //�õ�����
    BOOL GetParamInt(GBParamID Param, int &nOutputVal);

    //��������ü�����
    virtual BOOL SetOutputROI(int nwidth,int nheight, int noffsetX,int noffsetY=0);

    //���ò����Ի���
    void CallParamDialog();

    //�õ�������Ϣ
    void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo);

    //////////////////////////////////////////////////////////////////////////
    // ��˾���ר�нӿ�	
    BOOL MERSetParamInt(MERParamID Param, int nInputVal1,int nInputVal2 = 0,int nInputVal3 = 0,int nInputVal4 = 0,int nInputVal5 = 0,void *sInputVal6 = NULL);
    BOOL MERGetParamInt(MERParamID Param, int &nOutputVal1,int &nOutputVal2,int &nOutputVal3);	
    BOOL MERGetParamString(MERParamID Param, char* sOutputString,int size);	
    int  MERSetOutputParamInt(int nOutputPort,int nOutputVal);//��������Զ���������� 
    //����ֵ 0:�ɹ� 1:��֧���Զ����ź� 2:�����Զ����ź�ʧ�� 3:�򿪶˿�ʧ�� 4:д���ֵʧ�� 
    //��ȡ�������
    static BOOL MERGetCameraCount(int &nCameraCount);
    //����Ż�ȡ������к�,��Ŵ�0��ʼ
    static BOOL MerGetCameraSN(int nCameraNumber,char sCameraSN[MaxSNLen]);

    void SaveParamToINI();		// �������������Ϣ
    
    //
    //////////////////////////////////////////////////////////////////////////
    int CamerColorOrGray;
    //����
protected:
    CGrabber *m_pGrabber;
};

#endif// !defined(DHGrabberMER_h_)