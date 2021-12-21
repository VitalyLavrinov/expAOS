#pragma once


// CSTAT dialog

class CSTAT : public CDialogEx
{
	DECLARE_DYNAMIC(CSTAT)

public:
	CSTAT(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSTAT();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DSTAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
};
