//--------------------------------------------------------------- 
/** 
\file      CameraBase.h
\brief     Declarations for camerabase method 
\version   v1.0.1305.9301 
\date      2013-5-30
*/ 
//--------------------------------------------------------------- 

#if !defined(_CAMERABASE_H)
#define _CAMERABASE_H

#include "GxIAPI.h"
#include "DxImageProc.h"
#include "DHGrabberForMER.h"
#include "SetParamDialog.h"
#include <string>
#include <iostream>
using namespace std;

#define _WIN32_DCOM
#include <ObjBase.h>
#pragma comment(lib, "ole32.lib")

#include "Vfw.h"
#pragma comment(lib, "Vfw32.lib")

// ��ʼ���ļ�������󳤶�
const int MaxFileNameLen = 255;

///////////////////////////////////////////////////////////+++
// һЩ�ṹ��Ķ���

typedef struct _HVCamDHSignalInfoStruct        //�ص���Ϣ�ṹ
{
    PVOID Context;                                // ���ӵ�лص������Ķ����thisָ��
    int nGrabberSN;                                // �ɼ������
    int nErrorCode;                                // ������루0��������
    int nFrameCount;                            // �Ѳɼ���֡����
    char strDescription[MaxFileNameLen];        // ������Ϣ
}s_DC_SIGNALINFO;    //�ص���Ϣ�ṹ


typedef struct _HVCamDHInitStruct            // ������ʼ���ṹ
{
    int iGrabberTypeSN ;                    //�ɼ����������
    int nGrabberSN;                            // �ɼ������
    char strDeviceName[MaxFileNameLen];        // �豸����
    char strDeviceMark[MaxFileNameLen];        // �豸����

    char strGrabberFile[MaxFileNameLen];    // ���ڳ�ʼ�����ļ�
    PGBCALLBACK CallBackFunc;                // �ص�����ָ��
    PVOID Context;                            // ���ӵ�лص������Ķ����thisָ��
}s_DC_INITSTRUCT;// ������ʼ���ṹ

typedef struct _DCErrorInfoStruct//������Ϣ�ṹ
{
    int nErrorCode;                            // ������루0:������
    char strErrorDescription[MaxFileNameLen];    // ������Ϣ
    char strErrorRemark[MaxFileNameLen];        // ������Ϣ
}s_DCERRORINFO;//������Ϣ�ṹ


enum DCErrorStruct
{
    DCErrorNoFindCam = 0,    // û���ҵ����
    DCErrorGrabberSN,        // �����ų����ܹ��ҵ��������
    DCErrorGetCamInfo,        // �õ������Ϣʧ��
    DCErrorValOverFlow,        // �������
    DCErrorOpenCam,            // �����ʧ��
    DCErrorInit,            // ��ʼ��ʧ��
    DCErrorStartGrab,        // ��ʼ�ɼ�ʧ��
    DCErrorStopGrab,        // ֹͣ�ɼ�ʧ��
    DCErrorClose,            // �ر�ʧ��
    DCErrorSendBufAdd,        // ����ͼ���ڴ��ַʧ��
    DCErrorGetParam,        // �Ƿ���ȡ����
    DCErrorSetParam,        // �Ƿ����ò���
    DCErrorParamDlg,        // ���ò������öԻ���ʧ��
    DCErrorReadIniFile        // ��ȡ�����ļ�ʧ��
};


//////////////////////////////////////////////////////////////////////////

class CVxCriticalSection  
{
public:
    /// ����
    CVxCriticalSection () {InitializeCriticalSection(&m_cs);}
    /// ����
    virtual ~CVxCriticalSection() {DeleteCriticalSection(&m_cs);}
public:
    /// �����ٽ���
    void Enter(){EnterCriticalSection(&m_cs);}
    /// �뿪�ٽ���
    void Leave(){LeaveCriticalSection(&m_cs);}
private:
    CRITICAL_SECTION m_cs; ///< �ٽ�α���    
};


class CSetParamDialog;
class CameraBase:public CGrabber
{
public:
    CameraBase();
    virtual ~CameraBase();
public:
    // ��ʼ��
    BOOL Init(const s_GBINITSTRUCT* pInitParam);
    //��ʼ�ɼ�
    BOOL StartGrab(); 
    //ֹͣ�ɼ�
    BOOL StopGrab(); 
    // ��֡�ɼ� 
    BOOL Snapshot();            
    //�ر�
    BOOL Close();
    //����ͨ�ò���
    BOOL SetParamInt(GBParamID Param, int nReturnVal);    
    //�õ�ͨ�ò���
    BOOL GetParamInt(GBParamID Param, int &nReturnVal);
    //���ò����Ի���
    void CallParamDialog();
    // ��ô�����Ϣ
    void GetLastErrorInfo(s_GBERRORINFO *pErrorInfo);

