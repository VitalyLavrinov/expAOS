#include "pch.h"
#include "CWFSControler.h" 


CWFSControler::CWFSControler() : CSensor() {
	m_MConnected = false;
	m_HVon = false;

	m_coef = 0.5;
	m_u0 = 100.0;
	std::string fo = FODIR;
	WFSMirrLoadFO(fo);
}

CWFSControler::CWFSControler(const std::string& ini, const char* CamId, const std::string& ename) : CSensor(ini, CamId, ename) {
	m_MConnected = false;
	m_HVon = false;
	LoadMirrIni(ini);
	std::string fo = FODIR;
	WFSMirrLoadFO(fo);
}

void CWFSControler::LoadMirrIni(const std::string& ini) {
	pt::ptree bar;
	pt::ini_parser::read_ini(ini, bar);
	m_nact = std::stod(bar.get<std::string>("Controler.nact"));
	m_coef = std::stod(bar.get<std::string>("Controler.coef"));
	m_u0= std::stod(bar.get<std::string>("Controler.u0"));
}

void CWFSControler::SaveMirrIni(const std::string& ini) {
	pt::ptree bar;
	bar.put("General.speccnt", Get_m_Speccnt());
	bar.put("General.offsetx", GetOffsetx());
	bar.put("General.offsety", GetOffsety());
	bar.put("General.cdx", Get_cdx());
	bar.put("General.cdy", Get_cdy());
	bar.put("General.sdx", Get_sdx());
	bar.put("General.sdy", Get_sdy());
	bar.put("General.srastr", Get_srastr());
	bar.put("General.swnd", Get_swnd());
	bar.put("General.top", Get_top());
	bar.put("General.left", Get_left());
	bar.put("General.shftcol", Get_shftcol());
	bar.put("General.shftrow", Get_shftrow());
	bar.put("General.norm", Get_norm());
	bar.put("General.sub", Get_sub());
	bar.put("General.treshlow", Get_treshlow());
	bar.put("General.treshhigh", Get_treshhigh());
	bar.put("General.subtrah", Get_subtrah());
	bar.put("General.expos", GetExpos());

	bar.put("System.szelens", Get_szelens());
	bar.put("System.pixsze", Get_pixsze());
	bar.put("System.lwave", Get_lwave());
	bar.put("System.focuscam", Get_focuscam());
	bar.put("System.lpath", Get_lpath());
	bar.put("System.ngrid", Get_ngrid());
	bar.put("System.npzrnk", Get_npzrnk());

	bar.put("Statistics.entrancediameter", Get_entrancediameter());
	bar.put("Statistics.distbetsub", Get_distbetsub());
	bar.put("Statistics.r0k1", Get_r0k1());
	bar.put("Statistics.r0k2", Get_r0k2());
	bar.put("Statistics.r0k3x", Get_r0k3x());
	bar.put("Statistics.r0k3y", Get_r0k3y());
	bar.put("Statistics.cn2k1", Get_cn2k1());

	bar.put("Controler.coef", m_nact);
	bar.put("Controler.coef", m_coef);
	bar.put("Controler.u0", m_u0);
	pt::write_ini(ini, bar);
}

static int LoadMirrorDLL(LPCSTR dllname, HMODULE& h, WFC_INTERFACE* bi)// loaded from VISIONICA example
{
	int Ver;
	int i = tLoadDLL(dllname, h, bi);
	if (i) {
		i = tLoadDLL(dllname, h, (MIRROR_INTERFACE6_S*)bi);
		if (i) {
			i = tLoadDLL(dllname, h, (MIRROR_INTERFACE5_S*)bi);
			if (i) {
				i = tLoadDLL(dllname, h, (MIRROR_INTERFACE4_S*)bi);
				if (i) {
					i = tLoadDLL(dllname, h, (MIRROR_INTERFACE3_S*)bi);
					if (i) {
						i = tLoadDLL(dllname, h, (MIRROR_INTERFACE2_S*)bi);
						if (i) {
							i = tLoadDLL(dllname, h, (MIRROR_INTERFACE_S*)bi);
							if (i)
								Ver = 0;
							else
								Ver = 1;
						}
						else
							Ver = 2;
					}
					else
						Ver = 3;
				}
				else
					Ver = 4;
			}
			else
				Ver = 5;
		}
		else
			Ver = 6;
	}
	else
		Ver = 7;
	return i;
}

//connect mirror see const DEVN, DEVSTR for U-Flex-56-HEX-59
void CWFSControler::WFSMirrConnect() {
	m_MConnected = false;
	if (LoadMirrorDLL(DEVSTR, hdll, &m_WFCI) == 0) {
		if (m_WFCI.MirrorCheckDevNum) {
			unsigned x = 0;
			x = m_WFCI.MirrorCheckDevNum(DEVN);
			if (x == 0xffffffff) {
				m_MConnected = false;
			} else  m_MConnected = true;
		}
	}
}
// show GUI from mirror manufacturer
void CWFSControler::WFSMirrShowGUI() {
	m_WFCI.MirrorControlShow();
}
// mirror power up
void CWFSControler::WFSMirrPowerUp() {
	m_HVon = false;
	if (m_WFCI.MirrorSetPower(TRUE)) {
		if (m_WFCI.MirrorSetHighVoltage(TRUE)) {
			m_HVon = true;
		}
	}
}
// mirror power down
void CWFSControler::WFSMirrPowerDown() {
	if (m_WFCI.MirrorSetHighVoltage(FALSE)) {
		if (m_WFCI.MirrorSetPower(FALSE)) {
			m_HVon = false;
		}
	}
}


