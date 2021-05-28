
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#include "Grid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <winsock2.h>
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerDlg dialog

#define PORT 5150			// Порт по умолчанию
#define DATA_BUFSIZE 8192 	// Размер буфера по умолчанию

int  _portNumber = PORT; 	 // Порт для прослушивания подключений

typedef struct _SOCKET_INFORMATION {
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

BOOL CreateSocketInformation(SOCKET s, char *Str,
	CListBox *pLB);
void FreeSocketInformation(DWORD Event, char *Str,
	CListBox *pLB);

DWORD EventTotal = 0;
WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
LPSOCKET_INFORMATION SocketArray[WSA_MAXIMUM_WAIT_EVENTS];

HWND   hWnd_LB;  // Для вывода в других потоках

UINT ListenThread(PVOID lpParam);

void handleMessage(LPSOCKET_INFORMATION soket, DWORD Event, char *str, int len);

void sendMessage(DWORD Event, char *str);

struct SessionInfo {
	DWORD player1Event;
	DWORD player2Event;
	int player1_x;
	int player1_y;
	int player2_x;
	int player2_y;
	int cheese_x;
	int cheese_y;
	char* player1Name;
	char* player2Name;
	Grid* grid;
};

int const  MAX_SESSIONS = 256;
int const MESSAGE_SIZE = 1024;
SessionInfo _sessions[MAX_SESSIONS];
int sessionNum = 1;
DWORD queue = 0;
char queueName[100];

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _listBox);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CServerDlg::OnBnClickedStop)
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	char Str[128];

	sprintf_s(Str, sizeof(Str), "%d", _portNumber);
	GetDlgItem(IDC_PORT)->SetWindowText(Str);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerDlg::OnBnClickedStart()
{
	char Str[81];

	hWnd_LB = _listBox.m_hWnd;   // Для ListenThread
	GetDlgItem(IDC_PORT)->GetWindowText(Str, sizeof(Str));
	_portNumber = atoi(Str);
	if (_portNumber <= 0 || _portNumber >= 0x10000)
	{
		AfxMessageBox("Incorrect Port number");
		return;
	}

	AfxBeginThread(ListenThread, NULL);

	GetDlgItem(IDC_START)->EnableWindow(false);

}

void CServerDlg::OnBnClickedStop()
{
	exit(0);
}