    //����ר�в���
    BOOL MERSetParamInt(MERParamID Param,int nInputVal1,int nInputVal2 = 0,int nInputVal3 = 0,int nInputVal4 = 0,int nInputVal5 = 0,void *sInputVal6 = NULL);    
    //�õ����ר�в���
    BOOL MERGetParamInt(MERParamID Param, int &nReturnVal1,int &nReturnVal2,int &nReturnVal3);
    //�������������Ϣ
    void SaveParamToINI();
    //����˿ڷ����ź�
    BOOL SetOutputValue(int nOutputPort,int nOutputValue);
    //��ʼ���������
    void InitParamFromINI();    
    // ���ò���
    void SetInitParam();//�������ʹ�ò���

    BOOL MERGetParamString(MERParamID Param, char* sOutputString,int size);//��ȡ�ַ�������
	//���ü��п�
	BOOL MERSetROI(int noffsetX,int noffsetY,int nwidth,int nheight);

    GX_STATUS Open(const s_DC_INITSTRUCT* pInitParam);
    bool      IsOpen();         
    GX_STATUS RegisterShowWnd(void* pWnd);
    GX_STATUS RegisterLogWnd(void* pWnd);
    bool      IsSnaping();
    GX_STATUS StartCallbackAcq();
    GX_STATUS StopCallbackAcq();
    void      SetBMPFolder(char* pBMPFolder);
    void      SetAVIFolder(char* pAVIFolder);
    void      EnableShowImg(bool bShowImg);
    void      EnableSaveBMP(bool bSaveBMP);
    void      CompressedAVI(bool bCompressed);
    GX_STATUS EnableSaveAVI(bool bSaveAVI);    
    GX_STATUS IsImplemented(GX_FEATURE_ID emFeatureID, bool* pbIsImplemented);
    GX_STATUS GetFeatureName(GX_FEATURE_ID emFeatureID, char* pszName, size_t* pnSize);
    GX_STATUS GetIntRange(GX_FEATURE_ID emFeatureID, GX_INT_RANGE* pIntRange);
    GX_STATUS GetInt(GX_FEATURE_ID emFeatureID, int64_t* pnValue);
    GX_STATUS SetInt(GX_FEATURE_ID emFeatureID, int64_t nValue);
    GX_STATUS GetFloatRange(GX_FEATURE_ID emFeatureID, GX_FLOAT_RANGE* pFloatRange);
    GX_STATUS GetFloat(GX_FEATURE_ID emFeatureID, double* pdValue);
    GX_STATUS SetFloat(GX_FEATURE_ID emFeatureID, double dValue);
    GX_STATUS GetEnumEntryNums(GX_FEATURE_ID emFeatureID, uint32_t* pnEntryNums);
    GX_STATUS GetEnumDescription(GX_FEATURE_ID emFeatureID, GX_ENUM_DESCRIPTION* pEnumDescription, size_t* pBufferSize);
    GX_STATUS GetEnum(GX_FEATURE_ID emFeatureID, int64_t* pnValue);
    GX_STATUS SetEnum(GX_FEATURE_ID emFeatureID, int64_t nValue);
    GX_STATUS GetBool(GX_FEATURE_ID emFeatureID, bool* pbValue);
    GX_STATUS SetBool(GX_FEATURE_ID emFeatureID, bool bValue);
    GX_STATUS GetStringLength(GX_FEATURE_ID emFeatureID, size_t* pnSize);
    GX_STATUS GetString(GX_FEATURE_ID emFeatureID, char* pszContent, size_t* pnSize);
    GX_STATUS SetString(GX_FEATURE_ID emFeatureID, char* pszContent);
    GX_STATUS GetBufferLength(GX_FEATURE_ID emFeatureID, size_t* pnSize);
    GX_STATUS GetBuffer(GX_FEATURE_ID emFeatureID, uint8_t* pBuffer, size_t* pnSize);
    GX_STATUS SetBuffer(GX_FEATURE_ID emFeatureID, uint8_t* pBuffer, size_t nSize);
    GX_STATUS SendCommand(GX_FEATURE_ID emFeatureID);
    GX_STATUS GetImage(GX_FRAME_DATA* pFrameData, int32_t nTimeout);
    void      EnableGetImageProcess(bool bEnableGetImageProcess);
    bool      GetCameraCount(int &nCameraCount);
    bool      GetCameraSN(int nCameraNumber,char sCameraSN[MaxSNLen]);

public:
    //�������
    int64_t m_nMax_Width;         //����ӳ����
    int64_t m_nMax_Height;        //����ӳ��߶�
    int64_t m_nWidth;             //����Ȥ����ͼ����
    int64_t m_nHeight;            //����Ȥ����ͼ��߶�
    int64_t m_nOffsetX;           //����Ȥ����Xƫ����
    int64_t m_nOffsetY;           //����Ȥ����Yƫ����
    int m_AcqSpeedLevel;        //�ɼ��ٶȼ���
    int m_nExposureMode;        //�ع�ģʽ
    double m_dExposureTime;     //�ع�ʱ��
    bool m_bExposureAuto;       //�Զ��ع�ʹ��
    int m_nGain;                //����ֵ
    bool m_bGainAuto;           //�Զ�����ʹ��
    int m_nBlacklevel;          //�ڵ�ƽֵ
    bool m_bBlacklevelAuto;     //�Զ��ڵ�ƽʹ��
    double m_dBalanceRatio;     //��ƽ��ϵ��
    bool m_bBalanceRatioAuto;   //�Զ���ƽ��ʹ��
    bool m_bTriggerMode;        //�ⴥ��ģʽ����
    int m_nTriggerActivation;   //�������� 0������ 1�½���

