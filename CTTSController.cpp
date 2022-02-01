#include "pch.h"
#include "CTTSController.h" 

CTTSController::CTTSController() : CSensor(), CMDLController() {

}

CTTSController::CTTSController(const std::string& ini, const char* CamId, const std::string& ename, const char* COM) : CSensor(ini, CamId, ename), CMDLController( ini, COM) {

}

