#pragma once


// CVLT dialog

class CVLT : public CDialogEx
{
	DECLARE_DYNAMIC(CVLT)

public:
	CVLT(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CVLT();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DVLT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_VLTS;
};
