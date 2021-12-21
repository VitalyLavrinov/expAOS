
#include "pch.h"
#include "CCamera.h"

   // without starting  Vimba API, used only for model films
   CCamera::CCamera() {
    m_ini = "default.ini";
    m_FSTART = false;
    m_Connected = false;
    err = NULL;
    pCameras = NULL;             // A list of camera details
    nCount = 0;                // Number of found cameras
    nFoundCount = 0;                // Change of found cameras
    nTimeout = 2000;             // Timeout for Grab
    cameraAccessMode = VmbAccessModeFull;// We open the camera with full access
    cameraHandle = NULL;             // A handle to our camera
    bIsCommandDone = VmbBoolFalse;     // Has a command finished execution
    nPayloadSize = 0;                // The size of one frame

    m_fps = 0.0;
    m_cnt4fps = 1000; //!! because max Cam fps in our system 1000
    m_fcnt = 0;
    GetCamIni(m_ini);
  }
  // with starting  Vimba API, NOT USE  for model films
  CCamera::CCamera(const std::string &ini, const char* CI) {
            pCameraID = CI;
            m_ini = ini;
            m_FSTART = false;
            m_Connected = false;
            m_fps = 0.0;
            err = NULL;  
            pCameras = NULL;             // A list of camera details
            nCount = 0;                // Number of found cameras
            nFoundCount = 0;                // Change of found cameras
            nTimeout = 2000;             // Timeout for Grab
            cameraAccessMode = VmbAccessModeFull;// We open the camera with full access
            cameraHandle = NULL;             // A handle to our camera
            bIsCommandDone = VmbBoolFalse;     // Has a command finished execution
            nPayloadSize = 0;                // The size of one frame

            m_fps = 0.0;
            m_cnt4fps = 1000;
            m_fcnt = 0;

            VmbStartup();
            GetCamIni(m_ini);
}

  //add frame in bytefilm buffer
  void  CCamera::AddFrameInBuf(cv::Mat& frame) {
      framesBuf.push_back(frame);
  }

  //saveing buffer as bytefilm *flm
  void  CCamera::SaveFrameBufInFile(std::string filename) {
        std::ofstream outf(filename, std::ios::binary);
        if (outf) {
            for (int i = 0; i < framesBuf.size(); i++)
            {
                outf.write((char*)&framesBuf[i].data, sizeof framesBuf[i].data);
            }
            outf.close();
            framesBuf.clear();

        }
    //  fs::path filepath(filename);
    //  fs::ofstream outf(filepath, std::ios::binary);
    //  for (auto& it : framesBuf) outf.write((char*)&it.data, sizeof(it.data));

  }

        //output CDC set
        void CCamera::SetDC(CDC* odc) {
            outdc = odc;
        }

        //Get frame like CvMat
        cv::Mat CCamera::GetFrameMat() const {
            cv::Mat out;
            out.create(width, height, CV_8UC1);
            out.data = static_cast<uchar*>(frame.buffer);
            return out;
        }
        //Get frame like CvMat in asynchronous mode
        cv::Mat CCamera::GetFrameMat(int i) const {
            cv::Mat out;
            out.create(width, height, CV_8UC1);
            out.data = static_cast<uchar*>(frames[i].buffer);
            return out;
        }

        //Drow cam.frame with left top offsets on CDC with scale. (with translete BYTE* to cv::Mat by BitBlt) 
        void CCamera::DrowFrame(const cv::Mat& out, int left, int top,double scale) const {
            CBitmap bmp;
            CDC dcMem;
            cv::Mat dst;
            cvtColor(out, dst, cv::COLOR_GRAY2RGBA);
            if (scale)resize(dst, dst, cv::Size(0, 0), scale, scale, cv::INTER_LINEAR);
            dcMem.CreateCompatibleDC(outdc);
            bmp.CreateBitmap(dst.cols, dst.rows, 1, 32, dst.data);
            dcMem.SelectObject(&bmp);
            outdc->BitBlt(left, top, dst.cols + left, dst.rows + top, &dcMem, 0, 0, SRCCOPY);
            bmp.DeleteObject();
            DeleteDC(dcMem);
        }
       
        //Get Cam prop from inifile
        void CCamera::GetCamIni(const std::string& ini) {
           pt::ptree bar;
            pt::ini_parser::read_ini(ini, bar);
            offsetx = std::stoi(bar.get<std::string>("General.offsetx"));
            offsety = std::stoi(bar.get<std::string>("General.offsety"));
            width = std::stoi(bar.get<std::string>("General.cdx"));
            height = std::stoi(bar.get<std::string>("General.cdy"));
            expos = std::stoi(bar.get<std::string>("General.expos"));
        }
      
        // Get all cameras to pCameras list? return count cameras in system
        int CCamera::CameraList() {
            if (VmbErrorSuccess == VmbFeatureBoolGet(cameraHandle, "GeVTLIsPresent", &bGigE)) {
                if (bGigE== VmbBoolTrue) {
                    if (VmbErrorSuccess == VmbFeatureIntSet(cameraHandle, "GeVDiscoveryAllDuration", 250)) {
                        err = VmbFeatureCommandRun(cameraHandle, "GeVDiscoveryAllOnce");      // Send discovery packets to GigE cameras and wait 250 ms until they are answered
                    }
                }
            }
            if (VmbErrorSuccess == VmbCamerasList(NULL, 0, &nCount, sizeof * pCameras) && nCount != 0) {
                pCameras = (VmbCameraInfo_t*)malloc(nCount * sizeof(*pCameras));
                err = VmbCamerasList(pCameras, nCount, &nFoundCount, sizeof * pCameras);
                return nFoundCount;
            }
            else return 0;
        }

        //Connect Camera by id;
        bool CCamera::CameraConnect() {
            if (pCameraID !=NULL) {
                // Open camera
                err = VmbCameraOpen(pCameraID, cameraAccessMode, &cameraHandle);
                VmbFeatureBoolGet(cameraHandle, "GeVTLIsPresent", &bGigE);
                if (err == VmbErrorSuccess)
                {
                    // Set the GeV packet size to the highest possible value
                   if (VmbErrorSuccess == VmbFeatureCommandRun(cameraHandle, "GVSPAdjustPacketSize"))
                    {
                        do
                        {
                            if (VmbErrorSuccess != VmbFeatureCommandIsDone(cameraHandle,"GVSPAdjustPacketSize",&bIsCommandDone))
                            {
                                break;
                            }
                        } while (VmbBoolFalse == bIsCommandDone);
                    }

                    if (err == VmbErrorSuccess)
                    {
                        // Set pixel format. For the sake of simplicity we only support Mono 
                        err = VmbFeatureEnumSet(cameraHandle, "PixelFormat", "Mono8");
                        err = VmbFeatureIntGet(cameraHandle, "PayloadSize", &nPayloadSize); //!!!!!!!!!!
                        for (int i = 0; i < FRAME_COUNT; ++i)
                        {
                            frames[i].buffer = malloc(nPayloadSize);
                            frames[i].bufferSize = nPayloadSize;
                            // Anounce the frame
                            VmbFrameAnnounce(cameraHandle, &frames[i], sizeof(VmbFrame_t));
                        }
                        frame.buffer = (unsigned char*)malloc((VmbUint32_t)nPayloadSize);//!!!!!!!!!!
                        frame.bufferSize = (VmbUint32_t)nPayloadSize;//!!!!!!!!!!
                        // Announce Frame
                        err = VmbFrameAnnounce(cameraHandle, &frame, (VmbUint32_t)sizeof(VmbFrame_t));//!!!!!!!!!!
                        if (err == VmbErrorSuccess) {
                            // Start Capture Engine
                            err = VmbCaptureStart(cameraHandle);
                            if (err == VmbErrorSuccess) {
                                m_Connected = 1;
                                SetFrameFeacher(width, height, offsetx, offsety);
                                SetCamExpos(static_cast<float>(expos));
                            } else {m_Connected = 0; return false;}
                        } else { m_Connected = 0; return false;}
                    } else { m_Connected = 0; return false;}
                }  else { m_Connected = 0; return false;}
            } else { m_Connected = 0; return false; }
        }

        //Set frame  width offsets
        int CCamera::SetFrameFeacher(int  x, int y, int ox, int oy) {
            if (m_Connected) {
                width = x;
                height = y;
                offsetx = ox;
                offsety = oy;
                err = VmbFeatureIntSet(cameraHandle, "Width", width);
                err = VmbFeatureIntSet(cameraHandle, "Height", height);
                err = VmbFeatureIntSet(cameraHandle, "OffsetX", offsetx);
                err = VmbFeatureIntSet(cameraHandle, "OffsetY", offsety);
            } return err;
        }

        //Get expos
        int CCamera::GetCamExpos(){
            double val;
            if (bGigE == VmbBoolTrue)
                 err = VmbFeatureFloatGet(cameraHandle, "ExposureTimeAbs", &val);
             else
                err = VmbFeatureFloatGet(cameraHandle, "ExposureTime", &val);

            if (err== VmbErrorSuccess) return (static_cast<int>(val));
            else return 0;
        }

        //Set expos
        int CCamera::SetCamExpos(float val) {
            if (val == 0.0) val = static_cast<float>(expos);
            if (bGigE == VmbBoolTrue)
                err = VmbFeatureFloatSet(cameraHandle, "ExposureTimeAbs", val);
            else
                err = VmbFeatureFloatSet(cameraHandle, "ExposureTime", val);
            if (err== VmbErrorSuccess) expos = static_cast<int>(val);
            return err;
       }

        bool CCamera::GetConnected() const {
            // Gets connect flag 1 if connected
            return m_Connected;
        }
        
        //callback func to grabstream
        void VMB_CALL FrameDoneCallback(const VmbHandle_t hCamera, VmbFrame_t* pFrame)
        {
            if (VmbFrameStatusComplete == pFrame->receiveStatus) {
               // m_nFramesCounter++;
            }
            VmbCaptureFrameQueue(hCamera, pFrame, &FrameDoneCallback);
        }
 

        // start frame stream
        int CCamera::CameraFrameN() {
            int res;
            for (int i = 0; i < FRAME_COUNT; ++i)
            {
                VmbCaptureFrameQueue(cameraHandle, &frames[i], FrameDoneCallback);
            }
            res = VmbFeatureCommandRun(cameraHandle, "AcquisitionStart");
            return 1;
        }

        //get One frame
        int CCamera::CameraFrame() {
            int res = 0;
            // Queue Frame
            err = VmbCaptureFrameQueue(cameraHandle, &frame, NULL);
            if (VmbErrorSuccess == err) {
                // Start Acquisition
                err = VmbFeatureCommandRun(cameraHandle, "AcquisitionStart");
                // Capture one frame synchronously
                err = VmbCaptureFrameWait(cameraHandle, &frame, nTimeout);
                if (VmbErrorSuccess == err)
                {
                    if (VmbFrameStatusComplete == frame.receiveStatus)
                    {
                       // m_nFramesCounter++;
                        res = 1;
                    }
                }
            }
            return res;
        }

        // camera kill
        void CCamera::CameraClose() {
            // Stop Acquisition
            err = VmbFeatureCommandRun(cameraHandle, "AcquisitionStop");
            // Stop Capture Engine
            err = VmbCaptureEnd(cameraHandle);
            // Revoke frame
            err = VmbFrameRevoke(cameraHandle, &frame);
            free(frame.buffer);
            frame.buffer = NULL;
            err = VmbCameraClose(cameraHandle);
        }         

        //Check if a file exists
        bool fileExists(const std::string& filename)
        {
            struct stat buf;
            if (stat(filename.c_str(), &buf) != -1)
            {
                return true;
            }
            return false;
        }