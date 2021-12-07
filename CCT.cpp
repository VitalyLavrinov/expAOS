// CCT.cpp : implementation file
//

#include "pch.h"
#include "expAOS.h"
#include "CCT.h"
#include "afxdialogex.h"


// CCT dialog

IMPLEMENT_DYNAMIC(CCT, CDialogEx)

CCT::CCT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CT, pParent)
{

}

CCT::~CCT()
{
}

void CCT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CT, m_CT);
}


BEGIN_MESSAGE_MAP(CCT, CDialogEx)

END_MESSAGE_MAP()


// CCT message handlers




BOOL CCT::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/*
	m_CT.DeleteAllItems();
	m_CT.InsertColumn(0, "¹", LVCFMT_LEFT, 30);
	m_CT.InsertColumn(1, "Max", LVCFMT_LEFT, 35);
	m_CT.InsertColumn(2, "MaxX", LVCFMT_LEFT, 35);
	m_CT.InsertColumn(3, "MaxY", LVCFMT_LEFT, 35);
	m_CT.InsertColumn(4, "CTX", LVCFMT_LEFT, 80);
	m_CT.InsertColumn(5, "CTY", LVCFMT_LEFT, 80);
	m_CT.InsertColumn(6, "DifX", LVCFMT_LEFT, 80);
	m_CT.InsertColumn(7, "DifY", LVCFMT_LEFT, 80);
	m_CT.InsertColumn(8, "CorrX", LVCFMT_LEFT, 80);
	m_CT.InsertColumn(9, "CorrY", LVCFMT_LEFT, 80);


	DWORD ExStyle = m_CT.GetExtendedStyle();
	ExStyle |= LVS_EX_FULLROWSELECT;
	ExStyle |= LVS_EX_GRIDLINES; 
	m_CT.SetExtendedStyle(ExStyle);
	CString estr;
	for (int i = 0; i < 144; i++) {
		estr.Format("%u", i + 1);
		m_CT.InsertItem(i, estr);
		m_CT.SetItemText(i, 1, "0");
		m_CT.SetItemText(i, 2, "0");
		m_CT.SetItemText(i, 3, "0");
		m_CT.SetItemText(i, 4, "0");
		m_CT.SetItemText(i, 5, "0");
		m_CT.SetItemText(i, 6, "0");
		m_CT.SetItemText(i, 7, "0");
		m_CT.SetItemText(i, 8, "0");
		m_CT.SetItemText(i, 9, "0");
	}
	m_CT.SetTextBkColor(RGB(238, 176, 108));
	m_CT.SetBkColor(RGB(238, 176, 108));
	*/
	return TRUE;  // return TRUE unless you set the focus to a control
}
