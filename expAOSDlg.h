
// expAOSDlg.h : header file
//

#pragma once
#include "CWFSControler.h"
#include "CCT.h"
#include "CZRNK.h"
#include "CWFSPROP.h"
#include "CVLT.h"
#include "CSTAT.h"

UINT ThreadDrowWFS(LPVOID pParam);// wavefront sensor
UINT ThreadGrabWFS(LPVOID pParam);//WFS grab *flm from wavefront sensor
UINT ThreadDrowWFSLoop(LPVOID pParam);//WFS open loop
UINT ThreadGrabFO(LPVOID pParam);//WFS get response function from mirror
UINT ThreadWFSCloseLoop(LPVOID pParam);//WFS closeLoop
UINT ThreadStatWFSLoop(LPVOID pParam);//WFS Cn2 and R0
UINT ThreadDrowSpec(LPVOID pParam);// Drowing spectrum WFS
UINT ThreadDrowTTS(LPVOID pParam);// Tilt/Tip sensor
UINT ThreadStatTTSLoop(LPVOID pParam);//TT sensor Cn2 and R0
UINT ThreadDrowSpecTTS(LPVOID pParam);// Drowing spectrum TTS

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
	afx_msg void ShowFrameDataWfsOne(cv::Mat& out);
	afx_msg void ShowFrameDataTTSOne(cv::Mat& out);
	afx_msg void ShowFrameDataWfs(cv::Mat& out);
	afx_msg void IniCT();
	afx_msg void IniVLT();
	afx_msg void ShowCT();
	afx_msg void ShowCTTTS();
	afx_msg void ShowCorr();
	afx_msg void IniZRNK();
	afx_msg void ShowZRNK();
	afx_msg void ShowVLT();
	afx_msg void IniLensButt();
	afx_msg void IniStat();
	afx_msg void IniStatTTS();
	afx_msg void ShowSubStat();
	afx_msg void ShowSubStatOneSub();
	afx_msg void ShowCn2Stat();
	afx_msg void ShowCn2StatOneSub();
	afx_msg void OnBnClickedBtnwfsleschoisesave();
	afx_msg void OnBnClickedBtnwfsprop();
	afx_msg void OnBnClickedBtncnnctwfs2();
	afx_msg void OnBnClickedBtnsetlmore();
	afx_msg void OnBnClickedBtngrab();
	afx_msg void OnBnClickedBtnwfsmirrconnect();
	afx_msg void OnBnClickedBtnwfsmirrshowgui();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedBsavezrnk();
	afx_msg void OnBnClickedBgrabfo();
	afx_msg void OnBnClickedBsetvlttoe();
	afx_msg void OnBnClickedBtncloseloopwfs();
	afx_msg void OnBnClickedBstatwfs();
	afx_msg void OnBnClickedBfonext();
	afx_msg void OnBnClickedBfoprev();
	afx_msg void OnBnClickedBtncnncttts();
	afx_msg void OnBnClickedBtnsetexptts();
	afx_msg void OnBnClickedBtnonetts();
	afx_msg void OnBnClickedBtnstarttts();
	afx_msg void OnBnClickedBtnttsprop();
	afx_msg void OnBnClickedBtngereftts();
	afx_msg void OnBnClickedBstattts();
	afx_msg void OnBnClickedBtnstoptts();
	afx_msg void OnBnClickedBtnanlprop();
};
