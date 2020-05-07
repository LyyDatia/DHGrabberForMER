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

// 初始化
BOOL CameraBase::Init(const s_GBINITSTRUCT* pInitParam)
{
    //AfxMessageBox("asd");
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    emStatus = GXInitLib();//初始化设置库------------
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return 1;
    }


    BOOL bRet = false;

    s_DC_INITSTRUCT InitCardParam;
    InitCardParam.nGrabberSN = pInitParam->nGrabberSN;
    //将char的函数，写入到指定的字符串中
    sprintf(InitCardParam.strGrabberFile, pInitParam->strGrabberFile);//读取相机配置文件位置

    sprintf(InitCardParam.strDeviceName, pInitParam->strDeviceName);//读取设备的名称
    sprintf(InitCardParam.strDeviceMark, pInitParam->strDeviceMark);//读取SN

    InitCardParam.CallBackFunc = pInitParam->CallBackFunc;//设置回调函数
    InitCardParam.Context = pInitParam->Context;//将回调函数的窗口指针传递
    InitCardParam.iGrabberTypeSN = pInitParam->iGrabberTypeSN;// 采集卡类型编号 [10/19/2010 SJC]

    emStatus = Open(&InitCardParam);//打开相机设置，并且初始化

    if (emStatus == GX_STATUS_SUCCESS)
    {
        bRet = true;
    }
    return bRet;
}

//开始采集
BOOL CameraBase::StartGrab()
{
    StartCallbackAcq();
    return TRUE;
}

//停止采集
BOOL CameraBase::StopGrab()
{
    StopCallbackAcq();
    return TRUE;
}

// 单帧采集 
BOOL CameraBase::Snapshot()
{
    return TRUE;
}

//设置通用参数
BOOL CameraBase::SetParamInt(GBParamID Param, int nReturnVal)
{
    return TRUE;
}

//得到通用参数
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
        sprintf(m_LastErrorInfo.strErrorDescription, "得到参数出错");
        sprintf(m_LastErrorInfo.strErrorRemark, "GetParamInt()函数");

        return FALSE;
    }

    return TRUE;
}

//调用参数对话框
void CameraBase::CallParamDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (m_pParamSetDlg != NULL)
    {
        AfxMessageBox("控制窗口已打!");
        return;
    }

    m_pParamSetDlg = new CSetParamDialog();
    m_pParamSetDlg->m_pCamera = this;
    m_pParamSetDlg->Create(CSetParamDialog::IDD);
    m_pParamSetDlg->CenterWindow();
    m_pParamSetDlg->ShowWindow(SW_SHOW);

}

// 获得错误信息
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

//得到相机专有参数
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

//输出端口发送信号
BOOL CameraBase::SetOutputValue(int nOutputPort, int nOutputValue)
{
    return TRUE;
}

