// DeviceCP.h: interface for the CDeviceCP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__8A6C4546_2D2F_4471_A92B_44C1D1B9B4D8__INCLUDED_)
#define AFX_SERIAL_H__8A6C4546_2D2F_4471_A92B_44C1D1B9B4D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

	DWORD WINAPI ReadThread(LPVOID pParam);					// Потоковая функция чтения
	typedef void* lpFUNCCALLBACK (LPVOID lp,char* lpBuf,WPARAM iTypeEvent,LPARAM iEvents);	// Указатель на функцию которая будет обрабатывать Буфер

	#define CP_SHUTDOWN_TIMEOUT 500				// Время ожидания
	#define WM_DEVICE_EVENT WM_USER+1			// Произошло событие на порту
	#define WM_DEVICE_ERROR WM_USER+2			// Событие ошибка
	#define CE_CLOSE_PORT WM_USER+3
	#define INVALID_HANDLE (HANDLE)0xcccccccc
	// Рукопожатия (подтверждения)
	#define		CP_HANDSHAKEUNKNOWN		   -1	// Неизвестно
	#define		CP_HANDSHAKEOFF				0	// Без рукопажатия
	#define		CP_HANDSHAKEHARDWARE		1	// Аппаратное рукопожатие (RTS/CTS)
	#define		CP_HANDSHAKESOFTWARE		2	// Программное рукопожатие (XON/XOFF)
	// Установки тайм аутов
	#define		CP_READTIMEOUTUNKNOWN	   -1	// Неизвестно
	#define		CP_READTIMEOUTNONBLOKING	0	// Немедленный возврат данных
	#define		CP_READTIMEOUTBLOCKING		1	// Блокировать пока всё не получено	

class CSerial  
{
	friend DWORD WINAPI ReadThread(LPVOID pParam);
	friend DWORD WINAPI WriteThread(LPVOID pParam);
public:
	CSerial();
	virtual ~CSerial();
	int connected =0;
	CString comport;
	int IsConnected() { return connected; };
protected:
	typedef enum
	{
		EEventUnknown  	   = -1,			// Неизвестное событие
		EEventNone  	   = 0,				// Событие не имеющее причины
		EEventBreak 	   = EV_BREAK,		// Обрыв приёмной линии
		EEventCTS   	   = EV_CTS,		// Изменение состояния линии CTS
		EEventDSR   	   = EV_DSR,		// Изменение состояния линии DSR
		EEventError 	   = EV_ERR,		// Произошла ошибка чётности или целостности данных
		EEventRing  	   = EV_RING,		// Обнаружен сигнал звонка модема
		EEventRLSD  	   = EV_RLSD,		// Изменение состояния линии RLSD
		EEventRecv  	   = EV_RXCHAR,		// Приём любого символа
		EEventRcvEv 	   = EV_RXFLAG,		// Приём символа назначеного в структуре DCB в поле EvtChar
		EEventSend		   = EV_TXEMPTY,	// Из буфера передан последний символ
		EEventPrinterError = EV_PERR,		// Ошибка принтера
		EEventRx80Full	   = EV_RX80FULL,	// Приёмный буфер заполнен на 80%
		EEventProviderEvt1 = EV_EVENT1,		// Спецефическое событие 1 определённое провайдером
		EEventProviderEvt2 = EV_EVENT2,		// Спецефическое событие 2 определённое провайдером
	} 
	EEvent;
	
	// Ошибки порта
	typedef enum
	{
		EErrorUnknown = 0,			// Неизвестная ошибка
		EErrorBreak   = CE_BREAK,	// Нарушен обмен данными
		EErrorFrame   = CE_FRAME,	// Ошибка синхронизации кадра
		EErrorIOE     = CE_IOE,		// Ошибка ввода вывода
		EErrorMode    = CE_MODE,	// Неподдерживаемый режим
		EErrorOverrun = CE_OVERRUN,	// Входной буфер переполнен, Потеря байтов
		EErrorRxOver  = CE_RXOVER,	// Входной буфер полон, приём невозможен
		EErrorParity  = CE_RXPARITY,// Ошибка чётности принятых данных
		EErrorTxFull  = CE_TXFULL	// Выходной буфер полный
	}
	EError;
protected:
	
