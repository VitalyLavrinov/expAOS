#pragma once
#include "CCamera.h"  

class CSensor : public CCamera
{
private:
	/*S-H wavefront sensor grid prop*/
	int m_cdx; //frame size
	int m_cdy;
	int m_srastr;// rastr size
	int m_sdx; //subaperture size
	int m_sdy;
	int m_swnd; //analysis window size
	int m_left; // left rastr offset
	int m_top; // top raster offset
	int m_shftcol; // shift between subapertures (cols)
	int m_shftrow; // -||- (rows)
	int m_norm; // normalization to get circle of unit radius in pxls ( radius of the circumscribed and inscribed circle)
	int m_sub; // active subaperture to analysis
	int m_nlens;// used  instead of "srastr" if we have incompleted lenslet array

	/*wavefront in Zernike modes prop*/
	double m_szelens; // lens diameter in mm
	double m_pixsze; // pixel size mm
	double m_lwave; // wavelenth mm
	double m_focuscam;// lenslet array focus mm
	double m_lpath;// path length m
	double m_coefshift;// calculates from uppers 5, used to get coef. Zernike polynomials;
	int m_npzrnk; // cnt calculated  coef. Zernike polynomials
	int m_ngrid; // phase and interferogram output windowsize
	int m_treshlow;//thresholding vals
	int m_treshhigh;

	CDC* odc;// output CDC to frame
	CDC* wfdc;// output CDC to wavefront

	cv::Mat_<cv::Rect> wnds;// analysis window rects
	//cv::Mat_<cv::Rect> subs;// subapertures rects
	cv::Mat x0; // grid to zrnk coef
	cv::Mat y0;
	cv::Mat x0l; // -||- but only active lenses
	cv::Mat y0l; // -||- but only active lenses


	/*zernike modes & wavefront surface*/
	cv::Mat x_lens; //grid to WF
	cv::Mat y_lens; //grid to WF
	cv::Mat Pzrnk; // zrnk coef.
	cv::Mat wfphase;//drow phase array 
	cv::Mat wfinterf;//drow  array 

	void Plnm_Zrnk(double u, double v);//Zernike modes to xylens grid to drow wavefront surface
	void Pre_Zrnk(cv::Mat& r, int nlens, int cntzrnk, const cv::Mat& x, const cv::Mat& y, int tt);//Pre-calculation AT*(AT*A)^(-1)=F 
	void AlgRWF(cv::Mat& r, int i, double u, double  v); //Zernike modes in polar coordinates without 
	void AlgRWFTT(cv::Mat& r, int i, double u, double  v); //Zernike modes in polar coordinates without Tip/Tilt

	void LoadIni(const std::string& ini);// boost::fsystem format of writing and reading raises questions !!!

	unsigned int m_Speccnt;// sample length for spectrum calc
	cv::Point2d m_maxindec;// max val in deque of (x,y)
	cv::Point2d m_averindec;// abs(mean) val in deque of (x,y)

	int m_subtrah; // subaperture number to diff subtraction

	 struct  turbstat {
		double m_R0k1;// constants of the formula
		double m_R0k2;//for calculating the coefficients
		double m_R0k3;//of the structure function
		double m_Cn2k1;//of the refractive index
		double m_Dispersion;//
		double m_R0;//Fried parameter
		double m_Cn2;//structure function coefficients of the refractive index

	};


	 double m_EntranceDiameter;// lens diameter on photodetector matrix in sm !!!   G=14.817449
	 double m_distbetSub;//....distance between centers of subapertures on telescope (sub--subtrah) ?!!!
	 double m_r0k1;//CONSTANT FROM INI file to r0=r0K1*lwave^6/5 *(  [r0K2*D^-1/3 - r0K3(xy)*d^-1/3] / DISP )^3/5
	 double m_r0k2;
	 double m_r0k3x;
	 double m_r0k3y;
	 double m_cn2k1;// cn2=DISP/ (Ñ1*L*[r0K2*D^-1/3 - r0K3(xy)*d^-1/3])

	void TurbConstInit();

public:
	cv::Mat_<cv::Rect> subs;// subapertures rects
	using CCamera::frames;//frames buffer
	cv::Mat outframe;// drow array
	cv::Mat accumframe;// array to get rafframe from cnt frames
	cv::Mat_<cv::Point> tPtr; //left top points of subapertures
	cv::Mat Lenses;//active lenses in rastr
	cv::Mat_<cv::Point> subMax; //maxval coords in subapertures 
	cv::Mat MaxI; //maxvals in subapertures 
	cv::Mat CTMax; //coords CoG  in subs
	cv::Mat CTMaxReff; //refframe cords CoG in subs
	cv::Mat CTMaxDif; // coords of CoG - coords FeFrame  CoG in subs
	cv::Mat CTMaxDifl;// -||- but only active lenses
	/*coef. Zernike modes*/
	cv::Mat Fmat; 
	cv::Mat Fmatl; // F= AT* (AT* A) ^ (-1) matrix, calculated once during sensor initialization
	cv::Mat FmatTT; // Zrnkcoef C= B*F where B offsets of spot coordinates
	cv::Mat FmatTTl;

