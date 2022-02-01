#pragma once
#include "CSensor.h"
#include "CCOM.h"
class CMDLController 
{
private:
	/*Prop to control mirror*/
	double m_coefx; // transmission ratio to get  mirror voltages 
	double m_coefy; // feedback ratio to get  mirror voltages 
	double m_coeftts; // feedback ratio to get  mirror voltages 
	int m_cntTT;// TipTilts cnt in file
	cv::Point2d UTTS;
	cv::Point2d UTTSCOR;
	cv::Point2d UTTSPREV;
	CSerial m_Serial;
	void LoadMdlIni(const std::string& ini);//load inival for mirror
public:
	bool m_PLAY;
	CMDLController();
	CMDLController(const std::string& ini,  const char* COM);
	~CMDLController()
	{
		if (m_Serial.IsOpen()) m_Serial.CloseComPort();
		m_Serial.~CSerial();
	}
	void TTConnect(const char* COM);//
	int TTIsConnected() { return m_Serial.IsConnected(); };
	void TTSetU(short x, short y);// Set x,y to Tip/Tilt
	void TTSetU(cv::Point2d tmp);// Set tmp.x,tmp.y to Tip/Tilt 
	void TTSetU(cv::Point2d U, cv::Point2d tmpU);//Set voltages in close loop tmpU voltages on preview frame
	cv::Point2d TTSU(double x, double y); //Calculate x, y voltages to Tip / Tilt with transmission ratio 
	
	cv::Mat TTx, TTy;
	int LoadCZrnk(CString path, cv::Mat& TipTilts);//Load Tilt/Tip from file ti cvMat

	double Get_coeftts() const { return m_coeftts; };
	double Get_coefx() const { return m_coefx; };
	double Get_coefy() const { return m_coefy; };
	int Get_cntTT() const { return m_cntTT; };
	cv::Point2d Get_UTTS() const { return UTTS; };
	cv::Point2d Get_UTTSPREV() const { return UTTSPREV; };
	cv::Point2d Get_UTTSCOR() const { return UTTSCOR; };
	void Set_UTTS(cv::Point2d tmp) { UTTS = tmp; };
	void Set_UTTSPREVzero() { UTTSPREV.x = 0.0; UTTSPREV.y = 0.0; };
	void Set_UTTSCOR(cv::Point2d tmp) { UTTSCOR = tmp; };
	void Set_coeftts(double val) { m_coeftts = val; };
	void Set_coefx(double val) { m_coefx = val; };
	void Set_coefy(double val) { m_coefy = val; };
};