UINT ListenThread(PVOID lpParam)
{
	SOCKET Listen;
	SOCKET Accept;
	SOCKADDR_IN InternetAddr;
	DWORD Event;
	WSANETWORKEVENTS NetworkEvents;
	WSADATA wsaData;
	DWORD Ret;
	DWORD Flags;
	DWORD RecvBytes;
	DWORD SendBytes;
	char  Str[200];
	CListBox  *pLB =
		(CListBox *)(CListBox::FromHandle(hWnd_LB));

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
	{
		sprintf_s(Str, sizeof(Str),
			"WSAStartup() failed with error %d", Ret);
		pLB->AddString(Str);
		return 1;
	}

	if ((Listen = socket(AF_INET, SOCK_STREAM, 0)) ==
		INVALID_SOCKET)
	{
		sprintf_s(Str, sizeof(Str),
			"socket() failed with error %d",
			WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}
	CreateSocketInformation(Listen, Str, pLB);

	if (WSAEventSelect(Listen, EventArray[EventTotal - 1],
		FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str),
			"WSAEventSelect() failed with error %d",
			WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(_portNumber);

	if (bind(Listen, (PSOCKADDR)&InternetAddr,
		sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str),
			"bind() failed with error %d",
			WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}

	if (listen(Listen, 5))
	{
		sprintf_s(Str, sizeof(Str),
			"listen() failed with error %d",
			WSAGetLastError());
		pLB->AddString(Str);
		return 1;
	}
	while (TRUE)
	{
		// Дожидаемся уведомления о событии на любом сокете
		if ((Event = WSAWaitForMultipleEvents(EventTotal,
			EventArray, FALSE, WSA_INFINITE, FALSE)) ==
			WSA_WAIT_FAILED)
		{
			sprintf_s(Str, sizeof(Str),
				"WSAWaitForMultipleEvents failed"
				" with error %d", WSAGetLastError());
			pLB->AddString(Str);
			return 1;
		}

		if (WSAEnumNetworkEvents(
			SocketArray[Event - WSA_WAIT_EVENT_0]->Socket,
			EventArray[Event - WSA_WAIT_EVENT_0],
			&NetworkEvents) == SOCKET_ERROR)
		{
			sprintf_s(Str, sizeof(Str),
				"WSAEnumNetworkEvents failed"
				" with error %d", WSAGetLastError());
			pLB->AddString(Str);
			return 1;
		}
		if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (NetworkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str),
					"FD_ACCEPT failed with error %d",
					NetworkEvents.iErrorCode[FD_ACCEPT_BIT]);
				pLB->AddString(Str);
				break;
			}

			// Прием нового соединения и добавление его
			// в списки сокетов и событий
			if ((Accept = accept(
				SocketArray[Event - WSA_WAIT_EVENT_0]->Socket,
				NULL, NULL)) == INVALID_SOCKET)
			{
				sprintf_s(Str, sizeof(Str),
					"accept() failed with error %d",
					WSAGetLastError());
				pLB->AddString(Str);
				break;
			}

			// Слишком много сокетов. Закрываем соединение.
			if (EventTotal > WSA_MAXIMUM_WAIT_EVENTS)
			{
				sprintf_s(Str, sizeof(Str),
					"Too many connections - closing socket.");
				pLB->AddString(Str);
				closesocket(Accept);
				break;
			}

			CreateSocketInformation(Accept, Str, pLB);

			if (WSAEventSelect(Accept,
				EventArray[EventTotal - 1],
				FD_READ | FD_WRITE | FD_CLOSE) ==
				SOCKET_ERROR)
			{
				sprintf_s(Str, sizeof(Str),
					"WSAEventSelect()failed"
					" with error %d", WSAGetLastError());
				pLB->AddString(Str);
				return 1;
			}

			sprintf_s(Str, sizeof(Str),
				"Socket %d connected", Accept);
			pLB->AddString(Str);
		}
		// Пытаемся читать или писать данные, 
			  // если произошло соответствующее событие

		if (NetworkEvents.lNetworkEvents & FD_READ ||
			NetworkEvents.lNetworkEvents & FD_WRITE)
		{
			if (NetworkEvents.lNetworkEvents & FD_READ &&
				NetworkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str),
					"FD_READ failed with error %d",
					NetworkEvents.iErrorCode[FD_READ_BIT]);
				pLB->AddString(Str);
				break;
			}

			if (NetworkEvents.lNetworkEvents & FD_WRITE &&
				NetworkEvents.iErrorCode[FD_WRITE_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str),
					"FD_WRITE failed with error %d",
					NetworkEvents.iErrorCode[FD_WRITE_BIT]);
				pLB->AddString(Str);
				break;
			}

			LPSOCKET_INFORMATION SocketInfo =
				SocketArray[Event - WSA_WAIT_EVENT_0];

			// Читаем данные только если приемный буфер пуст

			if (SocketInfo->BytesRECV == 0)
			{
				SocketInfo->DataBuf.buf = SocketInfo->Buffer;
				SocketInfo->DataBuf.len = DATA_BUFSIZE;

				Flags = 0;
				if (WSARecv(SocketInfo->Socket,
					&(SocketInfo->DataBuf), 1, &RecvBytes,
					&Flags, NULL, NULL) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						sprintf_s(Str, sizeof(Str),
							"WSARecv()failed with "
							" error %d", WSAGetLastError());
						pLB->AddString(Str);
						FreeSocketInformation(
							Event - WSA_WAIT_EVENT_0, Str, pLB);
						return 1;
					}
				}
				else
				{
					SocketInfo->BytesRECV = RecvBytes;
					// Вывод сообщения, если требуется

					unsigned l = sizeof(Str) - 1;
					if (l > RecvBytes) l = RecvBytes;
					strncpy_s(Str, SocketInfo->Buffer, l);
					Str[l] = 0;
					pLB->AddString(Str);


					handleMessage(SocketInfo, Event, SocketInfo->Buffer, l);
				}

				SocketInfo->BytesSEND = 0;
				SocketInfo->BytesRECV = 0;

			}
			/*
			// Отправка данных, если это возможно

			if (SocketInfo->BytesRECV > SocketInfo->BytesSEND)
			{
				SocketInfo->DataBuf.buf =
					SocketInfo->Buffer + SocketInfo->BytesSEND;
				SocketInfo->DataBuf.len =
					SocketInfo->BytesRECV - SocketInfo->BytesSEND;


				if (WSASend(SocketInfo->Socket,
					&(SocketInfo->DataBuf), 1,
					&SendBytes, 0, NULL, NULL) ==
					SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						sprintf_s(Str, sizeof(Str),
							"WSASend() failed with "
							"error %d", WSAGetLastError());
						pLB->AddString(Str);
						FreeSocketInformation(
							Event - WSA_WAIT_EVENT_0, Str, pLB);
						return 1;
					}

					// Произошла ошибка WSAEWOULDBLOCK. 
					// Событие FD_WRITE будет отправлено, когда
					// в буфере будет больше свободного места
				}
				else
				{
					SocketInfo->BytesSEND += SendBytes;

					if (SocketInfo->BytesSEND ==
						SocketInfo->BytesRECV)
					{
						SocketInfo->BytesSEND = 0;
						SocketInfo->BytesRECV = 0;
					}
				}
			}*/
		}
		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			if (NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
			{
				sprintf_s(Str, sizeof(Str),
					"FD_CLOSE failed with error %d",
					NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
				pLB->AddString(Str);
			}

			sprintf_s(Str, sizeof(Str),
				"Closing socket information %d",
				SocketArray[Event - WSA_WAIT_EVENT_0]->Socket);
			pLB->AddString(Str);

			FreeSocketInformation(Event - WSA_WAIT_EVENT_0,
				Str, pLB);
		}
	} // while
	return 0;
}