//----------------------------------------------------------------------------------
/**
\brief  打开设备
\return 打开设备成功则返回GX_STATUS_SUCCESS，否则返回错误码
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::Open(const s_DC_INITSTRUCT* pInitParam)
{
    m_nGrabberSN = pInitParam->nGrabberSN + 1;			//序号 // 公司相机序号从1开始,为了和其他相机相兼容，自加1 [9/16/2010 SJC]
    m_CallBackFunc = pInitParam->CallBackFunc;			//回调函数指针
    m_Context = pInitParam->Context;				//存放调用初始化函数的对象的this指针

    m_sInitFile = pInitParam->strGrabberFile;		//用于初始化的文件????

    m_sDeviceName = pInitParam->strDeviceName;			//采集设备名
    m_strDeviceMark.Format("%s", pInitParam->strDeviceMark);
    m_nGrabberType = pInitParam->iGrabberTypeSN;		// 采集卡类型

    CFileFind fileSerch;
    if (!fileSerch.FindFile(m_sInitFile))
    {
        m_LastErrorInfo.nErrorCode = DCErrorOpenCam;
        sprintf(m_LastErrorInfo.strErrorDescription, "初始化文件不存在!");
        sprintf(m_LastErrorInfo.strErrorRemark, "请查证设备初始化文件名是否正确");
        return GX_STATUS_ERROR;
    }


    InitParamFromINI();// 读取相机配置文件，本身配置文件


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
        //枚举设备个数，超时时间1000ms[超时时间用户自行设置，没有推荐值]
        uint32_t nDeviceNum = 0;
        emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);//获取设备个数
        m_nTotalNum = nDeviceNum;

        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }
        if (m_nTotalNum <= 0)
        {
            return GX_STATUS_ERROR;
        }


        //根据序列号打开相机
        GX_OPEN_PARAM openParam;
        openParam.openMode = GX_OPEN_SN;
        openParam.accessMode = GX_ACCESS_CONTROL;
        openParam.pszContent = (LPSTR)(LPCTSTR)m_strDeviceMark;
        emStatus = GXOpenDevice(&openParam, &m_hDevice);//设置指定的当前设备

        if (emStatus != GX_STATUS_SUCCESS)
        {
            char error[255];
            size_t size = 255;
            GXGetLastError(&emStatus, error, &size);
            return emStatus;
        }
        m_bIsOpen = true;

        //--------------------------打开相机设备完毕------------------------------------		
        if (!m_bInitSuccess)
        {
            // 设置参数
            SetInitParam();

            emStatus = __PrepareForShowImg();
            if (emStatus != GX_STATUS_SUCCESS)
            {
                return emStatus;
            }

            if (m_nImageWidth < m_nWidth || m_nImageHeight < m_nHeight)
            {
                m_LastErrorInfo.nErrorCode = DCErrorValOverFlow;
                sprintf(m_LastErrorInfo.strErrorDescription, "宽度或者高度超出最大允许值");
                sprintf(m_LastErrorInfo.strErrorRemark, "Init()函数中的HVGetDeviceInfo()");
                return GX_STATUS_ERROR;
            }
            m_bInitSuccess = TRUE;
        }
        else
        {
            ////设备初始化时 恢复原始视野
            //MERSetParamInt(MERImageOffsetHoriz, 0);
            //MERSetParamInt(MERImageOffsetVerti, 0);
            //MERSetParamInt(MERCarveImageWidth, m_nMaxWidth);
            //MERSetParamInt(MERCarveImageHeight, m_nMaxHeight);

            m_LastErrorInfo.nErrorCode = DCErrorInit;
            sprintf(m_LastErrorInfo.strErrorDescription, "相机已经初始化成功!");
            sprintf(m_LastErrorInfo.strErrorRemark, "相机重复初始化");
            return GX_STATUS_SUCCESS;
        }
    }
    catch (...)
    {
        m_LastErrorInfo.nErrorCode = DCErrorInit;
        sprintf(m_LastErrorInfo.strErrorDescription, "初始化失败");
        sprintf(m_LastErrorInfo.strErrorRemark, "Init()函数中try捕捉到的异常");
        return GX_STATUS_ERROR;
    }

    return emStatus;
}

//读取配置文件
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
        sprintf(m_LastErrorInfo.strErrorDescription, "读取配置文件出错");
        sprintf(m_LastErrorInfo.strErrorRemark, "InitParamFromINI()函数");
    }
}

//保存相机设置信息
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
    //设置感兴趣区域
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

    //设置采集速度级别
    status = SetInt(GX_INT_ACQUISITION_SPEED_LEVEL, m_AcqSpeedLevel);

    //设置触发源
    status = SetEnum(GX_ENUM_TRIGGER_SOURCE, GX_TRIGGER_SOURCE_LINE0);

    //设置触发模式
    if (m_bTriggerMode)
    {
        status = SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
    }
    else
    {
        status = SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
    }
    //--------------------------------------------------------------------------------------------------

    //设置触发激活方式
    switch (m_nTriggerActivation)
    {
    case 0:
        status = SetEnum(GX_ENUM_TRIGGER_ACTIVATION, GX_TRIGGER_ACTIVATION_RISINGEDGE);
        break;
    case 1:
        status = SetEnum(GX_ENUM_TRIGGER_ACTIVATION, GX_TRIGGER_ACTIVATION_FALLINGEDGE);
        break;
    }


    //设置曝光模式
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
    //设置自动曝光
    if (m_bExposureAuto)
    {
        status = SetEnum(GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_CONTINUOUS);
    }
    else
    {
        status = SetEnum(GX_ENUM_EXPOSURE_AUTO, GX_EXPOSURE_AUTO_OFF);
        //设置曝光时间
        status = SetFloat(GX_FLOAT_EXPOSURE_TIME, m_dExposureTime);
    }

    //设置自动增益
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
        //设置增益
        status = SetFloat(GX_FLOAT_GAIN, (double)m_nGain);
    }

    if (m_bBlacklevelAuto)
    {
        status = SetEnum(GX_ENUM_BLACKLEVEL_AUTO, GX_BLACKLEVEL_AUTO_CONTINUOUS);
    }
    else
    {
        status = SetEnum(GX_ENUM_BLACKLEVEL_AUTO, GX_BLACKLEVEL_AUTO_OFF);
        //设置黑电平
        status = SetFloat(GX_FLOAT_BLACKLEVEL, (double)m_nBlacklevel);
    }

    //------------------------------------------------------------------------------------------------------

    // 	//设置自动白平衡
    // 	if (m_bBalanceRatioAuto)
    // 	{
    // 		status = SetEnum(GX_ENUM_BALANCE_WHITE_AUTO,GX_BALANCE_WHITE_AUTO_CONTINUOUS);
    // 	}
    // 	else
    // 	{
    // 		status = SetEnum(GX_ENUM_BALANCE_WHITE_AUTO,GX_BALANCE_WHITE_AUTO_OFF);
    // 		//设置白平衡
    //      	status = SetFloat(GX_FLOAT_BALANCE_RATIO,m_dBalanceRatio);
    // 	}


    //设置引脚输入输出状态 line0为trigger输入 line1为strobe输出

    //引脚选择为Line0
    status = SetEnum(GX_ENUM_LINE_SELECTOR, GX_ENUM_LINE_SELECTOR_LINE0);
    //设置引脚方向为输出
    status = SetEnum(GX_ENUM_LINE_MODE, GX_ENUM_LINE_MODE_INPUT);

    //引脚选择为Line1
    status = SetEnum(GX_ENUM_LINE_SELECTOR, GX_ENUM_LINE_SELECTOR_LINE1);
    //设置引脚方向为输出
    status = SetEnum(GX_ENUM_LINE_MODE, GX_ENUM_LINE_MODE_OUTPUT);
    //可选操作引脚电平反转
    //emStatus = SetEnum(GX_BOOL_LINE_INVERTER, true);
    //设置输出源为闪光灯
    status = SetEnum(GX_ENUM_LINE_SOURCE, GX_ENUM_LINE_SOURCE_STROBE);



}

//----------------------------------------------------------------------------------
/**
\brief  关闭设备
\return 关闭设备成功则返回GX_STATUS_SUCCESS，否则返回错误码
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
\brief  设备打开状态
\return 设备打开则返回true，否则返回false
*/
//----------------------------------------------------------------------------------
bool CameraBase::IsOpen()
{
    return m_bIsOpen;
}



