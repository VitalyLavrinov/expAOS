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

	lpFuncBuf=inClbkFuncBuf;	// Адресс функции CALLBACK для обработки приёмного буфера
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

	// Создание событий
	OVERLAPPED ov;
	memset(&ov,0,sizeof(ov));
	ov.hEvent = CreateEvent(0,TRUE,FALSE,0);
	CDCP->m_StopReadThread=CreateEvent(0,TRUE,FALSE,0);
    	HANDLE ahWait[2];
		ahWait[0] = ov.hEvent;
		ahWait[1] = CDCP->m_StopReadThread;			
	// Цикл для чтения
	bool fContinue = true;
	do
	{
		CDCP->WaitEvent(&ov);
		// Ожидаем оно из событий
		switch (::WaitForMultipleObjects(sizeof(ahWait)/sizeof(*ahWait),ahWait,FALSE,INFINITE))
		{
		case WAIT_OBJECT_0:
			{
				ResetEvent(ahWait[0]);
				// Сохраняем событие
				CDCP->eEvent = CDCP->GetEventType();

				// Событие break
				if (CDCP->eEvent & CDCP->EEventBreak)
				{	// Обрыв приёма
					(*CDCP->lpFuncBuf)(CDCP->lpParams,"Обрыв приёмной линии\r\n",WM_DEVICE_EVENT,EV_BREAK);
				}

				// Событие CTS
				if (CDCP->eEvent & CDCP->EEventCTS)
				{	// Очистка передачи
					ZeroMemory(buffer,sizeof(buffer)); 
					sprintf_s(buffer,"Clear to send %s", CDCP->GetCTS()?"ON":"OFF");
					(*CDCP->lpFuncBuf)(CDCP->lpParams,buffer,WM_DEVICE_EVENT,EV_CTS);
				}

				// Событие DSR
				if (CDCP->eEvent & CDCP->EEventDSR)
				{	// Данные готовы к передаче
					ZeroMemory(buffer,sizeof(buffer));
					sprintf_s(buffer,"Data set ready %s", CDCP->GetDSR()?"ON":"OFF");
					(*CDCP->lpFuncBuf)(CDCP->lpParams,buffer,WM_DEVICE_EVENT,EV_DSR);
				}

				// Событие error
				if (CDCP->eEvent & CDCP->EEventError)
				{
					switch (CDCP->GetError())
					{
					case CDCP->EErrorBreak:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Нарушен обмен данными\r\n",WM_DEVICE_ERROR,CE_BREAK);
							break;
					case CDCP->EErrorFrame:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Нарушена синхронизация кадра\r\n",WM_DEVICE_ERROR,CE_FRAME);
							break;
					case CDCP->EErrorIOE:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Ошибка ввода/вывода\r\n",WM_DEVICE_ERROR,CE_IOE);
							break;
					case CDCP->EErrorMode:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Неподдерживаемый режим\r\n",WM_DEVICE_ERROR,CE_MODE);
							break;
					case CDCP->EErrorOverrun:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Переполнение буфера\r\n",WM_DEVICE_ERROR,CE_OVERRUN);
							break;
					case CDCP->EErrorRxOver:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Буфер приёма полный\r\n",WM_DEVICE_ERROR,CE_RXOVER);
							break;
					case CDCP->EErrorParity:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Ошибка чётности принятых данных\r\n",WM_DEVICE_ERROR,CE_RXPARITY);
							break;
					case CDCP->EErrorTxFull:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Буфер передачи полон\r\n",WM_DEVICE_ERROR,CE_TXFULL);
							break;
					default:
							(*CDCP->lpFuncBuf)(CDCP->lpParams,"Неизвестно\r\n",WM_DEVICE_ERROR,0);
							break;
					}
				}

				// Событие ring
				if (CDCP->eEvent & CDCP->EEventRing)
					(*CDCP->lpFuncBuf)(CDCP->lpParams,"RING",WM_DEVICE_EVENT,EV_RING);

				// Событие RLSD/CD
				if (CDCP->eEvent & CDCP->EEventRLSD)
				{
					ZeroMemory(buffer,sizeof(buffer));
					sprintf_s(buffer,"RLSD/CD %s", CDCP->GetRLSD()?"ON":"OFF");
					(*CDCP->lpFuncBuf)(CDCP->lpParams,buffer,WM_DEVICE_EVENT,EV_RLSD);
				}

				// Событие приём данных
				// Если нужен приём данных по флагу то нужно поменять маску и установить здесь EEventRcvEv
				// Или EEventRecv. В данном случае и то и другое
				if ((CDCP->eEvent & CDCP->EEventRecv) )//|| (CDCP->eEvent & CDCP->EEventRcvEv))
				{
					// Чтение данных
					DWORD dwBytesRead = 0;
					do
					{
						char szBuffer[101];
						// Чтение данных из ком порта
						lLastError = CDCP->Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead,&ov);
						if (lLastError != ERROR_SUCCESS)
							// Неизвестная ошибка
							(*CDCP->lpFuncBuf)(CDCP->lpParams,szBuffer,WM_DEVICE_ERROR,0);
						if (dwBytesRead > 0)
						{
							// Окончание приёма данных
							szBuffer[dwBytesRead] = '\0';
							// Обработать виртуальной функцией данные
							if(CDCP->lpFuncBuf!=0)
								(*CDCP->lpFuncBuf)(CDCP->lpParams,szBuffer,WM_DEVICE_EVENT,EV_RXCHAR);
						}
					}
					while (dwBytesRead > 0);
				}
			}
			break;

		case WAIT_OBJECT_0+1:
				// Установить бит в false, для выхода
				fContinue = false;
			break;

		default:
				// Кое-что пошло не так, как надо
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
	(*CDCP->lpFuncBuf)(CDCP->lpParams,"Порт Закрыт\r\n",WM_DEVICE_ERROR,CE_CLOSE_PORT);
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
	   // Открываем порт(по умолчанию COM1)
    if(Open(strPort.GetBuffer(strPort.GetLength()))==0){
		// Настраиваем порт (9600,8,N,1, настройки по умолчанию)
		wErr+=SetProperties();
		wErr+=SetHandshaking(CP_HANDSHAKEOFF);
		// EV_RXCHAR получает символы по их приходу и выдаёт из буфера
		// EV_RXFLAG накапливает симолы в буфере и выдаёт их по приходу определённого символа,
		// значит буквально означает конец посылки по определённому символу,
		// надо не забывать что для использования этого флага надо задать символ
		// по которомы будет отслеживатся приём.
		// для этого используется функция SetEventChar
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
	// Проверка порта открыт или закрыт
	if (m_hFile!=INVALID_HANDLE_VALUE)
		return ERROR_ALREADY_INITIALIZED;

	// Открываем порт
	m_hFile = ::CreateFile(lpszDevice,
						   GENERIC_READ|GENERIC_WRITE,
						   0,
						   0,
						   OPEN_EXISTING,
						   FILE_FLAG_OVERLAPPED,
						   0);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return GetLastError();
	

	// Настройка устройства по умолчанию
 	COMMCONFIG commConfig = {0};
	DWORD dwSize = sizeof(commConfig);
	commConfig.dwSize = dwSize;
	if (::GetDefaultCommConfig(lpszDevice,&commConfig,&dwSize))
	{
		// Установка конфигурации по умолчанию
		if (!::SetCommConfig(m_hFile,&commConfig,dwSize))
			AfxMessageBox("Невозможно настроить конфигурацию");
	}
	else
	{
		AfxMessageBox("Невозможно настроить конфигурацию по умолчанию");
	}

	// Успешный возврат
	return 0;
}

LONG CSerial::SetSizeQueue(DWORD dwInQueue, DWORD dwOutQueue)
{
	// Настройка приёмного и передающего буфера
	if (dwInQueue || dwOutQueue)
	{
		// Удостоверьтесь, что размеры очереди в пределах разумного. Если выполняется в Win9X крах системы
		// если входной размер очереди - ноль. Обе очереди должны быть по крайней мере
		// 16 байтов шириной
		_ASSERTE(dwInQueue >= 16);
		_ASSERTE(dwOutQueue >= 16);

		if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
		{
			// Закрыть порт
			CloseHandle(m_hFile);
			return GetLastError();	
		}
	}
	return 0;
}

LONG CSerial::SetProperties (DWORD dwBaudrate, BYTE btDataBits, BYTE btParity, BYTE btStopBits)
{
	// Проверка порта открыт или закрыт
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return GetLastError();
	dcb.BaudRate = dwBaudrate;
	dcb.ByteSize = btDataBits;
	dcb.Parity   = btParity;
	dcb.StopBits = btStopBits;
	// Определение используется ли паритет
	dcb.fParity  = (btParity!= NOPARITY);
	if (!::SetCommState(m_hFile,&dcb))
		return GetLastError();
	// Успешный выход
	return 0;
}





LONG CSerial::SetHandshaking (WORD wHandshake)
{
	// Проверка открыто устройство да или нет
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;
	CDCB dcb;
	if (!::GetCommState(m_hFile,&dcb))
		return GetLastError();
	switch (wHandshake)
	{
	case CP_HANDSHAKEOFF:
		dcb.fOutxCtsFlow = false;					// Выключен мониторинг CTS (Готовность к передаче данных)
		dcb.fOutxDsrFlow = false;					// Выключен мониторинг DSR (Готовность удалённого устройства)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Выключен мониторинг DTR (Готовность этого устройства)
		dcb.fOutX = false;							// Выключено программное подтверждение передачи XON/XOFF
		dcb.fInX = false;							// Выключено программное подтверждение приёма XON/XOFF
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Выключен RTS (Запрос на передачу данных)
		break;

	case CP_HANDSHAKEHARDWARE:
		dcb.fOutxCtsFlow = true;					// Разрешён  CTS мониторинг (Готовность к передаче данных)
		dcb.fOutxDsrFlow = true;					// Разрешён DSR мониторинг (Готовность удалённого устройства)
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Разрешён DTR мониторинг (Готовность этого устройства)
		dcb.fOutX = false;							// Выключено программное подтверждение передачи XON/XOFF
		dcb.fInX = false;							// Выключено программное подтверждение приёма XON/XOFF
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Разрешён RTS мониторинг (Запрос на передачу данных)
		break;

	case CP_HANDSHAKESOFTWARE:
		dcb.fOutxCtsFlow = false;					// Выключен мониторинг CTS (Готовность к передаче данных)
		dcb.fOutxDsrFlow = false;					// Выключен мониторинг DSR (Готовность удалённого устройства)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Выключен мониторинг DTR (Готовность этого устройства)
		dcb.fOutX = true;							// Разрешено программное подтверждение передачи XON/XOFF
		dcb.fInX = true;							// Разрешено программное подтверждение приёма XON/XOFF
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Выключен мониторинг RTS (Запрос на передачу данных)
		break;

	default:
		// Эта функция не выполнена
		_ASSERTE(false);
		return E_INVALIDARG;
	}

	if (!::SetCommState(m_hFile,&dcb))
		return GetLastError();
	// Успешный выход
	return 0;
}

LONG CSerial::SetMask (DWORD dwEventMask)
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;


	// Установка новой маски. Обратите внимание, что это создаст событие EEventNone
	// если асинхронное ожидание WaitCommEvent
	if (!::SetCommMask(m_hFile,dwEventMask))
		return GetLastError();

	// Сохраняем маску и выходим
	m_dwEventMask = dwEventMask;
	return 0;
}

