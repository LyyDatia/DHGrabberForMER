#if !defined(AFX_SETPARAMDIALOG_H__EACE81E9_5A1A_4DB3_8B1D_727DBB438015__INCLUDED_)
#define AFX_SETPARAMDIALOG_H__EACE81E9_5A1A_4DB3_8B1D_727DBB438015__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetParamDialog.h : header file
//

#include "GrabberRegular.h"
#include "CameraBase.h"

/////////////////////////////////////////////////////////////////////////////
// CSetParamDialog dialog
class CameraBase;
class CSetParamDialog : public CDialog
{
// Construction
public:
	CSetParamDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetParamDialog)
	enum { IDD = IDD_SetParamDialog };
	int		m_ExposureTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetParamDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

	public:
		CameraBase* m_pCamera;
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetParamDialog)
	afx_msg void OnSetParam();
	afx_msg void OnSaveParam();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPARAMDIALOG_H__EACE81E9_5A1A_4DB3_8B1D_727DBB438015__INCLUDED_)
