// 
//
//
// need to use JSON instead of cv::Mat serialization becouse it isn't work in _DEBUG & DM mirror drivers only win32 and for _DEBUG:(
//
//
//

#include "pch.h"
#include "framework.h"
#include "expAOS.h"
#include "expAOSDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment( lib, "VimbaC.lib")

std::string WFSini = "defaultWFS.ini"; // change to const later
std::string Lensini = "strlenses.ini";
CSensor pWFS(WFSini, "DEV_1AB228000271");

CCT pCT; //CT write window
CZRNK pZRNK;  //Zernike write & wavefront drow window
std::vector<CButton*> pButton;//active lenses checkbuttons

CexpAOSDlg::CexpAOSDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXPAOS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CexpAOSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CexpAOSDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BtnOneWFS, &CexpAOSDlg::OnBnClickedBtnonewfs)
	ON_BN_CLICKED(IDC_BtnSetExpWFS, &CexpAOSDlg::OnBnClickedBtnsetexpwfs)
	ON_BN_CLICKED(IDC_BtnStartWFS, &CexpAOSDlg::OnBnClickedBtnstartwfs)
	ON_BN_CLICKED(IDC_BtnStopWFS, &CexpAOSDlg::OnBnClickedBtnstopwfs)
	ON_BN_CLICKED(IDC_BtnGeRefWFS, &CexpAOSDlg::OnBnClickedBtngerefwfs)
	ON_BN_CLICKED(IDC_BtnCloseWFS, &CexpAOSDlg::OnBnClickedBtnclosewfs)
	ON_BN_CLICKED(IDC_BtnWFSLESCHOISE, &CexpAOSDlg::OnBnClickedBtnwfsleschoise)
	ON_BN_CLICKED(IDC_BtnWFSLESCHOISESAVE, &CexpAOSDlg::OnBnClickedBtnwfsleschoisesave)
	ON_BN_CLICKED(IDC_BtnWfsProp, &CexpAOSDlg::OnBnClickedBtnwfsprop)
	ON_BN_CLICKED(IDC_BtnCnnctWFS2, &CexpAOSDlg::OnBnClickedBtncnnctwfs2)
	ON_BN_CLICKED(IDC_BtnSetLMore, &CexpAOSDlg::OnBnClickedBtnsetlmore)
	ON_BN_CLICKED(IDC_BtnGrab, &CexpAOSDlg::OnBnClickedBtngrab)
END_MESSAGE_MAP()



