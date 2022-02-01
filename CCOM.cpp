// DeviceCP.cpp: implementation of the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "CCOM.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CSerial::SetOwnerObject(lpFUNCCALLBACK* inClbkFuncBuf,LPVOID lpParam)
{

	lpFuncBuf=inClbkFuncBuf;	// ������ ������� CALLBACK ��� ��������� �������� ������
	lpParams=lpParam;
}

CSerial::EError CSerial::GetError (void)
{
	if (m_hFile == 0)
		return EErrorUnknown;
	DWORD dwErrors = 0;
	if (!::ClearCommError(m_hFile,&dwErrors,0))
		return EErrorUnknown;
	return EError(dwErrors);
}

DWORD WINAPI ReadThread(LPVOID pParam) 
{ 
	
	LONG    lLastError = ERROR_SUCCESS;
	CSerial* CDCP=(CSerial*)pParam;
	char buffer[20];

	// �������� �������
	OVERLAPPED ov;
	memset(&ov,0,sizeof(ov));
	ov.hEvent = CreateEvent(0,TRUE,FALSE,0);
	CDCP->m_StopReadThread=CreateEvent(0,TRUE,FALSE,0);
    	HANDLE ahWait[2];
		ahWait[0] = ov.hEvent;
		ahWait[1] = CDCP->m_StopReadThread;			
	// ���� ��� ������
	bool fContinue = true;
	do
	{
		CDCP->WaitEvent(&ov);
		// ������� ��� �� �������
		switch (::WaitForMultipleObjects(sizeof(ahWait)/sizeof(*ahWait),ahWait,FALSE,INFINITE))
		{
		case WAIT_OBJECT_0:
			{
				ResetEvent(ahWait[0]);
				// ��������� �������
				CDCP->eEvent = CDCP->GetEventType();

				// ������� break
				if (CDCP->eEvent & CDCP->EEventBreak)
				{	// ����� �����
					(*CDCP->lpFuncBuf)(CDCP->lpParams,"����� ������� �����\r\n",WM_DEVICE_EVENT,EV_BREAK);
				}

				// ������� CTS
				if (CDCP->eEvent & CDCP->EEventCTS)
				{	// ������� ��������
					ZeroMemory(buffer,sizeof(buffer)); 
					sprintf_s(buffer,"Clear to send %s", CDCP->GetCTS()?"ON":"OFF");
					(*CDCP->lpFuncBuf)(CDCP->lpParams,buffer,WM_DEVICE_EVENT,EV_CTS);
				}

				// ������� DSR
				if (CDCP->eEvent & CDCP->EEventDSR)
				{	// ������ ������ � ��������
					ZeroMemory(buffer,sizeof(buffer));
					sprintf_s(buffer,"Data set ready %s", CDCP->GetDSR()?"ON":"OFF");
					(*CDCP->lpFuncBuf)(CDCP->lpParams,buffer,WM_DEVICE_EVENT,EV_DSR);
				}

				// ������� error
				if (CDCP->eEvent & CDCP->EEventError)
				{
					switch (CDCP->GetError())
					{
					case CDCP->EErrorBreak:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"������� ����� �������\r\n",WM_DEVICE_ERROR,CE_BREAK);
							break;
					case CDCP->EErrorFrame:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"�������� ������������� �����\r\n",WM_DEVICE_ERROR,CE_FRAME);
							break;
					case CDCP->EErrorIOE:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"������ �����/������\r\n",WM_DEVICE_ERROR,CE_IOE);
							break;
					case CDCP->EErrorMode:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"���������������� �����\r\n",WM_DEVICE_ERROR,CE_MODE);
							break;
					case CDCP->EErrorOverrun:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"������������ ������\r\n",WM_DEVICE_ERROR,CE_OVERRUN);
							break;
					case CDCP->EErrorRxOver:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"����� ����� ������\r\n",WM_DEVICE_ERROR,CE_RXOVER);
							break;
					case CDCP->EErrorParity:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"������ �������� �������� ������\r\n",WM_DEVICE_ERROR,CE_RXPARITY);
							break;
					case CDCP->EErrorTxFull:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"����� �������� �����\r\n",WM_DEVICE_ERROR,CE_TXFULL);
							break;
					default:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"����������\r\n",WM_DEVICE_ERROR,0);
							break;
					}
				}

				// ������� ring
				if (CDCP->eEvent & CDCP->EEventRing)
					(*CDCP->lpFuncBuf)(CDCP->lpParams,"RING",WM_DEVICE_EVENT,EV_RING);

				// ������� RLSD/CD
				if (CDCP->eEvent & CDCP->EEventRLSD)
				{
					ZeroMemory(buffer,sizeof(buffer));
					sprintf_s(buffer,"RLSD/CD %s", CDCP->GetRLSD()?"ON":"OFF");
					(*CDCP->lpFuncBuf)(CDCP->lpParams,buffer,WM_DEVICE_EVENT,EV_RLSD);
				}

				// ������� ���� ������
				// ���� ����� ���� ������ �� ����� �� ����� �������� ����� � ���������� ����� EEventRcvEv
				// ��� EEventRecv. � ������ ������ � �� � ������
				if ((CDCP->eEvent & CDCP->EEventRecv) )//|| (CDCP->eEvent & CDCP->EEventRcvEv))
				{
					// ������ ������
					DWORD dwBytesRead = 0;
					do
					{
						char szBuffer[101];
						// ������ ������ �� ��� �����
						lLastError = CDCP->Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead,&ov);
						if (lLastError != ERROR_SUCCESS)
							// ����������� ������
							(*CDCP->lpFuncBuf)(CDCP->lpParams,szBuffer,WM_DEVICE_ERROR,0);
						if (dwBytesRead > 0)
						{
							// ��������� ����� ������
							szBuffer[dwBytesRead] = '\0';
							// ���������� ����������� �������� ������
							if(CDCP->lpFuncBuf!=0)
								(*CDCP->lpFuncBuf)(CDCP->lpParams,szBuffer,WM_DEVICE_EVENT,EV_RXCHAR);
						}
					}
					while (dwBytesRead > 0);
				}
			}
			break;

		case WAIT_OBJECT_0+1:
				// ���������� ��� � false, ��� ������
				fContinue = false;
			break;

		default:
				// ���-��� ����� �� ���, ��� ����
				return 0;
			break;
		}
	}
	while (fContinue);
	if(ov.hEvent!=INVALID_HANDLE){
		CloseHandle(ov.hEvent);
		ov.hEvent=INVALID_HANDLE;
	}
	if(CDCP->m_StopReadThread!=INVALID_HANDLE){
		CloseHandle(CDCP->m_StopReadThread);
		CDCP->m_StopReadThread=INVALID_HANDLE;
	}
	(*CDCP->lpFuncBuf)(CDCP->lpParams,"���� ������\r\n",WM_DEVICE_ERROR,CE_CLOSE_PORT);
    return 0;
} 

