#pragma once


// CCT dialog

class CCT : public CDialogEx
{
	DECLARE_DYNAMIC(CCT)

public:
	CCT(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCT();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CListCtrl m_CT;

	virtual BOOL OnInitDialog();
};