LONG CSerial::SetReadTimeouts (WORD wReadTimeout)
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// Определение структуры тайм аутов
	COMMTIMEOUTS cto;
	if (!::GetCommTimeouts(m_hFile,&cto))
		return GetLastError();

	// установка нового тайм аута
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
		// Эта функция не выполнена
		_ASSERTE(false);
		return E_INVALIDARG;
	}

	// Установка новых значений струтуры
	if (!::SetCommTimeouts(m_hFile,&cto))
	{
		return GetLastError();
	}

	// Успешный выход
	return 0;
}

LONG CSerial::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Проверка подерживаются ли таум ауты
	CheckTimeout(dwTimeout);

	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// Создание нового оверлаппеда если переданный в функцию оверлаппед равен 0
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// Инициализация структуры
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = CreateEvent(0,FALSE,TRUE,0);
		// Использование вновь созданного оверлаппеда с внешним
		lpOverlapped = &ovInternal;
	}

	// Удостоверяемся, что структура, с которой будем работать не занята
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

	// Ожидаем события
	if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped))
	{
		// Установим внутренний код
		long lLastError = ::GetLastError();

		// Проверяем имела ли ошибки асинхронная операция
		if (lLastError != ERROR_IO_PENDING)
			return lLastError;

		// Блокировали если бы струтуры не определены
		if (lpOverlapped == &ovInternal)
		{
			// Ожидание завершения асинхронной операции
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// Асинхронная операция в комплекте
				break;

			case WAIT_TIMEOUT:
				// Отмена операции ввода вывода
				CancelIo();
				// Вышло время ожидания
				return ERROR_TIMEOUT;

			default:
				// Установим внутренний код ошибки
				return GetLastError();
			}
		}
	}
	else
	{
		// Операция немедленно завершена. в связи с этим установим новое событие
		::SetEvent(lpOverlapped->hEvent);
	}

	// Успешный выход
	return 0;
}