	EEvent GetEventType ();
	EEvent eEvent;
	EError GetError();

	LONG Open (LPCTSTR lpszDevice= "COM1");
	bool GetCTS();
	bool GetDSR();
	bool GetRing();
	bool GetRLSD();
	void CloseThread();
	LONG Break();
	LONG WaitEvent (LPOVERLAPPED lpOverlapped = 0, DWORD dwTimeout = INFINITE);
	LONG Write (const void* pData, size_t iLen, DWORD* pdwWritten = 0, LPOVERLAPPED lpOverlapped = 0, DWORD dwTimeout = INFINITE);
	LONG Read(void* pData, size_t iLen, DWORD* pdwRead = 0, LPOVERLAPPED lpOverlapped = 0, DWORD dwTimeout = INFINITE);
	void EnumerateSerialPorts(CUIntArray& ports);
public:
//	CString GetEnumPort(int &iEnum,char* czNumbers=NULL);
	
    int CSerial::GetEnumPort(int* czNumbers);
	int OpenComPort(CString strPort = "COM1");
	void CloseComPort();
	LONG WriteData(char* szData);
	LONG WriteD(const void* szData1, size_t iLen1);
	LONG SetSizeQueue(DWORD dwInQueue=0, DWORD dwOutQueue=0);
//	LONG SetProperties (DWORD dwBaudrate = CBR_115200,BYTE btDataBits = DATABITS_8,BYTE btParity   = NOPARITY,
    LONG SetProperties (DWORD dwBaudrate = CBR_128000,BYTE btDataBits = DATABITS_8,BYTE btParity   = NOPARITY,
	BYTE btStopBits = ONESTOPBIT);

	void SetOwnerObject(lpFUNCCALLBACK* inClbkFuncBuf,LPVOID lpParam);
	LONG SetEventChar(char bEvtChar);
	LONG SetHandshaking (WORD wHandshake);
	LONG SetMask (DWORD dwMask = EEventBreak|EEventError|EEventRecv);
	LONG SetReadTimeouts (WORD wReadTimeout);
	DWORD GetBaudrate();
	BYTE GetDataBits();
	BYTE GetEventChar();
	DWORD GetEventMask();
	WORD GetHandshaking();
	bool IsOpen();
	LONG SetTilt(__int16 XT,__int16 YT); 
protected:
	LPVOID lpParams;
	lpFUNCCALLBACK*	lpFuncBuf;
	HANDLE m_hReadThread;
	HANDLE m_StopReadThread;
	HANDLE m_StopWriteThread;
	HANDLE m_RunWriteThread;
	HANDLE	m_hFile;			// Дескриптор файла
	EEvent	m_eEvent;			// Тип событий
	DWORD	m_dwEventMask;		// Маска событий
	char czWrites[101];
protected:
	class CDCB : public DCB
	{
	public:
		CDCB() { DCBlength = sizeof(DCB); }
	};
private: 
	//#define ON_WM_SERIAL(memberFxn)	\
	//ON_REGISTERED_MESSAGE(CDeviceCP::mg_nDefaultComMsg,memberFxn)
protected:
	
#ifdef SERIAL_NO_CANCELIO
	// Windows 95 support needs a wrapper
	void CheckTimeout (DWORD dwTimeout);
	BOOL CancelIo (void);
#else
	// No Windows 95 support means, simply forwarding the call
	void CheckTimeout (DWORD /*dwTimeout*/)		{}
	BOOL CancelIo (void)						{ return ::CancelIo(m_hFile); }
#endif	// SERIAL_NO_CANCELIO
};

#endif // !defined(AFX_SERIAL_H__8A6C4546_2D2F_4471_A92B_44C1D1B9B4D8__INCLUDED_)
