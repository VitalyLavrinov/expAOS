// CANL.cpp : implementation file
//

#include "pch.h"
#include "expAOS.h"
#include "CANL.h"
#include "afxdialogex.h"


// CANL dialog

IMPLEMENT_DYNAMIC(CANL, CDialogEx)

CANL::CANL(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DANL, pParent)
{

}

CANL::~CANL()
{
}

void CANL::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER, m_CLL);
}


BEGIN_MESSAGE_MAP(CANL, CDialogEx)

	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER, &CANL::OnNMCustomdrawSlider)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CANL message handlers






void CANL::OnNMCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CANL::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	// TODO: Add your message handler code here and/or call default
	if (pScrollBar == (CScrollBar*)&m_CLL) {
		int val = m_CLL.GetPos();
		SetDlgItemInt(IDC_SCLL, val);
		UpdateData(FALSE);
	}
	else {
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
	// TODO: Add your message handler code here and/or call default
	
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