void CWFSControler::WFSMirrSetUGroupClose(cv::Mat& D)
{
	UApplied = Uregister + ( D * m_coef);
	UApplied.copyTo(Uregister);
	int actid = 0;
	for (int i = 1; i <= m_nact; i++) {
		if (actarray.at<UINT8>(i - 1) == 1) {
			m_WFCI.MirrorSetGroupVoltage(i * -1, UApplied.at<double>(actid), false); //if -i then voltage  written to the mirror memory only
			actid++;
		}
		else m_WFCI.MirrorSetGroupVoltage(i * -1, 0.0, false);
	}
	//if (m_HVon) 
	m_WFCI.MirrorSetGroupVoltage(0, 0., true);//apply voltages all at once
}

//set voltages on mirror all at once
void CWFSControler::WFSMirrSetUGroup(cv::Mat& D)
{
	UApplied = D * m_coef;
	int actid = 0;
	for (int i = 1; i <=m_nact; i++) {
		if (actarray.at<UINT8>(i-1) == 1) {
			m_WFCI.MirrorSetGroupVoltage(i * -1, UApplied.at<double>(actid), false); //if -i then voltage  written to the mirror memory only
			actid++;
		}
	    else m_WFCI.MirrorSetGroupVoltage(i * -1, 0.0, false);
	}
	//if (m_HVon) 
		m_WFCI.MirrorSetGroupVoltage(0, 0., true);//apply voltages all at once
}
//set all 0.0 voltages
void CWFSControler::WFSMirrSetAllZero() {
	//if (m_HVon) 
	m_WFCI.MirrorSetAllVoltage(0.0, true);
}

//set on id mirror electrode voltage val another set 0.0
void CWFSControler::WFSMirrSetActU(int id, double val)
{
	//if (m_HVon) {
		m_WFCI.MirrorSetAllVoltage(0.0, true);
		m_WFCI.MirrorSetVoltage(id, val, true);
	//}
}

//calculate mirror electrode voltages from frame zrnk coef.
void CWFSControler::Get_WFSMirrU(cv::Mat& F, cv::Mat& frameplnms, cv::Mat& U) {
	U =frameplnms*F;
	U = U * m_u0 * -1; // opposite mirror surface -1
}
//prepare F matrix, F=AT*(AT*A)^(-1)
void CWFSControler :: Pre_WFSMirrPlnm(cv::Mat& FO, cv::Mat& rf){
	cv::Mat ry, rcft;
	rcft = cv::Mat::zeros(m_nactiveact, m_nactiveact, CV_64FC1);//AT*A 
	ry = cv::Mat::zeros(m_nactiveact, m_nactiveact, CV_64FC1);//(AT*A)^(-1) 
	//FileStorage fs;// ("rf.xml", FileStorage::WRITE);
	//fs.open("rf.yml", FileStorage::WRITE);
	cv::mulTransposed(FO, rcft, FALSE);//B=(A*AT)  
	cv::invert(rcft, ry, cv::DECOMP_SVD);// C=B^(-1)=(A*AT)^(-1)  
	rf = FO.t() * ry;
	//fs << "rf" << rf;
	//fs.release();
}

//check mirror response functions from FO DIR in m_nactiveact calc number of active
/*  if 'i' FO file for 'i' electrode is exist
	then m_nactiveact++
	& 'i' actuator is active (actarray[i]=1)
*/
int CWFSControler::WFSMirrCheckFO(std::string& dirname) {
	std::string strf;


	UApplied = cv::Mat::zeros(1, m_nact, CV_64FC1);
	Uregister = cv::Mat::zeros(1, m_nact, CV_64FC1);
	actarray = cv::Mat::zeros(1, m_nact, CV_8UC1);

	m_nactiveact = 0;
	for (int j = 0; j < m_nact; j++) {
		strf = dirname + "\\controlF" + std::to_string(j) + ".txt";
		if (fileExists(strf)) {
			m_nactiveact++;
			actarray.at<UINT8>(j) = 1;
		} else actarray.at<UINT8>(j) = 0;
	}

	if (m_nactiveact == 0)m_nactiveact = 1;

	FOMirrAll = cv::Mat::zeros(m_nactiveact, Get_npzrnk(), CV_64FC1);
	FOMirr = cv::Mat::zeros(m_nactiveact, Get_npzrnk() - 2, CV_64FC1);
	UMirrAll = cv::Mat::zeros(1, m_nactiveact, CV_64FC1);

	rfPL = cv::Mat::zeros(Get_npzrnk() - 2, m_nactiveact, CV_64FC1);
	rfPLAll = cv::Mat::zeros(Get_npzrnk(), m_nactiveact, CV_64FC1);
	return m_nactiveact;
}

//load mirror response functions from FO DIR
int CWFSControler::WFSMirrLoadFO(std::string& dirname) {
	int actid = 0;
	std::string strf;
	double tmp;

	int res=WFSMirrCheckFO(dirname);

	for (int j = 0; j < m_nact; j++) {
		strf =  dirname + "\\controlF" + std::to_string(j) + ".txt";
		if (fileExists(strf)) {
			std::ifstream in(strf); 
			if (in.is_open())
			{
				for (int i = 0; i < Get_npzrnk(); i++) {
					in >> tmp;
					FOMirrAll.at<double>(actid, i) = tmp;
					if (i > 1)FOMirr.at<double>(actid, i - 2) = tmp;
				}
				in.close();
				actid++;
			}
		}
	}

	Pre_WFSMirrPlnm(FOMirrAll, rfPLAll);
	Pre_WFSMirrPlnm(FOMirr, rfPL);
	return res;
}