CSerial::CSerial()
{
	m_hFile=INVALID_HANDLE_VALUE;
	m_StopReadThread=INVALID_HANDLE_VALUE;
	m_StopWriteThread=INVALID_HANDLE_VALUE;
	m_hReadThread=INVALID_HANDLE;
}

CSerial::~CSerial()
{
	CloseComPort();
}

int CSerial::OpenComPort(CString strPort)
{
	DWORD wErr=0;
	   // ��������� ����(�� ��������� COM1)
    if(Open(strPort.GetBuffer(strPort.GetLength()))==0){
		// ����������� ���� (9600,8,N,1, ��������� �� ���������)
		wErr+=SetProperties();
		wErr+=SetHandshaking(CP_HANDSHAKEOFF);
		// EV_RXCHAR �������� ������� �� �� ������� � ����� �� ������
		// EV_RXFLAG ����������� ������ � ������ � ����� �� �� ������� ������������ �������,
		// ������ ��������� �������� ����� ������� �� ������������ �������,
		// ���� �� �������� ��� ��� ������������� ����� ����� ���� ������ ������
		// �� �������� ����� ������������ ����.
		// ��� ����� ������������ ������� SetEventChar
		wErr+=SetMask(EV_BREAK|EV_CTS|EV_DSR|EV_ERR|EV_RING|EV_RLSD|EV_RXCHAR);
		wErr+=SetReadTimeouts(CP_READTIMEOUTNONBLOKING);
		
		if(wErr>0){
			CloseComPort();
			return -1;
		}
		if (m_hReadThread!=INVALID_HANDLE)
			CloseHandle(m_hReadThread);
		m_hReadThread = CreateThread(NULL, 0, ReadThread, this, 0, NULL);
		wErr=GetLastError();
		if (wErr>0){
			CloseComPort();
			return wErr;
			}		
		return 0;
	}
	else
		return -1;
}

