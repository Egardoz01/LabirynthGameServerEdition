
// LabyrinthGameDoc.cpp : implementation of the CLabyrinthGameDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "LabyrinthGame.h"
#endif

#include "LabyrinthGameDoc.h"
#include "LabyrinthGameView.h"
#include <propkey.h>
#include "CongratDialog.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
*/
// CLabyrinthGameDoc


UINT ListenThread(PVOID lpParam);


UINT ListenThread(PVOID lpParam)
{


	while (true)
	{

		//handleMessage(netHelper.Receive());
		CLabyrinthGameDoc *pDoc = NULL;
		pDoc = pDoc->GetDoc();
		if (pDoc->GameStarted == false && pDoc->WaitingForSecondPlayer==false)
			return 0;

		char* buf = pDoc->netHelper.Receive();
		if (buf != NULL &&strcmp(buf,"CLOSETHREAD")==0)
		{
			pDoc->FinishGame(false, false);
			return 0;
		}
		if (buf != NULL)
		{
			pDoc->handleMessage(buf);
		}
		Sleep(100);
	}
}



CLabyrinthGameDoc * CLabyrinthGameDoc::GetDoc()
{
	CDocument* pDoc = NULL;
	CWnd* pWndMain = AfxGetApp()->m_pMainWnd;
	if (NULL != pWndMain)
	{
		if (pWndMain->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
		{
			CFrameWnd* aa = (CFrameWnd*)pWndMain;
			auto view = (CLabyrinthGameView*)aa->GetActiveView();
			pDoc = view->GetDocument();
		}
		else
		{
			ASSERT(FALSE);
		}
	}

	return (CLabyrinthGameDoc *)pDoc;
}


IMPLEMENT_DYNCREATE(CLabyrinthGameDoc, CDocument)

BEGIN_MESSAGE_MAP(CLabyrinthGameDoc, CDocument)
//	ON_COMMAND(ID_NEW_GAME, &CLabyrinthGameDoc::OnNewGame)
//ON_COMMAND(ID_NEW_GAME, &CLabyrinthGameDoc::OnNewGame)
END_MESSAGE_MAP()


// CLabyrinthGameDoc construction/destruction

CLabyrinthGameDoc::CLabyrinthGameDoc() noexcept
{
	playerName = "BIBA";
	port = 5150;
	server = "localhost";

	GameStarted = false;
	LGrid.Initialize(20, 20);
}

char* CLabyrinthGameDoc:: GetMessageGameStart()
{
	char * kek = new char[1024];
	kek[0] = 1;//new session
	kek[1] = playerName.GetLength();
	for (int i = 0; i < playerName.GetLength(); i++)
		kek[i + 2] = playerName[i];
	return kek;
}


char* CLabyrinthGameDoc::GetMessageMouseMove(int x)
{
	char * kek = new char[1024];
	kek[0] = 3;
	kek[1] = sessionNumber;
	kek[2] = player;
	kek[3] = x;//1-left 2-top 3-right 4-bot
	return kek;
}

char* CLabyrinthGameDoc::GetMessageGameFinish()
{
	char * kek = new char[1024];
	kek[0] = 5;//delete session
	return kek;
}

int CLabyrinthGameDoc::handleGameStart(char * str)
{
	sessionNumber = str[1];
	player = str[2];
	return 3;
}

int CLabyrinthGameDoc::handleFillGrid(char * str)
{
	int nRows = str[1];
	int nColumns = str[2];
	char *buff, *names;
	buff = str + 3;
	names = buff + nRows * nColumns;
	LGrid.FillGrid(nRows, nColumns, buff);
	char *name1 = names + 1;
	int len1 = names[0];
	char *name2 = name1 + len1;

	player1Name = new char[len1 + 1];
	for (int i = 0; i < len1; i++)
		player1Name[i] = name1[i];
	player1Name[len1] = 0;


	int len2 = name2[0];
	player2Name = new char[len2 + 1];
	name2 = name2 + 1;
	for (int i = 0; i < len2; i++)
		player2Name[i] = name2[i];

	player2Name[len2] = 0;

	GameStarted = true;
	WaitingForSecondPlayer = false;


	return 3 + nRows * nColumns + 2 + len1 + len2;
}

int CLabyrinthGameDoc::handleMousePosition(char * str)
{
	if (player == 1)
	{
		MouceCell_x = str[1] - 1;
		MouceCell_y = str[2] - 1;
		Enemy_x = str[3] - 1;
		Enemy_y = str[4] - 1;
		CheeseCell_x = str[5] - 1;
		CheeseCell_y = str[6] - 1;
	}
	if (player == 2)
	{
		MouceCell_x = str[3] - 1;
		MouceCell_y = str[4] - 1;
		Enemy_x = str[1] - 1;
		Enemy_y = str[2] - 1;
		CheeseCell_x = str[5] - 1;
		CheeseCell_y = str[6] - 1;
	}

		
	return 7;
}

void CLabyrinthGameDoc::handleMessage(char * str)
{
	if (str[0] == 2)//new session
	{
		int off = 0;
		off+=handleGameStart(str);
		off+=handleFillGrid(str+off);
		handleMousePosition(str+off);

		CLabyrinthGameView * curView = NULL;
		POSITION pos = GetFirstViewPosition();
		if (pos != NULL)
		{
			curView = (CLabyrinthGameView*)GetNextView(pos);
			curView->StartGame();
		}
	}
	
	if (str[0] == 6)//mouses Positions
	{
		
		handleMousePosition(str);

		CLabyrinthGameView * curView = NULL;
		POSITION pos = GetFirstViewPosition();
		if (pos != NULL)
		{
			curView = (CLabyrinthGameView*)GetNextView(pos);
			curView->RedrawWindow();
		}
		CheckForGameFinish();

	}

	delete str;
}



void CLabyrinthGameDoc::StartGame()
{
	if (netHelper.Connect(server,port))
	{
		CLabyrinthGameView * curView = NULL;
		POSITION pos = GetFirstViewPosition();
		if (pos != NULL)
		{
			curView = (CLabyrinthGameView*)GetNextView(pos);
			curView->RedrawWindow();

		}
		//Sleep(1000);
		netHelper.Send(GetMessageGameStart());
		WaitingForSecondPlayer = true;
		AfxBeginThread(ListenThread, NULL);

		
	}
}


void CLabyrinthGameDoc::DoCongratulations(CString text)
{
	CongratDialog dlg;
	dlg.strCongrText = text;
	dlg.DoModal();
}

void CLabyrinthGameDoc::CheckForGameFinish()
{
	if (MouceCell_x == CheeseCell_x && MouceCell_y == CheeseCell_y)
		FinishGame(true, true);
	if(Enemy_x == CheeseCell_x && Enemy_y == CheeseCell_y)
		FinishGame(true, false);

}

void CLabyrinthGameDoc::FinishGame(bool congrat, bool win)
{
	GameStarted = false;
	WaitingForSecondPlayer = false;
	CLabyrinthGameView * curView = NULL;
	POSITION pos = GetFirstViewPosition();
	LGrid.Initialize(20, 20);
	if (pos != NULL)
	{
		curView = (CLabyrinthGameView*)GetNextView(pos);
		curView->FinishGame();
		curView->OnInitialUpdate();

	}
	if (congrat)
	{
		CString strCongratulations;
		if(win)
			strCongratulations.Format(_T("ПОЗДРАВЛЯЕМ!!!\nВы выбрались из DUNGEON за %d секунд"), CurSeconds);
		else
			strCongratulations.Format(_T("Вы проиграли\nНе растраивайтесь, повезет в следующей катке"));

		DoCongratulations(strCongratulations);
	}

	netHelper.Send(GetMessageGameFinish());
	//OnNewDocument();
}

/*
void CLabyrinthGameView* CLabyrinthGameDoc::GetView()
{
	CLabyrinthGameView * curView = NULL;
	POSITION pos = GetFirstViewPosition();
	if (pos != NULL)
		curView = (CLabyrinthGameView*)GetNextView(pos);

	return curView;
}*/

void CLabyrinthGameDoc::RightStep()
{
	if (MouceCell_x < LGrid.nColumns - 1)
	{
		if (LGrid.grid[MouceCell_y][MouceCell_x].right == false)
		{
			netHelper.Send(GetMessageMouseMove(3));
			//MouceCell_x++;
		}
	}
}

void CLabyrinthGameDoc::LeftStep()
{
	if (MouceCell_x > 0)
	{
		if (LGrid.grid[MouceCell_y][MouceCell_x - 1].right == false)
		{
			netHelper.Send(GetMessageMouseMove(1));
			//MouceCell_x--;
		}
	}
}

void CLabyrinthGameDoc::UpStep()
{
	if (MouceCell_y > 0)
	{
		if (LGrid.grid[MouceCell_y][MouceCell_x].top == false)
		{
			netHelper.Send(GetMessageMouseMove(2));
			//MouceCell_y--;
		}
	}
}

void CLabyrinthGameDoc::DownStep()
{
	if (MouceCell_y < LGrid.nRows - 1)
	{
		if (LGrid.grid[MouceCell_y + 1][MouceCell_x].top == false)
		{
			netHelper.Send(GetMessageMouseMove(4));
			//MouceCell_y++;
		}
	}
}

CLabyrinthGameDoc::~CLabyrinthGameDoc()
{

}

//BOOL CLabyrinthGameDoc::OnNewDocument()
//{
//	if (!CDocument::OnNewDocument())
//		return FALSE;
//
//	// TODO: add reinitialization code here
//	// (SDI documents will reuse this document)
//
//	return TRUE;
//}



// CLabyrinthGameDoc serialization

void CLabyrinthGameDoc::Serialize(CArchive& ar)
{
	
}




#ifdef SHARED_HANDLERS

// Support for thumbnails
void CLabyrinthGameDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CLabyrinthGameDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CLabyrinthGameDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CLabyrinthGameDoc diagnostics

#ifdef _DEBUG
void CLabyrinthGameDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLabyrinthGameDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLabyrinthGameDoc commands