BOOL CreateSocketInformation(SOCKET s, char *Str,
	CListBox *pLB)
{
	LPSOCKET_INFORMATION SI;

	if ((EventArray[EventTotal] = WSACreateEvent()) ==
		WSA_INVALID_EVENT)
	{
		sprintf_s(Str, sizeof(Str),
			"WSACreateEvent() failed with error %d",
			WSAGetLastError());
		pLB->AddString(Str);
		return FALSE;
	}

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
		sizeof(SOCKET_INFORMATION))) == NULL)
	{
		sprintf_s(Str, sizeof(Str),
			"GlobalAlloc() failed with error %d",
			GetLastError());
		pLB->AddString(Str);
		return FALSE;
	}

	// Подготовка структуры SocketInfo для использования.
	SI->Socket = s;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SocketArray[EventTotal] = SI;
	EventTotal++;
	return(TRUE);
}

void FreeSocketInformation(DWORD Event, char *Str,
	CListBox *pLB)
{
	LPSOCKET_INFORMATION SI = SocketArray[Event];
	DWORD i;
	if (SI != NULL)
	{
		closesocket(SI->Socket);
		GlobalFree(SI);
		WSACloseEvent(EventArray[Event]);
	}
	// Сжатие массивов сокетов и событий

	for (i = Event; i < EventTotal; i++)
	{
		EventArray[i] = EventArray[i + 1];
		SocketArray[i] = SocketArray[i + 1];
	}

	EventTotal--;
}

char *MessageNewSession(char *str, int player, int sessionNumber)
{
	str[0] = 2;//new session
	str[1] = sessionNumber;//session number
	str[2] = player;//player number
	return str;
}