LONG CSerial::Open (LPCTSTR lpszDevice)
{
	// �������� ����� ������ ��� ������
	if (m_hFile!=INVALID_HANDLE_VALUE)
		return ERROR_ALREADY_INITIALIZED;

	// ��������� ����
	m_hFile = ::CreateFile(lpszDevice,
						   GENERIC_READ|GENERIC_WRITE,
						   0,
						   0,
						   OPEN_EXISTING,
						   FILE_FLAG_OVERLAPPED,
						   0);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return GetLastError();
	

	// ��������� ���������� �� ���������
 	COMMCONFIG commConfig = {0};
	DWORD dwSize = sizeof(commConfig);
	commConfig.dwSize = dwSize;
	if (::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize))
	{
		// ��������� ������������ �� ���������
		if (!::SetCommConfig(m_hFile,&commConfig,dwSize))
			AfxMessageBox("���������� ��������� ������������");
	}
	else
	{
		AfxMessageBox("���������� ��������� ������������ �� ���������");
	}

	// �������� �������
	return 0;
}

LONG CSerial::SetSizeQueue(DWORD dwInQueue, DWORD dwOutQueue)
{
	// ��������� �������� � ����������� ������
	if (dwInQueue || dwOutQueue)
	{
		// ��������������, ��� ������� ������� � �������� ���������. ���� ����������� � Win9X ���� �������
		// ���� ������� ������ ������� - ����. ��� ������� ������ ���� �� ������� ����
		// 16 ������ �������
		_ASSERTE(dwInQueue >= 16);
		_ASSERTE(dwOutQueue >= 16);

		if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
		{
			// ������� ����
			CloseHandle(m_hFile);
			return GetLastError();	
		}
	}
	return 0;
}

LONG CSerial::SetProperties (DWORD dwBaudrate, BYTE btDataBits, BYTE btParity, BYTE btStopBits)
{
	// �������� ����� ������ ��� ������
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return GetLastError();
	dcb.BaudRate = dwBaudrate;
	dcb.ByteSize = btDataBits;
	dcb.Parity   = btParity;
	dcb.StopBits = btStopBits;
	// ����������� ������������ �� �������
	dcb.fParity  = (btParity!= NOPARITY);
	if (!::SetCommState(m_hFile,&dcb))
		return GetLastError();
	// �������� �����
	return 0;
}





LONG CSerial::SetHandshaking (WORD wHandshake)
{
	// �������� ������� ���������� �� ��� ���
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return GetLastError();
	switch (wHandshake)
	{
	case CP_HANDSHAKEOFF:
		dcb.fOutxCtsFlow = false;					// �������� ���������� CTS (���������� � �������� ������)
		dcb.fOutxDsrFlow = false;					// �������� ���������� DSR (���������� ��������� ����������)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// �������� ���������� DTR (���������� ����� ����������)
		dcb.fOutX = false;							// ��������� ����������� ������������� �������� XON/XOFF
		dcb.fInX = false;							// ��������� ����������� ������������� ����� XON/XOFF
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// �������� RTS (������ �� �������� ������)
		break;

	case CP_HANDSHAKEHARDWARE:
		dcb.fOutxCtsFlow = true;					// ��������  CTS ���������� (���������� � �������� ������)
		dcb.fOutxDsrFlow = true;					// �������� DSR ���������� (���������� ��������� ����������)
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// �������� DTR ���������� (���������� ����� ����������)
		dcb.fOutX = false;							// ��������� ����������� ������������� �������� XON/XOFF
		dcb.fInX = false;							// ��������� ����������� ������������� ����� XON/XOFF
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// �������� RTS ���������� (������ �� �������� ������)
		break;

	case CP_HANDSHAKESOFTWARE:
		dcb.fOutxCtsFlow = false;					// �������� ���������� CTS (���������� � �������� ������)
		dcb.fOutxDsrFlow = false;					// �������� ���������� DSR (���������� ��������� ����������)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// �������� ���������� DTR (���������� ����� ����������)
		dcb.fOutX = true;							// ��������� ����������� ������������� �������� XON/XOFF
		dcb.fInX = true;							// ��������� ����������� ������������� ����� XON/XOFF
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// �������� ���������� RTS (������ �� �������� ������)
		break;

	default:
		// ��� ������� �� ���������
		_ASSERTE(false);
		return E_INVALIDARG;
	}

	if (!::SetCommState(m_hFile,&dcb))
		return GetLastError();
	// �������� �����
	return 0;
}

