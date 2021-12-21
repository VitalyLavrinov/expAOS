#pragma once

#include "CSensor.h"  
#include "WFCinterface.h"//fckng realisation! only for DEBUG WIN32! & with mirror GUI
constexpr int DEVN(31);// for U-Flex-56-HEX-59
constexpr LPCSTR DEVSTR("eth_Xv622.dll");// dll for U-Flex-56-HEX-59
constexpr int NACT(59);// 59 electrodes are used
constexpr char FODIR[] = "FO";// mmmmmm  more then 20++ std::string DIR("FO");

class CWFSControler : public CSensor
{
private:
	WFC_INTERFACE m_WFCI; // mirror interface
	HMODULE hdll = NULL;//mirror dll handle
	/*Prop to control mirror*/
	double m_coef; // feedback ratio to get  mirror voltages 
	int m_nact;  //cnt mirror actuators
	double m_u0; // reference voltage( used to get mirror electrode response functions) 
	int m_nactiveact; // used  instead of "nact" if we have used  not all mirror electrodes
	bool m_MConnected;// mirror connected
	bool m_HVon;// high voltage ON/OFF

	void LoadMirrIni(const std::string& ini);//load inival for mirror


public:
	CWFSControler();
	CWFSControler(const std::string& ini, const char* CamId);
	~CWFSControler() //Must Set mirror poweroff & high Voltage to OFF !! if PC poweroff we will have problem.
	{
		if (Get_WFSMirrConnected()) {
			if (Get_WFSMirrHVon())WFSMirrSetAllZero();
			m_WFCI.MirrorSetHighVoltage(FALSE);
			m_WFCI.MirrorSetPower(FALSE); 
		}
		if (hdll) {
			FreeLibrary(hdll);
			hdll = NULL;
		}
	}
	void SaveMirrIni(const std::string& ini);//save inival for mirror
	void WFSMirrConnect();//connect mirror see const DEVN, DEVSTR for U-Flex-56-HEX-59
	void WFSMirrShowGUI();// show GUI from mirror manufacturer
	void WFSMirrPowerUp();// mirror power up
	void WFSMirrPowerDown();// mirror power down

	cv::Mat UApplied; //applied voltages with feedback ratio
	cv::Mat Uregister; //applied voltages with feedback ratio
	cv::Mat actarray; //active actuators

	void WFSMirrSetUGroupClose(cv::Mat& D);//set voltages from D on Mirror in close loop. Simultaneously!!

	void WFSMirrSetUGroup(cv::Mat& D);//set voltages from D on Mirror. Simultaneously!!
	void WFSMirrSetAllZero();//set all 0.0 voltages
	void WFSMirrSetActU(int id, double val); //set on id mirror electrode voltage val another set 0.0

	cv::Mat FOMirr;// mirror response functions (pzrnk coef. on each active electrod)
	cv::Mat FOMirrAll;// mirror response functions (pzrnk coef. on each active electrod)
	cv::Mat UMirrAll;// calculated voltage value for all electrodes
	cv::Mat UMirr;// calculated voltage value for all electrodes
	cv::Mat rfPL;//AT*(AT*A)^(-1)=F  U=B*F without Tip/Tilt
	cv::Mat rfPLAll;// -||- with Tip/Tilt

	int WFSMirrCheckFO(std::string& dirname);//check mirror response functions from FO DIR in m_nactiveact calc number of active
	int WFSMirrLoadFO(std::string& dirname);//load mirror response functions from FO DIR
	void Pre_WFSMirrPlnm(cv::Mat& FO, cv::Mat& rf);//prepare F matrix, F=AT*(AT*A)^(-1)
	void Get_WFSMirrU(cv::Mat& F, cv::Mat& frameplnms, cv::Mat& U);//calculate mirror electrode voltages from frame zrnk coef.

	bool Get_WFSMirrConnected() const { return m_MConnected; }
	bool Get_WFSMirrHVon() const { return m_HVon; }
	double Get_WFSMirru0() const { return m_u0; }
	int Get_WFSMirrnactiveact() const { return m_nactiveact; }
	double Get_WFSMirrcoef() const { return m_coef; };
	int Get_WFSMirrnact()  const { return m_nact; };

	void Set_WFSMirrcoef(double val) {m_coef=val; };
};