//----------------------------------------------------------------------------------
/**
\brief  注册图像显示窗口
\param  pWnd  指向图像显示窗口指针
\return 图像显示窗口指针不为NULL，返回GX_STATUS_SUCCESS；否则返回GX_STATUS_INVALID_PARAMETER
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
\brief  注册日志显示窗口
\param  pWnd  指向日志显示窗口指针
\return 日志显示窗口指针不为NULL，返回GX_STATUS_SUCCESS；否则返回GX_STATUS_INVALID_PARAMETER
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
\brief  开始注册回调
\return 当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
        //	__PrintInfotoWnd(MsgComposer("注册采集回调函数"));
        emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_START);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            __UnPrepareForShowImg();
            return emStatus;
        }
        //	__PrintInfotoWnd(MsgComposer("发送开采命令"));
        m_bIsSnaping = true;
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  停止注册回调
\return 当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::StopCallbackAcq()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen() && IsSnaping())
    {
        //如果当前正在录制视频流，先停止录制
        if (m_bSaveAVI)
        {
            EnableSaveAVI(false);
        }

        emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }
        //__PrintInfotoWnd(MsgComposer("发送停采命令"));
        emStatus = GXUnregisterCaptureCallback(m_hDevice);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }
        //__PrintInfotoWnd(MsgComposer("注销采集回调函数"));

        m_bIsSnaping = false;
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  使能显示图像功能
\param  bShowImg  true表示显示图像；false表示不显示图像
\return 无
*/
//----------------------------------------------------------------------------------
void CameraBase::EnableShowImg(bool bShowImg)
{
    m_bShowImg = bShowImg;
}