LONG CSerial::SetMask (DWORD dwEventMask)
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;


	// ��������� ����� �����. �������� ��������, ��� ��� ������� ������� EEventNone
	// ���� ����������� �������� WaitCommEvent
	if (!::SetCommMask(m_hFile,dwEventMask))
		return GetLastError();

	// ��������� ����� � �������
	m_dwEventMask = dwEventMask;
	return 0;
}

LONG CSerial::SetReadTimeouts (WORD wReadTimeout)
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// ����������� ��������� ���� �����
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))
		return GetLastError();

	// ��������� ������ ���� ����
	switch (wReadTimeout)
	{
	case CP_READTIMEOUTBLOCKING:
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	case CP_READTIMEOUTNONBLOKING:
		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;
		break;
	default:
		// ��� ������� �� ���������
		_ASSERTE(false);
		return E_INVALIDARG;
	}

	// ��������� ����� �������� ��������
	if (!::SetCommTimeouts(m_hFile,&cto))
	{
		return GetLastError();
	}

	// �������� �����
	return 0;
}

LONG CSerial::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// �������� ������������� �� ���� ����
	CheckTimeout(dwTimeout);

	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// �������� ������ ����������� ���� ���������� � ������� ���������� ����� 0
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// ������������� ���������
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = CreateEvent(0,FALSE,TRUE,0);
		// ������������� ����� ���������� ����������� � �������
		lpOverlapped = &ovInternal;
	}

	// ��������������, ��� ���������, � ������� ����� �������� �� ������
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

	// ������� �������
	if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped))
	{
		// ��������� ���������� ���
		long lLastError = ::GetLastError();

		// ��������� ����� �� ������ ����������� ��������
		if (lLastError != ERROR_IO_PENDING)
			return lLastError;

		// ����������� ���� �� �������� �� ����������
		if (lpOverlapped == &ovInternal)
		{
			// �������� ���������� ����������� ��������
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// ����������� �������� � ���������
				break;

			case WAIT_TIMEOUT:
				// ������ �������� ����� ������
				CancelIo();
				// ����� ����� ��������
				return ERROR_TIMEOUT;

			default:
				// ��������� ���������� ��� ������
				return GetLastError();
			}
		}
	}
	else
	{
		// �������� ���������� ���������. � ����� � ���� ��������� ����� �������
		::SetEvent(lpOverlapped->hEvent);
	}

	// �������� �����
	return 0;
}

CSerial::EEvent CSerial::GetEventType (void)
{
#ifdef _DEBUG
	// �������� �������� �� ������� � ��������� �����
	if ((m_eEvent & m_dwEventMask) == 0)
		_RPTF2(_CRT_WARN,"CSerial::GetEventType - Event %08Xh not within mask %08Xh.\n", m_eEvent, m_dwEventMask);
#endif

	// ��������� ������� (����������� �� ������ ������� �������� �����)
	EEvent eEvent = EEvent(m_eEvent & m_dwEventMask);

	// ����� ���������� �������
	m_eEvent = EEventNone;

	// ������� ������� ������� �������
	return eEvent;
}

bool CSerial::GetCTS (void)
{
	// ��������� ��������� ���������
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_CTS_ON) != 0;
}

bool CSerial::GetDSR (void)
{
	// ��������� ��������� ���������
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_DSR_ON) != 0;
}