CSerial::EEvent CSerial::GetEventType (void)
{
#ifdef _DEBUG
	// Проверка являются ли события в диапозоне маски
	if ((m_eEvent & m_dwEventMask) == 0)
		_RPTF2(_CRT_WARN,"CSerial::GetEventType - Event %08Xh not within mask %08Xh.\n", m_eEvent, m_dwEventMask);
#endif

	// Получение события (отбрасываем не нужные сабытия согласно маске)
	EEvent eEvent = EEvent(m_eEvent & m_dwEventMask);

	// Сброс внутренних событий
	m_eEvent = EEventNone;

	// Возврат текущей причины события
	return eEvent;
}

bool CSerial::GetCTS (void)
{
	// Получение модемного состояния
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_CTS_ON) != 0;
}

bool CSerial::GetDSR (void)
{
	// Получение модемного состояния
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_DSR_ON) != 0;
}

bool CSerial::GetRing (void)
{
	// Получение модемного состояния
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_RING_ON) != 0;
}

bool CSerial::GetRLSD (void)
{
	// Получение модемного состояния
	DWORD dwModemStat = 0;
	if (!::GetCommModemStatus(m_hFile,&dwModemStat))
		return false;
	return (dwModemStat & MS_RLSD_ON) != 0;
}

bool CSerial::IsOpen()
{
	// Определение открыт порт да или нет
	if (m_hFile!=INVALID_HANDLE_VALUE)
		return true;
	return false;
}

