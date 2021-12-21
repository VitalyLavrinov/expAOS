#pragma once


// CWFSPROP dialog

class CWFSPROP : public CDialogEx
{
	DECLARE_DYNAMIC(CWFSPROP)

public:
	CWFSPROP(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CWFSPROP();
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DWFSPROP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_SDX;
	CString m_SDY;
	CString m_SWND;
	CString m_TOP;
	CString m_LEFT;
	CString m_SHFTCOL;
	CString m_SHFTROW;
	CString m_NORM;
	CString m_SUB;
	CString m_NGRID;
	CString m_NLENS;
	CString m_SRASTR;
	CString m_NPZRNK;
	CString m_TRESHLOW;
	CString m_TRESHHIGH;

	CString m_SZELENS;
	CString m_PIXSZE;
	CString m_LWAVE;
	CString m_FOCUSCAM;
	CString m_LPATH;
	CString m_COEFSHIFT;
	CString m_SUBTRAH;

	CString m_ENTRANCESUB;
	CString m_DISTBET;
};