bool CSerial::GetRing (void)
{
	// ��������� ��������� ���������
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_RING_ON) != 0;
}

bool CSerial::GetRLSD (void)
{
	// ��������� ��������� ���������
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_RLSD_ON) != 0;
}

bool CSerial::IsOpen()
{
	// ����������� ������ ���� �� ��� ���
	if (m_hFile!=INVALID_HANDLE_VALUE)
		return true;
	return false;
}

LONG CSerial::Read( void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// �������� ������������� �� ���� ����
	CheckTimeout(dwTimeout);

	// ����������� �������� ������ ���� ���������� � pdwRead
	_ASSERTE(!lpOverlapped || pdwRead);

	// ������������� ����� ���������� ��� �������� ��������� ����
	DWORD dwRead;
	if (pdwRead == 0)
	{
		pdwRead = &dwRead;
	}

	// ����� ���������� ��������� ����
	*pdwRead = 0;
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// ������������� �����������
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = CreateEvent(0,FALSE,TRUE,0);
		lpOverlapped = &ovInternal;
	}

	// ��������������, ��� ���������, � ������� ����� �������� �� ������
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

#ifdef _DEBUG
	// ���������� ������ ��������� ������ ��������� ������ 0xDC �������, 
	// ����� ��������� �������� ������ ��� ����� ������.
	memset(pData,0xDC,iLen);
#endif
	
	// ������ ������
	if (!::ReadFile(m_hFile,pData,iLen,pdwRead,lpOverlapped))
	{
		// ��������� ���������� ������
		long lLastError = ::GetLastError();

		// �������� ������������ ����� ������ ����� �� ������?
		if (lLastError != ERROR_IO_PENDING)
			return lLastError;

		// ��������� ���� ������� �������� �� ����������
		if (lpOverlapped == &ovInternal)
		{
			// ������� ���������� ����������� ��������
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// ����������� �������� ������� ���������
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
					return GetLastError();
				break;

			case WAIT_TIMEOUT:
				// ����� �����
				CancelIo();
				return ERROR_TIMEOUT;

			default:
				// ��������� ��� ���������� ������
				return GetLastError();
			}
		}
	}
	else
	{
		// ������� �������
		::SetEvent(lpOverlapped->hEvent);
	}

	// �������� �������
	return 0;
}


void CSerial::CloseComPort()
{
	CloseThread(); // ��������� ������
	if (m_hFile!=INVALID_HANDLE_VALUE){
		CloseHandle(m_hFile);
		m_hFile=INVALID_HANDLE_VALUE;
	}
}

LONG CSerial::WriteData(char* szData)
{
	return Write(szData,strlen(szData));
}
LONG CSerial::WriteD(const void* szData1, size_t iLen1)
{
	return Write(szData1,iLen1);
}
void CSerial::CloseThread()
{
	// �������� ���������� ������� � �� ����������
	if(m_hReadThread!=INVALID_HANDLE){
		PulseEvent(m_StopReadThread); // ���� ����� ������ �� ������ ������� ��� ���������� ������
		// ������� ���������� ������
		if(WaitForSingleObject(m_hReadThread,CP_SHUTDOWN_TIMEOUT)!=WAIT_OBJECT_0){	
			DWORD   dwStatus;
			SuspendThread(m_hReadThread); // ������������� �����
			GetExitCodeThread(m_hReadThread, &dwStatus); // �������� ��� ������
			TerminateThread(m_hReadThread,dwStatus);} // ���������� �����
		else
			CloseHandle(m_hReadThread);	// ��������� ����� ������
		m_hReadThread=INVALID_HANDLE; // �������
	}
}

LONG CSerial::Break ()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

    // ������������� ����� ������
    ::SetCommBreak(m_hFile);
    ::Sleep(100);
    ::ClearCommBreak(m_hFile);

	// �������� �����
	return 0;
}

DWORD CSerial::GetBaudrate ()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return -1;
	// ���������� ��������� dcb
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return -1;
	return dcb.BaudRate;
}

BYTE CSerial::GetDataBits ()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return -1;
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return -1;
	return dcb.ByteSize;
}

BYTE CSerial::GetEventChar ()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return 0;
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return 0;
	return dcb.EvtChar;
}