void CexpAOSDlg::ShowFrameDataWfs(cv::Mat &out) {

	pWFS.DrowFrame(out, 5, 5, 0.5);
	pWFS.DrowSub(out,out.cols/2+20,5,4.0);
	pWFS.GetCTOffsetsWfs(out);
	ShowCT();

	//pWFS.GetCTWfsCorr(out);

	//pWFS.wfCT = pWFS.Get_Zrnk(pWFS.CTMaxDif, pWFS.Fmat);
	pWFS.wfCTl = pWFS.Get_Zrnk(pWFS.CTMaxDifl, pWFS.Fmatl);
	//pWFS.wfCTTT = pWFS.Get_Zrnk(pWFS.CTMaxDif, pWFS.FmatTT);
	//pWFS.wfCTTTl = pWFS.Get_Zrnk(pWFS.CTMaxDifl, pWFS.FmatTTl);

	//pWFS.wfCorr = pWFS.Get_Zrnk(pWFS.CTCorr, pWFS.Fmat);
	//pWFS.wfCorrl = pWFS.Get_Zrnk(pWFS.CTCorrl, pWFS.Fmatl);
	//ShowCorr();

	ShowZRNK();
    /*
	pWFS.GetWF(pWFS.wfCT, 0, 0);
	pWFS.DrowPhase(460, 30, 1.0, 0);
	pWFS.DrowPhase(460, pWFS.Get_ngrid() + 40, 1.0, 1);
	pWFS.GetWF(pWFS.wfCT, 2, 0);
	pWFS.DrowPhase(460 + pWFS.Get_ngrid() + 10, 30, 1.0, 0);
	pWFS.DrowPhase(460 + pWFS.Get_ngrid() + 10, pWFS.Get_ngrid() + 40, 1.0, 1);
	pWFS.GetWF(pWFS.wfCTTT, 0, 2);
	pWFS.DrowPhase(460 + 2 * pWFS.Get_ngrid() + 20, 30, 1.0, 0);
	pWFS.DrowPhase(460 + 2 * pWFS.Get_ngrid() + 20, pWFS.Get_ngrid() + 40, 1.0, 1);

	pWFS.GetWF(pWFS.wfCorr, 0, 0);
	pWFS.DrowPhase(460 + 3 * pWFS.Get_ngrid() + 30, 30, 1.0, 0);
	pWFS.DrowPhase(460 + 3 * pWFS.Get_ngrid() + 30, pWFS.Get_ngrid() + 40, 1.0, 1);
	pWFS.GetWF(pWFS.wfCorr, 2, 0);
	pWFS.DrowPhase(460 + 4 * pWFS.Get_ngrid() + 40, 30, 1.0, 0);
	pWFS.DrowPhase(460 + 4 * pWFS.Get_ngrid() + 40, pWFS.Get_ngrid() + 40, 1.0, 1);
	*/
//	pWFS.GetWF(pWFS.wfCTl, 0, 0);
//	pWFS.DrowPhase(460, 2*pWFS.Get_ngrid()+ 80, 1.0, 0);
//	pWFS.DrowPhase(460, 3*pWFS.Get_ngrid() + 90, 1.0, 1);
	pWFS.GetWF(pWFS.wfCTl, 2, 0);
	pWFS.DrowPhase(460 + pWFS.Get_ngrid() + 10, 2 * pWFS.Get_ngrid() + 80, 1.0, 0);
	pWFS.DrowPhase(460 + pWFS.Get_ngrid() + 10, 3 * pWFS.Get_ngrid() + 90, 1.0, 1);
	//pWFS.GetWF(pWFS.wfCTTTl, 0, 2);
	//pWFS.DrowPhase(460 + 2 * pWFS.Get_ngrid() + 20, 2 * pWFS.Get_ngrid() + 80, 1.0, 0);
	//pWFS.DrowPhase(460 + 2 * pWFS.Get_ngrid() + 20, 3 * pWFS.Get_ngrid() + 90, 1.0, 1);
	/*
	pWFS.GetWF(pWFS.wfCorrl, 0, 0);
	pWFS.DrowPhase(460 + 3 * pWFS.Get_ngrid() + 30, 2 * pWFS.Get_ngrid() + 80, 1.0, 0);
	pWFS.DrowPhase(460 + 3 * pWFS.Get_ngrid() + 30, 3 * pWFS.Get_ngrid() + 90, 1.0, 1);
	pWFS.GetWF(pWFS.wfCorrl, 2, 0);
	pWFS.DrowPhase(460 + 4 * pWFS.Get_ngrid() + 40, 2 * pWFS.Get_ngrid() + 80, 1.0, 0);
	pWFS.DrowPhase(460 + 4 * pWFS.Get_ngrid() + 40, 3 * pWFS.Get_ngrid() + 90, 1.0, 1);
	*/
}

// CexpAOSDlg message handlers
void CexpAOSDlg::IniCT() {
	pCT.m_CT.DeleteAllItems();
	pCT.m_CT.InsertColumn(0, "�", LVCFMT_LEFT, 30);
	pCT.m_CT.InsertColumn(1, "Max", LVCFMT_LEFT, 35);
	pCT.m_CT.InsertColumn(2, "MaxX", LVCFMT_LEFT, 35);
	pCT.m_CT.InsertColumn(3, "MaxY", LVCFMT_LEFT, 35);
	pCT.m_CT.InsertColumn(4, "CTX", LVCFMT_LEFT, 50);
	pCT.m_CT.InsertColumn(5, "CTY", LVCFMT_LEFT, 50);
	pCT.m_CT.InsertColumn(6, "DifX", LVCFMT_LEFT, 50);
	pCT.m_CT.InsertColumn(7, "DifY", LVCFMT_LEFT, 50);
	pCT.m_CT.InsertColumn(8, "ReffX", LVCFMT_LEFT, 50);
	pCT.m_CT.InsertColumn(9, "ReffY", LVCFMT_LEFT, 50);


	DWORD ExStyle = pCT.m_CT.GetExtendedStyle();
	ExStyle |= LVS_EX_FULLROWSELECT;
	ExStyle |= LVS_EX_GRIDLINES;
	pCT.m_CT.SetExtendedStyle(ExStyle);
	CString estr;
	for (int i = 0; i < pWFS.Get_srastr()* pWFS.Get_srastr(); i++) {
		estr.Format("%u", i + 1);
		pCT.m_CT.InsertItem(i, estr);
		pCT.m_CT.SetItemText(i, 1, "0");
		pCT.m_CT.SetItemText(i, 2, "0");
		pCT.m_CT.SetItemText(i, 3, "0");
		pCT.m_CT.SetItemText(i, 4, "0");
		pCT.m_CT.SetItemText(i, 5, "0");
		pCT.m_CT.SetItemText(i, 6, "0");
		pCT.m_CT.SetItemText(i, 7, "0");
		pCT.m_CT.SetItemText(i, 8, "0");
		pCT.m_CT.SetItemText(i, 9, "0");
	}
	pCT.m_CT.SetTextBkColor(RGB(238, 176, 108));
	pCT.m_CT.SetBkColor(RGB(238, 176, 108));
}