//----------------------------------------------------------------------------------
/**
\brief  使能保存BMP功能
\param  bSaveBMP  true表示保存；false表示不保存
\return 无
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
\brief  使能保存AVI功能
\param  bSaveAVI  true表示保存；false表示不保存
\return 无
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
\brief  设置保存BMP图片的文件夹路径
\param  pBMPFolder  路径，尾部不带斜杠
\return 无
*/
//----------------------------------------------------------------------------------
void CameraBase::SetBMPFolder(char* pBMPFolder)
{
    m_strBMPFolder = string(pBMPFolder);
}

//----------------------------------------------------------------------------------
/**
\brief  设置保存AVI视频的文件夹路径
\param  szAVIFolder  路径，尾部不带斜杠
\return 无
*/
//----------------------------------------------------------------------------------
void CameraBase::SetAVIFolder(char* pAVIFolder)
{
    m_strAVIFolder = string(pAVIFolder);
}

//----------------------------------------------------------------------------------
/**
\brief  设备开始采集状态
\return 设备开始采集则返回true，否则返回false
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
        //打印日志信息
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("失败:GXIsImplemented 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "IsImplemented()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取功能码名称
\param  emFeatureID  功能码ID
\param  pszName    指向返回功能码名称的指针
\param  pnSize     功能码的字符串长度
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetFeatureName(GX_FEATURE_ID emFeatureID, char* pszName, size_t* pnSize)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetFeatureName(m_hDevice, emFeatureID, pszName, pnSize);
        //打印日志信息
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("失败:GXGetFeatureName 功能名称:%s 错误码:%d", pszName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetFeatureName()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取Int类型参数最小值、最大值、步长
\param  emFeatureID  功能码ID
\param  pIntRange  指向返回int型结构体的指针
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetIntRange(GX_FEATURE_ID emFeatureID, GX_INT_RANGE* pIntRange)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetIntRange(m_hDevice, emFeatureID, pIntRange);
        //打印日志信息
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("失败:GXGetIntRange 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetIntRange()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取int型参数当前值
\param  emFeatureID  功能码ID
\param  pnValue    指向返回当前值的指针
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::GetInt(GX_FEATURE_ID emFeatureID, int64_t* pnValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXGetInt(m_hDevice, emFeatureID, pnValue);
        //打印日志信息
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("失败:GXGetInt 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetInt()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  设置Int类型值功能参数
\param  emFeatureID  功能码ID
\param  pnValue    用户将要设置的值
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::SetInt(GX_FEATURE_ID emFeatureID, int64_t nValue)
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    if (IsOpen())
    {
        emStatus = GXSetInt(m_hDevice, emFeatureID, nValue);
        //打印日志信息
        char chFeatureName[64] = { 0 };
        size_t nSize = 64;
        GXGetFeatureName(m_hDevice, emFeatureID, chFeatureName, &nSize);
        if (emStatus != GX_STATUS_SUCCESS)
        {
            CString str;
            str.Format("失败:GXSetInt 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetInt()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取Float类型参数的最小值、最大值、步长、单位等描述信息
\param  emFeatureID    功能码ID
\param  pFloatRange  指向返回Float类型结构体的指针
\return              当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetFloatRange 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetFloatRange()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取Float类型当前参数值
\param  emFeatureID    功能码ID
\param  pdValue      指向返回Float类型参数值的指针
\return              当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetFloat 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetFloat()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  设置Float类型参数值
\param  emFeatureID    功能码ID
\param  dValue       用户将要设置的值
\return              当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXSetFloat 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetFloat()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取枚举项的可选项个数
\param  emFeatureID    功能码ID
\param  pnEntryNums  指向返回可选项个数的指针
\return              当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetEnumEntryNums 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetEnumEntryNums()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取枚举类型值的描述信息
\param  emFeatureID         功能码ID
\param  pEnumDescription  指向枚举项的描述信息的数组指针
\param  pBufferSize       指向所有枚举项描述信息的数组大小
\return                   当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetEnumDescription 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetEnumDescription()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取当前枚举值
\param  emFeatureID  功能码ID
\param  pnValue    指向返回的枚举值的指针
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetEnum 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetEnum()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  设置枚举值
\param  emFeatureID  功能码ID
\param  nValue     用户将要设置的枚举值
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetEnum 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetEnum()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取bool类型值
\param  emFeatureID  功能码ID
\param  nValue     指向返回的布尔值的指针
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetBool 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetBool()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  设置bool类型值
\param  emFeatureID  功能码ID
\param  bValue     用户将要设置的布尔值
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXSetBool 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetBool()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取字符串类型值的长度
\param  emFeatureID  功能码ID
\param  bValue     指向返回的长度值的指针，长度值带末位'\0'，长度单位字节
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetStringLength 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetStringLength()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取字符串类型值的内容
\param  emFeatureID  功能码ID
\param  pszContent 指向用户申请的字符串缓存地址
\param  pnSize     表示用户输入的字符串缓冲区地址的长度
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetString 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetString()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  设置字符串值内容
\param  emFeatureID  功能码ID
\param  pszContent 指向用户将要设置的字符串地址
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXSetString 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetString()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取块儿数据长度
\param  emFeatureID  功能码ID
\param  pnSize     指向返回的长度值的指针，长度单位字节
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetBufferLength 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetBufferLength()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  获取块儿数据
\param  emFeatureID  功能码ID
\param  pBuffer    指向用户申请的块儿数据内存地址指针
\param  pnSize     表示用户输入的缓冲区地址的长度
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXGetBuffer 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorGetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "GetBuffer()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  设置块儿数据
\param  emFeatureID  功能码ID
\param  pBuffer    指向用户申请的块儿数据内存地址指针
\param  nSize      用户输入的缓冲区地址的长度
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
            str.Format("失败:GXSetBuffer 功能名称:%s 错误码:%d", chFeatureName, emStatus);
            m_LastErrorInfo.nErrorCode = DCErrorSetParam;
            sprintf(m_LastErrorInfo.strErrorDescription, str);
            sprintf(m_LastErrorInfo.strErrorRemark, "SetBuffer()函数");
        }
    }
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  发送命令码
\param  emFeatureID  功能码ID
\return            当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
\brief  发送开采命令后,此接口可以直接获取图像（注意此接口不能与回调采集方式混用）
\param  pFrameData  指向用户传入的用来接收图像数据的地址指针
\param  nTimeout    取图的超时时间
\return             当接口调用成功，则返回GX_STATUS_SUCCESS，否则返回GX_STATUS_ERROR
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
    // 			__PrintInfotoWnd(MsgComposer("失败:GXGetImage 错误码:%d", emStatus));
    // 		}
    // 		else
    // 		{
    // 			if (m_bEnableGetImageProcess)
    // 			{
    // 				//首先将回调中输出的图像数据，处理成RGB数据，以备后面的显示和存储
    // 				__ProcessData((BYTE*)pFrameData->pImgBuf,m_pImgRGBBuffer, m_nImageWidth, m_nImageHeight);
    // 				//在指定窗口上一帧一帧的画图
    // 				__DrawImg(m_pImgRGBBuffer, m_nImageWidth, m_nImageHeight);
    // 				//在指定存储目录下一帧一帧的存储bmp图
    // 				__SaveBMP(m_pImgRGBBuffer, m_nImageWidth, m_nImageHeight);
    // 			}
    // 		}
    // 		
    // 	}
    return emStatus;
}

//----------------------------------------------------------------------------------
/**
\brief  使能GetImage接口的内部处理过程，包括图片信息的显示及保存
\param  bEnableGetImageProcess  true表示使能；false表示禁用
\return 无
*/
//----------------------------------------------------------------------------------
void CameraBase::EnableGetImageProcess(bool bEnableGetImageProcess)
{
    m_bEnableGetImageProcess = bEnableGetImageProcess;
}



