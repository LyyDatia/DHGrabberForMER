// SetParamDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GrabberRegular.h"
#include "SetParamDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetParamDialog dialog


CSetParamDialog::CSetParamDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetParamDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetParamDialog)
	m_ExposureTime = 0;
	//}}AFX_DATA_INIT
}


void CSetParamDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetParamDialog)
	DDX_Text(pDX, IDC_EDIT_ExpTime, m_ExposureTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetParamDialog, CDialog)
	//{{AFX_MSG_MAP(CSetParamDialog)
	ON_BN_CLICKED(IDSET, OnSetParam)
	ON_BN_CLICKED(IDSAVE, OnSaveParam)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetParamDialog message handlers

void CSetParamDialog::OnSetParam() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_pCamera->StopGrab();
	if (((CButton *)GetDlgItem(IDC_RADIO_Tri))->GetCheck())
	{
		m_pCamera->m_bTriggerMode = true;
		m_pCamera->SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
	} 
	else
	{
		m_pCamera->m_bTriggerMode = false;
		m_pCamera->SetEnum(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
	}
	if (m_ExposureTime > 0)
	{
		m_pCamera->m_dExposureTime = m_ExposureTime;
		m_pCamera->SetFloat(GX_FLOAT_EXPOSURE_TIME,m_ExposureTime);
	}
	m_pCamera->StartGrab();
}

void CSetParamDialog::OnSaveParam() 
{
	// TODO: Add your control notification handler code here
	OnSetParam();
	m_pCamera->SaveParamToINI();
	
}

void CSetParamDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_pCamera->m_pParamSetDlg = NULL ;
	CDialog::OnCancel();
	DestroyWindow();
}

BOOL CSetParamDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_pCamera->m_bTriggerMode == true)
	{
		((CButton *)GetDlgItem(IDC_RADIO_Tri))->SetCheck(TRUE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_RADIO_Con))->SetCheck(TRUE);
	}
	m_ExposureTime = (int)m_pCamera->m_dExposureTime;
	UpdateData(FALSE) ;

	SetWindowPos(&wndTopMost , 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE); 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetParamDialog::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	m_pCamera->m_pParamSetDlg = NULL ;
	CDialog::OnClose();
	DestroyWindow();
}
