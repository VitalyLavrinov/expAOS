
// expAOSDlg.h : header file
//

#pragma once
#include "CSensor.h"
#include "CCT.h"
#include "CZRNK.h"
#include "CWFSPROP.h"

UINT ThreadDrowWFS(LPVOID pParam);
UINT ThreadGrabWFS(LPVOID pParam);
UINT ThreadDrowWFSLoop(LPVOID pParam);

// CexpAOSDlg dialog
class CexpAOSDlg : public CDialogEx
{
// Construction
public:
	CexpAOSDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPAOS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnonewfs();
	afx_msg void OnBnClickedBtnsetexpwfs();
	afx_msg void OnBnClickedBtnstartwfs();
	afx_msg void OnBnClickedBtnstopwfs();
	afx_msg void OnBnClickedBtngerefwfs();
	afx_msg void OnBnClickedBtnclosewfs();
	afx_msg void OnBnClickedBtnwfsleschoise();
	afx_msg void ShowFrameDataWfs(cv::Mat& out);
	afx_msg void IniCT();
	afx_msg void ShowCT();
	afx_msg void ShowCorr();
	afx_msg void IniZRNK();
	afx_msg void ShowZRNK();
	afx_msg void IniLensButt();
	afx_msg void ShowSubStat();
	afx_msg void ShowCn2Stat();
	afx_msg void OnBnClickedBtnwfsleschoisesave();
	afx_msg void OnBnClickedBtnwfsprop();
	afx_msg void OnBnClickedBtncnnctwfs2();
	afx_msg void OnBnClickedBtnsetlmore();
	afx_msg void OnBnClickedBtngrab();
};