LONG CSerial::Read( void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// Проверка подерживаются ли таум ауты
	CheckTimeout(dwTimeout);

	// Асинхронная операция должна быть определена с pdwRead
	_ASSERTE(!lpOverlapped || pdwRead);

	// Использование новой переменной для подсчёта считанных байт
	DWORD dwRead;
	if (pdwRead == 0)
	{
		pdwRead = &dwRead;
	}

	// Сброс количества считанных байт
	*pdwRead = 0;
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// Инициализация оверлаппеда
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = CreateEvent(0,FALSE,TRUE,0);
		lpOverlapped = &ovInternal;
	}

	// Удостоверьтесь, что структура, с которой будем работать не занята
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

#ifdef _DEBUG
	// Отладочная версия заполняет полную структуру данных 0xDC байтами, 
	// чтобы захватить буферные ошибки как можно скорее.
	memset(pData,0xDC,iLen);
#endif
	
	// Чтение данных
	if (!::ReadFile(m_hFile,pData,iLen,pdwRead,lpOverlapped))
	{
		// Установка внутренней ошибки
		long lLastError = ::GetLastError();

		// Операция асинхронного ввода вывода имеет ли ошибку?
		if (lLastError != ERROR_IO_PENDING)
			return lLastError;

		// Блокируем если входная струтура не определена
		if (lpOverlapped == &ovInternal)
		{
			// Ожидаем завершения асинхронной операции
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// Асинхронная операция успешно завершена
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
					return GetLastError();
				break;

			case WAIT_TIMEOUT:
				// Время вышло
				CancelIo();
				return ERROR_TIMEOUT;

			default:
				// Установим код внутренней ошибки
				return GetLastError();
			}
		}
	}
	else
	{
		// Сбросим событие
		::SetEvent(lpOverlapped->hEvent);
	}

	// Успешный возврат
	return 0;
}


