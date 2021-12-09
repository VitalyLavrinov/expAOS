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

		return TRUE;  // return TRUE unless you set the focus to a control
}