LONG CSerial::SetEventChar(char bEvtChar) 
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return GetLastError();
	dcb.EvtChar=bEvtChar;
	if (!::SetCommState(m_hFile,&dcb))
		return GetLastError();
	return 0;
}

DWORD CSerial::GetEventMask()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return 0;
	return m_dwEventMask;
}

WORD CSerial::GetHandshaking()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return CP_HANDSHAKEUNKNOWN;
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return CP_HANDSHAKEUNKNOWN;

	// �������� ������������ �� ���������� �������� �����
	if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
		return CP_HANDSHAKEHARDWARE;

	// �������� ������������ �� ����������� �������� �����
	if (dcb.fOutX && dcb.fInX)
		return CP_HANDSHAKESOFTWARE;

	// �� ������������ �������� �����
	return CP_HANDSHAKEOFF;
}

LONG CSerial::Write (const void* pData, size_t iLen, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// ��� ����������� ��������, ������ ���������� ���������� pdwWritten 
	_ASSERTE(!lpOverlapped || pdwWritten);

	// �������� ������������� �� ���� ����
	CheckTimeout(dwTimeout);

	// ���������� ���������� ���� ����������� ��� ������
	DWORD dwWritten;
	if (pdwWritten == 0)
	{
		pdwWritten = &dwWritten;
	}

	// ���������� ������� �������� � ����
	*pdwWritten = 0;

	// ��������� ������ �� ����
	if (m_hFile == INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// �������������
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// ��������� ���������
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = CreateEvent(0,FALSE,TRUE,0);
		// ���� ������� ��������� �� ���������� �� ������ �����
		lpOverlapped = &ovInternal;
	}

	// ��������� ����� �������������� �� ������ �� ���������
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

	// ���������� ������
	if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,lpOverlapped))
	{
		// ���������� ���������� ��� ������
		long lLastError = ::GetLastError();

		// ��������� ����� ����� ������ ����������� �������� ����� ������, �� ��� ���
		if (lLastError != ERROR_IO_PENDING)// ERROR_IO_PENDING ��� �� ������ � ���������� ��� ��� ��� ��������� ����������� ���� �����
			return lLastError;

		// ���� ��������� �� ����������
		if (lpOverlapped == &ovInternal)
		{
			// ������� ���������� ��������
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// �������� ���������
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
					return GetLastError();
				break;

			case WAIT_TIMEOUT:
				// ����� �����
				CancelIo();
				return ERROR_TIMEOUT;

			default:
				// ��������� ��� ���������� ������
				return GetLastError();
			}
		}
	}
	else
	{
		// ��������� �������
		::SetEvent(lpOverlapped->hEvent);
	}

	// �������� ����������
	return 0;
}

#pragma warning(disable : 4996)


void CSerial::EnumerateSerialPorts(CUIntArray& ports)
{
  // ������� ������ ���������
  ports.RemoveAll();

  // ����� ������ OS
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL bGetVer = GetVersionEx(&osvi);

  // ������ NT ���������� ������� API QueryDosDevice 
  if (bGetVer && (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT))
  {
    // ����������� ������� QueryDosDevice ��� ������ ���� ������. ��� �������� ������ �������� 
    // ��� ��� �� ������� �������� ��� ������
    TCHAR szDevices[65535];
    DWORD dwChars = QueryDosDevice(NULL, szDevices, 65535);
    if (dwChars)
    {
      int i=0;

      for (;;)
      {
        // ����������� ����� �������� ����������
        TCHAR* pszCurrentDevice = &szDevices[i];

        // ���� ��� ��� ���� ����� �������� ��� ��� � ������� ������� ����� ��������
        int nLen = _tcslen(pszCurrentDevice);
        if (nLen > 3 && _tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0)
        {
          // ��������� ������ �����
          int nPort = _ttoi(&pszCurrentDevice[3]);
          ports.Add(nPort);
        }

        // �������� ������� ��������� ����
        while(szDevices[i] != _T('\0'))
          i++;

        // ��������� �� ��������� ������
        i++;

        // ���� ������� ������ ����� ����� ����� �������
        // now NULL, we're at the end
        if (szDevices[i] == _T('\0'))
          break;
      }
    }
    else
      TRACE(_T("Failed in call to QueryDosDevice, GetLastError:%d\n"), GetLastError());
  }
  else
  {
    //������ WIN 95/98 ��������� ���� � ������ ������ ���� �������� ������ ����������
    for (UINT i=1; i<256; i++)
    {
      // ��������� ��� �����
      CString sPort;
      sPort.Format(_T("\\\\.\\COM%d"), i);

      // ��������� ����
      BOOL bSuccess = FALSE;
      HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
      if (hPort == INVALID_HANDLE_VALUE)
      {
        DWORD dwError = GetLastError();

        //�������� ������� ������, ��� ��� ������ ���� ������ ����������� ��� ��� ����� �����
        if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE)
          bSuccess = TRUE;
      }
      else
      {
        // ���� ������ �������
        bSuccess = TRUE;

        // ��� ��� ���� ������ �� ����� �� ��� ���������
        CloseHandle(hPort);
      }

      //�������� ����� ����� � ������
      if (bSuccess)
        ports.Add(i);
    }
  }
}