char *messageFillGrid(char *str, int sessionNumber)
{
	str[0] = 4;//new Grid

	Grid* grid = new Grid();
	grid->Initialize(5, 5);
	str[1] = grid->nRows;
	str[2] = grid->nColumns;
	int next = 3;
	for (int i = 0; i < grid->nRows; i++)
		for (int j = 0; j < grid->nColumns; j++)
		{
			if (grid->grid[i][j].top == false && grid->grid[i][j].right == false)
			{
				str[next++] = 1;
			}

			if (grid->grid[i][j].top == false && grid->grid[i][j].right == true)
			{
				str[next++] = 2;
			}

			if (grid->grid[i][j].top == true && grid->grid[i][j].right == false)
			{
				str[next++] = 3;
			}

			if (grid->grid[i][j].top == true && grid->grid[i][j].right == true)
			{
				str[next++] = 4;
			}
		}


	str[next++] = strlen(_sessions[sessionNumber].player1Name);
	for (int i = 0; i < strlen(_sessions[sessionNumber].player1Name); i++)
		str[next++] = _sessions[sessionNumber].player1Name[i];

	str[next++] = strlen(_sessions[sessionNumber].player2Name);
	for (int i = 0; i < strlen(_sessions[sessionNumber].player2Name); i++)
		str[next++] = _sessions[sessionNumber].player2Name[i];

	_sessions[sessionNumber].grid = grid;

	_sessions[sessionNumber].player1_x = 0;
	_sessions[sessionNumber].player1_y = 0;


	_sessions[sessionNumber].player2_x = grid->nRows - 1;
	_sessions[sessionNumber].player2_y = grid->nColumns - 1;

	_sessions[sessionNumber].cheese_x = grid->nRows / 2;
	_sessions[sessionNumber].cheese_y = grid->nColumns / 2;

	return str;
}


char *messageCords(char *str, int sessionNumber)
{
	str[0] = 6;//new cords
	str[1] = _sessions[sessionNumber].player1_x+1;
	str[2] = _sessions[sessionNumber].player1_y+1;
	str[3] = _sessions[sessionNumber].player2_x+1;
	str[4] = _sessions[sessionNumber].player2_y+1;
	str[5] = _sessions[sessionNumber].cheese_x+1;
	str[6] = _sessions[sessionNumber].cheese_y+1;

	return str;
}

char* initNewMessage()
{
	char *str = new char[MESSAGE_SIZE];
	for (int i = 0; i < MESSAGE_SIZE; i++)
		str[i] = 0;
	return str;
}

