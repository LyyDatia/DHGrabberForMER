//--------------------------------------------------------------- 
/**
\file      CameraBase.cpp
\brief     CameraBase function
\version   v1.0.13035.9301
\date      2013-5-30
*/
//--------------------------------------------------------------- 

#include "stdafx.h"
#include "CameraBase.h"

#include <fstream>;
using namespace std;

//#define MERLOG

CameraBase::CameraBase()
{
    m_nImageWidth = 0;
    m_nImageHeight = 0;
    m_nWndWidth = 0;
    m_nWndHeight = 0;
    m_nPayLoadSize = 0;
    m_pShowWnd = NULL;
    m_pDC = NULL;
    m_pLogWnd = NULL;
    m_hDevice = NULL;
    m_chBmpBuf = new char[2048];
    m_bIsOpen = false;
    m_bIsSnaping = false;
    m_bShowImg = true;
    m_bSaveBMP = true;
    m_bSaveAVI = false;
    m_bEnableGetImageProcess = false;
    m_pBmpInfo = NULL;
    m_pImgRGBBuffer = NULL;
    m_pImgRaw8Buffer = NULL;
    m_strBMPFolder = "C:\\test";
    m_strAVIFolder = "";
    m_pAVIFile = NULL;
    m_bCompressed = false;
    m_nTimeFrame = 0;
    m_nTotalNum = 0;
    m_bInitSuccess = false;
    m_nImageByteCount = 1;
    m_nFrameCount = 0;

    m_nWidth = 0;
    m_nHeight = 0;
    m_nOffsetX = 0;
    m_nOffsetY = 0;
    m_nExposureMode = 0;
    m_dExposureTime = 0;
    m_bExposureAuto = false;
    m_nGain = 0;
    m_bGainAuto = false;
    m_nBlacklevel = 0;
    m_bBlacklevelAuto = false;
    m_dBalanceRatio = 0;
    m_bBalanceRatioAuto = false;
    m_bTriggerMode = false;
    m_nTriggerActivation = 0;
    m_pParamSetDlg = NULL;
    m_AcqSpeedLevel = 0;
}

CameraBase::~CameraBase()
{
    if (m_chBmpBuf != NULL)
    {
        delete[]m_chBmpBuf;
        m_chBmpBuf = NULL;
    }
}

// ��ʼ��
BOOL CameraBase::Init(const s_GBINITSTRUCT* pInitParam)
{
    //AfxMessageBox("asd");
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    emStatus = GXInitLib();//��ʼ�����ÿ�------------
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return 1;
    }


    BOOL bRet = false;

    s_DC_INITSTRUCT InitCardParam;
    InitCardParam.nGrabberSN = pInitParam->nGrabberSN;
    //��char�ĺ�����д�뵽ָ�����ַ�����
    sprintf(InitCardParam.strGrabberFile, pInitParam->strGrabberFile);//��ȡ��������ļ�λ��

    sprintf(InitCardParam.strDeviceName, pInitParam->strDeviceName);//��ȡ�豸������
    sprintf(InitCardParam.strDeviceMark, pInitParam->strDeviceMark);//��ȡSN

    InitCardParam.CallBackFunc = pInitParam->CallBackFunc;//���ûص�����
    InitCardParam.Context = pInitParam->Context;//���ص������Ĵ���ָ�봫��
    InitCardParam.iGrabberTypeSN = pInitParam->iGrabberTypeSN;// �ɼ������ͱ�� [10/19/2010 SJC]

    emStatus = Open(&InitCardParam);//��������ã����ҳ�ʼ��

    if (emStatus == GX_STATUS_SUCCESS)
    {
        bRet = true;
    }
    return bRet;
}

//��ʼ�ɼ�
BOOL CameraBase::StartGrab()
{
    StartCallbackAcq();
    return TRUE;
}

//ֹͣ�ɼ�
BOOL CameraBase::StopGrab()
{
    StopCallbackAcq();
    return TRUE;
}

// ��֡�ɼ� 
BOOL CameraBase::Snapshot()
{
    return TRUE;
}

//����ͨ�ò���
BOOL CameraBase::SetParamInt(GBParamID Param, int nReturnVal)
{
    return TRUE;
}

//�õ�ͨ�ò���
BOOL CameraBase::GetParamInt(GBParamID Param, int& nReturnVal)
{
    switch (Param)
    {
    case GBImageWidth:
        nReturnVal = m_nWidth;
        break;
    case GBImageHeight:
        nReturnVal = m_nHeight;
        break;
    case GBImagePixelSize:
        nReturnVal = m_nImageByteCount;
        break;
    case GBImageBufferSize:
        nReturnVal = m_nWidth * m_nHeight * m_nImageByteCount;
        break;
    case GBImageBufferAddr:
        nReturnVal = (int)m_pImgRGBBuffer;
        break;
    default:
        AfxMessageBox("GetParamInt switch(Param)  default:...");
        m_LastErrorInfo.nErrorCode = DCErrorGetParam;
        sprintf(m_LastErrorInfo.strErrorDescription, "�õ���������");
        sprintf(m_LastErrorInfo.strErrorRemark, "GetParamInt()����");

        return FALSE;
    }

    return TRUE;
}

//���ò����Ի���
void CameraBase::CallParamDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_pParamSetDlg != NULL)
    {
        AfxMessageBox("���ƴ����Ѵ�!");
        return;
    }

    m_pParamSetDlg = new CSetParamDialog();
    m_pParamSetDlg->m_pCamera = this;
    m_pParamSetDlg->Create(CSetParamDialog::IDD);
    m_pParamSetDlg->CenterWindow();
    m_pParamSetDlg->ShowWindow(SW_SHOW);

}

// ��ô�����Ϣ
void CameraBase::GetLastErrorInfo(s_GBERRORINFO* pErrorInfo)
{
}