	cv::Mat wfCT;// results for different fillings of the lenslet, taking into account the tip/tilt presence
	cv::Mat wfCTTT;
	cv::Mat wfCTl;
	cv::Mat wfCTtmpl;
	cv::Mat wfCTTTl;

	/*Correlation offsets*/
    cv::Mat CTCorr;
	cv::Mat CTCorrl;
	cv::Mat wfCorr;
	cv::Mat wfCorrl;
	cv::Mat Han;// Hanning window
	cv::Mat ReffKor;// reference frame for calculating correlation

	CSensor();
	CSensor(const std::string& ini, const char* CamId, const std::string& expname);
	~CSensor() {
		CTdecX.clear();
		SpecX.clear();
		CTdecY.clear();
		SpecY.clear();
		CTDiffX.clear();
		CTDiffY.clear();
		CTdecX.~deque();
		SpecX.~deque();
		CTdecY.~deque();
		SpecY.~deque();
		CTDiffX.~deque();
		CTDiffY.~deque();
	};
	void ReLoadData();// called when the sensor parameters are changed
	virtual void DrowFrame(const cv::Mat& out, int left, int top, double scale) const; //Drow cam.frame with left top offsets with scale. 
	virtual void DrowSub(const cv::Mat& out, int left, int top, double scale) const;//Drow m_sub
	void CheckLens(const std::string& ini);// read active lenses from file & reload data as need
	void GetAccumFrames(int cnt);//get accumulate cnt frames matrix with tresholding   into cvMat accumframe
    void GetRefFrame(int cnt); // get refframe from sensor cam

	/*CofG offsets*/
	void GetMaxWfs(cv::Mat& src);//!!need remaking USE RECTS//calc maxvals in subs
	void GetCTWfs(cv::Mat& src);//!! may be need remaking //calc offsets coords center of gravity of the spots in subs
	void GetCTOffsetsWfs(cv::Mat& src);//!!CALLed GetMaxWfs & GetCTWfs// calc offsets between CoG coords & RefFrame coordsin subs(without nonactive lenses, its==0.0)

	void GetCTWfsCorr(const cv::Mat& src);//calc subapertures images offsets


	/*coef. Zernike modes*/
	cv::Mat Get_Zrnk(cv::Mat& difcor, cv::Mat& refar);//calculate Zernike coef.
	void SaveFrameZrnk(std::string filename, cv::Mat& ZRNK);//saveing zernike coef. like *txt file
	void GetWF(cv::Mat& wfzrnk, int begin, int endshft);//calculate wavefront surface & interferogram
	void DrowPhase(int left, int top, double scale, int interfer);// drow wave front surfsce if  interfer==1 interferogram

	virtual void SetDC(CDC* dc) {odc = dc;}//output CDC set
	virtual void SetWfDC(CDC* dc) {wfdc = dc;}//wavefront output CDC set
	void SaveLenses(const std::string& ini) const;//saving active lens array
	void SetLensesMoreThanI(int in); // active lenses choice by intensity
	virtual void SaveIni(const std::string& ini) const;//save sensor propeties to ini file, boost::fsystem format of writing and reading raises questions !!!


	int Get_cdx() const { return m_cdx; }//getter
	int Get_cdy() const { return m_cdy; }//getter
	int Get_sdx() const { return m_sdx; }//getter
	int Get_sdy() const { return m_sdy; }//getter
	int Get_swnd() const { return m_swnd; }//getter
	int Get_left() const { return m_left; }//getter
	int Get_top() const { return m_top; }//getter
	int Get_shftcol() const { return m_shftcol; }//getter
	int Get_shftrow() const { return m_shftrow; }//getter
	int Get_norm() const { return m_norm; }//getter
	int Get_sub() const { return m_sub; }//getter
	int Get_subtrah() const {return m_subtrah;}
	double Get_szelens() const { return m_szelens; }//getter
	double Get_pixsze() const { return m_pixsze; }//getter
	double Get_lwave() const { return m_lwave; }//getter
	double Get_focuscam() const { return m_focuscam; }//getter
	double Get_lpath() const { return m_lpath; }//getter
	double Get_coefshift() const { return m_coefshift; }//getter
	int Get_ngrid() const { return m_ngrid; }//getter
	int Get_nlens() const { return m_nlens; }//getter
	int Get_srastr() const { return m_srastr; }//getter
	int Get_npzrnk() const { return m_npzrnk; }//getter
	int Get_treshlow() const { return m_treshlow; }//getter
	int Get_treshhigh() const { return m_treshhigh; }//getter