void CexpAOSDlg::ShowCT()
{
	CString str;
	int k = 0;
	int n = 0;
		for (int j = 0; j < pWFS.Get_srastr(); j++) 
			for (int i = 0; i < pWFS.Get_srastr(); i++) {
				//if (pWFS.Lenses.at<int>(n) != 0) {
					str.Format("%u", n + 1);
					pCT.m_CT.SetItemText(n, 0, str);
					str.Format("%d", static_cast<int>(pWFS.MaxI.at<double>(i, j)));
					pCT.m_CT.SetItemText(n, 1, str);
					str.Format("%d", pWFS.subMax(i, j).x);
					pCT.m_CT.SetItemText(n, 2, str);
					str.Format("%d", pWFS.subMax(i, j).y);
					pCT.m_CT.SetItemText(n, 3, str);
					str.Format("%3.2f", pWFS.CTMax.at<double>(k));
					pCT.m_CT.SetItemText(n, 4, str);
					str.Format("%3.2f", pWFS.CTMax.at<double>(k + 1));
					pCT.m_CT.SetItemText(n, 5, str);
					str.Format("%3.2f", pWFS.CTMaxDif.at<double>(k));
					pCT.m_CT.SetItemText(n, 6, str);
					str.Format("%3.2f", pWFS.CTMaxDif.at<double>(k + 1));
					pCT.m_CT.SetItemText(n, 7, str);
					str.Format("%3.2f", pWFS.CTMaxReff.at<double>(k));
					pCT.m_CT.SetItemText(n, 8, str);
					str.Format("%3.2f", pWFS.CTMaxReff.at<double>(k + 1));
					pCT.m_CT.SetItemText(n, 9, str);
					k += 2;
				//}
				n++;
		}
}

void CexpAOSDlg::ShowSubStat() {
	CString str;
	str.Format("Current sub:%d", pWFS.Get_sub());
	SetDlgItemText(IDC_SSUB, str);
	str.Format("%3.2f", pWFS.Get_CTm_subinPoint().x);
	SetDlgItemText(IDC_SMINX, str);
	str.Format("%3.2f", pWFS.Get_CTm_subinPoint().y);
	SetDlgItemText(IDC_SMINY, str);
	str.Format("%3.2f", pWFS.Get_maxindec().x);
	SetDlgItemText(IDC_SMAXX, str);
	str.Format("%3.2f", pWFS.Get_maxindec().y);
	SetDlgItemText(IDC_SMAXY, str);
	str.Format("%3.2f", pWFS.Get_averindec().x);
	SetDlgItemText(IDC_SAVERX, str);
	str.Format("%3.2f", pWFS.Get_averindec().y);
	SetDlgItemText(IDC_SAVERY, str);
}

void CexpAOSDlg::ShowCn2Stat() {
	CString str;

	str.Format("%2.4f", pWFS.Get_Dispersion());
	SetDlgItemText(IDC_SDISPY, str);
	str.Format("%2.4f", pWFS.Get_Cn2());
	SetDlgItemText(IDC_SCNY, str);
	str.Format("%2.4f", pWFS.Get_R0());
	SetDlgItemText(IDC_SR0Y, str);
}

void CexpAOSDlg::ShowCorr()
{
	CString str;
	int k = 0;
	int n = 0;
	for (int j = 0; j < pWFS.Get_srastr(); j++)
		for (int i = 0; i < pWFS.Get_srastr(); i++) {
			//if (pWFS.Lenses.at<int>(n) != 0) {
			str.Format("%3.2f", pWFS.CTCorr.at<double>(k));
			pCT.m_CT.SetItemText(n, 8, str);
			str.Format("%3.2f", pWFS.CTCorr.at<double>(k + 1));
			pCT.m_CT.SetItemText(n, 9, str);
			k += 2;
			//}
			n++;
		}
}