BOOL CameraBase::MERSetParamInt(MERParamID Param, int nInputVal1, int nInputVal2, int nInputVal3, int nInputVal4, int nInputVal5, void* sInputVal6)
{
    bool bRet = false;
    if (m_bIsSnaping)
    {
        StopGrab();
        bRet = true;
    }
    BOOL bReturn = TRUE;
    //test

    fstream ofs;
    string strOut = "";
    GX_STATUS ret = GX_STATUS_ERROR;

    int maxheight, maxwidth, t2, t3;
    maxheight = maxwidth = t2 = t3 = -1;;
    MERGetParamInt(MERImageMaxHeight, maxheight, t2, t3);
    MERGetParamInt(MERImageMaxWidth, maxwidth, t2, t3);
#ifdef MERLOG
    ofs.open("MERlog.txt", ios::out | ios::app);
#endif
    switch (Param)
    {
    case MERSnapMode:
        if (nInputVal1 == 0)
        {
            if (SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF) == GX_STATUS_SUCCESS)
            {
                m_bTriggerMode = false;
            }
            else
            {
                bReturn = FALSE;
            }
        }
        else
        {
            if (SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON) == GX_STATUS_SUCCESS)
            {
                m_bTriggerMode = true;
            }
            else
            {
                bReturn = FALSE;
            }
        }
        break;
    case MERExposure:
        if (SetFloat(GX_FLOAT_EXPOSURE_TIME, (float)nInputVal1) == GX_STATUS_SUCCESS)
        {
            m_dExposureTime = nInputVal1;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERGain:
        GX_FLOAT_RANGE gainRange;
        GetFloatRange(GX_FLOAT_GAIN, &gainRange);
        if (nInputVal1 < gainRange.dMin)
        {
            nInputVal1 = (int)gainRange.dMin;
        }
        else if (nInputVal1 > gainRange.dMax)
        {
            nInputVal1 = (int)gainRange.dMax;
        }
        if (SetFloat(GX_FLOAT_GAIN, (double)nInputVal1) == GX_STATUS_SUCCESS)
        {
            m_nGain = nInputVal1;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERBlacklevel:
        if (SetFloat(GX_FLOAT_BLACKLEVEL, (double)nInputVal1) == GX_STATUS_SUCCESS)
        {
            m_nBlacklevel = nInputVal1;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERImageOffsetHoriz:
        strOut = "MERImageOffsetHoriz";
        if (nInputVal1 > maxwidth)
        {
            bReturn = FALSE;
            break;
        }
        if ((ret = SetInt(GX_INT_OFFSET_X, nInputVal1)) == GX_STATUS_SUCCESS)
        {
            m_nOffsetX = nInputVal1;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERImageOffsetVerti:
        strOut = "MERImageOffsetVerti";
        if (nInputVal1 > maxheight)
        {
            bReturn = FALSE;
            break;
        }
        if ((ret = SetInt(GX_INT_OFFSET_Y, nInputVal1)) == GX_STATUS_SUCCESS)
        {
            m_nOffsetY = nInputVal1;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERCarveImageWidth:
        strOut = "MERCarveImageWidth";
        if (nInputVal1 > maxwidth)
        {
            bReturn = FALSE;
            break;
        }
        if ((ret = SetInt(GX_INT_WIDTH, nInputVal1)) == GX_STATUS_SUCCESS)
        {
            m_nWidth = nInputVal1;
            //m_nImageWidth = nInputVal1;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERCarveImageHeight:
        strOut = "MERCarveImageHeight";
        if (nInputVal1 > maxwidth)
        {
            bReturn = FALSE;
            break;
        }
        if ((ret = SetInt(GX_INT_HEIGHT, nInputVal1)) == GX_STATUS_SUCCESS)
        {
            m_nHeight = nInputVal1;
           // m_nImageHeight = nInputVal1;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    }
    if (ofs.is_open())
    {
        if (strOut != "")
        {
            ofs << "set " << strOut << " MaxH:" << maxheight << " MaxW:" << maxwidth << " Value:" << nInputVal1;
            if (bReturn)
            {
                ofs << " Success" << endl;
            }
            else
            {
                ofs << " Failed ret:" << (int)ret << endl;
            }
        }
        ofs.close();
    }
    if (bRet)
    {
        StartGrab();
    }
    return bReturn;
}

//�õ����ר�в���
BOOL CameraBase::MERGetParamInt(MERParamID Param, int& nReturnVal1, int& nReturnVal2, int& nReturnVal3)
{
    __int64 ntemp;
    double dtemp;
    BOOL bReturn = TRUE;
    switch (Param)
    {
    case MERSnapMode:
        if (GetEnum(GX_ENUM_TRIGGER_MODE, &ntemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = ntemp;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERExposure:
        if (GetFloat(GX_FLOAT_EXPOSURE_TIME, &dtemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = (int)dtemp;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERGain:
        if (GetFloat(GX_FLOAT_GAIN, &dtemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = (int)dtemp;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERBlacklevel:
        if (GetFloat(GX_FLOAT_BLACKLEVEL, &dtemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = (int)dtemp;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERImageOffsetHoriz:
        if (GetInt(GX_INT_OFFSET_X, &ntemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = ntemp;
            if (ntemp != m_nOffsetX)
            {
                m_nOffsetX = ntemp;
            }
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERImageOffsetVerti:
        if (GetInt(GX_INT_OFFSET_Y, &ntemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = ntemp;
            if (ntemp != m_nOffsetY)
            {
                m_nOffsetY = ntemp;
            }
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERCarveImageWidth:
        if (GetInt(GX_INT_WIDTH, &ntemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = ntemp;
            if (ntemp != m_nWidth)
            {
                m_nWidth = ntemp;
            }
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERCarveImageHeight:
        if (GetInt(GX_INT_HEIGHT, &ntemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = ntemp;
            if (ntemp != m_nHeight)
            {
                m_nHeight = ntemp;
            }
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERExposurTimeMin:
        if (GetFloat(GX_FLOAT_AUTO_EXPOSURE_TIME_MIN, &dtemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = dtemp;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERImageMaxWidth:
        if (GetInt(GX_INT_WIDTH_MAX, &ntemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = ntemp;
            if (ntemp != m_nMaxWidth)
            {
                m_nMaxWidth = ntemp;
            }
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    case MERImageMaxHeight:
        if (GetInt(GX_INT_HEIGHT_MAX, &ntemp) == GX_STATUS_SUCCESS)
        {
            nReturnVal1 = ntemp;
            if (ntemp != m_nMaxHeight)
            {
                m_nMaxHeight = ntemp;
            }
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    }

    return bReturn;
}

BOOL CameraBase::MERGetParamString(MERParamID Param, char* sOutputString, int size)
{
    size_t nSize = size;
    BOOL bReturn = TRUE;
    switch (Param)
    {
    case MERDeviceVersion:
        if (GXGetString(m_hDevice, GX_STRING_DEVICE_VERSION, sOutputString, &nSize) == GX_STATUS_SUCCESS)
        {
            bReturn = TRUE;
        }
        else
        {
            bReturn = FALSE;
        }
        break;
    default:
        bReturn = FALSE;
        break;
    }
    return bReturn;
}

//����˿ڷ����ź�
BOOL CameraBase::SetOutputValue(int nOutputPort, int nOutputValue)
{
    return TRUE;
}

//----------------------------------------------------------------------------------
/**
\brief  ���豸
\return ���豸�ɹ��򷵻�GX_STATUS_SUCCESS�����򷵻ش�����
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::Open(const s_DC_INITSTRUCT* pInitParam)
{
    m_nGrabberSN = pInitParam->nGrabberSN + 1;			//��� // ��˾�����Ŵ�1��ʼ,Ϊ�˺������������ݣ��Լ�1 [9/16/2010 SJC]
    m_CallBackFunc = pInitParam->CallBackFunc;			//�ص�����ָ��
    m_Context = pInitParam->Context;				//��ŵ��ó�ʼ�������Ķ����thisָ��

    m_sInitFile = pInitParam->strGrabberFile;		//���ڳ�ʼ�����ļ�????

    m_sDeviceName = pInitParam->strDeviceName;			//�ɼ��豸��
    m_strDeviceMark.Format("%s", pInitParam->strDeviceMark);
    m_nGrabberType = pInitParam->iGrabberTypeSN;		// �ɼ�������

    CFileFind fileSerch;
    if (!fileSerch.FindFile(m_sInitFile))
    {
        m_LastErrorInfo.nErrorCode = DCErrorOpenCam;
        sprintf(m_LastErrorInfo.strErrorDescription, "��ʼ���ļ�������!");
        sprintf(m_LastErrorInfo.strErrorRemark, "���֤�豸��ʼ���ļ����Ƿ���ȷ");
        return GX_STATUS_ERROR;
    }


    InitParamFromINI();// ��ȡ��������ļ������������ļ�


    //////////////////test
    // 	bool bRet;
    // 	int nCameraCount;
    // 	char sCameraSN[MaxSNLen];
    // 	bRet = GetCameraCount(nCameraCount);
    // 	for (int i=0;i<nCameraCount;i++)
    // 	{
    // 		bRet = GetCameraSN(i,sCameraSN);
    // 	}
    //////////////////////

    GX_STATUS emStatus = GX_STATUS_SUCCESS;

    try
    {
        //ö���豸��������ʱʱ��1000ms[��ʱʱ���û��������ã�û���Ƽ�ֵ]
        uint32_t nDeviceNum = 0;
        emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);//��ȡ�豸����
        m_nTotalNum = nDeviceNum;

        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }
        if (m_nTotalNum <= 0)
        {
            return GX_STATUS_ERROR;
        }


        //�������кŴ����
        GX_OPEN_PARAM openParam;
        openParam.openMode = GX_OPEN_SN;
        openParam.accessMode = GX_ACCESS_CONTROL;
        openParam.pszContent = (LPSTR)(LPCTSTR)m_strDeviceMark;
        emStatus = GXOpenDevice(&openParam, &m_hDevice);//����ָ���ĵ�ǰ�豸

        if (emStatus != GX_STATUS_SUCCESS)
        {
            char error[255];
            size_t size = 255;
            GXGetLastError(&emStatus, error, &size);
            return emStatus;
        }
        m_bIsOpen = true;

        //--------------------------������豸���------------------------------------		
        if (!m_bInitSuccess)
        {
            // ���ò���
            SetInitParam();

            emStatus = __PrepareForShowImg();
            if (emStatus != GX_STATUS_SUCCESS)
            {
                return emStatus;
            }

            if (m_nImageWidth < m_nWidth || m_nImageHeight < m_nHeight)
            {
                m_LastErrorInfo.nErrorCode = DCErrorValOverFlow;
                sprintf(m_LastErrorInfo.strErrorDescription, "��Ȼ��߸߶ȳ����������ֵ");
                sprintf(m_LastErrorInfo.strErrorRemark, "Init()�����е�HVGetDeviceInfo()");
                return GX_STATUS_ERROR;
            }
            m_bInitSuccess = TRUE;
        }
        else
        {
            ////�豸��ʼ��ʱ �ָ�ԭʼ��Ұ
            //MERSetParamInt(MERImageOffsetHoriz, 0);
            //MERSetParamInt(MERImageOffsetVerti, 0);
            //MERSetParamInt(MERCarveImageWidth, m_nMaxWidth);
            //MERSetParamInt(MERCarveImageHeight, m_nMaxHeight);

            m_LastErrorInfo.nErrorCode = DCErrorInit;
            sprintf(m_LastErrorInfo.strErrorDescription, "����Ѿ���ʼ���ɹ�!");
            sprintf(m_LastErrorInfo.strErrorRemark, "����ظ���ʼ��");
            return GX_STATUS_SUCCESS;
        }
    }
    catch (...)
    {
        m_LastErrorInfo.nErrorCode = DCErrorInit;
        sprintf(m_LastErrorInfo.strErrorDescription, "��ʼ��ʧ��");
        sprintf(m_LastErrorInfo.strErrorRemark, "Init()������try��׽�����쳣");
        return GX_STATUS_ERROR;
    }

    return emStatus;
}

//��ȡ�����ļ�
void CameraBase::InitParamFromINI()
{
#ifdef MERLOG
    fstream ofs;
    ofs.open("MERlog.txt", ios::out | ios::app);
    if (ofs.is_open())
    {
        ofs << "DHGrabberForMER Init from INI" << endl;
        ofs.close();
    }
#endif
    try
    {
        m_nMaxWidth = GetPrivateProfileInt("Camera", "Width", 0, m_sInitFile);
        m_nMaxHeight = GetPrivateProfileInt("Camera", "Height", 0, m_sInitFile);
        m_nWidth = GetPrivateProfileInt("Camera", "CarveWidth", 0, m_sInitFile);
        m_nHeight = GetPrivateProfileInt("Camera", "CarveHeight", 0, m_sInitFile);
        m_nOffsetX = GetPrivateProfileInt("Camera", "OffsetX", 0, m_sInitFile);
        m_nOffsetY = GetPrivateProfileInt("Camera", "OffsetY", 0, m_sInitFile);
        m_nExposureMode = GetPrivateProfileInt("Camera", "ExposureMode", 0, m_sInitFile);
        m_dExposureTime = GetPrivateProfileInt("Camera", "ExposureTime", 10000, m_sInitFile);
        m_bExposureAuto = GetPrivateProfileInt("Camera", "ExposureAuto", 0, m_sInitFile);
        m_nGain = GetPrivateProfileInt("Camera", "Gain", 0, m_sInitFile);
        m_bGainAuto = GetPrivateProfileInt("Camera", "GainAuto", 0, m_sInitFile);
        m_nBlacklevel = GetPrivateProfileInt("Camera", "Blacklevel", 0, m_sInitFile);
        m_bBlacklevelAuto = GetPrivateProfileInt("Camera", "BlacklevelAuto", 0, m_sInitFile);
        m_dBalanceRatio = GetPrivateProfileInt("Camera", "BalanceRatio", 0, m_sInitFile);
        m_bBalanceRatioAuto = GetPrivateProfileInt("Camera", "BalanceRatioAuto", 0, m_sInitFile);
        m_bTriggerMode = GetPrivateProfileInt("Camera", "TriggerMode", 0, m_sInitFile);
        m_nTriggerActivation = GetPrivateProfileInt("Camera", "TriggerActivation", 0, m_sInitFile);
        m_AcqSpeedLevel = GetPrivateProfileInt("Camera", "AcqSpeedLevel", 0, m_sInitFile);
    }
    catch (...)
    {
        m_LastErrorInfo.nErrorCode = DCErrorReadIniFile;
        sprintf(m_LastErrorInfo.strErrorDescription, "��ȡ�����ļ�����");
        sprintf(m_LastErrorInfo.strErrorRemark, "InitParamFromINI()����");
    }
}

//�������������Ϣ
void CameraBase::SaveParamToINI()
{
#ifdef MERLOG
    fstream ofs;
    ofs.open("MERlog.txt", ios::out | ios::app);
    if (ofs.is_open())
    {
        ofs << "DHGrabberForMER Save To INI" << endl;
        ofs.close();
    }
#endif
    CString str;
    str.Format("%d", m_nMaxWidth);
    WritePrivateProfileString("Camera", "Width", str, m_sInitFile);
    str.Format("%d", m_nMaxHeight);
    WritePrivateProfileString("Camera", "Height", str, m_sInitFile);
    str.Format("%d", m_nWidth);
    WritePrivateProfileString("Camera", "CarveWidth", str, m_sInitFile);
    str.Format("%d", m_nHeight);
    WritePrivateProfileString("Camera", "CarveHeight", str, m_sInitFile);
    str.Format("%d", m_nOffsetX);
    WritePrivateProfileString("Camera", "OffsetX", str, m_sInitFile);
    str.Format("%d", m_nOffsetY);
    WritePrivateProfileString("Camera", "OffsetY", str, m_sInitFile);
    str.Format("%d", m_nExposureMode);
    WritePrivateProfileString("Camera", "ExposureMode", str, m_sInitFile);
    str.Format("%d", (int)m_dExposureTime);
    WritePrivateProfileString("Camera", "ExposureTime", str, m_sInitFile);
    str.Format("%d", (int)m_bExposureAuto);
    WritePrivateProfileString("Camera", "ExposureAuto", str, m_sInitFile);
    str.Format("%d", m_nGain);
    WritePrivateProfileString("Camera", "Gain", str, m_sInitFile);
    str.Format("%d", (int)m_bGainAuto);
    WritePrivateProfileString("Camera", "GainAuto", str, m_sInitFile);
    str.Format("%d", m_nBlacklevel);
    WritePrivateProfileString("Camera", "Blacklevel", str, m_sInitFile);
    str.Format("%d", (int)m_bBlacklevelAuto);
    WritePrivateProfileString("Camera", "BlacklevelAuto", str, m_sInitFile);
    str.Format("%lf", m_dBalanceRatio);
    WritePrivateProfileString("Camera", "BalanceRatio", str, m_sInitFile);
    str.Format("%d", (int)m_bBalanceRatioAuto);
    WritePrivateProfileString("Camera", "BalanceRatioAuto", str, m_sInitFile);
    str.Format("%d", (int)m_bTriggerMode);
    WritePrivateProfileString("Camera", "TriggerMode", str, m_sInitFile);
    str.Format("%d", m_nTriggerActivation);
    WritePrivateProfileString("Camera", "TriggerActivation", str, m_sInitFile);
    str.Format("%d", m_AcqSpeedLevel);
    WritePrivateProfileString("Camera", "AcqSpeedLevel", str, m_sInitFile);
}

void CameraBase::SetInitParam()
{
    //���ø���Ȥ����
    GX_STATUS status = GX_STATUS_SUCCESS;
    if (m_nWidth == 0 || m_nHeight == 0)
    {
        status = GetInt(GX_INT_WIDTH_MAX, &m_nMaxWidth);
        status = GetInt(GX_INT_HEIGHT_MAX, &m_nMaxHeight);
    }
    status = SetInt(GX_INT_WIDTH, m_nWidth);
    status = SetInt(GX_INT_HEIGHT, m_nHeight);
    status = SetInt(GX_INT_OFFSET_X, m_nOffsetX);
    status = SetInt(GX_INT_OFFSET_Y, m_nOffsetY);


    status = SetEnum(GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    status = SetEnum(GX_ENUM_GAIN_AUTO, GX_GAIN_AUTO_CONTINUOUS);
    //status=SetEnum(GX_ENUM_PIXEL_FORMAT,mono)

    //���òɼ��ٶȼ���
    status = SetInt(GX_INT_ACQUISITION_SPEED_LEVEL, m_AcqSpeedLevel);

    //���ô���Դ
    status = SetEnum(GX_ENUM_TRIGGER_SOURCE, GX_TRIGGER_SOURCE_LINE0);

    //���ô���ģʽ
    if (m_bTriggerMode)
    {
        status = SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
    }
    else
    {
        status = SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
    }
    //--------------------------------------------------------------------------------------------------

    //���ô������ʽ
    switch (m_nTriggerActivation)
    {
    case 0:
        status = SetEnum(GX_ENUM_TRIGGER_ACTIVATION, GX_TRIGGER_ACTIVATION_RISINGEDGE);
        break;
    case 1:
        status = SetEnum(GX_ENUM_TRIGGER_ACTIVATION, GX_TRIGGER_ACTIVATION_FALLINGEDGE);
        break;
    }


    //�����ع�ģʽ
    switch (m_nExposureMode)
    {
    case 1:
        status = SetEnum(GX_ENUM_EXPOSURE_MODE, GX_EXPOSURE_MODE_TIMED);
        break;
    case 2:
        status = SetEnum(GX_ENUM_EXPOSURE_MODE, GX_EXPOSURE_MODE_TRIGGERWIDTH);
        break;
    default:
        status = SetEnum(GX_ENUM_EXPOSURE_MODE, GX_EXPOSURE_MODE_TIMED);
        break;
    }
    //�����Զ��ع�
    if (m_bExposureAuto)
    {
        status = SetEnum(GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_CONTINUOUS);
    }
    else
    {
        status = SetEnum(GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_OFF);
        //�����ع�ʱ��
        status = SetFloat(GX_FLOAT_EXPOSURE_TIME, m_dExposureTime);
    }

    //�����Զ�����
    if (m_bGainAuto)
    {
        status = SetEnum(GX_ENUM_GAIN_AUTO, GX_GAIN_AUTO_CONTINUOUS);
    }
    else
    {
        status = SetEnum(GX_ENUM_GAIN_AUTO, GX_GAIN_AUTO_OFF);
        GX_FLOAT_RANGE gainRange;
        GetFloatRange(GX_FLOAT_GAIN, &gainRange);
        if (m_nGain < gainRange.dMin)
        {
            m_nGain = gainRange.dMin;
        }
        else if (m_nGain > gainRange.dMax)
        {
            m_nGain = gainRange.dMax;
        }
        //��������
        status = SetFloat(GX_FLOAT_GAIN, (double)m_nGain);
    }

    if (m_bBlacklevelAuto)
    {
        status = SetEnum(GX_ENUM_BLACKLEVEL_AUTO, GX_BLACKLEVEL_AUTO_CONTINUOUS);
    }
    else
    {
        status = SetEnum(GX_ENUM_BLACKLEVEL_AUTO, GX_BLACKLEVEL_AUTO_OFF);
        //���úڵ�ƽ
        status = SetFloat(GX_FLOAT_BLACKLEVEL, (double)m_nBlacklevel);
    }

    //------------------------------------------------------------------------------------------------------

    // 	//�����Զ���ƽ��
    // 	if (m_bBalanceRatioAuto)
    // 	{
    // 		status = SetEnum(GX_ENUM_BALANCE_WHITE_AUTO,GX_BALANCE_WHITE_AUTO_CONTINUOUS);
    // 	}
    // 	else
    // 	{
    // 		status = SetEnum(GX_ENUM_BALANCE_WHITE_AUTO,GX_BALANCE_WHITE_AUTO_OFF);
    // 		//���ð�ƽ��
    //      	status = SetFloat(GX_FLOAT_BALANCE_RATIO,m_dBalanceRatio);
    // 	}


    //���������������״̬ line0Ϊtrigger���� line1Ϊstrobe���

    //����ѡ��ΪLine0
    status = SetEnum(GX_ENUM_LINE_SELECTOR, GX_ENUM_LINE_SELECTOR_LINE0);
    //�������ŷ���Ϊ���
    status = SetEnum(GX_ENUM_LINE_MODE, GX_ENUM_LINE_MODE_INPUT);

    //����ѡ��ΪLine1
    status = SetEnum(GX_ENUM_LINE_SELECTOR, GX_ENUM_LINE_SELECTOR_LINE1);
    //�������ŷ���Ϊ���
    status = SetEnum(GX_ENUM_LINE_MODE, GX_ENUM_LINE_MODE_OUTPUT);
    //��ѡ�������ŵ�ƽ��ת
    //emStatus = SetEnum(GX_BOOL_LINE_INVERTER, true);
    //�������ԴΪ�����
    status = SetEnum(GX_ENUM_LINE_SOURCE, GX_ENUM_LINE_SOURCE_STROBE);



}

//----------------------------------------------------------------------------------
/**
\brief  �ر��豸
\return �ر��豸�ɹ��򷵻�GX_STATUS_SUCCESS�����򷵻ش�����
*/
//----------------------------------------------------------------------------------
BOOL CameraBase::Close()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    BOOL bRet = FALSE;
    if (IsOpen())
    {
        if (IsSnaping())
        {
            emStatus = StopCallbackAcq();
            if (emStatus != GX_STATUS_SUCCESS)
            {
                return FALSE;
            }
        }

        __UnPrepareForShowImg();

        emStatus = GXCloseDevice(m_hDevice);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            return FALSE;
        }
        bRet = TRUE;
        m_bIsOpen = false;
        SaveParamToINI();
    }
    return bRet;
}

//----------------------------------------------------------------------------------
/**
\brief  �豸��״̬
\return �豸���򷵻�true�����򷵻�false
*/
//----------------------------------------------------------------------------------
bool CameraBase::IsOpen()
{
    return m_bIsOpen;
}



//----------------------------------------------------------------------------------
/**
\brief  ע��ͼ����ʾ����
\param  pWnd  ָ��ͼ����ʾ����ָ��
\return ͼ����ʾ����ָ�벻ΪNULL������GX_STATUS_SUCCESS�����򷵻�GX_STATUS_INVALID_PARAMETER
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::RegisterShowWnd(void* pWnd)
{
    GX_STATUS emStatus = GX_STATUS_SUCCESS;

    if (pWnd == NULL)
    {
        return GX_STATUS_INVALID_PARAMETER;
    }

    m_pShowWnd = pWnd;

    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ע����־��ʾ����
\param  pWnd  ָ����־��ʾ����ָ��
\return ��־��ʾ����ָ�벻ΪNULL������GX_STATUS_SUCCESS�����򷵻�GX_STATUS_INVALID_PARAMETER
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::RegisterLogWnd(void* pWnd)
{
    GX_STATUS emStatus = GX_STATUS_SUCCESS;

    if (pWnd == NULL)
    {
        return GX_STATUS_INVALID_PARAMETER;
    }

    m_pLogWnd = pWnd;

    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ʼע��ص�
\return ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::StartCallbackAcq()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;

    if (IsOpen())
    {
        emStatus = GXRegisterCaptureCallback(m_hDevice, this, __OnFrameCallbackFun);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            __UnPrepareForShowImg();
            return emStatus;
        }
        //	__PrintInfotoWnd(MsgComposer("ע��ɼ��ص�����"));
        emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_START);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            __UnPrepareForShowImg();
            return emStatus;
        }
        //	__PrintInfotoWnd(MsgComposer("���Ϳ�������"));
        m_bIsSnaping = true;
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ֹͣע��ص�
\return ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::StopCallbackAcq()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen() && IsSnaping())
    {
        //�����ǰ����¼����Ƶ������ֹͣ¼��
        if (m_bSaveAVI)
        {
            EnableSaveAVI(false);
        }

        emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }
        //__PrintInfotoWnd(MsgComposer("����ͣ������"));
        emStatus = GXUnregisterCaptureCallback(m_hDevice);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }
        //__PrintInfotoWnd(MsgComposer("ע���ɼ��ص�����"));

        m_bIsSnaping = false;
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ʹ����ʾͼ����
\param  bShowImg  true��ʾ��ʾͼ��false��ʾ����ʾͼ��
\return ��
*/
//----------------------------------------------------------------------------------
void CameraBase::EnableShowImg(bool bShowImg)
{
    m_bShowImg = bShowImg;
}

//----------------------------------------------------------------------------------
/**
\brief  ʹ�ܱ���BMP����
\param  bSaveBMP  true��ʾ���棻false��ʾ������
\return ��
*/
//----------------------------------------------------------------------------------
void CameraBase::EnableSaveBMP(bool bSaveBMP)
{
    m_bSaveBMP = bSaveBMP;
}

void CameraBase::CompressedAVI(bool bCompressed)
{
    m_bCompressed = bCompressed;
}
//----------------------------------------------------------------------------------
/**
\brief  ʹ�ܱ���AVI����
\param  bSaveAVI  true��ʾ���棻false��ʾ������
\return ��
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::EnableSaveAVI(bool bSaveAVI)
{
    //	m_SaverCriSec.Enter();
    // 	m_bSaveAVI = bSaveAVI;
    // 
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    // 	if (m_bSaveAVI)
    // 	{
    // 		emStatus = __PrepareForSaveAVI();
    // 		if (emStatus != GX_STATUS_SUCCESS)
    // 		{
    // 			m_SaverCriSec.Leave();
    // 			__UnPrepareForSaveAVI();
    // 			return emStatus;
    // 		}
    // 	}
    // 	else
    // 	{
    // 		__UnPrepareForSaveAVI();
    // 	}
    // 	m_SaverCriSec.Leave();
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ���ñ���BMPͼƬ���ļ���·��
\param  pBMPFolder  ·����β������б��
\return ��
*/
//----------------------------------------------------------------------------------
void CameraBase::SetBMPFolder(char* pBMPFolder)
{
    m_strBMPFolder = string(pBMPFolder);
}

//----------------------------------------------------------------------------------
/**
\brief  ���ñ���AVI��Ƶ���ļ���·��
\param  szAVIFolder  ·����β������б��
\return ��
*/
//----------------------------------------------------------------------------------
void CameraBase::SetAVIFolder(char* pAVIFolder)
{
    m_strAVIFolder = string(pAVIFolder);
}

//----------------------------------------------------------------------------------
/**
\brief  �豸��ʼ�ɼ�״̬
\return �豸��ʼ�ɼ��򷵻�true�����򷵻�false
*/
//----------------------------------------------------------------------------------
bool CameraBase::IsSnaping()
{
    return m_bIsSnaping;
}

GX_STATUS CameraBase::IsImplemented(GX_FEATURE_ID emFeatureID, bool* pbIsImplemented)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXIsImplemented(m_hDevice, emFeatureID, pbIsImplemented);
        //��ӡ��־��Ϣ
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXIsImplemented ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "IsImplemented()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡ����������
\param  emFeatureID  ������ID
\param  pszName    ָ�򷵻ع��������Ƶ�ָ��
\param  pnSize     ��������ַ�������
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetFeatureName(GX_FEATURE_ID emFeatureID, char* pszName, size_t* pnSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetFeatureName(m_hDevice, emFeatureID, pszName, pnSize);
        //��ӡ��־��Ϣ
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetFeatureName ��������:%s ������:%d", pszName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetFeatureName()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡInt���Ͳ�����Сֵ�����ֵ������
\param  emFeatureID  ������ID
\param  pIntRange  ָ�򷵻�int�ͽṹ���ָ��
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetIntRange(GX_FEATURE_ID emFeatureID, GX_INT_RANGE* pIntRange)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetIntRange(m_hDevice, emFeatureID, pIntRange);
        //��ӡ��־��Ϣ
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetIntRange ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetIntRange()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡint�Ͳ�����ǰֵ
\param  emFeatureID  ������ID
\param  pnValue    ָ�򷵻ص�ǰֵ��ָ��
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetInt(GX_FEATURE_ID emFeatureID, int64_t* pnValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetInt(m_hDevice, emFeatureID, pnValue);
        //��ӡ��־��Ϣ
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetInt ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetInt()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ����Int����ֵ���ܲ���
\param  emFeatureID  ������ID
\param  pnValue    �û���Ҫ���õ�ֵ
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SetInt(GX_FEATURE_ID emFeatureID, int64_t nValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSetInt(m_hDevice, emFeatureID, nValue);
        //��ӡ��־��Ϣ
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXSetInt ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetInt()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡFloat���Ͳ�������Сֵ�����ֵ����������λ��������Ϣ
\param  emFeatureID    ������ID
\param  pFloatRange  ָ�򷵻�Float���ͽṹ���ָ��
\return              ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetFloatRange(GX_FEATURE_ID emFeatureID, GX_FLOAT_RANGE* pFloatRange)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetFloatRange(m_hDevice, emFeatureID, pFloatRange);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        emStatus = GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetFloatRange ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetFloatRange()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡFloat���͵�ǰ����ֵ
\param  emFeatureID    ������ID
\param  pdValue      ָ�򷵻�Float���Ͳ���ֵ��ָ��
\return              ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetFloat(GX_FEATURE_ID emFeatureID, double* pdValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetFloat(m_hDevice, emFeatureID, pdValue);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetFloat ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetFloat()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ����Float���Ͳ���ֵ
\param  emFeatureID    ������ID
\param  dValue       �û���Ҫ���õ�ֵ
\return              ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SetFloat(GX_FEATURE_ID emFeatureID, double dValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSetFloat(m_hDevice, emFeatureID, dValue);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXSetFloat ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetFloat()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡö����Ŀ�ѡ�����
\param  emFeatureID    ������ID
\param  pnEntryNums  ָ�򷵻ؿ�ѡ�������ָ��
\return              ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetEnumEntryNums(GX_FEATURE_ID emFeatureID, uint32_t* pnEntryNums)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetEnumEntryNums(m_hDevice, emFeatureID, pnEntryNums);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetEnumEntryNums ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetEnumEntryNums()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡö������ֵ��������Ϣ
\param  emFeatureID         ������ID
\param  pEnumDescription  ָ��ö�����������Ϣ������ָ��
\param  pBufferSize       ָ������ö����������Ϣ�������С
\return                   ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetEnumDescription(GX_FEATURE_ID emFeatureID, GX_ENUM_DESCRIPTION* pEnumDescription, size_t* pBufferSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetEnumDescription(m_hDevice, emFeatureID, pEnumDescription, pBufferSize);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetEnumDescription ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetEnumDescription()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡ��ǰö��ֵ
\param  emFeatureID  ������ID
\param  pnValue    ָ�򷵻ص�ö��ֵ��ָ��
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetEnum(GX_FEATURE_ID emFeatureID, int64_t* pnValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetEnum(m_hDevice, emFeatureID, pnValue);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetEnum ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetEnum()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ����ö��ֵ
\param  emFeatureID  ������ID
\param  nValue     �û���Ҫ���õ�ö��ֵ
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SetEnum(GX_FEATURE_ID emFeatureID, int64_t nValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSetEnum(m_hDevice, emFeatureID, nValue);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetEnum ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetEnum()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡbool����ֵ
\param  emFeatureID  ������ID
\param  nValue     ָ�򷵻صĲ���ֵ��ָ��
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetBool(GX_FEATURE_ID emFeatureID, bool* pbValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetBool(m_hDevice, emFeatureID, pbValue);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetBool ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetBool()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ����bool����ֵ
\param  emFeatureID  ������ID
\param  bValue     �û���Ҫ���õĲ���ֵ
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SetBool(GX_FEATURE_ID emFeatureID, bool bValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSetBool(m_hDevice, emFeatureID, bValue);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXSetBool ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetBool()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡ�ַ�������ֵ�ĳ���
\param  emFeatureID  ������ID
\param  bValue     ָ�򷵻صĳ���ֵ��ָ�룬����ֵ��ĩλ'\0'�����ȵ�λ�ֽ�
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetStringLength(GX_FEATURE_ID emFeatureID, size_t* pnSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetStringLength(m_hDevice, emFeatureID, pnSize);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetStringLength ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetStringLength()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡ�ַ�������ֵ������
\param  emFeatureID  ������ID
\param  pszContent ָ���û�������ַ��������ַ
\param  pnSize     ��ʾ�û�������ַ�����������ַ�ĳ���
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetString(GX_FEATURE_ID emFeatureID, char* pszContent, size_t* pnSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetString(m_hDevice, emFeatureID, pszContent, pnSize);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetString ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetString()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  �����ַ���ֵ����
\param  emFeatureID  ������ID
\param  pszContent ָ���û���Ҫ���õ��ַ�����ַ
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SetString(GX_FEATURE_ID emFeatureID, char* pszContent)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSetString(m_hDevice, emFeatureID, pszContent);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXSetString ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetString()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡ������ݳ���
\param  emFeatureID  ������ID
\param  pnSize     ָ�򷵻صĳ���ֵ��ָ�룬���ȵ�λ�ֽ�
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetBufferLength(GX_FEATURE_ID emFeatureID, size_t* pnSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetBufferLength(m_hDevice, emFeatureID, pnSize);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetBufferLength ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetBufferLength()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ��ȡ�������
\param  emFeatureID  ������ID
\param  pBuffer    ָ���û�����Ŀ�������ڴ��ַָ��
\param  pnSize     ��ʾ�û�����Ļ�������ַ�ĳ���
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetBuffer(GX_FEATURE_ID emFeatureID, uint8_t* pBuffer, size_t* pnSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetBuffer(m_hDevice, emFeatureID, pBuffer, pnSize);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXGetBuffer ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetBuffer()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ���ÿ������
\param  emFeatureID  ������ID
\param  pBuffer    ָ���û�����Ŀ�������ڴ��ַָ��
\param  nSize      �û�����Ļ�������ַ�ĳ���
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SetBuffer(GX_FEATURE_ID emFeatureID, uint8_t* pBuffer, size_t nSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSetBuffer(m_hDevice, emFeatureID, pBuffer, nSize);
        char chFeatureName[64] = { 0 };
        size_t nSizeValue = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSizeValue);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("ʧ��:GXSetBuffer ��������:%s ������:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetBuffer()����");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ����������
\param  emFeatureID  ������ID
\return            ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SendCommand(GX_FEATURE_ID emFeatureID)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSendCommand(m_hDevice, emFeatureID);
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ���Ϳ��������,�˽ӿڿ���ֱ�ӻ�ȡͼ��ע��˽ӿڲ�����ص��ɼ���ʽ���ã�
\param  pFrameData  ָ���û��������������ͼ�����ݵĵ�ַָ��
\param  nTimeout    ȡͼ�ĳ�ʱʱ��
\return             ���ӿڵ��óɹ����򷵻�GX_STATUS_SUCCESS�����򷵻�GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetImage(GX_FRAME_DATA* pFrameData, int32_t nTimeout)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    // 	if (IsOpen())
    // 	{
    // 		emStatus = GXGetImage(m_hDevice, pFrameData, nTimeout);
    // 		if (emStatus != GX_STATUS_SUCCESS)
    // 		{
    // 			__PrintInfotoWnd(MsgComposer("ʧ��:GXGetImage ������:%d", emStatus));
    // 		}
    // 		else
    // 		{
    // 			if (m_bEnableGetImageProcess)
    // 			{
    // 				//���Ƚ��ص��������ͼ�����ݣ������RGB���ݣ��Ա��������ʾ�ʹ洢
    // 				__ProcessData((BYTE*)pFrameData->pImgBuf,m_pImgRGBBuffer, m_nImageWidth, m_nImageHeight);
    // 				//��ָ��������һ֡һ֡�Ļ�ͼ
    // 				__DrawImg(m_pImgRGBBuffer, m_nImageWidth, m_nImageHeight);
    // 				//��ָ���洢Ŀ¼��һ֡һ֡�Ĵ洢bmpͼ
    // 				__SaveBMP(m_pImgRGBBuffer, m_nImageWidth, m_nImageHeight);
    // 			}
    // 		}
    // 		
    // 	}
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  ʹ��GetImage�ӿڵ��ڲ�������̣�����ͼƬ��Ϣ����ʾ������
\param  bEnableGetImageProcess  true��ʾʹ�ܣ�false��ʾ����
\return ��
*/
//----------------------------------------------------------------------------------
void CameraBase::EnableGetImageProcess(bool bEnableGetImageProcess)
{
    m_bEnableGetImageProcess = bEnableGetImageProcess;
}



//----------------------------------------------------------------------------------
/**
\brief  Ϊͼ����ʾ׼����Դ
\return �ɹ�����GX_STATUS_SUCCESS;���򷵻���Ӧ�����룬�����ѯGX_STATUS_LIST
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::__PrepareForShowImg()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    //��ȡͼ�����ݴ�С
    emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &m_nPayLoadSize);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return emStatus;
    }
    //���ͼ���ʽ
    emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, &m_nPixelFormat);//?????????????????

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return emStatus;
    }
    switch (m_nPixelFormat)
    {
    case GX_PIXEL_FORMAT_MONO8:
    case GX_PIXEL_FORMAT_MONO10:
    case GX_PIXEL_FORMAT_MONO12:
        m_nImageByteCount = 1;
        break;
    default:
        m_nImageByteCount = 3;
        break;
    }
    //��ѯ��ǰ����Ƿ�֧��GX_ENUM_PIXEL_COLOR_FILTER
    bool bIsImplemented = false;
    GXIsImplemented(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &bIsImplemented);
    if (bIsImplemented)
    {
        emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &m_nPixelColorFilter);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }
    }

    //��ȡ���
    emStatus = GXGetInt(m_hDevice, GX_INT_WIDTH, &m_nImageWidth);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return emStatus;
    }
    //��ȡ�߶�
    emStatus = GXGetInt(m_hDevice, GX_INT_HEIGHT, &m_nImageHeight);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return emStatus;
    }
    if ((m_nImageWidth != 0) && (m_nImageHeight != 0))
    {
        GX_STATUS ret = GX_STATUS_ERROR;
        if ((ret = __PrepareImgSaveArea()) != GX_STATUS_SUCCESS)
        {
            return ret;
        }
    }
    else
    {
        return GX_STATUS_ERROR;
    }

    return emStatus;
}


//----------------------------------------------------------------------------------
/**
\brief  �ͷ�Ϊͼ����ʾ׼����Դ
\return �޷���ֵ
*/
//----------------------------------------------------------------------------------
void CameraBase::__UnPrepareForShowImg()
{
    if (m_pImgRGBBuffer != NULL)
    {
        delete[]m_pImgRGBBuffer;
        m_pImgRGBBuffer = NULL;
    }
    if (m_pImgRaw8Buffer != NULL)
    {
        delete[]m_pImgRaw8Buffer;
        m_pImgRaw8Buffer = NULL;
    }
}

//----------------------------------------------------------------------------------
/**
\brief  ����������ԭʼ����ת��ΪRGB����
\param  pImageBuf  ָ��ͼ�񻺳�����ָ��
\param  pImageRGBBuf  ָ��RGB���ݻ�������ָ��
\param  nImageWidth ͼ���
\param  nImageHeight ͼ���
\return �޷���ֵ
*/
//----------------------------------------------------------------------------------
void CameraBase::__ProcessData(BYTE* pImageBuf, BYTE* pImageRGBBuf, int64_t nImageWidth, int64_t nImageHeight)
{
    switch (m_nPixelFormat)
    {
        //�����ݸ�ʽΪ12λʱ��λ��ת��Ϊ4-11
    case GX_PIXEL_FORMAT_BAYER_GR12:
    case GX_PIXEL_FORMAT_BAYER_RG12:
    case GX_PIXEL_FORMAT_BAYER_GB12:
    case GX_PIXEL_FORMAT_BAYER_BG12:
        //��12λ��ʽ��ͼ��ת��Ϊ8λ��ʽ
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_4_11);

        //��Raw8ͼ��ת��ΪRGBͼ���Թ���ʾ
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(m_nPixelColorFilter), TRUE);
        break;

        //�����ݸ�ʽΪ12λʱ��λ��ת��Ϊ2-9
    case GX_PIXEL_FORMAT_BAYER_GR10:
    case GX_PIXEL_FORMAT_BAYER_RG10:
    case GX_PIXEL_FORMAT_BAYER_GB10:
    case GX_PIXEL_FORMAT_BAYER_BG10:
        ////��12λ��ʽ��ͼ��ת��Ϊ8λ��ʽ,��Чλ��2-9
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_2_9);

        //��Raw8ͼ��ת��ΪRGBͼ���Թ���ʾ
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(m_nPixelColorFilter), TRUE);
        break;

    case GX_PIXEL_FORMAT_BAYER_GR8:
    case GX_PIXEL_FORMAT_BAYER_RG8:
    case GX_PIXEL_FORMAT_BAYER_GB8:
    case GX_PIXEL_FORMAT_BAYER_BG8:
        //��Raw8ͼ��ת��ΪRGBͼ���Թ���ʾ
        DxRaw8toRGB24(pImageBuf, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(m_nPixelColorFilter), TRUE);
        break;

    case GX_PIXEL_FORMAT_MONO12:
        //��12λ��ʽ��ͼ��ת��Ϊ8λ��ʽ
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_4_11);
        //��Raw8ͼ��ת��ΪRGBͼ���Թ���ʾ
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(NONE), TRUE);
        break;

    case GX_PIXEL_FORMAT_MONO10:
        //��10λ��ʽ��ͼ��ת��Ϊ8λ��ʽ
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_4_11);
        //��Raw8ͼ��ת��ΪRGBͼ���Թ���ʾ
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(NONE), TRUE);
        break;

    case GX_PIXEL_FORMAT_MONO8:
        //��Raw8ͼ��ת��ΪRGBͼ���Թ���ʾ
        DxRaw8toRGB24(pImageBuf, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(NONE), TRUE);


    default:
        break;
    }
}


//----------------------------------------------------------------------------------
/**
\brief  �ص�����
\param  pFrame  ָ��ɼ��ص������ݽṹָ��
\return �޷���ֵ
*/
//----------------------------------------------------------------------------------
void __stdcall CameraBase::__OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame)
{
    CameraBase* pCamera = (CameraBase*)(pFrame->pUserParam);
    if (pFrame->status == 0)
    {
        BOOL bRet = FALSE;
        try
        {
            //���Ƚ��ص��������ͼ�����ݣ������RGB���ݣ��Ա��������ʾ�ʹ洢
#ifdef MERLOG
            fstream ofs;
            ofs.open("MERlog.txt", ios::out | ios::app);
            if (ofs.is_open())
            {
                ofs << "DHGrabberForMER Callback Width:" << pFrame->nWidth << " Height:" << pFrame->nHeight
                    << " PixSize:" << pFrame->nImgSize << endl;
                ofs.close();
            }
#endif
            //check image save area and may renew save area
            if (pCamera->m_nImageWidth != pFrame->nWidth || pCamera->m_nImageHeight != pFrame->nHeight)
            {
                pCamera->m_nImageWidth = pFrame->nWidth;
                pCamera->m_nImageHeight = pFrame->nHeight;
                pCamera->__PrepareImgSaveArea();
            }

            if (pCamera->m_nImageByteCount == 3)
            {
                pCamera->__ProcessData((BYTE*)pFrame->pImgBuf, pCamera->m_pImgRGBBuffer, pCamera->m_nImageWidth, pCamera->m_nImageHeight);

            }
            if (pCamera->m_nImageByteCount == 1)
            {
                memcpy(pCamera->m_pImgRGBBuffer, (BYTE*)pFrame->pImgBuf, pCamera->m_nImageWidth * pCamera->m_nImageHeight);

            }

            //�����ݽ��д�ֱ������
            //int i=0;

            //	break;for( i=0;i<pCamera->m_nImageHeight;i++)

            //memcpy(pCamera->m_pImgRGBBuffer+pCamera->m_nImageWidth*i,(BYTE*)pFrame->pImgBuf+pCamera->m_nImageWidth*(pCamera->m_nImageHeight-i-1), pCamera->m_nImageWidth);
            //memcpy(pCamera->m_pImgRGBBuffer,(BYTE*)pFrame->pImgBuf, pCamera->m_nImageWidth*pCamera->m_nImageHeight);

            //pCamera->__SaveBMP(pCamera->m_pImgRGBBuffer, pCamera->m_nImageWidth, pCamera->m_nImageHeight);
        }
        catch (...)
        {
            bRet = TRUE;
            pCamera->m_LastErrorInfo.nErrorCode = DCErrorSendBufAdd;
            sprintf(pCamera->m_LastErrorInfo.strErrorDescription, "����ͼ���ڴ��ַʧ��");
            sprintf(pCamera->m_LastErrorInfo.strErrorRemark, "__OnFrameCallbackFun()����");
        }

        s_GBSIGNALINFO GrabInfo;
        GrabInfo.Context = pCamera->m_Context;
        GrabInfo.iGrabberTypeSN = pCamera->m_nGrabberType; // �ɼ������ͱ��
        GrabInfo.nGrabberSN = pCamera->m_nGrabberSN - 1; // �����Ŵ�0��ʼ,��֤���������һ��
        GrabInfo.nErrorCode = GBOK;
        if (bRet)
        {
            GrabInfo.nErrorCode = pCamera->m_LastErrorInfo.nErrorCode;
            sprintf(GrabInfo.strDescription, pCamera->m_LastErrorInfo.strErrorDescription);
        }
        pCamera->m_CallBackFunc(&GrabInfo);
    }
}

GX_STATUS CameraBase::__PrepareImgSaveArea()
{
    if (m_pImgRaw8Buffer != NULL)
    {
        delete[]m_pImgRaw8Buffer;
        m_pImgRaw8Buffer = NULL;
    }
    if (m_pImgRGBBuffer != NULL)
    {
        delete[]m_pImgRGBBuffer;
        m_pImgRGBBuffer = NULL;
    }

    //Ϊ�洢Raw8���ݿ��ٿռ�
    m_pImgRaw8Buffer = new BYTE[size_t(m_nImageWidth * m_nImageHeight)];
    if (m_pImgRaw8Buffer == NULL)
    {
        return GX_STATUS_ERROR;
    }

    //Ϊ�洢RGB���ݿ��ٿռ�
    m_pImgRGBBuffer = new BYTE[size_t(m_nImageWidth * m_nImageHeight * m_nImageByteCount)];
    if (m_pImgRGBBuffer == NULL)
    {
        if (m_pImgRaw8Buffer != NULL)
        {
            delete[]m_pImgRaw8Buffer;
            m_pImgRaw8Buffer = NULL;
        }
        return GX_STATUS_ERROR;
    }
    return GX_STATUS_SUCCESS;
}

//----------------------------------------------------------------------------------
/**
\brief  ����bmpͼ��
\param  pImageBuf  ָ��ͼ�񻺳�����ָ��
\param  nImageWidth ͼ���
\param  nImageHeight ͼ���
\return �޷���ֵ
*/
//----------------------------------------------------------------------------------
void CameraBase::__SaveBMP(BYTE* pImageBuf, int64_t nImageWidth, int64_t nImageHeight)
{
    if (!m_bSaveBMP || m_strBMPFolder.length() == 0)
    {
        return;
    }

    BITMAPFILEHEADER	bfh = { 0 };
    DWORD				dwBytesRead = 0;
    DWORD               dwImageSize = DWORD(nImageWidth * nImageHeight * 3);

    bfh.bfType = (WORD)'M' << 8 | 'B';			                        //�����ļ�����
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);	//�����ļ�ͷ��С��ɫ
    bfh.bfSize = bfh.bfOffBits + dwImageSize;		                    //�ļ���С

    SYSTEMTIME system;
    GetLocalTime(&system);
    string strBMPFileName = MsgComposer("\\%02d%02d%02d%03d.bmp",
        system.wHour,
        system.wMinute,
        system.wSecond,
        system.wMilliseconds);

    string strBMPFilePath = m_strBMPFolder + strBMPFileName;

    HANDLE hFile = ::CreateFile((LPCSTR)strBMPFilePath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hFile != INVALID_HANDLE_VALUE)
    {
        ::WriteFile(hFile, &bfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
        ::WriteFile(hFile, m_pBmpInfo, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
        ::WriteFile(hFile, pImageBuf, dwImageSize, &dwBytesRead, NULL);
        CloseHandle(hFile);
    }
}


//��ȡ�������
bool    CameraBase::GetCameraCount(int& nCameraCount)
{
    GX_STATUS emStatus;
    emStatus = GXInitLib();//��ʼ�����ÿ�------------
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }
    //ö���豸��������ʱʱ��1000ms[��ʱʱ���û��������ã�û���Ƽ�ֵ]
    uint32_t nDeviceNum = 0;
    emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);//��ȡ�豸����

    nCameraCount = nDeviceNum;

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return FALSE;
    }
    if (nDeviceNum <= 0)
    {
        return FALSE;
    }

    return true;
}


//��ȡ������к�
bool CameraBase::GetCameraSN(int nCameraNumber, char sCameraSN[MaxSNLen])
{
    GX_STATUS emStatus;

    //ö���豸��������ʱʱ��1000ms[��ʱʱ���û��������ã�û���Ƽ�ֵ]
    uint32_t nDeviceNum = 0;
    emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);//��ȡ�豸����
    //ö���豸ʱ�����˴����ڴ��쳣�����.
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }

    if (nCameraNumber >= nDeviceNum)
    {
        return false;
    }

    //��ȡ�豸���к�
    GX_DEVICE_BASE_INFO deviceInfo[10];
    int i;
    size_t size = sizeof(deviceInfo) * nDeviceNum;
    emStatus = GXGetAllDeviceBaseInfo(deviceInfo, &size);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }
    for (i = 0; i < MaxSNLen; i++)
    {
        sCameraSN[i] = deviceInfo[nCameraNumber].szSN[i];
    }
    return true;
}