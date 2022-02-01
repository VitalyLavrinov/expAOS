#pragma once


// CANL dialog

class CANL : public CDialogEx
{
	DECLARE_DYNAMIC(CANL)

public:
	CANL(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CANL();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DANL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CSliderCtrl m_CLL;
	afx_msg void OnNMCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