void CexpAOSDlg::IniZRNK() {
	pZRNK.m_ZRNK.DeleteAllItems();
	pZRNK.m_ZRNK.InsertColumn(0, "Cell", LVCFMT_LEFT, 20);
	pZRNK.m_ZRNK.InsertColumn(1, "Zrnk", LVCFMT_LEFT, 70);
	pZRNK.m_ZRNK.InsertColumn(2, "Zrnk_Lens", LVCFMT_LEFT, 70);
	pZRNK.m_ZRNK.InsertColumn(3, "Zrnk_TT", LVCFMT_LEFT, 70);
	pZRNK.m_ZRNK.InsertColumn(4, "Zrnk_TTl", LVCFMT_LEFT, 70);
	pZRNK.m_ZRNK.InsertColumn(5, "Zrnk_Corr", LVCFMT_LEFT, 70);
	pZRNK.m_ZRNK.InsertColumn(6, "Zrnk_Corrl", LVCFMT_LEFT, 70);
	DWORD ExStyle = pZRNK.m_ZRNK.GetExtendedStyle();
	ExStyle |= LVS_EX_FULLROWSELECT; 
	ExStyle |= LVS_EX_GRIDLINES; 
	pZRNK.m_ZRNK.SetExtendedStyle(ExStyle);
	CString estr;

	for (int i = 1; i < 36; i++) {
		estr.Format("%u", i);
		pZRNK.m_ZRNK.InsertItem(i, estr);
		pZRNK.m_ZRNK.SetItemText(i, 1, "0");
		pZRNK.m_ZRNK.SetItemText(i, 2, "0");
		pZRNK.m_ZRNK.SetItemText(i, 3, "0");
		pZRNK.m_ZRNK.SetItemText(i, 4, "0");
		pZRNK.m_ZRNK.SetItemText(i, 5, "0");
		pZRNK.m_ZRNK.SetItemText(i, 6, "0");
	}
}

/*creating checkbuttons array for active lenses choice*/
void CexpAOSDlg::IniLensButt() {
	CButton* pButt;
	CString buttid;
	CRect rct;

	for (int i = 0; i < pWFS.Get_srastr(); i++)
		for (int j = 0; j < pWFS.Get_srastr(); j++){
		
		pButt = new(CButton);
		buttid.Format("%d", pWFS.Get_srastr()*j+ i + 1);
		rct.left = pWFS.tPtr.at<cv::Point>(pWFS.Get_srastr() * i + j).x + 5;
		rct.top = pWFS.tPtr.at<cv::Point>(pWFS.Get_srastr() * i + j).y + 5;
		rct.right = pWFS.tPtr.at<cv::Point>(pWFS.Get_srastr() * i + j).x +50;
		rct.bottom = pWFS.tPtr.at<cv::Point>(pWFS.Get_srastr()*i+ j).y + 20;
		if (pButt->Create(buttid, BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE, rct, this, 1500 + pWFS.Get_srastr() * j + i)) {
			if (pWFS.Lenses.at<int>(pWFS.Get_srastr() * j + i) == 1) {
				pButt->SetCheck(BST_CHECKED);
				pButt->ShowWindow(SW_HIDE);
				pButt->EnableWindow(FALSE);
			}
			else {
				pButt->SetCheck(BST_UNCHECKED);
				pButt->ShowWindow(SW_HIDE);
				pButt->EnableWindow(FALSE);

			}
		}
		pButton.push_back(pButt);
	}
}

void CexpAOSDlg::ShowZRNK() {
	CString str;
	int n = 0;
	for (int i = 0; i < pWFS.Get_npzrnk(); i++) {
		str.Format("%u", i + 1);
		pZRNK.m_ZRNK.SetItemText(i, 0, str);

		//str.Format("%3.2f", pWFS.wfCT.at<double>(i));
		//pZRNK.m_ZRNK.SetItemText(i, 1, str);

		str.Format("%3.2f", pWFS.wfCTl.at<double>(i));
		pZRNK.m_ZRNK.SetItemText(i, 2, str);

		//if (i > 1)str.Format("%3.2f", pWFS.wfCTTT.at<double>(i - 2));
		//else str = "_";
		//pZRNK.m_ZRNK.SetItemText(i, 3, str);

		//if (i > 1)str.Format("%3.2f", pWFS.wfCTTTl.at<double>(i - 2));
		//else str = "_";
		//pZRNK.m_ZRNK.SetItemText(i, 4, str);

		//str.Format("%3.2f", pWFS.wfCorr.at<double>(i));
		//pZRNK.m_ZRNK.SetItemText(i, 5, str);

		//str.Format("%3.2f", pWFS.wfCorrl.at<double>(i));
		//pZRNK.m_ZRNK.SetItemText(i, 6, str);

	}
}

