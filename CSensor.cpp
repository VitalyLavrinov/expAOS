// Class for S-H wavefronte & Tip/Tilt sensors
// need to use JSON instead of boostini
#include "pch.h"
#include "CSensor.h"


CSensor::CSensor() : CCamera() {
	LoadIni("default.ini");

	m_treshlow = 7; // can be changed in Sensor Properties window
	m_treshhigh=255; // can be changed in Sensor Properties window
	m_Speccnt = 500; //!! change to cntframes in work with model films
	m_idframe = 0;

	m_subtrah=8;
	m_R0k1=0.90135;
	m_R0k2=1.46/ (1.692 * PI * PI);
	m_R0k3=0.097;
	m_Cn2k1 = 1.69;
	m_Cn2k2 = 0.423;
	ReLoadData();
}
CSensor::CSensor(const std::string& ini, const char* CamId) : CCamera(ini,CamId) {
	LoadIni(ini);
	
	m_treshlow = 7;// can be changed in Sensor Properties window
	m_treshhigh = 255; // can be changed in Sensor Properties window
	m_Speccnt = 1000; //!! because max Cam fps in our system 1000
	m_idframe = 0;

	m_subtrah=71;
	m_R0k1 = 0.90135;
	m_R0k2 = 1.46 / (1.692 * PI * PI);
	m_R0k3 = 0.097;
	m_Cn2k1 = 1.69;
	m_Cn2k2 = 0.423;
	ReLoadData();
}

//change or init all grids, arrays after reloading sensor parameters 
void CSensor::ReLoadData() {

	for (int i = 0; i < m_Speccnt; i++) {
		CTdecX.push_back(0.0);
		SpecX.push_back(0.0);
		CTdecY.push_back(0.0);
		SpecY.push_back(0.0);
		CTDiffX.push_back(0.0);
		CTDiffY.push_back(0.0);
	}


	accumframe= cv::Mat::zeros(m_cdx, m_cdy, CV_64FC1);
	outframe.create(m_cdx, m_cdy, CV_8UC1);
	m_coefshift = ((m_srastr * m_szelens * sqrt(2.0f) * 0.5f * m_pixsze) / (m_focuscam * m_lwave));

	tPtr.create(m_srastr, m_srastr);
	Lenses.create(1, m_srastr * m_srastr, CV_32S);
	subs.create(m_srastr, m_srastr);
	wnds.create(m_srastr, m_srastr);
	subMax.create(m_srastr, m_srastr);
	x0 = cv::Mat::zeros(1, m_srastr * m_srastr, CV_64FC1);
	y0 = cv::Mat::zeros(1, m_srastr * m_srastr, CV_64FC1);
	MaxI.create(m_srastr, m_srastr, CV_64F);

	for (int j = 0; j < m_srastr; j++)
		for (int i = 0; i < m_srastr; i++) {
			tPtr(i, j).x = m_left + i * (m_sdx + m_shftcol);
			tPtr(i, j).y = m_top + (m_sdy + m_shftrow) * j;
			subs(i, j) = cv::Rect::Rect_(tPtr(i, j), cv::Size(m_sdx, m_sdy));
			subMax(i, j) = cv::Point(m_swnd / 2, m_swnd / 2);
			MaxI.at<double>(i, j) = 0.0; 
			Lenses.at<int>(j * m_srastr + i) = 1;
			x0.at<double>(j * m_srastr + i) = static_cast<double>((m_srastr * m_srastr) / 2 + i * (m_srastr * m_srastr) - m_cdx / 2) / (m_cdx * sqrt(2.0f) / 2.0f); //??? (cdx / 2);
			y0.at<double>(j * m_srastr + i) = static_cast<double>((m_srastr * m_srastr) / 2 + j * (m_srastr * m_srastr) - m_cdy / 2) / (m_cdy * sqrt(2.0f) / 2.0f); //???(cdx / 2);
		}

	CTMax = cv::Mat::zeros(1, m_srastr * m_srastr * 2, CV_64FC1);
    CTMaxReff = cv::Mat::ones(1, m_srastr * m_srastr * 2, CV_64FC1) * m_sdx / 2; //init like subs centers
    CTMaxDif = cv::Mat::zeros(1, m_srastr * m_srastr * 2, CV_64FC1);
 	wfCT = cv::Mat::zeros(1, m_npzrnk, CV_64FC1);
	wfCTl = cv::Mat::zeros(1, m_npzrnk, CV_64FC1);
    wfCTTT = cv::Mat::zeros(1, m_npzrnk-2 , CV_64FC1);
    wfCTTTl = cv::Mat::zeros(1, m_npzrnk - 2, CV_64FC1);

	Fmat = cv::Mat::zeros(m_srastr * m_srastr * 2, m_npzrnk, CV_64FC1);//AT*(AT*A)^(-1)=F to all lens
	FmatTT = cv::Mat::zeros(m_srastr * m_srastr * 2, m_npzrnk-2, CV_64FC1);//AT*(AT*A)^(-1)=F to all lens without tip/tilt calc

	CTCorr = cv::Mat::zeros(1, m_srastr * m_srastr * 2, CV_64FC1);
    wfCorr = cv::Mat::zeros(1, m_npzrnk, CV_64FC1);
	wfCorrl = cv::Mat::zeros(1, m_npzrnk, CV_64FC1);
	createHanningWindow(Han, cv::Size(m_sdx, m_sdy), CV_64F);

	Pzrnk = cv::Mat::zeros(1, 35, CV_64FC1);
	x_lens = cv::Mat::ones(m_ngrid, m_ngrid, CV_64FC1);
	y_lens = cv::Mat::ones(m_ngrid, m_ngrid, CV_64FC1);
	wfphase = cv::Mat::zeros(m_ngrid, m_ngrid, CV_64FC1);
	wfinterf = cv::Mat::zeros(m_ngrid, m_ngrid, CV_64FC1);

	for (int j = 0; j < m_ngrid; j++)
		for (int i = 0; i < m_ngrid; i++) {
			x_lens.at<double>(i, j) = (-m_ngrid / 2 + i) / (m_ngrid * sqrt(2.0) / 2.0);
			y_lens.at<double>(i, j) = (-m_ngrid / 2 + j) / (m_ngrid * sqrt(2.0) / 2.0);
		}
}

