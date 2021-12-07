// CZRNK.cpp : implementation file
//

#include "pch.h"
#include "expAOS.h"
#include "CZRNK.h"
#include "afxdialogex.h"


// CZRNK dialog

IMPLEMENT_DYNAMIC(CZRNK, CDialogEx)

CZRNK::CZRNK(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ZRNK, pParent)
{

}

CZRNK::~CZRNK()
{
}

void CZRNK::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZRNK, m_ZRNK);
}


BEGIN_MESSAGE_MAP(CZRNK, CDialogEx)


END_MESSAGE_MAP()


// CZRNK message handlers