//----------------------------------------------------------------------------------
/**
\brief  为图像显示准备资源
\return 成功返回GX_STATUS_SUCCESS;否则返回相应错误码，具体查询GX_STATUS_LIST
*/
//----------------------------------------------------------------------------------
GX_STATUS CameraBase::__PrepareForShowImg()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    //获取图象数据大小
    emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &m_nPayLoadSize);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return emStatus;
    }
    //获得图像格式
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
    //查询当前相机是否支持GX_ENUM_PIXEL_COLOR_FILTER
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

    //获取宽度
    emStatus = GXGetInt(m_hDevice, GX_INT_WIDTH, &m_nImageWidth);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return emStatus;
    }
    //获取高度
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
\brief  释放为图像显示准备资源
\return 无返回值
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
\brief  将相机输出的原始数据转换为RGB数据
\param  pImageBuf  指向图像缓冲区的指针
\param  pImageRGBBuf  指向RGB数据缓冲区的指针
\param  nImageWidth 图像宽
\param  nImageHeight 图像高
\return 无返回值
*/
//----------------------------------------------------------------------------------
void CameraBase::__ProcessData(BYTE* pImageBuf, BYTE* pImageRGBBuf, int64_t nImageWidth, int64_t nImageHeight)
{
    switch (m_nPixelFormat)
    {
        //当数据格式为12位时，位数转换为4-11
    case GX_PIXEL_FORMAT_BAYER_GR12:
    case GX_PIXEL_FORMAT_BAYER_RG12:
    case GX_PIXEL_FORMAT_BAYER_GB12:
    case GX_PIXEL_FORMAT_BAYER_BG12:
        //将12位格式的图像转换为8位格式
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_4_11);

        //将Raw8图像转换为RGB图像以供显示
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(m_nPixelColorFilter), TRUE);
        break;

        //当数据格式为12位时，位数转换为2-9
    case GX_PIXEL_FORMAT_BAYER_GR10:
    case GX_PIXEL_FORMAT_BAYER_RG10:
    case GX_PIXEL_FORMAT_BAYER_GB10:
    case GX_PIXEL_FORMAT_BAYER_BG10:
        ////将12位格式的图像转换为8位格式,有效位数2-9
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_2_9);

        //将Raw8图像转换为RGB图像以供显示
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(m_nPixelColorFilter), TRUE);
        break;

    case GX_PIXEL_FORMAT_BAYER_GR8:
    case GX_PIXEL_FORMAT_BAYER_RG8:
    case GX_PIXEL_FORMAT_BAYER_GB8:
    case GX_PIXEL_FORMAT_BAYER_BG8:
        //将Raw8图像转换为RGB图像以供显示
        DxRaw8toRGB24(pImageBuf, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(m_nPixelColorFilter), TRUE);
        break;

    case GX_PIXEL_FORMAT_MONO12:
        //将12位格式的图像转换为8位格式
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_4_11);
        //将Raw8图像转换为RGB图像以供显示
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(NONE), TRUE);
        break;

    case GX_PIXEL_FORMAT_MONO10:
        //将10位格式的图像转换为8位格式
        DxRaw16toRaw8(pImageBuf, m_pImgRaw8Buffer, (VxUint32)nImageWidth, (VxUint32)nImageHeight, DX_BIT_4_11);
        //将Raw8图像转换为RGB图像以供显示
        DxRaw8toRGB24(m_pImgRaw8Buffer, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(NONE), TRUE);
        break;

    case GX_PIXEL_FORMAT_MONO8:
        //将Raw8图像转换为RGB图像以供显示
        DxRaw8toRGB24(pImageBuf, pImageRGBBuf, (VxUint32)nImageWidth, (VxUint32)nImageHeight, RAW2RGB_NEIGHBOUR,
            DX_PIXEL_COLOR_FILTER(NONE), TRUE);


    default:
        break;
    }
}