void CSerial::CloseComPort()
{
	CloseThread(); // Закрываем потоки
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
	// Выясняем активность потоков и их уничтожаем
	if(m_hReadThread!=INVALID_HANDLE){
		PulseEvent(m_StopReadThread); // Если поток создан то делаем событие для зовершения потока
		// Ожидаем завершения потока
		if(WaitForSingleObject(m_hReadThread,CP_SHUTDOWN_TIMEOUT)!=WAIT_OBJECT_0){	
			DWORD   dwStatus;
			SuspendThread(m_hReadThread); // Останавливаем поток
			GetExitCodeThread(m_hReadThread, &dwStatus); // Выясняем код выхода
			TerminateThread(m_hReadThread,dwStatus);} // Уничтожаем поток
		else
			CloseHandle(m_hReadThread);	// Закрываем хэндл потока
		m_hReadThread=INVALID_HANDLE; // Убиваем
	}
}

LONG CSerial::Break ()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

    // Устанавливаем режим отката
    ::SetCommBreak(m_hFile);
    ::Sleep(100);
    ::ClearCommBreak(m_hFile);

	// Успешный выход
	return 0;
}

DWORD CSerial::GetBaudrate ()
{
	if (m_hFile==INVALID_HANDLE_VALUE)
		return -1;
	// Определяем структуру dcb
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

	// Проверка используется ли аппаратная проверка связи
	if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
		return CP_HANDSHAKEHARDWARE;

	// Проверка используется ли программная проверка связи
	if (dcb.fOutX && dcb.fInX)
		return CP_HANDSHAKESOFTWARE;

	// Не используется проверка связи
	return CP_HANDSHAKEOFF;
}

LONG CSerial::Write (const void* pData, size_t iLen, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
	// При асинхронной операции, должна определена переменная pdwWritten 
	_ASSERTE(!lpOverlapped || pdwWritten);

	// Проверка подерживаются ли таум ауты
	CheckTimeout(dwTimeout);

	// Определяем количество байт необходимых для записи
	DWORD dwWritten;
	if (pdwWritten == 0)
	{
		pdwWritten = &dwWritten;
	}

	// Сбрасываем входной параметр в ноль
	*pdwWritten = 0;

	// Проверяем открыт ли порт
	if (m_hFile == INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// Инициализация
	OVERLAPPED ovInternal;
	if (lpOverlapped == 0)
	{
		// настройка структуры
		memset(&ovInternal,0,sizeof(ovInternal));
		ovInternal.hEvent = CreateEvent(0,FALSE,TRUE,0);
		// если входная структура не определена то создаём новую
		lpOverlapped = &ovInternal;
	}

	// Проверяем перед использованием не занята ли структура
	_ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

	// Записываем данные
	if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,lpOverlapped))
	{
		// Установить внутренний код ошибки
		long lLastError = ::GetLastError();

		// Проверяем имело место ошибка асинхронной операции ввода вывода, да или нет
		if (lLastError != ERROR_IO_PENDING)// ERROR_IO_PENDING Это не ошибка а показывает что идёт или произошло асинхронный ввод вывод
			return lLastError;

		// Если структуры не определены
		if (lpOverlapped == &ovInternal)
		{
			// Ожидаем завершения операции
			switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
			{
			case WAIT_OBJECT_0:
				// Операция зовершена
				if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
					return GetLastError();
				break;

			case WAIT_TIMEOUT:
				// Время вышло
				CancelIo();
				return ERROR_TIMEOUT;

			default:
				// возращаем код внутренней ошибки
				return GetLastError();
			}
		}
	}
	else
	{
		// Установим событие
		::SetEvent(lpOverlapped->hEvent);
	}

	// Успешное выполнение
	return 0;
}