BOOL CexpAOSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	pWFS.CheckLens(Lensini);

	pCT.Create(IDD_CT, this);
	pCT.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE);
	pZRNK.Create(IDD_ZRNK, this);
	pZRNK.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE);

	pWFS.SetDC(this->GetDC());
	pWFS.SetWfDC(pZRNK.GetDC());
	IniCT();
	IniZRNK();
	IniLensButt();

	SetDlgItemInt(IDC_EWFSREFCNT, 100);
	SetDlgItemText(IDC_ESCL, "250.0");
	SetDlgItemInt(IDC_ELENI, 50);
	SetDlgItemInt(IDC_CNTGRAB, 1000);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CexpAOSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CexpAOSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CexpAOSDlg::OnBnClickedBtnonewfs()
{
	if (pWFS.GetConnected()) {
		if (!pWFS.m_FSTART) {
			pCT.ShowWindow(SW_SHOW);
			pZRNK.ShowWindow(SW_SHOW);
			if (pWFS.CameraFrame()) {
			//	pWFS.outframe = pWFS.GetFrameMat();
				ShowFrameDataWfs(pWFS.GetFrameMat());
			}
		}
		else SetDlgItemText(IDC_SSTSWFS, "Cam already runing");
	}
	else SetDlgItemText(IDC_SSTSWFS, "WFS NO connections!");
}

void CexpAOSDlg::OnBnClickedBtnsetexpwfs()
{
	if (pWFS.SetCamExpos(static_cast<float>(GetDlgItemInt(IDC_EEXPOS))) == 0)	SetDlgItemInt(IDC_EEXPOS, pWFS.GetCamExpos());
	else SetDlgItemText(IDC_EEXPOS, "err");
}

void CexpAOSDlg::OnBnClickedBtnstartwfs()
{
	if (pWFS.GetConnected()) {
		if (!pWFS.m_FSTART) {
			pCT.ShowWindow(SW_SHOW);
			pZRNK.ShowWindow(SW_SHOW);
			pWFS.m_FSTART = 1;
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			AfxBeginThread(ThreadDrowWFS, this, THREAD_PRIORITY_TIME_CRITICAL);
		}
		else SetDlgItemText(IDC_SSTSWFS, "Cam already runing");
	}
	else SetDlgItemText(IDC_SSTSWFS, "WFS NO connections!");
}

UINT ThreadDrowWFS(LPVOID pParam)
{
	CexpAOSDlg* ptrView = (CexpAOSDlg*)pParam;
	pWFS.setzerotime();
	while (pWFS.m_FSTART) {
			if (pWFS.CameraFrame()) {
				
				ptrView->ShowFrameDataWfs(pWFS.GetFrameMat());
                ptrView->SetDlgItemInt(IDC_SWFSFPS, static_cast<int>(pWFS.fpsiter()));
			}
	}
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
	return 0;
}

void CexpAOSDlg::OnBnClickedBtnstopwfs()
{
	pWFS.m_FSTART = 0;
}

void CexpAOSDlg::OnBnClickedBtngerefwfs()
{
	
	if (pWFS.GetConnected()) {
		if (!pWFS.m_FSTART) {
			pWFS.m_FSTART = 1;
			int cntref = GetDlgItemInt(IDC_EWFSREFCNT);
			if (cntref <= 0) cntref = 1;
			pWFS.GetRefFrame(cntref);
			pCT.ShowWindow(SW_SHOW);
			ShowCT();
			pWFS.DrowFrame(pWFS.outframe,5, 5, 0.5);
			pWFS.m_FSTART = 0;
		}
		else SetDlgItemText(IDC_SSTSWFS, "Stop cam!");
	}
	else SetDlgItemText(IDC_SSTSWFS, "WFS NO connections!");
}