int CSerial::GetEnumPort(int* czNumbers)
{// ������� ��� ������������ ������ ��������� � �������
	CUIntArray ports;
	EnumerateSerialPorts(ports);//
	int cnt;

	CString m_enums;
	CString czenums;

	cnt=ports.GetSize();
	for (int i=0; i<ports.GetSize(); i++)
	{	
		//sprintf_s(czenums, "COM%d",ports.ElementAt(i));
		czenums.Format("COM%d", ports.ElementAt(i));


		m_enums+=czenums;
        czNumbers[i] =ports.ElementAt(i);
		/*if(czNumbers!=NULL)
		{
			sprintf(czenums,"%d",ports.ElementAt(i));
			czNumbers[i]=czenums[i];
		}*/

	}
	czenums="";
	return cnt;
}



LONG CSerial::SetTilt(__int16 XT,__int16 YT) 
{
unsigned __int16 XTilt, YTilt,XTtmp, YTtmp;

unsigned __int8 XTiltH, XTiltL, YTiltH, YTiltL,tmp;
char cc1,cc2,cc3,cc4;
CString BUF;

  if(XT<-1000) XT=-1000;
  if(YT<-1000) YT=-1000; 
  if(XT>1000) XT=1000;
  if(YT>1000) YT=1000;

  XTilt=XT+2047;
  YTilt=YT+2047;

  XTtmp=XTilt<<2;
  XTiltH=XTtmp>>8;

  XTtmp=XTilt<<10;
  XTiltL=XTtmp>>10;

  YTtmp=YTilt<<2;
  YTiltH=YTtmp>>8;
  YTtmp=YTilt<<10;
  YTiltL=YTtmp>>10;

  cc1 = (char)XTiltH;

  tmp =0x01;
  tmp=tmp<<6;
  XTiltL=XTiltL | tmp;

  cc2 = (char)XTiltL;

  tmp =0x02;
  tmp=tmp<<6;
  YTiltH=YTiltH | tmp;
 
  cc3 = (char)YTiltH;

  tmp=0x03;
  tmp=tmp<<6;
  YTiltL=YTiltL | tmp;
  
  cc4 = (char)YTiltL;

  BUF.Format("a%c%c%c%c",cc1,cc2,cc3,cc4);
	
 return WriteD(BUF,5);
}




void CheckTimeout (DWORD dwTimeout)
{
	if ((dwTimeout != 0) && (dwTimeout != INFINITE))
	{
		// ����� �� ����������
		::MessageBox(0,_T("���� ���� �� ��������������, ����� SERIAL_WIN95_SUPPORT ��������"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}
}



#ifdef SERIAL_NO_CANCELIO

void CheckTimeout (DWORD dwTimeout)
{
	if ((dwTimeout != 0) && (dwTimeout != INFINITE))
	{
		// ����� �� ����������
		::MessageBox(0,_T("���� ���� �� ��������������, ����� SERIAL_WIN95_SUPPORT ��������"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}
}

BOOL CSerial::CancelIo (void)
{
	::DebugBreak();
	return FALSE;
}

#endif	// SERIAL_NO_CANCELIO
