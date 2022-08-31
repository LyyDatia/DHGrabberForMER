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
    MERSnapMode,				// ͼ��ɼ�ģʽ
    MERGain,					// ����
    MERExposure,				// �ع�ʱ��
    MERBlacklevel,			    // �ڵ�ƽ
    MERBalanceRatio,            // ��ƽ��
    MERDeviceVersion,           // �汾��
    MER_INT_WIDTH,              // ���ͼƬ��
    MER_INT_HEIGHT,             // ���ͼƬ��
    MER_INT_OFFSETX,            // ���ͼƬˮƽƫ��
    MER_INT_OFFSETY,            // ���ͼƬ��ֱƫ��
    MERExposurTimeMin,			// ��С�ع�ʱ�� (����)
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
    //���ü��п�
    BOOL MERSetROI(int noffsetX,int noffsetY,int nwidth,int nheight);

    void SaveParamToINI();		// �������������Ϣ
    
    //
    //////////////////////////////////////////////////////////////////////////
    int CamerColorOrGray;
    //����
protected:
    CGrabber *m_pGrabber;
};

#endif// !defined(DHGrabberMER_h_)