//Drow cam.frame with left top offsets on CDC with scale. (with translete BYTE* to cv::Mat by BitBlt) 
//Added sensor greed and Max on subaperture & etc graf info 
void CSensor::DrowFrame(const cv::Mat& out, int left, int top, double scale) const {
	CBitmap bmp;
	CDC dcMem;
	cv::Scalar yellow(0, 255, 255);
	cv::Scalar green(0, 255, 0);
	cv::Scalar red(0, 0, 255);
	cv::Mat dst;
	cv::Point tmp1, tmp2;
	int max;
	double submax;
	cv::Point isubmax;
	cvtColor(out, dst, cv::COLOR_GRAY2RGBA);
	int tmpx = tPtr(0, 0).x + (m_srastr / 2) * m_sdx +(m_srastr % 2) * (m_sdx/2);
	int tmpy = tPtr(0, 0).y + (m_srastr / 2) * m_sdy +(m_srastr % 2) * (m_sdy/2);

	ellipse(dst, cv::Point(tmpx, tmpy), cv::Size(m_norm, m_norm), 0, 0, 360, cv::Scalar(0, 255, 255), 1, 1);
		
	for (int j = 0; j < m_srastr; j++)
		for (int i = 0; i < m_srastr; i++) {
			if (Lenses.at<int>(j * m_srastr + i) == 0) 
				rectangle(dst, tPtr(i, j), cv::Point((tPtr(i, j).x + m_sdx - 1), (tPtr(i, j).y + m_sdy - 1)), cv::Scalar(0, 0, 0), 1);
			else {
				minMaxLoc(MaxI, NULL, &submax, NULL, &isubmax);
				rectangle(dst(subs(i, j)), cv::Point(subMax(i, j).x - m_swnd / 2, subMax(i, j).y - m_swnd / 2), cv::Point(subMax(i, j).x + m_swnd / 2, subMax(i, j).y + m_swnd / 2), green, 1);
				max = static_cast<int>(MaxI.at<double>(i, j));
			
				if ((i + j * m_srastr + 1) == m_sub || (i + j * m_srastr + 1) == m_subtrah) {
					rectangle(dst, tPtr(i, j), cv::Point((tPtr(i, j).x + m_sdx - 1), (tPtr(i, j).y + m_sdy - 1)), cv::Scalar(255, 0, 0), 1);
				}else {
				    if (max < 255) rectangle(dst, tPtr(i, j), cv::Point((tPtr(i, j).x + m_sdx - 1), (tPtr(i, j).y + m_sdy - 1)), cv::Scalar(0, max, max), 1);
				    else rectangle(dst, tPtr(i, j), cv::Point((tPtr(i, j).x + m_sdx - 1), (tPtr(i, j).y + m_sdy - 1)), cv::Scalar(0, 0, 255), 1);
				}
				tmp1 = tmp2 = subMax(i, j);
				tmp1.x = subMax(i, j).x - 3;
				tmp2.x = subMax(i, j).x + 3;
				line(dst(subs(i, j)), tmp1, tmp2, red, 1);
				tmp1 = tmp2 = subMax(i, j);
				tmp1.y = subMax(i, j).y - 3;
				tmp2.y = subMax(i, j).y + 3;
				line(dst(subs(i, j)), tmp1, tmp2, red, 1);
    		}
		}

	line(dst, cv::Point(tmpx - 10, tmpy), cv::Point(tmpx + 10, tmpy ), yellow, 1);
	line(dst, cv::Point(tmpx , tmpy + 10), cv::Point(tmpx , tmpy - 10), yellow, 1);

	if (scale)resize(dst, dst, cv::Size(0, 0), scale, scale, cv::INTER_LINEAR);
	dcMem.CreateCompatibleDC(odc);
	bmp.CreateBitmap(dst.cols, dst.rows, 1, 32, dst.data);
	dcMem.SelectObject(&bmp);
	odc->BitBlt(left, top, dst.cols + left, dst.rows + top, &dcMem, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	DeleteDC(dcMem);
}
void CSensor::DrowSub(const cv::Mat& out, int left, int top, double scale) const {
	CBitmap bmp;
	CDC dcMem;
	cv::Mat tmp;
	//cv::Scalar yellow(0, 255, 255);
	//cv::Scalar green(0, 255, 0);
	cv::Scalar red(0, 0, 255);
	cv::Mat dst;
	cv::Point tmp1, tmp2;
	int x, y;
	    x = static_cast<int>(m_sub % m_srastr) - 1;
		y = static_cast<int>(m_sub / m_srastr);
	tmp = out(subs(x, y));

	cvtColor(tmp, dst, cv::COLOR_GRAY2RGBA);

	tmp1 = tmp2 = subMax(x , y);
	tmp1.x = subMax(x, y).x - 3;
	tmp2.x = subMax(x, y).x + 3;
	line(dst, tmp1, tmp2, red, 1);
	tmp1 = tmp2 = subMax(x, y);
	tmp1.y = subMax(x, y).y - 3;
	tmp2.y = subMax(x, y).y + 3;
	line(dst, tmp1, tmp2, red, 1);

	if (scale)resize(dst, dst, cv::Size(0, 0), scale, scale, cv::INTER_LINEAR);
	dcMem.CreateCompatibleDC(odc);
	bmp.CreateBitmap(dst.cols, dst.rows, 1, 32, dst.data);
	dcMem.SelectObject(&bmp);
	odc->BitBlt(left, top, dst.cols + left, dst.rows + top, &dcMem, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	DeleteDC(dcMem);
}

// drow wave front surface
void CSensor::DrowPhase(int left, int top, double scale, int interfer) {
	    cv::Mat dst;
		CBitmap bmp;
		CDC dcMem;
		if(interfer) cv::normalize(wfinterf, dst, 255, 0, cv::NORM_MINMAX, CV_8UC1);
		else cv::normalize(wfphase, dst, 255, 0, cv::NORM_MINMAX, CV_8UC1);
		cv::cvtColor(dst, dst, cv::COLOR_GRAY2RGBA);
		if (scale)resize(dst, dst, cv::Size(0, 0), scale, scale, cv::INTER_LINEAR);
		dcMem.CreateCompatibleDC(wfdc);
		bmp.CreateBitmap(dst.cols, dst.rows, 1, 32, dst.data);
		dcMem.SelectObject(&bmp);
		wfdc->BitBlt(left, top, dst.cols + left, dst.rows + top, &dcMem, 0, 0, SRCCOPY);
		bmp.DeleteObject();
		DeleteDC(dcMem);
}


//read parameters from ini file 
void CSensor::LoadIni(const std::string& ini) {
	pt::ptree bar;
	pt::ini_parser::read_ini(ini, bar);
	m_cdx = std::stoi(bar.get<std::string>("General.cdx"));
	m_cdy = std::stoi(bar.get<std::string>("General.cdy"));
	m_sdx = std::stoi(bar.get<std::string>("General.sdx"));
	m_sdy = std::stoi(bar.get<std::string>("General.sdy"));
	m_srastr = std::stoi(bar.get<std::string>("General.srastr"));
	m_swnd = std::stoi(bar.get<std::string>("General.swnd"));
	m_left = std::stoi(bar.get<std::string>("General.left"));
	m_top = std::stoi(bar.get<std::string>("General.top"));
	m_shftcol = std::stoi(bar.get<std::string>("General.shftcol"));
	m_shftrow = std::stoi(bar.get<std::string>("General.shftrow"));
	m_norm = std::stoi(bar.get<std::string>("General.norm"));
	m_sub = std::stoi(bar.get<std::string>("General.sub"));
	m_szelens = std::stod(bar.get<std::string>("System.szelens"));
	m_pixsze = std::stod(bar.get<std::string>("System.pixsze"));
	m_lwave = std::stod(bar.get<std::string>("System.lwave"));
	m_focuscam = std::stod(bar.get<std::string>("System.focuscam"));
	m_lpath = std::stod(bar.get<std::string>("System.lpath"));
	m_ngrid = std::stoi(bar.get<std::string>("System.ngrid"));
	m_npzrnk = std::stoi(bar.get<std::string>("System.npzrnk"));
}

void CSensor::SaveIni(const std::string& ini) const {
	pt::ptree bar;

	bar.put("General.expos", GetExpos());
	bar.put("General.offsetx", GetOffsetx());
	bar.put("General.offsety", GetOffsety());
	bar.put("General.cdx", m_cdx);
	bar.put("General.cdy", m_cdy);
	bar.put("General.sdx", m_sdx);
	bar.put("General.sdy", m_sdy);
	bar.put("General.srastr", m_srastr);
	bar.put("General.swnd", m_swnd);
	bar.put("General.top", m_top);
	bar.put("General.left", m_left);
	bar.put("General.shftcol", m_shftcol);
	bar.put("General.shftrow", m_shftrow);
	bar.put("General.norm", m_norm);
	bar.put("General.sub", m_sub);

	bar.put("System.szelens", m_szelens);
	bar.put("System.pixsze", m_pixsze);
	bar.put("System.lwave", m_lwave);
	bar.put("System.focuscam", m_focuscam);
	bar.put("System.lpath", m_lpath);
	bar.put("System.ngrid", m_ngrid);
	bar.put("System.npzrnk", m_npzrnk);
	
	pt::write_ini(ini, bar);
}

//add mesuared vals in deques
void CSensor::CTDeqAdd() {
	CTdecX.pop_front();
	CTdecX.push_back(CTMaxDif.at<double>(m_sub - 1));
	CTdecY.pop_front();
	CTdecY.push_back(CTMaxDif.at<double>(m_sub));
	CTDiffX.pop_front();
	//CTDiffX.push_back(CTMaxDif.at<double>(m_sub - 1)-CTMaxDif.at<double>(m_subtrah - 1));
	CTDiffX.push_back(CTMaxDif.at<double>(m_subtrah - 1));
	CTDiffY.pop_front();
	//CTDiffY.push_back(CTMaxDif.at<double>(m_sub) -CTMaxDif.at<double>(m_subtrah));
	CTDiffY.push_back(CTMaxDif.at<double>(m_subtrah));
}

//calc max & aver val in deq
void CSensor::GetStatDeq(std::deque<double>& deq,double& maxin, double& averin) {
	maxin=0.0;
	averin =  0.0;
	double sum = 0.0;
	auto MMx = std::minmax_element(deq.begin(), deq.end());
	maxin = std::max(abs(*MMx.first), *MMx.second);
	//for (auto& it : dec)
	for (int i=0;i< m_Speccnt;i++)
	{
		sum += abs(deq.at(i));
	}
	averin = sum / m_Speccnt;
}

//get statistic in CT deq
void CSensor::GetStatCTDeq() {
		GetStatDeq(CTdecX,m_maxindec.x,m_averindec.x);
		GetStatDeq(CTdecY, m_maxindec.y, m_averindec.y);
}

//calculate spectrum from deque cv::Point2d Cdec to cv::Point2d Spec  
//fen nyquist frequency f0 start frequency fd-
// xy==1 x coord xy=0 y coord
//code from VS 6.0 adapted
void CSensor::Spectrum(int N, double f0, double fen, double fd, std::deque<double>& CT, std::deque<double>& SP)
{
	int i, k;
	double s, c, fs, s1, c1, yc, ys, sm, cm, arg;
	int mn = N - 1;
	double df = 1.0f / (N / (fen - f0));
	int nfr = static_cast<int>((fen - f0) / df + 1);
	double pn = 2.0f / (fd * N);
	double a = 2.0f * PI / fd;

	//if (!CT.empty()) {
		for (k = 0; k < nfr; k++) {
			s = 0.0f;
			c = 1.0f;
			fs = f0 + (k - 1) * df;
			arg = a * fs;
			s1 = sin(arg);
			c1 = cos(arg);
			yc = CT[0];
			ys = 0.0f;
			for (i = 0; i < mn; i++) {
				sm = s;
				cm = c;
				c = c1 * cm - s1 * sm;
				s = c1 * sm + s1 * cm;
				yc = yc + CT[i + 1] * c;
			}
			SP[k] = pn * (yc * yc + ys * ys);
		}
	//}
}

//drow spectrum on CDC
void CSensor::DrowSpectrum(int N, int x, int y, double scl1, int fd, std::deque<double>& SP) {
	CBitmap bmp;
	CDC dcMem;
	cv::Scalar yellow(0, 255, 255);
	//cv::Scalar green(0, 255, 0);
	cv::Scalar red(0, 0, 255);
	cv::Scalar black(0, 0, 0);
	cv::Mat drow = cv::Mat::ones(255, N / 2, CV_8UC1) * 230;
	cv::cvtColor(drow, drow, cv::COLOR_GRAY2RGBA);
	int c;

	cv::line(drow, cv::Point(2, 2), cv::Point(2, 253), yellow, 1);
	cv::line(drow, cv::Point(2, 253), cv::Point(N / 2 - 2, 253), yellow, 1);

	//if (!Spec.empty()) {
		for (int i = 0; i < N / 2; i++) {
			if (i <= fd) {
				c = static_cast<int>(SP[i] * scl1);
				cv::line(drow, cv::Point(i + 3, 253 - c), cv::Point(i + 3, 253), red, 1);
			}
			else  cv::line(drow, cv::Point(i + 3, 253), cv::Point(i + 3, 253), red, 1);
		}
	//}

	line(drow, cv::Point(27, 252), cv::Point(27, 255), black, 1);
	line(drow, cv::Point(52, 250), cv::Point(52, 255), black, 1);
	line(drow, cv::Point(102, 250), cv::Point(102, 255), black, 1);
	line(drow, cv::Point(77, 252), cv::Point(77, 255), black, 1);

	dcMem.CreateCompatibleDC(odc);
	bmp.CreateBitmap(drow.cols, drow.rows, 1, 32, drow.data);
	dcMem.SelectObject(&bmp);
	odc->BitBlt(x, y, drow.cols + x, drow.rows + y, &dcMem, 0, 0, SRCCOPY);
	bmp.DeleteObject();
	DeleteDC(dcMem);
}


//Set active lenses in Lenses array by intensity 
void  CSensor::SetLensesMoreThanI(int in) {
	for (int j = 0; j < m_srastr; j++)
		for (int i = 0; i < m_srastr; i++)
		{
			static_cast<int>(MaxI.at<double>(i, j))>in?Lenses.at<int>(j * m_srastr + i) = 1:Lenses.at<int>(j * m_srastr + i) = 0;
	}
}

//saving active lens array
void CSensor::SaveLenses(const std::string& ini) const {
	pt::ptree bar;
	for (int i = 0; i < m_srastr * m_srastr; i++) {
		bar.add("System.LENS" + std::to_string(i+1), Lenses.at<int>(i));
	 }
	pt::ini_parser::write_ini(ini, bar);
}
// reading active lenses from file & reload data as need
void CSensor::CheckLens(const std::string &ini){
	pt::ptree bar;
	pt::ini_parser::read_ini(ini, bar);
	m_nlens = 0;
	for (int i = 1; i <= m_srastr * m_srastr; i++){
		Lenses.at<int>(i-1) = std::stoi(bar.get<std::string>("System.LENS" + std::to_string(i)));
		if (Lenses.at<int>(i-1) != 0) m_nlens++;
	}
	x0l = cv::Mat::zeros(1, m_nlens, CV_64FC1);
	y0l = cv::Mat::zeros(1, m_nlens, CV_64FC1);
	CTMaxDifl = cv::Mat::zeros(1, m_nlens * 2, CV_64FC1);
	CTCorrl = cv::Mat::zeros(1, m_nlens * 2, CV_64FC1);
	int ii = 0;
	for (int i = 0; i < m_srastr * m_srastr; i++)
	{
		if (Lenses.at<int>(i) != 0) {
			x0l.at<double>(ii) = x0.at<double>(i);
			y0l.at<double>(ii) = y0.at<double>(i);
			ii++;
		}
	}

	//Pre-calculate AT*(AT*A)^(-1) for all variants 
	Fmatl = cv::Mat::zeros(m_nlens * 2, m_npzrnk, CV_64FC1);//AT*(AT*A)^(-1)=F  to active lens
	FmatTTl = cv::Mat::zeros(m_nlens * 2, m_npzrnk - 2, CV_64FC1);//AT*(AT*A)^(-1)=F to active lens without tip/tilt calc

	Pre_Zrnk(Fmat, m_srastr * m_srastr, m_npzrnk, x0, y0, 1);
	Pre_Zrnk(Fmatl, m_nlens, m_npzrnk, x0l, y0l, 1);
	Pre_Zrnk(FmatTT, m_srastr * m_srastr, m_npzrnk - 2, x0, y0, 0);
	Pre_Zrnk(FmatTTl, m_nlens, m_npzrnk - 2, x0l, y0l, 0);
}

//calc maxvals in subs
void CSensor::GetMaxWfs(cv::Mat& src) {
	//!! need remaking using cv::rects
	BYTE* Data;
	BYTE* Kadr;
	int itmp, i, j, x, y, Max, MaxX, MaxY;
	Kadr = src.ptr<BYTE>();
	for (j = 0; j < m_srastr; j++)
		for (i = 0; i < m_srastr; i++)
		{
			Data = Kadr + (tPtr(i, j).y * m_cdx + tPtr(i, j).x);
			Max = 0;
			MaxX = m_sdx / 2;
			MaxY = m_sdy / 2;
			if (Lenses.at<int>(j * m_srastr + i) != 0) {
				for (y = 0; y < m_sdy; y++)
				{
					for (x = 0; x < m_sdx; x++)
					{
						itmp = *(Data + x);
						if (itmp >= Max)
						{
							Max = itmp;
							MaxX = x;
							MaxY = y;
						}
					}
					Data += m_cdx;
				}
			}
			MaxI.at<double>(i, j) = static_cast<double>(Max);
			subMax(i, j).x = MaxX;
			subMax(i, j).y = MaxY;
		}
}

//calc subapertures images offsets
// if func used in closeloop needs reload ReffKor array(refframe) by lust frame!
// another way is to use the previously accumulated image (in GetRefframe). Not very suitable for a point source.
void CSensor::GetCTWfsCorr(const cv::Mat& src)
{
	cv::Point2d sft;
	cv::Mat tmp;

	src.convertTo(tmp, CV_64F);

	int k = 0;
	int iil = 0;
	for (int j = 0; j < m_srastr; j++)
		for (int i = 0; i < m_srastr; i++) {
			sft = cv::phaseCorrelate(tmp(subs(i, j)), ReffKor(subs(i, j)), Han);
			CTCorr.at<double>(k) = sft.x;
			CTCorr.at<double>(k + 1) = sft.y;
			if (Lenses.at<int>(j * m_srastr + i) != 0) {
				CTCorrl.at<double>(iil) = CTCorr.at<double>(k);
				CTCorrl.at<double>(iil + 1) = CTCorr.at<double>(k + 1);
				iil += 2;
			}
			k += 2;
		}

	//tmp.copyTo(ReffKor);

}

//calc offsets coords center of gravity of the spots in subs with calculated RefFrame CofG!! because if Lens(i)==0  offsets coord of spots must be zero!!! 
void CSensor::GetCTWfs(cv::Mat& src) {
	//!!may be need remaking (worked only Win32)
	BYTE* Data;
	BYTE* Kadr;
	unsigned char NX4, NY;
	unsigned int ecx_jump;//!!!!
	unsigned int ffff, fxxx, fyyy;
	double ix, iy;
	ULARGE_INTEGER x01020304, dx04040404, y01010101, dy01010101;
	ULARGE_INTEGER sumxf12, sumyf12;
	int k = 0;
	int iil = 0;

	union {
		ULARGE_INTEGER sumF;
		unsigned short int sumF4[4];
	}sumFee;

	NX4 = m_swnd / 4;
	NY = m_swnd;
	ecx_jump = m_cdx - m_swnd;//!!!!
	Kadr = src.ptr<BYTE>();

	for (int j = 0; j < m_srastr; j++)
		for (int i = 0; i < m_srastr; i++){
			Data = Kadr + (tPtr(i, j).y * m_cdx + tPtr(i, j).x) + m_cdx * (subMax(i, j).y - m_swnd / 2 + 1) + (subMax(i, j).x - m_swnd / 2 + 1);

			if (Lenses.at<int>(j * m_srastr + i) != 0) {
				x01020304.LowPart = 0x00020001;
				x01020304.HighPart = 0x00040003;
				dx04040404.LowPart = 0x00040004;
				dx04040404.HighPart = 0x00040004;
				y01010101.LowPart = 0x00010001;
				y01010101.HighPart = 0x00010001;
				dy01010101.LowPart = 0x00010001;
				dy01010101.HighPart = 0x00010001;
				__asm{
					push	eax
					push	ebx
					push	ecx
					push	edx
					mov		ecx, Data
					pxor	mm3, mm3; sum = f(x, y)
					pxor	mm2, mm2; Sum = (x * f(x, y)) / m_wnd
					pxor	mm1, mm1; Sum = (y * f(x, y)) / m_wnd
					movq	mm5, x01020304
					movq	mm6, dx04040404
					movq	mm7, y01010101
					mov		ah, NY
					LoopY :
					mov		al, NX4
						LoopX :
					    movd	mm4, [ecx]; f(x, y[0..3])
						pxor	mm0, mm0; zero
						punpcklbw	mm4, mm0; f(x[0..3]) < -word
						add		ecx, 4; inc ptr f(x, y)
						movq	mm0, mm4; f(x[0..3]) < -word
						paddusw	mm3, mm4; Sum(f) + f
						pmaddwd	mm4, mm5; x* f
						pmaddwd	mm0, mm7; y* f
						paddd	mm2, mm4; Sum(x * f) + x * f
						paddd	mm1, mm0; Sum(y * f) + y * f
						paddusb	mm5, mm6; next X axis
						sub		al, 1
					jne		LoopX
						add	ecx, ecx_jump
						movq	mm5, x01020304
						movq	mm0, dy01010101
						paddusb	mm7, mm0; next Y axis
						sub		ah, 1
					jne		LoopY
						movq	sumFee.sumF, mm3
						movq	sumxf12, mm2
						movq	sumyf12, mm1
						EMMS;
					    pop	edx
						pop	ecx
						pop	ebx
						pop eax
				}
				ffff = (unsigned int)sumFee.sumF4[0];
				ffff += (unsigned int)sumFee.sumF4[1];
				ffff += (unsigned int)sumFee.sumF4[2];
				ffff += (unsigned int)sumFee.sumF4[3];
				fxxx = sumxf12.LowPart;
				fxxx += sumxf12.HighPart;
				fyyy = sumyf12.LowPart;
				fyyy += sumyf12.HighPart;
				ix = (double)fxxx / (double)ffff - m_swnd / 2;
				iy = (double)fyyy / (double)ffff - m_swnd / 2;

				if (ix < 0) ix = 0;
				if (iy < 0) iy = 0;

				CTMax.at<double>(k) = subMax(i, j).x + ix;
				CTMax.at<double>(k + 1) = subMax(i, j).y + iy;
			}
			else {
				CTMax.at<double>(k) = CTMaxReff.at<double>(k);
				CTMax.at<double>(k + 1) = CTMaxReff.at<double>(k + 1);
			}
			k += 2;
		}
}

// calc offsets between CoG coords & RefFrame coordsin subs(without nonactive lenses, its==0.0)
void CSensor::GetCTOffsetsWfs(cv::Mat& src) {
	GetMaxWfs(src);
	GetCTWfs(src);
	int k = 0;
	int iil = 0;
	for (int j = 0; j < m_srastr; j++)
		for (int i = 0; i < m_srastr; i++) {
			CTMaxDif.at<double>(k) = CTMax.at<double>(k) - CTMaxReff.at<double>(k);
			CTMaxDif.at<double>(k + 1) = CTMax.at<double>(k + 1) - CTMaxReff.at<double>(k + 1);
			if (Lenses.at<int>(j * m_srastr + i) != 0) {
				CTMaxDifl.at<double>(iil) = CTMax.at<double>(k) - CTMaxReff.at<double>(k);//CTMaxDif.at<double>(k);
				CTMaxDifl.at<double>(iil + 1) = CTMax.at<double>(k + 1) - CTMaxReff.at<double>(k + 1);//CTMaxDif.at<double>(k + 1);
				iil += 2;
			}
			k += 2;
		}
}

//get accumulate cnt frames matrix with tresholding   into cvMat accumframe
void CSensor::GetAccumFrames(int cnt) {
	cv::Mat treshedframe;
	int i = 0;
	for (int ikl = 0; ikl < cnt; ikl++) {
		if (CameraFrameN()) {
			for (int j = 0; j < FRAME_COUNT; j++) {
				if (frames[j].receiveStatus == 0) {
					outframe.data = static_cast<uchar*>(frames[j].buffer);
					cv::threshold(outframe, treshedframe, m_treshlow, m_treshhigh, CV_THRESH_TOZERO);
					cv::accumulate(treshedframe, accumframe);
					i++;
					frames[j].receiveStatus = -1;
				}
			}

		}
	}
	accumframe = accumframe / i;
}

// get refframe from sensor cam
void CSensor::GetRefFrame(int cnt) {
	    cv::Mat acc;
		GetAccumFrames(cnt);
		/**/
		//may be bad to a point source(need use lust frame without accum?) in closeloop pointless
		//outframe.copyTo(ReffKor); 
		//ReffKor.convertTo(ReffKor, CV_64F);
		accumframe.copyTo(ReffKor);
		/**/
		accumframe.convertTo(outframe, CV_8UC1, 1.0);
		accumframe.zeros(m_cdx, m_cdy, CV_64F);

		GetMaxWfs(outframe);
		GetCTWfs(outframe);
		CTMax.copyTo(CTMaxReff);
}

//calculate coef. Zernike offsets from difcor
cv::Mat CSensor::Get_Zrnk(cv::Mat& difcor, cv::Mat& F){
	cv::Mat dst;
	difcor *= m_coefshift;
	dst = difcor * F;
	return dst;
}

//saveing zernike coef. like *txt file
void  CSensor::SaveFrameZrnk(std::string filename, cv::Mat& ZRNK) {
	std::ofstream outf(filename);
	if (outf.is_open()) {
		for (int i = 0; i < m_npzrnk; i++)
		{
			outf << ZRNK.at<double>(i) << std::endl;
		}
		outf.close();
	}
}

//calculate wavefront surface & interferogram
void CSensor::GetWF(cv::Mat& wfzrnk, int begin, int endshft){
	//begin=2 & endshft=0 if normal wf without Tip/Tilt to drow
	//begin=0 & endshft=2 if Zernike calculated realy without Tip/Tilt
	for (int j = 0; j < m_ngrid; j++)
		for (int i = 0; i < m_ngrid; i++) {
			wfphase.at<double>(i, j) = 0.0;
			wfinterf.at<double>(i, j) = 0.0;
			Plnm_Zrnk(y_lens.at<double>(i, j), x_lens.at<double>(i, j));
			for (int k = begin; k < m_npzrnk - endshft; k++) {
				wfphase.at<double>(i, j) = wfphase.at<double>(i, j) + wfzrnk.at<double>(k) * Pzrnk.at<double>(k+ endshft);
			}
			wfinterf.at<double>(i, j) = 2.0f * (1.0f + cos(DPI * wfphase.at<double>(i, j)));
		}
}

//Zernike modes to xylens grid to drow wavefront surface
void CSensor::Plnm_Zrnk(double u, double v)
{
	double x1 = u;
	double x2 = u * x1;
	double x3 = u * x2;
	double x4 = u * x3;
	double x5 = u * x4;
	double x6 = u * x5;
	double x7 = u * x6;
	double x8 = u * x7;

	double y1 = v;
	double y2 = v * y1;
	double y3 = v * y2;
	double y4 = v * y3;
	double y5 = v * y4;
	double y6 = v * y5;
	double y7 = v * y6;
	double y8 = v * y7;

	double s2 = (double)sqrt(2.0f);
	double s3 = (double)sqrt(3.0f);
	double s5 = (double)sqrt(5.0f);
	double s6 = (double)sqrt(6.0f);
	double s7 = (double)sqrt(7.0f);
	double s10 = (double)sqrt(10.0f);
	double s14 = (double)sqrt(14.0f);

	Pzrnk.at<double>(0) = 2.0f * x1;
	Pzrnk.at<double>(1) = 2.0f * y1;
	Pzrnk.at<double>(2) = 2.0f * s6 * x1 * y1;
	Pzrnk.at<double>(3) = s3 * (2.0f * x2 + 2.0f * y2 - 1.0f);
	Pzrnk.at<double>(4) = s6 * (x2 - y2);
	Pzrnk.at<double>(5) = 2.0f * s2 * (3.0f * x2 * y1 - y3);
	Pzrnk.at<double>(6) = 2.0f * s2 * (3.0f * x2 * y1 + 3.0f * y3 - 2.0f * y1);
	Pzrnk.at<double>(7) = 2.0f * s2 * (3.0f * x3 + 3.0f * x1 * y2 - 2.0f * x1);
	Pzrnk.at<double>(8) = 2.0f * s2 * (x3 - 3.0f * x1 * y2);
	Pzrnk.at<double>(9) = 4.0f * s10 * (x3 * y1 - x1 * y3);
	Pzrnk.at<double>(10) = 2.0f * s10 * (4.0f * x3 * y1 + 4.0f * x1 * y3 - 3.0 * x1 * y1);
	Pzrnk.at<double>(11) = s5 * (6.0f * x4 + 12.0f * x2 * y2 + 6.0f * y4 - 6.0f * x2 - 6.0f * y2 + 1.0f);
	Pzrnk.at<double>(12) = s10 * (4.0f * x4 - 4.0f * y4 - 3.0f * x2 + 3.0f * y2);
	Pzrnk.at<double>(13) = s10 * (x4 - 6.0f * x2 * y2 + y4);
	Pzrnk.at<double>(14) = 2.0f * s3 * (5.0f * x4 * y1 - 10.0f * x2 * y3 + y5);
	Pzrnk.at<double>(15) = 2.0f * s3 * (15.0f * x4 * y1 + 10.0f * x2 * y3 - 12.0f * x2 * y1 - 5.0f * y5 + 4.0f * y3);
	Pzrnk.at<double>(16) = 2.0f * s3 * (10.0f * x4 * y1 + 20.0f * x2 * y3 + 10.0f * y5 - 12.0f * x2 * y1 - 12.0f * y3 + 3.0f * y1);
	Pzrnk.at<double>(17) = 2.0f * s3 * (10.0f * x5 + 20.0f * x3 * y2 + 10.0f * x1 * y4 - 12.0f * x3 - 12.0f * x1 * y2 + 3.0f * x1);
	Pzrnk.at<double>(18) = 2.0f * s3 * (5.0f * x5 - 4.0f * x3 - 10.0f * x3 * y2 - 15.0f * x1 * y4 + 12.0f * x1 * y2);
	Pzrnk.at<double>(19) = 2.0f * s3 * (x5 - 10.0f * x3 * y2 + 5.0f * x1 * y4);
	Pzrnk.at<double>(20) = s14 * (6.0f * x5 * y1 - 20.0f * x3 * y3 + 6.0f * x1 * y5);
	Pzrnk.at<double>(21) = s14 * (24.0f * x5 * y1 - 20.0f * x3 * y1 - 24.0f * x1 * y5 + 20.0f * x1 * y3);
	Pzrnk.at<double>(22) = 2.0f * s14 * (15.0f * x5 * y1 + 30.0f * x3 * y3 + 15.0f * x1 * y5 - 20.0f * x3 * y1 - 20.0f * x1 * y3 + 6.0f * x1 * y1);
	Pzrnk.at<double>(23) = s7 * (20.0f * x6 + 20.0f * y6 + 60.0f * x4 * y2 + 60.0f * x2 * y4 - 30.0f * x4 - 30.0f * y4 - 60.0f * x2 * y2 + 12.0f * x2 + 12.0f * y2 - 1.0f);
	Pzrnk.at<double>(24) = s14 * (15.0f * x6 - 15.0f * y6 + 15.0 * x4 * y2 - 15.0f * x2 * y4 + 20.0f * y4 - 20.0f * x4 + 6.0f * x2 - 6.0f * y2);
	Pzrnk.at<double>(25) = s14 * (6.0f * x6 + 6.0f * y6 - 30.0f * x4 * y2 - 30.0f * x2 * y4 + 30.0f * x2 * y2 - 5.0f * x4 - 5.0f * y4);
	Pzrnk.at<double>(26) = s14 * (x6 - 15.0f * x4 * y2 + 15.0f * x2 * y4 - y6);
	Pzrnk.at<double>(27) = 4.0f * (7.0f * x6 * y1 - 35.0f * x4 * y3 + 21.0f * x2 * y5 - y7);
	Pzrnk.at<double>(28) = 4.0f * (7.0f * y7 - 63.0f * x2 * y5 - 35.0f * x4 * y3 + 35.0f * x6 * y1 - 30.0f * x4 * y1 - 6.0f * y5 + 60.0f * x2 * y3);
	Pzrnk.at<double>(29) = 4.0f * (63.0f * x6 * y1 + 105.0f * x4 * y3 + 21.0f * x2 * y5 - 21.0f * y7 - 90.0f * x4 * y1 - 60.0f * x2 * y3 + 30.0f * y5 + 30.0f * x2 * y1 - 10.0f * y3);
	Pzrnk.at<double>(30) = 4.0f * (35.0f * x6 * y1 + 105.0f * x4 * y3 + 105.0f * x2 * y5 + 35.0f * y7 - 60.0f * x4 * y1 - 120.0f * x2 * y3 - 60.0f * y5 + 30.0f * x2 * y1 + 30.0f * y3 - 4.0f * y1);
	Pzrnk.at<double>(31) = 4.0f * (35.0f * x7 + 105.0f * x5 * y2 + 105.0f * x3 * y4 + 35.0f * x1 * y6 - 60.0f * x5 - 120.0f * x3 * y2 - 60.0f * x1 * y4 + 30.0f * x3 + 30.0f * x1 * y2 - 4.0f * x1);
	Pzrnk.at<double>(32) = 4.0f * (21.0f * x7 - 21.0f * x5 * y2 - 105.0f * x3 * y4 - 63.0f * x1 * y6 - 30.0f * x5 + 60.0f * x3 * y2 + 90.0f * x1 * y4 + 10.0f * x3 - 30.0f * x1 * y2);
	Pzrnk.at<double>(33) = 4.0f * (7.0f * x7 - 63.0f * x5 * y2 - 35.0f * x3 * y4 + 35.0f * x1 * y6 - 6.0f * x5 + 60.0f * x3 * y2 - 30.0f * x1 * y4);
	Pzrnk.at<double>(34) = 4.0f * (x7 - 21.0f * x5 * y2 + 35.0f * x3 * y4 - 7.0f * x1 * y6);
}

//Zernike modes in polar coordinates
void CSensor::AlgRWF(cv::Mat& r, int i, double u, double  v)
{
	double X;

	double x1 = u;
	double x2 = u * x1;
	double x3 = u * x2;
	double x4 = u * x3;
	double x5 = u * x4;
	double x6 = u * x5;
	double x7 = u * x6;
	double x8 = u * x7;

	double y1 = v;
	double y2 = v * y1;
	double y3 = v * y2;
	double y4 = v * y3;
	double y5 = v * y4;
	double y6 = v * y5;
	double y7 = v * y6;
	double y8 = v * y7;

	double s2 = (double)sqrt(2.0f);
	double s3 = (double)sqrt(3.0f);
	double s5 = (double)sqrt(5.0f);
	double s6 = (double)sqrt(6.0f);
	double s7 = (double)sqrt(7.0f);
	double s10 = (double)sqrt(10.0f);
	double s14 = (double)sqrt(14.0f);

	X = 2.0f;
	r.at<double>(0, i) = X;
	X = 0.0f;
	r.at<double>(1, i) = X;
	X = 2.0f * s6 * y1;
	r.at<double>(2, i) = X;
	X = 4.0f * s3 * x1;
	r.at<double>(3, i) = X;
	X = 2.0f * s6 * x1;
	r.at<double>(4, i) = X;
	X = 12.0f * s2 * x1 * y1;
	r.at<double>(5, i) = X;
	X = 12.0f * s2 * x1 * y1;
	r.at<double>(6, i) = X;
	X = 2.0f * s2 * (9.0f * x2 + 3.0f * y2 - 2.0f);
	r.at<double>(7, i) = X;
	X = 6.0f * s2 * (x2 - y2);
	r.at<double>(8, i) = X;
	X = 4.0f * s10 * (3.0f * x2 * y1 - y3);
	r.at<double>(9, i) = X;
	X = 2.0f * s10 * (12.0f * x2 * y1 + 4.0f * y3 - 3.0f * y1);
	r.at<double>(10, i) = X;
	X = 12.0f * s5 * (2.0f * x3 + 2.0f * x1 * y2 - x1);
	r.at<double>(11, i) = X;
	X = s10 * (16.0f * x3 - 6.0f * x1);
	r.at<double>(12, i) = X;
	X = 4.0f * s10 * (x3 - 3.0f * x1 * y2);
	r.at<double>(13, i) = X;
	X = 40.0f * s3 * (x3 * y1 - x1 * y3);
	if (m_npzrnk > 14)r.at<double>(14, i) = X;
	X = 8.0f * s3 * (15.0f * x3 * y1 + 5.0f * x1 * y3 - 6.0f * x1 * y1);
	if (m_npzrnk > 15)r.at<double>(15, i) = X;
	X = 16.0f * s3 * (5.0f * x3 * y1 + 5.0f * x1 * y3 - 3.0f * x1 * y1);
	if (m_npzrnk > 16)r.at<double>(16, i) = X;
	X = 2.0f * s3 * (50.0f * x4 + 60.0f * x2 * y2 + 10.0f * y4 - 36.0f * x2 - 12.0f * y2 + 3.0f);
	if (m_npzrnk > 17)r.at<double>(17, i) = X;
	X = 2.0f * s3 * (25.0f * x4 - 30.0f * x2 * y2 - 15.0f * y4 - 12.0f * x2 + 12.0f * y2);
	if (m_npzrnk > 18)r.at<double>(18, i) = X;
	X = 10.0f * s3 * (x4 - 6.0f * x2 * y2 + y4);
	if (m_npzrnk > 19)r.at<double>(19, i) = X;
	X = 6.0f * s14 * (5.0f * x4 * y1 - 10.0f * x2 * y3 + y5);
	if (m_npzrnk > 20)r.at<double>(20, i) = X;
	X = 4.0f * s14 * (30.0f * x4 * y1 - 15.0f * x2 * y1 - 6.0f * y5 + 5.0f * y3);
	if (m_npzrnk > 21)r.at<double>(21, i) = X;
	X = 2.0f * s14 * (75.0f * x4 * y1 + 90.0f * x2 * y3 + 15.0f * y5 - 60.0f * x2 * y1 - 20.0f * y3 + 6.0f * y1);
	if (m_npzrnk > 22)r.at<double>(22, i) = X;
	X = 24.0f * s7 * (5.0f * x5 + 10.0f * x3 * y2 + 5.0f * x1 * y4 - 5.0f * x3 - 5.0f * x1 * y2 + x1);
	if (m_npzrnk > 23)r.at<double>(23, i) = X;
	X = s14 * (90.0f * x5 + 60.0f * x3 * y2 - 30.0f * x1 * y4 - 80.0f * x3 + 12.0f * x1);
	if (m_npzrnk > 24)r.at<double>(24, i) = X;
	X = 4.0f * s14 * (9.0f * x5 - 30.0f * x3 * y2 - 15.0f * x1 * y4 - 5.0f * x3 + 15.0f * x1 * y2);
	if (m_npzrnk > 25)r.at<double>(25, i) = X;
	X = 6.0f * s14 * (x5 - 10.0f * x3 * y2 + 5.0f * x1 * y4);
	if (m_npzrnk > 26)r.at<double>(26, i) = X;
	X = 56.0f * (3.0f * x5 * y1 - 10.0f * x3 * y3 + 3.0f * x1 * y5);
	if (m_npzrnk > 27)r.at<double>(27, i) = X;
	X = 4.0f * (210.0f * x5 * y1 - 126.0f * x1 * y5 - 140.0f * x3 * y3 - 120.0f * x3 * y1 + 120.0f * x1 * y3);
	if (m_npzrnk > 28)r.at<double>(28, i) = X;
	X = 24.0f * (63.0f * x5 * y1 + 70.0f * x3 * y3 + 7.0f * x1 * y5 - 60.0f * x3 * y1 - 20.0f * x1 * y3 + 10.0f * x1 * y1);
	if (m_npzrnk > 29)r.at<double>(29, i) = X;
	X = 120.0f * (7.0f * x5 * y1 + 14.0f * x3 * y3 + 7.0f * x1 * y5 - 8.0f * x3 * y1 - 8.0f * x1 * y3 + 2.0f * x1 * y1);
	if (m_npzrnk > 30)r.at<double>(30, i) = X;
	X = 4.0f * (245.0f * x6 + 525.0f * x4 * y2 + 315.0f * x2 * y4 + 35.0f * y6 - 300.0f * x4 - 360.0f * x2 * y2 - 60.0f * y4 + 90.0f * x2 + 30.0f * y2 - 4.0f);
	if (m_npzrnk > 31)r.at<double>(31, i) = X;
	X = 12.0f * (49.0f * x6 - 35.0f * x4 * y2 - 105.0f * x2 * y4 - 21.0f * y6 - 50.0f * x4 + 60.0f * x2 * y2 + 30.0f * y4 + 10.0f * x2 - 10.0f * y2);
	if (m_npzrnk > 32)r.at<double>(32, i) = X;
	X = 4.0f * (49.0f * x6 - 315.0f * x4 * y2 - 105.0f * x2 * y4 + 35.0f * y6 - 30.0f * x4 + 180.0f * x2 * y2 - 30.0f * y4);
	if (m_npzrnk > 33)r.at<double>(33, i) = X;
	X = 28.0f * (x6 - 15.0f * x4 * y2 + 15.0f * x2 * y4 - y6);
	if (m_npzrnk > 34)r.at<double>(34, i) = X;

	X = 0.0f;
	r.at<double>(0, i + 1) = X;
	X = 2.0f;
	r.at<double>(1, i + 1) = X;
	X = 2.0f * s6 * x1;
	r.at<double>(2, i + 1) = X;
	X = 4.0f * s3 * y1;
	r.at<double>(3, i + 1) = X;
	X = -2.0f * s6 * y1;
	r.at<double>(4, i + 1) = X;
	X = 6.0f * s2 * (x2 - y2);
	r.at<double>(5, i + 1) = X;
	X = 2.0f * s2 * (3.0f * x2 + 9.0f * y2 - 2.0f);
	r.at<double>(6, i + 1) = X;
	X = 12.0f * s2 * x1 * y1;
	r.at<double>(7, i + 1) = X;
	X = -12.0f * s2 * x1 * y1;
	r.at<double>(8, i + 1) = X;
	X = 4.0f * s10 * (x3 - 3.0f * x1 * y2);
	r.at<double>(9, i + 1) = X;
	X = 2.0f * s10 * (4.0f * x3 + 12.0f * x1 * y2 - 3.0f * x1);
	r.at<double>(10, i + 1) = X;
	X = 12.0f * s5 * (2.0f * x2 * y1 + 2.0f * y3 - y1);
	r.at<double>(11, i + 1) = X;
	X = s10 * (6.0f * y1 - 16.0f * y3);
	r.at<double>(12, i + 1) = X;
	X = 4.0f * s10 * (y3 - 3.0f * x2 * y1);
	r.at<double>(13, i + 1) = X;
	X = 10.0f * s3 * (x4 - 6.0f * x2 * y2 + y4);
	if (m_npzrnk > 14)r.at<double>(14, i + 1) = X;
	X = 2.0f * s3 * (15.0f * x4 + 30.0f * x2 * y2 - 25.0f * y4 - 12.0f * x2 + 12.0f * y2);
	if (m_npzrnk > 15)r.at<double>(15, i + 1) = X;
	X = 2.0f * s3 * (10.0f * x4 + 60.0f * x2 * y2 + 50.0f * y4 - 12.0f * x2 - 36.0f * y2 + 3.0f);
	if (m_npzrnk > 16)r.at<double>(16, i + 1) = X;
	X = 16.0f * s3 * (5.0f * x3 * y1 + 5.0f * x1 * y3 - 3.0f * x1 * y1);
	if (m_npzrnk > 17)r.at<double>(17, i + 1) = X;
	X = 8.0f * s3 * (6.0f * x1 * y1 - 5.0f * x3 * y1 - 15.0f * x1 * y3);
	if (m_npzrnk > 18)r.at<double>(18, i + 1) = X;
	X = 40.0f * s3 * (x1 * y3 - x3 * y1);
	if (m_npzrnk > 19)r.at<double>(19, i + 1) = X;
	X = 6.0f * s14 * (x5 - 10.0f * x3 * y2 + 5.0f * x1 * y4);
	if (m_npzrnk > 20)r.at<double>(20, i + 1) = X;
	X = 4.0f * s14 * (6.0f * x5 - 5.0f * x3 - 30.0f * x1 * y4 + 15.0f * x1 * y2);
	if (m_npzrnk > 21)r.at<double>(21, i + 1) = X;
	X = 2.0f * s14 * (15.0f * x5 + 90.0f * x3 * y2 + 75.0f * x1 * y4 - 20.0f * x3 - 60.0f * x1 * y2 + 6.0f * x1);
	if (m_npzrnk > 22)r.at<double>(22, i + 1) = X;
	X = 24.0f * s7 * (5.0f * y5 + 5.0f * x4 * y1 + 10.0f * x2 * y3 - 5.0f * y3 - 5.0f * x2 * y1 + y1);
	if (m_npzrnk > 23)r.at<double>(23, i + 1) = X;
	X = s14 * (30.0f * x4 * y1 - 90.0f * y5 - 60.0f * x2 * y3 + 80.0f * y3 - 12.0f * y1);
	if (m_npzrnk > 24)r.at<double>(24, i + 1) = X;
	X = 4.0f * s14 * (9.0f * y5 - 15.0f * x4 * y1 - 30.0f * x2 * y3 - 5.0f * y3 + 15.0f * x2 * y1);
	if (m_npzrnk > 25)r.at<double>(25, i + 1) = X;
	X = 6.0f * s14 * (10.0f * x2 * y3 - 5.0f * x4 * y1 - y5);
	if (m_npzrnk > 26)r.at<double>(26, i + 1) = X;
	X = 28.0f * (x6 - 35.0f * x4 * y2 + 35.0f * x2 * y4 - y6);
	if (m_npzrnk > 27)r.at<double>(27, i + 1) = X;
	X = 4.0f * (49.0f * y6 - 315.0f * x2 * y4 - 105.0f * x4 * y2 + 35.0f * x6 - 30.0f * x4 + 180.0f * x2 * y2 - 30.0f * y4);
	if (m_npzrnk > 28)r.at<double>(28, i + 1) = X;
	X = 4.0f * (63.0f * x6 + 315.0f * x4 * y2 + 105.0f * x2 * y4 - 147.0f * y6 - 90.0f * x4 - 180.0f * x2 * y2 + 150.0f * y4 + 30.0f * x2 - 30.0f * y2);
	if (m_npzrnk > 29)r.at<double>(29, i + 1) = X;
	X = 4.0f * (35.0f * x6 + 315.0f * x4 * y2 + 525.0f * x2 * y4 + 245.0f * y6 - 60.0f * x4 - 360.0f * x2 * y2 - 300.0f * y4 + 30.0f * x2 + 90.0f * y2 - 4.0f);
	if (m_npzrnk > 30)r.at<double>(30, i + 1) = X;
	X = 120.0f * (7.0f * x5 * y1 + 14.0f * x3 * y3 + 7.0f * x1 * y5 - 8.0f * x3 * y1 - 8.0f * x1 * y3 + 2.0f * x1 * y1);
	if (m_npzrnk > 31)r.at<double>(31, i + 1) = X;
	X = 24.0f * (20.0f * x3 * y1 - 7.0f * x5 * y1 - 70.0f * x3 * y3 - 63.0f * x1 * y5 + 60.0f * x1 * y3 - 10.0f * x1 * y1);
	if (m_npzrnk > 32)r.at<double>(32, i + 1) = X;
	X = 4.0f * (210.0f * x1 * y5 - 126.0f * x5 * y1 - 140.0f * x3 * y3 + 120.0f * x3 * y1 - 120.0f * x1 * y3);
	if (m_npzrnk > 33)r.at<double>(33, i + 1) = X;
	X = -56.0f * (3.0f * x5 * y1 - 10.0f * x3 * y3 + 3.0f * x1 * y5);
	if (m_npzrnk > 34)r.at<double>(34, i + 1) = X;
}

//Zernike modes in polar coordinates without Tip/Tilt
void CSensor::AlgRWFTT(cv::Mat& r, int i, double u, double  v)
{

	double X;

	double x1 = u;
	double x2 = u * x1;
	double x3 = u * x2;
	double x4 = u * x3;
	double x5 = u * x4;
	double x6 = u * x5;
	double x7 = u * x6;
	double x8 = u * x7;

	double y1 = v;
	double y2 = v * y1;
	double y3 = v * y2;
	double y4 = v * y3;
	double y5 = v * y4;
	double y6 = v * y5;
	double y7 = v * y6;
	double y8 = v * y7;

	double s2 = (double)sqrt(2.0f);
	double s3 = (double)sqrt(3.0f);
	double s5 = (double)sqrt(5.0f);
	double s6 = (double)sqrt(6.0f);
	double s7 = (double)sqrt(7.0f);
	double s10 = (double)sqrt(10.0f);
	double s14 = (double)sqrt(14.0f);

	X = 2.0f * s6 * y1;
	r.at<double>(0, i) = X;
	X = 4.0f * s3 * x1;
	r.at<double>(1, i) = X;
	X = 2.0f * s6 * x1;
	r.at<double>(2, i) = X;
	X = 12.0f * s2 * x1 * y1;
	r.at<double>(3, i) = X;
	X = 12.0f * s2 * x1 * y1;
	r.at<double>(4, i) = X;
	X = 2.0f * s2 * (9.0f * x2 + 3.0f * y2 - 2.0f);
	r.at<double>(5, i) = X;
	X = 6.0f * s2 * (x2 - y2);
	r.at<double>(6, i) = X;
	X = 4.0f * s10 * (3.0f * x2 * y1 - y3);
	r.at<double>(7, i) = X;
	X = 2.0f * s10 * (12.0f * x2 * y1 + 4.0f * y3 - 3.0f * y1);
	r.at<double>(8, i) = X;
	X = 12.0f * s5 * (2.0f * x3 + 2.0f * x1 * y2 - x1);
	r.at<double>(9, i) = X;
	X = s10 * (16.0f * x3 - 6.0f * x1);
	r.at<double>(10, i) = X;
	X = 4.0f * s10 * (x3 - 3.0f * x1 * y2);
	r.at<double>(11, i) = X;
	X = 40.0f * s3 * (x3 * y1 - x1 * y3);
	if (m_npzrnk > 14)r.at<double>(12, i) = X;
	X = 8.0f * s3 * (15.0f * x3 * y1 + 5.0f * x1 * y3 - 6.0f * x1 * y1);
	if (m_npzrnk > 15)r.at<double>(13, i) = X;
	X = 16.0f * s3 * (5.0f * x3 * y1 + 5.0f * x1 * y3 - 3.0f * x1 * y1);
	if (m_npzrnk > 16)r.at<double>(14, i) = X;
	X = 2.0f * s3 * (50.0f * x4 + 60.0f * x2 * y2 + 10.0f * y4 - 36.0f * x2 - 12.0f * y2 + 3.0f);
	if (m_npzrnk > 17)r.at<double>(15, i) = X;
	X = 2.0f * s3 * (25.0f * x4 - 30.0f * x2 * y2 - 15.0f * y4 - 12.0f * x2 + 12.0f * y2);
	if (m_npzrnk > 18)r.at<double>(16, i) = X;
	X = 10.0f * s3 * (x4 - 6.0f * x2 * y2 + y4);
	if (m_npzrnk > 19)r.at<double>(17, i) = X;
	X = 6.0f * s14 * (5.0f * x4 * y1 - 10.0f * x2 * y3 + y5);
	if (m_npzrnk > 20)r.at<double>(18, i) = X;
	X = 4.0f * s14 * (30.0f * x4 * y1 - 15.0f * x2 * y1 - 6.0f * y5 + 5.0f * y3);
	if (m_npzrnk > 21)r.at<double>(19, i) = X;
	X = 2.0f * s14 * (75.0f * x4 * y1 + 90.0f * x2 * y3 + 15.0f * y5 - 60.0f * x2 * y1 - 20.0f * y3 + 6.0f * y1);
	if (m_npzrnk > 22)r.at<double>(20, i) = X;
	X = 24.0f * s7 * (5.0f * x5 + 10.0f * x3 * y2 + 5.0f * x1 * y4 - 5.0f * x3 - 5.0f * x1 * y2 + x1);
	if (m_npzrnk > 23)r.at<double>(21, i) = X;
	X = s14 * (90.0f * x5 + 60.0f * x3 * y2 - 30.0f * x1 * y4 - 80.0f * x3 + 12.0f * x1);
	if (m_npzrnk > 24)r.at<double>(22, i) = X;
	X = 4.0f * s14 * (9.0f * x5 - 30.0f * x3 * y2 - 15.0f * x1 * y4 - 5.0f * x3 + 15.0f * x1 * y2);
	if (m_npzrnk > 25)r.at<double>(23, i) = X;
	X = 6.0f * s14 * (x5 - 10.0f * x3 * y2 + 5.0f * x1 * y4);
	if (m_npzrnk > 26)r.at<double>(24, i) = X;
	X = 56.0f * (3.0f * x5 * y1 - 10.0f * x3 * y3 + 3.0f * x1 * y5);
	if (m_npzrnk > 27)r.at<double>(25, i) = X;
	X = 4.0f * (210.0f * x5 * y1 - 126.0f * x1 * y5 - 140.0f * x3 * y3 - 120.0f * x3 * y1 + 120.0f * x1 * y3);
	if (m_npzrnk > 28)r.at<double>(26, i) = X;
	X = 24.0f * (63.0f * x5 * y1 + 70.0f * x3 * y3 + 7.0f * x1 * y5 - 60.0f * x3 * y1 - 20.0f * x1 * y3 + 10.0f * x1 * y1);
	if (m_npzrnk > 29)r.at<double>(27, i) = X;
	X = 120.0f * (7.0f * x5 * y1 + 14.0f * x3 * y3 + 7.0f * x1 * y5 - 8.0f * x3 * y1 - 8.0f * x1 * y3 + 2.0f * x1 * y1);
	if (m_npzrnk > 30)r.at<double>(28, i) = X;
	X = 4.0f * (245.0f * x6 + 525.0f * x4 * y2 + 315.0f * x2 * y4 + 35.0f * y6 - 300.0f * x4 - 360.0f * x2 * y2 - 60.0f * y4 + 90.0f * x2 + 30.0f * y2 - 4.0f);
	if (m_npzrnk > 31)r.at<double>(29, i) = X;
	X = 12.0f * (49.0f * x6 - 35.0f * x4 * y2 - 105.0f * x2 * y4 - 21.0f * y6 - 50.0f * x4 + 60.0f * x2 * y2 + 30.0f * y4 + 10.0f * x2 - 10.0f * y2);
	if (m_npzrnk > 32)r.at<double>(30, i) = X;
	X = 4.0f * (49.0f * x6 - 315.0f * x4 * y2 - 105.0f * x2 * y4 + 35.0f * y6 - 30.0f * x4 + 180.0f * x2 * y2 - 30.0f * y4);
	if (m_npzrnk > 33)r.at<double>(31, i) = X;
	X = 28.0f * (x6 - 15.0f * x4 * y2 + 15.0f * x2 * y4 - y6);
	if (m_npzrnk > 34)r.at<double>(32, i) = X;


	X = 2.0f * s6 * x1;
	r.at<double>(0, i + 1) = X;
	X = 4.0f * s3 * y1;
	r.at<double>(1, i + 1) = X;
	X = -2.0f * s6 * y1;
	r.at<double>(2, i + 1) = X;
	X = 6.0f * s2 * (x2 - y2);
	r.at<double>(3, i + 1) = X;
	X = 2.0f * s2 * (3.0f * x2 + 9.0f * y2 - 2.0f);
	r.at<double>(4, i + 1) = X;
	X = 12.0f * s2 * x1 * y1;
	r.at<double>(5, i + 1) = X;
	X = -12.0f * s2 * x1 * y1;
	r.at<double>(6, i + 1) = X;
	X = 4.0f * s10 * (x3 - 3.0f * x1 * y2);
	r.at<double>(7, i + 1) = X;
	X = 2.0f * s10 * (4.0f * x3 + 12.0f * x1 * y2 - 3.0f * x1);
	r.at<double>(8, i + 1) = X;
	X = 12.0f * s5 * (2.0f * x2 * y1 + 2.0f * y3 - y1);
	r.at<double>(9, i + 1) = X;
	X = s10 * (6.0f * y1 - 16.0f * y3);
	r.at<double>(10, i + 1) = X;
	X = 4.0f * s10 * (y3 - 3.0f * x2 * y1);
	r.at<double>(11, i + 1) = X;
	X = 10.0f * s3 * (x4 - 6.0f * x2 * y2 + y4);
	if (m_npzrnk > 14)r.at<double>(12, i + 1) = X;
	X = 2.0f * s3 * (15.0f * x4 + 30.0f * x2 * y2 - 25.0f * y4 - 12.0f * x2 + 12.0f * y2);
	if (m_npzrnk > 15)r.at<double>(13, i + 1) = X;
	X = 2.0f * s3 * (10.0f * x4 + 60.0f * x2 * y2 + 50.0f * y4 - 12.0f * x2 - 36.0f * y2 + 3.0f);
	if (m_npzrnk > 16)r.at<double>(14, i + 1) = X;
	X = 16.0f * s3 * (5.0f * x3 * y1 + 5.0f * x1 * y3 - 3.0f * x1 * y1);
	if (m_npzrnk > 17)r.at<double>(15, i + 1) = X;
	X = 8.0f * s3 * (6.0f * x1 * y1 - 5.0f * x3 * y1 - 15.0f * x1 * y3);
	if (m_npzrnk > 18)r.at<double>(16, i + 1) = X;
	X = 40.0f * s3 * (x1 * y3 - x3 * y1);
	if (m_npzrnk > 19)r.at<double>(17, i + 1) = X;
	X = 6.0f * s14 * (x5 - 10.0f * x3 * y2 + 5.0f * x1 * y4);
	if (m_npzrnk > 20)r.at<double>(18, i + 1) = X;
	X = 4.0f * s14 * (6.0f * x5 - 5.0f * x3 - 30.0f * x1 * y4 + 15.0f * x1 * y2);
	if (m_npzrnk > 21)r.at<double>(19, i + 1) = X;
	X = 2.0f * s14 * (15.0f * x5 + 90.0f * x3 * y2 + 75.0f * x1 * y4 - 20.0f * x3 - 60.0f * x1 * y2 + 6.0f * x1);
	if (m_npzrnk > 22)r.at<double>(20, i + 1) = X;
	X = 24.0f * s7 * (5.0f * y5 + 5.0f * x4 * y1 + 10.0f * x2 * y3 - 5.0f * y3 - 5.0f * x2 * y1 + y1);
	if (m_npzrnk > 23)r.at<double>(21, i + 1) = X;
	X = s14 * (30.0f * x4 * y1 - 90.0f * y5 - 60.0f * x2 * y3 + 80.0f * y3 - 12.0f * y1);
	if (m_npzrnk > 24)r.at<double>(22, i + 1) = X;
	X = 4.0f * s14 * (9.0f * y5 - 15.0f * x4 * y1 - 30.0f * x2 * y3 - 5.0f * y3 + 15.0f * x2 * y1);
	if (m_npzrnk > 25)r.at<double>(23, i + 1) = X;
	X = 6.0f * s14 * (10.0f * x2 * y3 - 5.0f * x4 * y1 - y5);
	if (m_npzrnk > 26)r.at<double>(24, i + 1) = X;
	X = 28.0f * (x6 - 35.0f * x4 * y2 + 35.0f * x2 * y4 - y6);
	if (m_npzrnk > 27)r.at<double>(25, i + 1) = X;
	X = 4.0f * (49.0f * y6 - 315.0f * x2 * y4 - 105.0f * x4 * y2 + 35.0f * x6 - 30.0f * x4 + 180.0f * x2 * y2 - 30.0f * y4);
	if (m_npzrnk > 28)r.at<double>(26, i + 1) = X;
	X = 4.0f * (63.0f * x6 + 315.0f * x4 * y2 + 105.0f * x2 * y4 - 147.0f * y6 - 90.0f * x4 - 180.0f * x2 * y2 + 150.0f * y4 + 30.0f * x2 - 30.0f * y2);
	if (m_npzrnk > 29)r.at<double>(27, i + 1) = X;
	X = 4.0f * (35.0f * x6 + 315.0f * x4 * y2 + 525.0f * x2 * y4 + 245.0f * y6 - 60.0f * x4 - 360.0f * x2 * y2 - 300.0f * y4 + 30.0f * x2 + 90.0f * y2 - 4.0f);
	if (m_npzrnk > 30)r.at<double>(28, i + 1) = X;
	X = 120.0f * (7.0f * x5 * y1 + 14.0f * x3 * y3 + 7.0f * x1 * y5 - 8.0f * x3 * y1 - 8.0f * x1 * y3 + 2.0f * x1 * y1);
	if (m_npzrnk > 31)r.at<double>(29, i + 1) = X;
	X = 24.0f * (20.0f * x3 * y1 - 7.0f * x5 * y1 - 70.0f * x3 * y3 - 63.0f * x1 * y5 + 60.0f * x1 * y3 - 10.0f * x1 * y1);
	if (m_npzrnk > 32)r.at<double>(30, i + 1) = X;
	X = 4.0f * (210.0f * x1 * y5 - 126.0f * x5 * y1 - 140.0f * x3 * y3 + 120.0f * x3 * y1 - 120.0f * x1 * y3);
	if (m_npzrnk > 33)r.at<double>(31, i + 1) = X;
	X = -56.0f * (3.0f * x5 * y1 - 10.0f * x3 * y3 + 3.0f * x1 * y5);
	if (m_npzrnk > 34)r.at<double>(32, i + 1) = X;
}

//Pre-calculation AT*(AT*A)^(-1)=F 
void CSensor::Pre_Zrnk(cv::Mat& r,int nlens, int cntzrnk,const cv::Mat& x,const cv::Mat& y,int tt) {
	//nlens= m_srastr * m_srastr  x=x0 y=y0;  to all lenses  
	//nlens=m_nlens   y=x0l x=y0l; to active lenses only  
	//cntzrnk =  m_npzrnk or =m_npzrnk-2 if without Tip/Tilt
	cv::Mat rx, ry,rf,rcft;
	rx = cv::Mat::zeros(cntzrnk, nlens * 2, CV_64FC1);//A array Zernike poly to aproximation
	rcft = cv::Mat::zeros(cntzrnk, cntzrnk, CV_64FC1);//AT*A 
	ry = cv::Mat::zeros(cntzrnk, cntzrnk, CV_64FC1);//(AT*A)^(-1) 

	int j = 0;
	if (tt) {
		for (int i = 0; i < nlens; i++)
		{
			AlgRWF(rx, j, x.at<double>(i), y.at<double>(i));
			j += 2;
		}
	 }
	else {
		for (int i = 0; i < nlens; i++)
		{
			AlgRWFTT(rx, j, x.at<double>(i), y.at<double>(i));
			j += 2;
		}
	}

	cv::mulTransposed(rx, rcft, FALSE);//B=(A*AT)  
	cv::invert(rcft, ry);// C=B^(-1)=(A*AT)^(-1)  
	r = rx.t() * ry;// AT*C
}



void CSensor::GetR0 (std::deque<double>& diff) {
	double  powdisp, powlLwave, powDa, powd,dst, powdst;

	double rad = (PI / 180);

	double sum = std::accumulate(diff.begin(), diff.end(), 0.0);
	double mean = sum / diff.size(); //or sze-1 for no biased slightly 
	std::vector<double> diff1(diff.size());
	std::transform(diff.begin(), diff.end(), diff1.begin(),	std::bind2nd(std::minus<double>(), mean));
	double sq_sum = std::inner_product(diff1.begin(), diff1.end(), diff1.begin(), 0.0);
	
	m_Dispersion = sq_sum / diff.size();
	//double cnst = (m_pixsze / m_focuscam) * rad;
	//cnst = cnst * cnst;
	//m_Dispersion = m_Dispersion*cnst;

	powdisp = pow(m_Dispersion, 3.0 / 5.0);
	powdisp = 1 / powdisp;
	powlLwave = pow(m_lwave, 6.0 / 5.0);
	powDa = pow(m_EntranceDiameter, 1.0 / 3.0);
	powDa = 1 / powDa;
	powd = pow(m_distbetSub, 1.0 / 3.0);
	powd = 1 / powd;
	dst = m_R0k2 * powDa - m_R0k3 * powd;
    powdst = pow(dst, 3.0 / 5.0);

	m_R0 = powdisp * m_R0k1 * powlLwave * powdst;
	m_Cn2 = (1 / pow((m_Cn2k1 * m_R0), 3.0 / 5.0)) / (m_Cn2k2 * (DPI / m_lwave) * (DPI / m_lwave) * m_lpath);//??
}