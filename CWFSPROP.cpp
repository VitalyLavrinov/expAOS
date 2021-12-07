// CWFSPROP.cpp : implementation file
//

#include "pch.h"
#include "expAOS.h"
#include "CWFSPROP.h"
#include "afxdialogex.h"


// CWFSPROP dialog

IMPLEMENT_DYNAMIC(CWFSPROP, CDialogEx)

CWFSPROP::CWFSPROP(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DWFSPROP, pParent)
	, m_SDX(_T(""))
	, m_SDY(_T(""))
	, m_SWND(_T(""))
	, m_TOP(_T(""))
	,  m_LEFT(_T(""))
	,  m_SHFTCOL(_T(""))
	,  m_SHFTROW(_T(""))
	,  m_NORM(_T(""))
	,  m_SUB(_T(""))
	,  m_NGRID(_T(""))
	,  m_NLENS(_T(""))
	,  m_SRASTR(_T(""))
	,  m_NPZRNK(_T(""))
	,  m_TRESHLOW(_T(""))
	,  m_TRESHHIGH(_T(""))
	,  m_SZELENS(_T(""))
	,  m_PIXSZE(_T(""))
	,  m_LWAVE(_T(""))
	,  m_FOCUSCAM(_T(""))
	,  m_LPATH(_T(""))
	,  m_COEFSHIFT(_T(""))

{

}

CWFSPROP::~CWFSPROP()
{
}

void CWFSPROP::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ESDX, m_SDX);
	DDX_Text(pDX, IDC_ESDY, m_SDY);
	DDX_Text(pDX, IDC_ESWND, m_SWND);
	DDX_Text(pDX, IDC_ELEFT, m_LEFT);
	DDX_Text(pDX, IDC_ETOP, m_TOP);
	DDX_Text(pDX, IDC_ESHFTCOL ,  m_SHFTCOL);
	DDX_Text(pDX, IDC_ESHFTROW ,  m_SHFTROW);
	DDX_Text(pDX, IDC_ENORM ,  m_NORM);
	DDX_Text(pDX, IDC_ESUB ,  m_SUB);
	DDX_Text(pDX, IDC_ENGRID ,  m_NGRID);
	DDX_Text(pDX, IDC_ENLENS ,  m_NLENS);
	DDX_Text(pDX, IDC_ESRASTR ,  m_SRASTR);
	DDX_Text(pDX, IDC_ENPZRNK ,  m_NPZRNK);
	DDX_Text(pDX, IDC_ETRESHLOW ,  m_TRESHLOW);
	DDX_Text(pDX, IDC_ETRESHHIGH ,  m_TRESHHIGH);

	DDX_Text(pDX, IDC_ESZELENS ,  m_SZELENS);
	DDX_Text(pDX, IDC_EPIXSZE ,  m_PIXSZE);
	DDX_Text(pDX, IDC_ELWAVE ,  m_LWAVE);
	DDX_Text(pDX, IDC_EFOCUSCAM ,  m_FOCUSCAM);
	DDX_Text(pDX, IDC_ELPATH ,  m_LPATH);
	DDX_Text(pDX, IDC_ECOEFSHIFT ,  m_COEFSHIFT);

}


BEGIN_MESSAGE_MAP(CWFSPROP, CDialogEx)
END_MESSAGE_MAP()


// CWFSPROP message handlers