void handleMessage(LPSOCKET_INFORMATION SocketInfo, DWORD Event, char *str, int len)
{
	if (str[0] == 1)//Starting session
	{
		if (queue == 0)
		{
			queue = Event;
			for (int i = 0; i < 100; i++)
				queueName[i] = 0;
			for (int i = 0; i < str[1]; i++)
				queueName[i] = str[2 + i];
		}
		else
		{

			
			int sessionNumber = sessionNum++;
			if (sessionNum >= MAX_SESSIONS)
				sessionNum = 1;
			_sessions[sessionNumber].player1Event = queue;
			_sessions[sessionNumber].player2Event = Event;
			
			_sessions[sessionNumber].player1Name = new char[100];
			_sessions[sessionNumber].player2Name = new char[100];

			char* p2Name = str + 2;

			for (int i = 0; i < strlen(queueName); i++)
			{
				_sessions[sessionNumber].player1Name[i] = queueName[i];
			}
			_sessions[sessionNumber].player1Name[strlen(queueName)] = 0;
			for (int i = 0; i < str[1]; i++)
			{
				_sessions[sessionNumber].player2Name[i] = p2Name[i];
			}
			_sessions[sessionNumber].player2Name[str[1]] = 0;

			queue = 0;

			char* mess1 = initNewMessage();
			char* mess2 = initNewMessage();
			
			MessageNewSession(mess1, 1, sessionNumber);
			MessageNewSession(mess2, 2, sessionNumber);

			messageFillGrid(mess1 + strlen(mess1), sessionNumber);
			messageFillGrid(mess2 + strlen(mess2), sessionNumber);
	
			messageCords(mess1 + strlen(mess1), sessionNumber);
			messageCords(mess2 + strlen(mess2), sessionNumber);

		
			sendMessage(_sessions[sessionNumber].player1Event, mess1);
			sendMessage(_sessions[sessionNumber].player2Event, mess2);

			delete mess1;
			delete mess2;
		}
	}

	if (str[0] == 3)//make move
	{
		int sessionNumber = str[1];
		int player = str[2];
		if (player == 1)
		{
			if (str[3] == 1 && _sessions[sessionNumber].player1_x > 0 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player1_y][_sessions[sessionNumber].player1_x-1].right == false)
				_sessions[sessionNumber].player1_x--;
			
			if(str[3]==2 && _sessions[sessionNumber].player1_y > 0 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player1_y][_sessions[sessionNumber].player1_x].top == false)
				_sessions[sessionNumber].player1_y--;
			
			if (str[3] == 3 && _sessions[sessionNumber].player1_x < _sessions[sessionNumber].grid->nColumns-1 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player1_y][_sessions[sessionNumber].player1_x].right == false)
				_sessions[sessionNumber].player1_x++;
			
			if (str[3] == 4 && _sessions[sessionNumber].player1_y < _sessions[sessionNumber].grid->nRows-1 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player1_y+1][_sessions[sessionNumber].player1_x].top == false)
				_sessions[sessionNumber].player1_y++;
		}

		if (player == 2)
		{
			if (str[3] == 1 && _sessions[sessionNumber].player2_x > 0 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player2_y][_sessions[sessionNumber].player2_x - 1].right == false)
				_sessions[sessionNumber].player2_x--;

			if (str[3] == 2 && _sessions[sessionNumber].player2_y > 0 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player2_y][_sessions[sessionNumber].player2_x].top == false)
				_sessions[sessionNumber].player2_y--;

			if (str[3] == 3 && _sessions[sessionNumber].player2_x < _sessions[sessionNumber].grid->nColumns - 1 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player2_y][_sessions[sessionNumber].player2_x].right == false)
				_sessions[sessionNumber].player2_x++;

			if (str[3] == 4 && _sessions[sessionNumber].player2_y < _sessions[sessionNumber].grid->nRows - 1 && _sessions[sessionNumber].grid->grid[_sessions[sessionNumber].player2_y + 1][_sessions[sessionNumber].player2_x].top == false)
				_sessions[sessionNumber].player2_y++;

		}


		char * mess4 = initNewMessage();
		messageCords(mess4,sessionNumber);
		sendMessage(_sessions[sessionNumber].player1Event, mess4);
		sendMessage(_sessions[sessionNumber].player2Event, mess4);
		delete mess4;

	}

	if (str[0] == 5)//finish game
	{
		char  Str[200];
		CListBox  *pLB =
			(CListBox *)(CListBox::FromHandle(hWnd_LB));
		FreeSocketInformation(
			Event - WSA_WAIT_EVENT_0, Str, pLB);
	}


}





void sendMessage(DWORD Event, char *str)
{
	DWORD SendBytes;
	char  Str[200];
	CListBox  *pLB =
		(CListBox *)(CListBox::FromHandle(hWnd_LB));

	LPSOCKET_INFORMATION SocketInfo = SocketArray[Event];

	if (SocketInfo == NULL)
		return;

	SocketInfo->DataBuf.buf = str;
	SocketInfo->DataBuf.len = strlen(str);

	if (WSASend(SocketInfo->Socket,
		&(SocketInfo->DataBuf), 1,
		&SendBytes, 0, NULL, NULL) ==
		SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			sprintf_s(Str, sizeof(Str),
				"WSASend() failed with "
				"error %d", WSAGetLastError());
			pLB->AddString(Str);
			FreeSocketInformation(
				Event - WSA_WAIT_EVENT_0, Str, pLB);
			return;
		}

		// Произошла ошибка WSAEWOULDBLOCK. 
		// Событие FD_WRITE будет отправлено, когда
		// в буфере будет больше свободного места
	}

}