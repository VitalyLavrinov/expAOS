#include "pch.h"
#include "CMDLController.h" 

CMDLController::CMDLController() {
	m_coeftts = 1.0;
	m_coefx = -0.012;
	m_coefy = 0.012;
	m_PLAY = 0;
	m_cntTT = 0;
}

CMDLController::CMDLController(const std::string& ini,  const char* COM)  {
	LoadMdlIni(ini);
	TTConnect(COM);
}

void CMDLController::LoadMdlIni(const std::string& ini) {
	pt::ptree bar;
	pt::ini_parser::read_ini(ini, bar);
	m_coeftts = std::stod(bar.get<std::string>("Controler.coeftts"));
	m_coefx = std::stod(bar.get<std::string>("Controler.coefx"));
	m_coefy = std::stod(bar.get<std::string>("Controler.coefy"));
}

void CMDLController::TTConnect(const char* COM) {
	if (!m_Serial.IsOpen()) {
		m_Serial.OpenComPort(COM);
	}
	if (m_Serial.IsOpen()) {
		m_Serial.SetProperties(CBR_128000, DATABITS_8, NOPARITY, ONESTOPBIT);
		m_Serial.connected = 1;
	}
}

void CMDLController::TTSetU(short x, short y) {
	if (m_Serial.IsConnected()) {
		m_Serial.SetTilt(x, y);
	}
}
void CMDLController::TTSetU(cv::Point2d U) {
	if (m_Serial.IsConnected()) {
		m_Serial.SetTilt(static_cast<short>(U.x), static_cast<short>(U.y));
	}
}
void CMDLController::TTSetU(cv::Point2d U, cv::Point2d prevU) {
	short x, y;
	x = static_cast<short>(U.x + prevU.x * m_coeftts);
	y = static_cast<short>(U.y + prevU.y * m_coeftts);
	m_Serial.SetTilt(x, y);
	UTTSPREV.x = x;
	UTTSPREV.y = y;
}
cv::Point2d CMDLController::TTSU(double x, double y) {
	cv::Point2d tmp;
	tmp.x = x / m_coefx;
	tmp.y = y / m_coefy;
	return tmp;
}

// rewrite!!!
//Load Tilt/Tip from file ti cvMat
int CMDLController::LoadCZrnk(CString path,cv::Mat& TipTilts) {
	int cnt = 0;
	FILE* stream;
	float x;

	fopen_s(&stream, path, "r");
	if (stream != NULL) {
		while (!feof(stream))
			if (fgetc(stream) == '\n')
				cnt++;
	}
	fclose(stream);

	TipTilts= cv::Mat::zeros(1, cnt, CV_64FC1);
	m_cntTT = cnt;//UB!!!
	fopen_s(&stream, path, "r");
	if (stream != NULL) {
		for (int i = 0; i < cnt; i++) {
			fscanf_s(stream, "%f", &x);
			TipTilts.at<double>(i) = x;
		}
		fclose(stream);
		return cnt;

	}
	else return 0;
}