void CexpAOSDlg::OnBnClickedBtnclosewfs()
{
	if (pWFS.GetConnected()) {
		if (!pWFS.m_FSTART) {
			pWFS.m_FSTART = 1;
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			AfxBeginThread(ThreadDrowWFSLoop, this, THREAD_PRIORITY_TIME_CRITICAL);
		}
		else SetDlgItemText(IDC_SSTSWFS, "Cam already runing");
	}
	else SetDlgItemText(IDC_SSTSWFS, "WFS NO connections!");
}

UINT ThreadDrowWFSLoop(LPVOID pParam)
{
	CexpAOSDlg* ptrView = (CexpAOSDlg*)pParam;
	CString str;
	ptrView->GetDlgItemText(IDC_ESCL, str);
	double scl = atof(str);
	int cnt;
	double fd;
	pWFS.CCamera::SetDC(ptrView->GetDC());
	int i = 0;
	pWFS.setzerotime();
	cnt = pWFS.Get_m_Speccnt();
	while (pWFS.m_FSTART) {
		if (pWFS.CameraFrameN()) {
			for (int j = 0; j < FRAME_COUNT; j++) {
				if (pWFS.frames[j].receiveStatus == 0) {
					pWFS.outframe.data = static_cast<uchar*>(pWFS.frames[j].buffer);
					pWFS.frames[j].receiveStatus = -1;

					pWFS.GetCTOffsetsWfs(pWFS.outframe);
				    pWFS.CTDeqAdd();
					//pWFS.wfCT = pWFS.Get_Zrnk(pWFS.CTMaxDifl, pWFS.Fmatl);

					fd = pWFS.fpsiter();
					if (i >= cnt) {
						    pWFS.GetStatCTDeq();
							ptrView->ShowSubStat();
							fd > 0.0 ? fd = pWFS.getfps() : fd = 2.0;//gag
							pWFS.GetR0(pWFS.CTdecY);
							ptrView->ShowCn2Stat();
							pWFS.Spectrum(cnt - 1, 1.0, fd / 2, fd, pWFS.CTdecX, pWFS.SpecX);// x axis coords
							pWFS.Spectrum(cnt - 1, 1.0, fd / 2, fd, pWFS.CTdecY, pWFS.SpecY);// y axis coords
							pWFS.DrowSpectrum(cnt, 820, 280, scl, static_cast<int>(fd), pWFS.SpecX);
							pWFS.DrowSpectrum(cnt, 820, 600, scl, static_cast<int>(fd), pWFS.SpecY);

							ptrView->SetDlgItemInt(IDC_SWFSFPS, static_cast<int>(fd));
						i=0;
					}
			
					pWFS.CCamera::DrowFrame(pWFS.outframe, 5, 5, 0.5);
					
					
					i++;
				}
			}
			
		}
	}
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
	return 0;
}

void CexpAOSDlg::OnBnClickedBtnwfsleschoise()
{
	pWFS.m_FSTART = 0;
	for (int i = 0; i < pWFS.Get_srastr() * pWFS.Get_srastr();i++) {
		    pButton[i]->ShowWindow(SW_HIDE);
			pButton[i]->EnableWindow(TRUE);
			pButton[i]->ShowWindow(SW_SHOW);
		}
}

void CexpAOSDlg::OnBnClickedBtnwfsleschoisesave()
{
	for (int i = 0; i < pWFS.Get_srastr() * pWFS.Get_srastr();i++) {
		if (IsDlgButtonChecked(1500 + i)) pWFS.Lenses.at<int>(i) = 1;
		else pWFS.Lenses.at<int>(i) = 0;
	}
	pWFS.SaveLenses(Lensini);
	pWFS.CheckLens(Lensini);
}

