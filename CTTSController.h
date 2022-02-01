#pragma once

#include "CSensor.h" 
#include "CMDLController.h" 

class CTTSController : public CSensor, public CMDLController
{
private:

public:
	CTTSController();
	CTTSController(const std::string& ini, const char* CamId, const std::string& expname, const char* COM);
	~CTTSController()
	{
	}
};