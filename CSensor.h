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
	cv::Mat_<cv::Rect> subs;// subapertures rects
	cv::Mat x0; //
	cv::Mat y0;
	cv::Mat x0l; // -||- but only active lenses
	cv::Mat y0l; // -||- but only active lenses
	cv::Mat accumframe;// array to get rafframe from cnt frames

	/*zernike modes & wavefront surface*/
	cv::Mat x_lens;
	cv::Mat y_lens;
	cv::Mat Pzrnk;
	cv::Mat wfphase;
	cv::Mat wfinterf;

	void Plnm_Zrnk(double u, double v);//Zernike modes to xylens grid to drow wavefront surface
	void Pre_Zrnk(cv::Mat& r, int nlens, int cntzrnk, const cv::Mat& x, const cv::Mat& y, int tt);//Pre-calculation AT*(AT*A)^(-1)=F 
	void AlgRWF(cv::Mat& r, int i, double u, double  v); //Zernike modes in polar coordinates without 
	void AlgRWFTT(cv::Mat& r, int i, double u, double  v); //Zernike modes in polar coordinates without Tip/Tilt

	void LoadIni(const std::string& ini);



	unsigned int m_Speccnt;
	unsigned int m_idframe;
	cv::Point2d m_maxindec;
	cv::Point2d m_averindec;

	int m_subtrah;
	double m_EntranceDiameter;
	double m_distbetSub;
	double m_R0k1;
	double m_R0k2;
	double m_R0k3;
	double m_Cn2k1;
	double m_Cn2k2;

	double m_Dispersion;
	double m_R0;
	double m_Cn2;

public:
	using CCamera::frames;
	cv::Mat outframe;
	cv::Mat_<cv::Point> tPtr; //left top points of subapertures
	cv::Mat Lenses;//active lenses in rastr
	cv::Mat_<cv::Point> subMax; //maxval coords in subapertures 
	cv::Mat MaxI; //maxvals in subapertures 
	cv::Mat CTMax; //coords CoG  in subs
	cv::Mat CTMaxReff; //refframe cords CoG in subs
	cv::Mat CTMaxDif; // coords of CoG - coords FeFrame  CoG in subs
	cv::Mat CTMaxDifl;// -||- but only active lenses
	/*coef. Zernike modes*/
	cv::Mat wfCT;
	cv::Mat wfCTTT;
	cv::Mat wfCTl;
	cv::Mat wfCTTTl;

	cv::Mat Fmat;
	cv::Mat Fmatl;
	cv::Mat FmatTT;
	cv::Mat FmatTTl;
	/*Correlation offsets*/
    cv::Mat CTCorr;
	cv::Mat CTCorrl;
	cv::Mat wfCorr;
	cv::Mat wfCorrl;
	cv::Mat Han;
	cv::Mat ReffKor;

	CSensor();
	CSensor(const std::string& ini, const char* CamId);
	void ReLoadData();
	virtual void DrowFrame(const cv::Mat& out, int left, int top, double scale) const; //Drow cam.frame with left top offsets with scale. 
	virtual void DrowSub(const cv::Mat& out, int left, int top, double scale) const;
	void CheckLens(const std::string& ini);// read active lenses from file & reload data as need
    void GetRefFrame(int cnt); // get refframe from sensor cam

	/*CofG offsets*/
	void GetMaxWfs(cv::Mat& src);//!!need remaking USE RECTS//calc maxvals in subs
	void GetCTWfs(cv::Mat& src);//!! may be need remaking //calc offsets coords center of gravity of the spots in subs
	void GetCTOffsetsWfs(cv::Mat& src);//!!CALLed GetMaxWfs & GetCTWfs// calc offsets between CoG coords & RefFrame coordsin subs(without nonactive lenses, its==0.0)

	void GetCTWfsCorr(const cv::Mat& src);//calc subapertures images offsets

	/*coef. Zernike modes*/
	cv::Mat Get_Zrnk(cv::Mat& difcor, cv::Mat refar);//calculate Zernike coef.
	void GetWF(cv::Mat& wfzrnk, int begin, int endshft);//calculate wavefront surface & interferogram
	void DrowPhase(int left, int top, double scale, int interfer);// drow wave front surfsce if  interfer==1 interferogram

	virtual void SetDC(CDC* dc) {odc = dc;}//output CDC set
	virtual void SetWfDC(CDC* dc) {wfdc = dc;}//wavefront output CDC set
	void SaveLenses(const std::string& ini) const;//saving active lens array
	void SetLensesMoreThanI(int in);
	void SaveIni(const std::string& ini) const;//save sensor propeties to ini file

	int Get_sdx() const { return m_sdx; }//getter
	int Get_sdy() const { return m_sdy; }//getter
	int Get_swnd() const { return m_swnd; }//getter
	int Get_left() const { return m_left; }//getter
	int Get_top() const { return m_top; }//getter
	int Get_shftcol() const { return m_shftcol; }//getter
	int Get_shftrow() const { return m_shftrow; }//getter
	int Get_norm() const { return m_norm; }//getter
	int Get_sub() const { return m_sub; }//getter
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

	cv::Point2d Get_maxindec() const { return m_maxindec; }//getter
	cv::Point2d Get_averindec() const { return m_averindec; }//getter
	cv::Point2d Get_CTm_subinPoint() const { return cv::Point2d(CTMaxDif.at<double>(m_sub-1), CTMaxDif.at<double>(m_sub)); } //Geter point shift CT.x CT.y in subaperture m_sum

	std::deque <double> CTdecX;// dx in deque
	std::deque <double> SpecX;// dx spectrum
	std::deque <double> CTdecY;//dy in deque
	std::deque <double> SpecY;// dx spectrum

	void CTDeqAdd();//add mesuared vals in deques
	void GetStatCTDeq();//get statistic in CT deq
	void Spectrum(int N, double f0, double fen, double fd, std::deque<double>& CT, std::deque<double>& SP);// calculate spectrum
	void DrowSpectrum(int N, int x, int y, double scl1, int fd, std::deque<double>& SP);//drow spectrum

	int Get_m_Speccnt() const { return m_Speccnt; }//getter

	/*Cn2*/
	std::deque <double> CTDiffX;// diffs between m_sub & m_subtresh
	std::deque <double> CTDiffY;// diffs between m_sub & m_subtresh
	std::deque <double> AngelAlfaX;// angels dx : focus 
	std::deque <double> AngelAlfaY;// angels dy : focus 

	void CSensor::GetR0(std::deque<double>& diff);//  calculate Freed parameter

	double Get_Dispersion() const { return m_Dispersion; }//getter
	double Get_R0() const { return m_R0; }//getter
	double Get_Cn2() const { return m_Cn2; }//getter

	void GetStatDeq(std::deque<double>& deq, double& maxin, double& averin);//calc max & aver val in deq

};