//----------------------------------------------------------------------------------
/**
\brief  回调函数
\param  pFrame  指向采集回调的数据结构指针
\return 无返回值
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
            //首先将回调中输出的图像数据，处理成RGB数据，以备后面的显示和存储
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

            //将数据进行垂直方向镜像
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
            sprintf(pCamera->m_LastErrorInfo.strErrorDescription, "传出图像内存地址失败");
            sprintf(pCamera->m_LastErrorInfo.strErrorRemark, "__OnFrameCallbackFun()函数");
        }

        s_GBSIGNALINFO GrabInfo;
        GrabInfo.Context = pCamera->m_Context;
        GrabInfo.iGrabberTypeSN = pCamera->m_nGrabberType; // 采集卡类型编号
        GrabInfo.nGrabberSN = pCamera->m_nGrabberSN - 1; // 相机序号从0开始,保证与其他相机一样
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

    //为存储Raw8数据开辟空间
    m_pImgRaw8Buffer = new BYTE[size_t(m_nImageWidth * m_nImageHeight)];
    if (m_pImgRaw8Buffer == NULL)
    {
        return GX_STATUS_ERROR;
    }

    //为存储RGB数据开辟空间
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
\brief  保存bmp图像
\param  pImageBuf  指向图像缓冲区的指针
\param  nImageWidth 图像宽
\param  nImageHeight 图像高
\return 无返回值
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

    bfh.bfType = (WORD)'M' << 8 | 'B';			                        //定义文件类型
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);	//定义文件头大小彩色
    bfh.bfSize = bfh.bfOffBits + dwImageSize;		                    //文件大小

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


//获取相机个数
bool    CameraBase::GetCameraCount(int& nCameraCount)
{
    GX_STATUS emStatus;
    emStatus = GXInitLib();//初始化设置库------------
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }
    //枚举设备个数，超时时间1000ms[超时时间用户自行设置，没有推荐值]
    uint32_t nDeviceNum = 0;
    emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);//获取设备个数

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


//获取相机序列号
bool CameraBase::GetCameraSN(int nCameraNumber, char sCameraSN[MaxSNLen])
{
    GX_STATUS emStatus;

    //枚举设备个数，超时时间1000ms[超时时间用户自行设置，没有推荐值]
    uint32_t nDeviceNum = 0;
    emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);//获取设备个数
    //枚举设备时发生了大量内存异常的情况.
    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }

    if (nCameraNumber >= nDeviceNum)
    {
        return false;
    }

    //获取设备序列号
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