	void Set_sdx(int val) { m_sdx = val; }//Setter
	void Set_sdy(int val) { m_sdy = val; }//Setter
	void Set_swnd(int val) { m_swnd = val; }//Setter
	void Set_left(int val) { m_left = val; }//Setter
	void Set_top(int val) { m_top = val; }//Setter
	void Set_shftcol(int val) { m_shftcol = val; }//Setter
	void Set_shftrow(int val) { m_shftrow = val; }//Setter
	void Set_norm(int val) { m_norm = val; }//Setter
	void Set_sub(int val) { m_sub = val; }//Setter
	void Set_szelens(double val) { m_szelens = val; }//Setter
	void Set_pixsze(double val) { m_pixsze = val; }//Setter
	void Set_lwave(double val) { m_lwave = val; }//Setter
	void Set_focuscam(double val) { m_focuscam = val; }//Setter
	void Set_lpath(double val) { m_lpath = val; }//Setter
	void Set_coefshift(double val) { m_coefshift = val; }//Setter
	void Set_ngrid(int val) { m_ngrid = val; }//Setter
	void Set_nlens(int val) { m_nlens = val; }//Setter
	void Set_srastr(int val) { m_srastr = val; }//Setter
	void Set_npzrnk(int val) { m_npzrnk = val; }//Setter
	void Set_treshlow(int val) { m_treshlow = val; }//Setter
	void Set_treshhigh(int val) { m_treshhigh = val; }//Setter
	void Set_subtrah(int val) { m_subtrah = val; }//Setter

	cv::Point2d Get_maxindec() const { return m_maxindec; }//getter
	cv::Point2d Get_averindec() const { return m_averindec; }//getter
	cv::Point2d Get_CTm_subinPoint() const { return cv::Point2d(CTMaxDif.at<double>(m_sub-1), CTMaxDif.at<double>(m_sub)); } //Geter point shift CT.x CT.y in subaperture m_sum

	std::deque <double> CTdecX;// dx in deque
	std::deque <double> SpecX;// dx spectrum
	std::deque <double> CTdecY;//dy in deque
	std::deque <double> SpecY;// dx spectrum

	void CTDeqAdd();//add mesuared vals in deques
	void CTDeqAddOneSub();//add mesuared vals in deques if used one sub

	void GetStatDeq(std::deque<double>& deq, double& maxin, double& averin);//calc max & abs(mean) val in deq
	void GetStatCTDeq();//get statistic in CTdecX,CTdecY res in Point2d

	void Spectrum(int N, double f0, double fen, double fd, std::deque<double>& CT, std::deque<double>& SP);// calculate spectrum
	void DrowSpectrum(CDC* sdc, int N, int x, int y, double scl1, int fd, std::deque<double>& SP);//drow spectrum

	int Get_m_Speccnt() const { return m_Speccnt; }//getter
	void Set_m_Speccnt(int val) { m_Speccnt=val; }//setter

	/*Cn2*/
	std::deque <double> CTDiffX;// diffs between m_sub & m_subtresh
	std::deque <double> CTDiffY;// diffs between m_sub & m_subtresh

	double CSensor::CalcDisp(std::deque<double>& diff);// calculate Dispersion
	void CSensor::CalcR0(std::deque<double>& diff, turbstat& stat,double Disp);//  calculate Freed parameter && cn2

	turbstat m_statx;//statistics struct for x and y axis to diff sub-subtrah
	turbstat m_staty;
	turbstat m_statonex;//statistics struct for one chanel x and y axis
	turbstat m_statoney;

	double Get_Dispersion(turbstat& stat) const { return stat.m_Dispersion; }//getter
	double Get_R0(turbstat& stat) const { return stat.m_R0; }//getter
	double Get_Cn2(turbstat& stat) const { return stat.m_Cn2; }//getter

	double Get_entrancediameter() const { return m_EntranceDiameter; }//getter
	double Get_distbetsub() const { return m_distbetSub; }//getter
	void Set_entrancediameter(double val) { m_EntranceDiameter = val; }//Setter
	void Set_distbetsub(double val) { m_distbetSub = val; }//Setter

	double Get_r0k1() const { return m_r0k1; }//getter
	double Get_r0k2() const { return m_r0k2; }//getter
	double Get_r0k3x() const { return m_r0k3x; }//getter
	double Get_r0k3y() const { return m_r0k3y; }//getter
	double Get_cn2k1() const { return m_cn2k1; }//getter


	cv::Mat Clahe(const cv::Mat& out, double ClipLimit) const;//CLAHE  8x8
};