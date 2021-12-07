#pragma once


// CZRNK dialog

class CZRNK : public CDialogEx
{
	DECLARE_DYNAMIC(CZRNK)

public:
	CZRNK(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CZRNK();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ZRNK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ZRNK;
};
