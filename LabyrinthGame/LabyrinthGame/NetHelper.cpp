#include "stdafx.h"
#include "NetHelper.h"
#include <winsock2.h>

bool NetHelper::Connect(CString serverStr, int port)
{
	char szServer[128];	// Имя или IP-адрес сервера
	int	iPort;			// Порт

	WSADATA	wsd;

	struct sockaddr_in 	server;
	struct hostent* host = NULL; //Для функции gethostbyname();

	CString str;


	iPort = port;
	
	sprintf(szServer, "%S", serverStr);
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		AfxMessageBox((LPTSTR)"Failed to load Winsock library!"); //((LPTSTR));
		return false;
	}
	/*
	* Функция сокета создает сокет, который привязан к определенному поставщику транспортных услуг.
	*
	AF_INET(2) - Семейство адресов Интернет-протокола версии 4 (IPv4).
	*
	SOCK_STREAM(1) Тип сокета, который обеспечивает последовательные, надежные, двусторонние байтовые
	* потоки на основе соединения с механизмом передачи данных OOB.
	* Этот тип сокета использует протокол управления передачей (TCP) для семейства интернет-адресов (AF_INET или AF_INET6).
	*
	IPPROTO_TCP(6)
	* Протокол управления передачей (TCP). Это возможное значение, если параметр af равен AF_INET или AF_INET6, а параметр типа - SOCK_STREAM .
	*
	*/
	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sClient == INVALID_SOCKET)
	{
		str.Format(_T("socket() failed: %d\n"), WSAGetLastError());
		AfxMessageBox(str);
		return false;
	}
	server.sin_family = AF_INET;
	/*
	* Функция htons преобразует u_short из хоста в сетевой порядок байтов TCP / IP (который является прямым порядком байтов).
	* Функция htons возвращает значение в сетевом порядке байтов TCP / IP.
	*/
	server.sin_port = htons(iPort);
	/*
	* Функция inet_addr преобразует строку, содержащую десятичный адрес IPv4 с точками, в правильный адрес для структуры IN_ADDR .
	* Если ошибки не возникает, функция inet_addr возвращает длинное значение без знака,
	* содержащее подходящее двоичное представление данного Интернет-адреса.
	*
	* Если строка в параметре cp не содержит допустимого адреса в Интернете,
	* например, если часть адреса «abcd» превышает 255, то inet_addr возвращает значение INADDR_NONE .
	*
	*/
	server.sin_addr.s_addr = inet_addr(szServer);

	if (server.sin_addr.s_addr == INADDR_NONE)
	{
		/*
		* Функция gethostbyname извлекает информацию о хосте, соответствующую имени хоста, из базы данных хоста.

		hostent * gethostbyname(
			const char *name
		);

		* Если ошибки не возникает, gethostbyname возвращает указатель на структуру хоста, описанную выше.
		* В противном случае он возвращает нулевой указатель, а конкретный номер ошибки можно получить, вызвав WSAGetLastError .
		*/
		host = gethostbyname(szServer);
		if (host == NULL)
		{
			str.Format(_T("Unable to resolve server: %s"), szServer);
			AfxMessageBox(str);
			return false;
		}
		CopyMemory(&server.sin_addr, host->h_addr_list[0], host->h_length);
	}

	//Функция connect устанавливает соединение с указанным сокетом.
	if (connect(m_sClient, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		str.Format(_T("connect() failed: %d"), WSAGetLastError());
		AfxMessageBox(str);
		return false;
	}
//	str.Format(_T("Connected successfully"));
	//AfxMessageBox(str);
	return true;
}

void NetHelper::Send(char *szMessage)
{
	CString str;
	int		ret;
	int len = strlen(szMessage);
	ret = send(m_sClient, szMessage,len, 0);
	if (ret == 0)
		return;
	if (ret == SOCKET_ERROR)
	{
		str.Format(_T("Failed to send"));
		AfxMessageBox(str);
		return;
	}

	delete szMessage;
}

char * NetHelper::Receive()
{
	CString str;
	int		ret;
	char*	szBuffer = new char[2048];
	ret = recv(m_sClient, szBuffer, 2048, 0);
	if (ret == 0)
		return NULL;
	if (ret == SOCKET_ERROR)
	{
		str.Format(_T("Failed to receive"));
		AfxMessageBox(str);
		strcpy(szBuffer, "CLOSETHREAD");
		return szBuffer;
	}
	
	return szBuffer;
}