    CSetParamDialog* m_pParamSetDlg;

protected:
    //��ʼ������    
    int m_nGrabberSN;                       // ���

    // ���Ӳɼ������ͷ���ֵ
    int m_nGrabberType;

    bool m_bInitSuccess;//�Ƿ��ʼ���ɹ�
    int m_nTotalNum;        // ���������

    CString m_sInitFile;                    // ���ڳ�ʼ�����ļ�
    CString m_sDeviceName;                  // �豸��
    CString m_strDeviceMark;                // �豸��ʶ

    PGBCALLBACK m_CallBackFunc;             // �ص�����ָ��
    PVOID m_Context;                        // ��ŵ��ó�ʼ�������Ķ����thisָ��

    s_DCERRORINFO m_LastErrorInfo;          // ������Ϣ

    // ���ص��ֽ�
    int m_nImageByteCount;
    int m_nFrameCount;    // �ɼ�֡����

private:
    GX_DEV_HANDLE m_hDevice;            ///< �豸���
    bool          m_bIsOpen;            ///< �豸��״̬
    bool          m_bIsSnaping;         ///< �豸�ɼ�״̬
    string        m_strBMPFolder;       ///< bmpͼƬ����Ŀ¼
    string        m_strAVIFolder;       ///< avi��Ƶ����Ŀ¼
    bool          m_bShowImg;           ///< �Ƿ���ʾͼ��Ĭ��ֵΪtrue
    bool          m_bSaveBMP;           ///< �Ƿ񱣴�bmpͼ��Ĭ��ֵΪfalse
    bool          m_bSaveAVI;           ///< �Ƿ񱣴�AVI��Ƶ��Ĭ��ֵΪfalse
    BITMAPINFO    *m_pBmpInfo;          ///< ������ʾͼ��Ľṹָ��
    BYTE          *m_pImgRGBBuffer;     ///< �洢RGBͼ�񻺳�����ָ��
    BYTE          *m_pImgRaw8Buffer;    ///< �洢Raw8ͼ�񻺳�����ָ��
    char          *m_chBmpBuf;          ///< BIMTAPINFO �洢��������m_pBmpInfo��ָ��˻�����
    int           m_nWndWidth;          ///< ͼ����ʾ���ڵĿ�
    int           m_nWndHeight;         ///< ͼ����ʾ���ڵĸ�
    int64_t         m_nImageWidth;          ///< ԭʼͼ���
    int64_t         m_nImageHeight;         ///< ԭʼͼ���
    int64_t         m_nPayLoadSize;         ///< ͼ���С
    void          *m_pShowWnd;          ///< ָ��ͼ����ʾ���ڵ�ָ��
    void          *m_pDC;               ///< ͼ����ʾ����ָ��
    void          *m_pLogWnd;           ///< ָ����־��ʾ���ڵ�ָ��
    int64_t         m_nPixelColorFilter;    ///< ���ظ�ʽ
    int64_t         m_nPixelFormat;         ///< ���ݸ�ʽ
    bool          m_bEnableGetImageProcess; ///< �Ƿ�����GetImage�ӿ����д����ȡ�����ݣ�������ʾ���߱���BMP

    //AVI�������
    bool        m_bCompressed;  ///< �Ƿ�ѹ���洢AVI
    PAVIFILE    m_pAVIFile;     ///< AVI�ļ��ľ��
    PAVISTREAM    m_ps;           ///< ͨ��AVI�ļ���������Ƶ��
    PAVISTREAM    m_psCompressed; ///< ͨ����Ƶ���ͱ�����������ѹ����Ƶ��
    int         m_nTimeFrame;   ///< д��avi�ļ���Ҫ��֡ID��
    HIC         m_hic;          ///< ���ʹ�õı�����
    CVxCriticalSection m_SaverCriSec;     ///< ���滥����

private:
    GX_STATUS __PrepareForShowImg();
    void __UnPrepareForShowImg();
    void __ProcessData(BYTE * pImageBuf, BYTE * pImageRGBBuf, int64_t nImageWidth, int64_t nImageHeight);
    void __SaveBMP(BYTE * pImageBuf, int64_t nImageWidth, int64_t nImageHeight);
    static void __stdcall __OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM* pFrame);

    GX_STATUS __PrepareImgSaveArea();
};

inline std::string  MsgComposer( const char* pFormat, ... )
{
    char pBuffer[ 256 ];
    va_list vap;
    va_start( vap, pFormat );

#if _MSC_VER >= 1400 //for vc8 vc9
    _vsnprintf_s( pBuffer, 256, pFormat, vap );
#elif _MSC_VER >= 1200 //for vc6
    _vsnprintf( pBuffer, 256, pFormat, vap );
#endif

    va_end(vap);
    return std::string( pBuffer);
};

#endif  //_CAMERABASE_H