void CexpAOSDlg::OnBnClickedBtnwfsprop()
{
	CWFSPROP pWFSPROP;
	CString str;

	pWFS.m_FSTART = 0;

	str.Format("%d", pWFS.Get_sdx());
	pWFSPROP.m_SDX = str; 
	str.Format("%d", pWFS.Get_sdy());
	pWFSPROP.m_SDY = str; 
	str.Format("%d", pWFS.Get_swnd());
	pWFSPROP.m_SWND = str; 
	str.Format("%d", pWFS.Get_top());
	pWFSPROP.m_TOP = str; 
	str.Format("%d", pWFS.Get_left());
	pWFSPROP.m_LEFT = str; 
	str.Format("%d", pWFS.Get_shftcol());
	pWFSPROP.m_SHFTCOL = str; 
	str.Format("%d", pWFS.Get_shftrow());
	pWFSPROP.m_SHFTROW = str;
	str.Format("%d", pWFS.Get_norm());
	pWFSPROP.m_NORM = str; 
	str.Format("%d", pWFS.Get_sub());
	pWFSPROP.m_SUB = str; 
	str.Format("%d", pWFS.Get_ngrid());
	pWFSPROP.m_NGRID = str; 
	str.Format("%d", pWFS.Get_nlens());
	pWFSPROP.m_NLENS = str; 
	str.Format("%d", pWFS.Get_srastr());
	pWFSPROP.m_SRASTR = str; 
	str.Format("%d", pWFS.Get_npzrnk());
	pWFSPROP.m_NPZRNK = str; 
	str.Format("%d", pWFS.Get_treshlow());
	pWFSPROP.m_TRESHLOW = str; 
	str.Format("%d", pWFS.Get_treshhigh());
	pWFSPROP.m_TRESHHIGH = str; 

	str.Format("%f", pWFS.Get_szelens());
	pWFSPROP.m_SZELENS = str; 
	str.Format("%f", pWFS.Get_pixsze());
	pWFSPROP.m_PIXSZE = str; 
	str.Format("%f", pWFS.Get_lwave());
	pWFSPROP.m_LWAVE = str;
	str.Format("%f", pWFS.Get_focuscam());
	pWFSPROP.m_FOCUSCAM = str;
	str.Format("%f", pWFS.Get_lpath());
	pWFSPROP.m_LPATH = str;
	str.Format("%f", pWFS.Get_coefshift());
	pWFSPROP.m_COEFSHIFT = str; 

	if (pWFSPROP.DoModal() == IDOK)
	{  
		atoi(pWFSPROP.m_SDX) >= 0 ? pWFS.Set_sdx(atoi(pWFSPROP.m_SDX)) : pWFS.Set_sdx(0);
		atoi(pWFSPROP.m_SDY) >= 0 ? pWFS.Set_sdy(atoi(pWFSPROP.m_SDY)) : pWFS.Set_sdy(0);
		atoi(pWFSPROP.m_SWND) >= 0 ? pWFS.Set_swnd(atoi(pWFSPROP.m_SWND)) : pWFS.Set_swnd(0);
		atoi(pWFSPROP.m_TOP) >= 0 ? pWFS.Set_top(atoi(pWFSPROP.m_TOP)) : pWFS.Set_top(0);
		atoi(pWFSPROP.m_LEFT) >= 0 ? pWFS.Set_left(atoi(pWFSPROP.m_LEFT)) : pWFS.Set_left(0);
		atoi(pWFSPROP.m_SHFTCOL) >= 0 ? pWFS.Set_shftcol(atoi(pWFSPROP.m_SHFTCOL)) : pWFS.Set_shftcol(0);
		atoi(pWFSPROP.m_SHFTROW) >= 0 ? pWFS.Set_shftrow(atoi(pWFSPROP.m_SHFTROW)) : pWFS.Set_shftrow(0);
		atoi(pWFSPROP.m_NORM) >= 0 ? pWFS.Set_norm(atoi(pWFSPROP.m_NORM)) : pWFS.Set_norm(0);
		atoi(pWFSPROP.m_SUB) >= 0 ? pWFS.Set_sub(atoi(pWFSPROP.m_SUB)) : pWFS.Set_sub(0);
		atoi(pWFSPROP.m_NGRID) >= 0 ? pWFS.Set_ngrid(atoi(pWFSPROP.m_NGRID)) : pWFS.Set_ngrid(0);
		atoi(pWFSPROP.m_NLENS) >= 0 ? pWFS.Set_nlens(atoi(pWFSPROP.m_NLENS)) : pWFS.Set_nlens(0);
		atoi(pWFSPROP.m_SRASTR) >= 0 ? pWFS.Set_srastr(atoi(pWFSPROP.m_SRASTR)) : pWFS.Set_srastr(0);
		atoi(pWFSPROP.m_NPZRNK) >= 0 ? pWFS.Set_npzrnk(atoi(pWFSPROP.m_NPZRNK)) : pWFS.Set_npzrnk(0);
		atoi(pWFSPROP.m_TRESHLOW) >= 0 ? pWFS.Set_treshlow(atoi(pWFSPROP.m_TRESHLOW)) : pWFS.Set_treshlow(0);
		atoi(pWFSPROP.m_TRESHHIGH) >= 0 ? pWFS.Set_treshhigh(atoi(pWFSPROP.m_TRESHHIGH)) : pWFS.Set_treshhigh(0);

		atof(pWFSPROP.m_SZELENS) >= 0 ? pWFS.Set_szelens(atof(pWFSPROP.m_SZELENS)) : pWFS.Set_szelens(0);
		atof(pWFSPROP.m_PIXSZE) >= 0 ? pWFS.Set_pixsze(atof(pWFSPROP.m_PIXSZE)) : pWFS.Set_pixsze(0);
		atof(pWFSPROP.m_LWAVE) >= 0 ? pWFS.Set_lwave(atof(pWFSPROP.m_LWAVE)) : pWFS.Set_lwave(0);
		atof(pWFSPROP.m_FOCUSCAM) >= 0 ? pWFS.Set_focuscam(atof(pWFSPROP.m_FOCUSCAM)) : pWFS.Set_focuscam(0);
		atof(pWFSPROP.m_LPATH) >= 0 ? pWFS.Set_lpath(atof(pWFSPROP.m_LPATH)) : pWFS.Set_lpath(0);
		atof(pWFSPROP.m_COEFSHIFT) >= 0 ? pWFS.Set_coefshift(atof(pWFSPROP.m_COEFSHIFT)) : pWFS.Set_coefshift(0);

		pWFS.SaveIni(WFSini);
		pWFS.ReLoadData();
		pWFS.CheckLens(Lensini);
	}
}

