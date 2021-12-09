// CVLT.cpp : implementation file
//

#include "pch.h"
#include "expAOS.h"
#include "CVLT.h"
#include "afxdialogex.h"


// CVLT dialog

IMPLEMENT_DYNAMIC(CVLT, CDialogEx)

CVLT::CVLT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DVLT, pParent)
{

}

CVLT::~CVLT()
{
}

void CVLT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_VLTS);
}


BEGIN_MESSAGE_MAP(CVLT, CDialogEx)
	
END_MESSAGE_MAP()


// CVLT message handlers





