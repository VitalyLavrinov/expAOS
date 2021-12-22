/*from VimbaC clib to Camera class*/
#pragma once
#include <string>
#include <chrono>
#include <deque>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
//#include <boost/filesystem/fstream.hpp> // linking error !!!
#include <numeric>


#include "VimbaC.h"  

bool fileExists(const std::string& filename);//Check if a file exists

//namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

constexpr double DPI(2.0*3.14159265359);
constexpr double PI(3.14159265359);
constexpr int FRAME_COUNT(3); //cnt frames to asynchronous input 

        class CCamera
        {
        private:
            CDC* outdc;
            std::string m_exposname;
            std::string m_ini;
            int width;
            int height;
            int offsetx;
            int offsety;
            int expos; 
            bool m_Connected;
            bool bGigE;// true if GigE cam interface, needs to SetExpos function??
            const char*         pCameraID;
            VmbError_t          err;     // Initialize the Vimba API
            VmbCameraInfo_t* pCameras;             // A list of camera details
            VmbUint32_t         nCount;                // Number of found cameras
            VmbUint32_t         nFoundCount;                // Change of found cameras ??? work if cam pluged after prog run
            VmbUint32_t         nTimeout;             // Timeout for Grab
            VmbAccessMode_t     cameraAccessMode;// We open the camera with full access
            VmbHandle_t         cameraHandle;             // A handle to our camera
            VmbBool_t           bIsCommandDone;     // Has a command finished execution
            VmbFrame_t          frame;                                  // The frame we capture
            VmbInt64_t          nPayloadSize;                // The size of one frame
           
            void GetCamIni(const std::string& ini);//Cam prop from inifile
            int SetFrameFeacher(int, int, int, int);//Seter frame width offsets 
            //fps counter
            double m_fps;
            using clock_t = std::chrono::high_resolution_clock;
            using second_t = std::chrono::duration<double, std::ratio<1> >;
            std::chrono::time_point<clock_t> m_beg;
            int m_fcnt; //framecounter to get fps
            int m_cnt4fps; //sample length to get fps

            std::deque<cv::Mat> framesBuf;// buffer to grab film

           public:
            bool m_FSTART;
            CCamera();
            CCamera(const std::string &ini, const char* CamId, const std::string& exposname);
            ~CCamera() { m_FSTART = 0; CameraClose(); /*VmbShutdown();*/ }
            int CameraConnect(); //Connect Camera by id;
            int CameraList();// Get all cameras to pCameras
            void CameraClose();// camera kill
            int GetCamExpos();//get expos from camera
            int SetCamExpos(float);//set expos tp camera
             int GetExpos() const { return expos; }//getter
            int GetOffsetx() const { return offsetx; }//getter
            int GetOffsety() const { return offsety; }//getter
            int Getheight() const { return height; }//getter
            bool GetConnected() const;// Gets connect flag
            int CameraFrame(); //get One frame
            int CameraFrameN();// start frame stream
            virtual void DrowFrame(const cv::Mat& out, int left, int top, double scale) const; //Drow cam.frame with left top offsets on CDC with scale. 
            virtual void SetDC(CDC* odc);//output CDC set
            cv::Mat GetFrameMat() const;//Get frame like CvMat
            cv::Mat GetFrameMat(int i) const; //Get frame like CvMat from array frames[] when asynchronous input 

            /*fps calculator*/
            void setzerotime() { m_beg = clock_t::now(); }
            void fps() { m_fps = m_fcnt / std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count(); }
            double elapsedtime() const { return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();}
            double fpsiter() { m_fcnt++; if (m_fcnt >= m_cnt4fps) { fps(); setzerotime(); m_fcnt = 0; } return m_fps;}//fps iter calc
            double getfps() const { return m_fps;}//fps getter

            void  AddFrameInBuf(cv::Mat& frame); //add frame in bytefilm buffer (deque)
            void  SaveFrameBufInFile(std::string filename);//saveing buffer as bytefilm *flm

           protected:
            VmbFrame_t          frames[FRAME_COUNT];        //buff used when asynchronous input  

        };