void CexpAOSDlg::OnBnClickedBtncnnctwfs2()
{
	if (!pWFS.GetConnected()) {
		pWFS.CameraConnect();
		if (pWFS.GetConnected()) {
			SetDlgItemInt(IDC_EEXPOS, pWFS.GetCamExpos());
			SetDlgItemText(IDC_SSTSWFS, "WFS connected.");
		}
		else SetDlgItemText(IDC_SSTSWFS, "WFS NO connections!");
	}
	else SetDlgItemText(IDC_SSTSWFS, "WFS connected ?");
}


void CexpAOSDlg::OnBnClickedBtnsetlmore()
{
	pWFS.SetLensesMoreThanI(GetDlgItemInt(IDC_ELENI));
}

void CexpAOSDlg::OnBnClickedBtngrab()
{

	if (pWFS.GetConnected()) {
		if (!pWFS.m_FSTART) {
			pWFS.m_FSTART = 1;
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			AfxBeginThread(ThreadGrabWFS, this, THREAD_PRIORITY_TIME_CRITICAL);
		}
		else SetDlgItemText(IDC_SSTSWFS, "Cam already runing");
	}
	else SetDlgItemText(IDC_SSTSWFS, "WFS NO connections!");
}

UINT ThreadGrabWFS(LPVOID pParam)
{
	CexpAOSDlg* ptrView = (CexpAOSDlg*)pParam;
	CString str;
	ptrView->GetDlgItemText(IDC_CNTGRAB, str);
	int cnt = atoi(str);
	pWFS.CCamera::SetDC(ptrView->GetDC());
	int i = 0;

	if (pWFS.CameraFrame()) {
		pWFS.CCamera::DrowFrame(pWFS.outframe, 5, 5, 0.5);
	}

	pWFS.setzerotime();
	while (i<cnt) {
		if (pWFS.CameraFrameN()) {
			for (int j = 0; j < FRAME_COUNT; j++) {
				if (pWFS.frames[j].receiveStatus == 0) {
					pWFS.outframe.data = static_cast<uchar*>(pWFS.frames[j].buffer);

					pWFS.AddFrameInBuf(pWFS.outframe);
	
					pWFS.frames[j].receiveStatus = -1;
					ptrView->SetDlgItemInt(IDC_SWFSFPS, static_cast<int>(pWFS.fpsiter()));
					i++;
				}
			}

		}
	}
	pWFS.CCamera::DrowFrame(pWFS.outframe, 5, 5, 0.5);

	//save file dlg
	CFileDialog fileDialog(FALSE, NULL, "*.flm");
	int result = fileDialog.DoModal();
	if (result == IDOK)
	{
		CString fnamestr = fileDialog.GetPathName();
		std::string stdfnamestr(fnamestr, fnamestr.GetLength());
		pWFS.SaveFrameBufInFile(stdfnamestr);
	}

	pWFS.m_FSTART = 0;
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
	return 0;
}