#pragma warning(disable : 4996)


void CSerial::EnumerateSerialPorts(CUIntArray& ports)
{
  // Очищаем массив элементов
  ports.RemoveAll();

  // Узнаём версию OS
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL bGetVer = GetVersionEx(&osvi);

  // Только NT используем функцию API QueryDosDevice 
  if (bGetVer && (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT))
  {
    // Используйте функцию QueryDosDevice для поиска всех портов. Это является лучшим решением 
    // так как не требует открытия ком портов
    TCHAR szDevices[65535];
    DWORD dwChars = QueryDosDevice(NULL, szDevices, 65535);
    if (dwChars)
    {
      int i=0;

      for (;;)
      {
        // Определение имени текущего устройства
        TCHAR* pszCurrentDevice = &szDevices[i];

        // Если это сом порт тогда добавьте его имя к массиву который будет возращён
        int nLen = _tcslen(pszCurrentDevice);
        if (nLen > 3 && _tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0)
        {
          // Получение номера порта
          int nPort = _ttoi(&pszCurrentDevice[3]);
          ports.Add(nPort);
        }

        // Ненужные символы заполняем нулём
        while(szDevices[i] != _T('\0'))
          i++;

        // Указываем на следующую строку
        i++;

        // если двойной список имеет конец тогда выходим
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
    //Только WIN 95/98 открываем порт с нужным именем если открылся значит существует
    for (UINT i=1; i<256; i++)
    {
      // Формируем имя порта
      CString sPort;
      sPort.Format(_T("\\\\.\\COM%d"), i);

      // Открываем порт
      BOOL bSuccess = FALSE;
      HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
      if (hPort == INVALID_HANDLE_VALUE)
      {
        DWORD dwError = GetLastError();

        //Выясняем причину ошибки, что это открыт порт другим приложением или это общий отказ
        if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE)
          bSuccess = TRUE;
      }
      else
      {
        // Порт открыт успешно
        bSuccess = TRUE;

        // Так как порт больше не нужен то его закрываем
        CloseHandle(hPort);
      }

      //Добавить номер порта в массив
      if (bSuccess)
        ports.Add(i);
    }
  }
}

int CSerial::GetEnumPort(int* czNumbers)
{// Функция для перечисления портов имеющихся в системе
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
		// Выход из приложения
		::MessageBox(0,_T("Тайм ауты не поддерживаются, когда SERIAL_WIN95_SUPPORT определён"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
		::DebugBreak();
		::ExitProcess(0xFFFFFFF);
	}
}



#ifdef SERIAL_NO_CANCELIO

void CheckTimeout (DWORD dwTimeout)
{
	if ((dwTimeout != 0) && (dwTimeout != INFINITE))
	{
		// Выход из приложения
		::MessageBox(0,_T("Тайм ауты не поддерживаются, когда SERIAL_WIN95_SUPPORT определён"),_T("Serial library"), MB_ICONERROR | MB_TASKMODAL);
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
