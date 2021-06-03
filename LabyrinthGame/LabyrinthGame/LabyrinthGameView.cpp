
// LabyrinthGameView.cpp : implementation of the CLabyrinthGameView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "LabyrinthGame.h"
#endif

#include "LabyrinthGameDoc.h"
#include "LabyrinthGameView.h"
#include "OptionsDialog.h"
#include <winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLabyrinthGameView

IMPLEMENT_DYNCREATE(CLabyrinthGameView, CView)

BEGIN_MESSAGE_MAP(CLabyrinthGameView, CView)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_NEW_GAME, &CLabyrinthGameView::OnNewGame)
	ON_COMMAND(ID_FINISH_GAME, &CLabyrinthGameView::OnFinishGame)
	ON_UPDATE_COMMAND_UI(ID_FINISH_GAME, &CLabyrinthGameView::OnUpdateFinishGame)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CLabyrinthGameView::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_NEW_GAME, &CLabyrinthGameView::OnUpdateNewGame)
//	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CLabyrinthGameView::OnUpdateFileOpen)
ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CLabyrinthGameView::OnUpdateFileOpen)
ON_COMMAND(ID_OPTIONS, &CLabyrinthGameView::OnOptions)
END_MESSAGE_MAP()



// CLabyrinthGameView construction/destruction

CLabyrinthGameView::CLabyrinthGameView() noexcept
{

}

HWND hwnd;

CLabyrinthGameView::~CLabyrinthGameView()
{


}

BOOL CLabyrinthGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
	
}

// CLabyrinthGameView drawing

void CLabyrinthGameView::OnDraw(CDC* pDC)
{
	CLabyrinthGameDoc* doc = GetDocument();
	ASSERT_VALID(doc);
	if (!doc)
		return;

	if (doc->GameStarted)
	{
		DrawNames(pDC);
		DrawMouse(pDC->m_hDC);
		DrawGrid(pDC);
	}else 
	if (doc->WaitingForSecondPlayer)
	{
		DrawGrid(pDC);
		DrawWaitingScreen(pDC);
	}
	else
	{
		DrawGrid(pDC);
		DrawInitialScreen(pDC);
	
	}
}





void CLabyrinthGameView::DrawMouse(HDC hdc)
{

	CLabyrinthGameDoc* doc = GetDocument();

	BITMAP          bitmap;
	HDC             hdcMem;
	HGDIOBJ         oldBitmap;


	cheese_x = sPoint.x + 5 + cellWidth * doc->CheeseCell_x;
	cheese_y = sPoint.y + 5 + cellHeight * doc->CheeseCell_y;



	hdcMem = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, hBitmapCheese);
	GetObject(hBitmapCheese, sizeof(bitmap), &bitmap);
	BitBlt(hdc, cheese_x, cheese_y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);


	mouse_x = sPoint.x + 5 + doc->MouceCell_x*cellWidth;
	mouse_y = sPoint.y + 5 + doc->MouceCell_y*cellHeight;

	hdcMem = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, hBitmapMouse);
	GetObject(hBitmapMouse, sizeof(bitmap), &bitmap);
	BitBlt(hdc, mouse_x, mouse_y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);


	enemy_x = sPoint.x + 5 + doc->Enemy_x*cellWidth;
	enemy_y = sPoint.y + 5 + doc->Enemy_y*cellHeight;

	hdcMem = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, hBitmapEnemy);
	GetObject(hBitmapEnemy, sizeof(bitmap), &bitmap);
	BitBlt(hdc, enemy_x, enemy_y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);


}

void CLabyrinthGameView::DrawWaitingScreen(CDC * pDC)
{

	CLabyrinthGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	RECT ClientRect;
	CFont Font;
	LOGFONT LF;
	int LineHeight;
	CFont *PtrOldFont;
	int X, Y;

	CFont TempFont;

	//TempFont.CreateStockObject(SYSTEM_FONT);

	TempFont.CreateStockObject(SYSTEM_FIXED_FONT);
	TempFont.GetObject(sizeof(LOGFONT), &LF);

	LF.lfCharSet = RUSSIAN_CHARSET; //RUSSIAN_CHARSET
	//LF.lfWeight = FW_BOLD;

	//LF.lfItalic = 1;

	//LF.lfUnderline = 1;


	Font.CreateFontIndirect(&LF);
	PtrOldFont = pDC->SelectObject(&Font);

	GetClientRect(&ClientRect);

	pDC->SetTextAlign(TA_CENTER);
	X = (ClientRect.left + ClientRect.right) / 2;

	// установка цвета текста и режима фона:
	pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	pDC->SetBkMode(TRANSPARENT);

	// вывод строк текста:
	LineHeight = LF.lfHeight;
	Y = (ClientRect.top + ClientRect.bottom) / 2 - 50;

	CRect rect(X - 200, Y - 50, X + 200, Y + 80);
	pDC->Rectangle(rect);

	CBrush brush;
	brush.CreateSolidBrush(RGB(210, 210, 210));
	pDC->FillRect(&rect, &brush);


	pDC->TextOut(X, Y, L"Вы находитесь в очереди на игру.");
	Y += LineHeight;
	pDC->TextOut(X, Y, _T("Подождите, пока кто-нибудь подключится,"));
	Y += LineHeight;
	pDC->TextOut(X, Y, _T("чтобы с вами поиграть."));


	// отмена выбора шрифта:
	pDC->SelectObject(PtrOldFont);
}

void CLabyrinthGameView::DrawInitialScreen(CDC * pDC)
{
	CLabyrinthGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	RECT ClientRect;
	CFont Font;
	LOGFONT LF;
	int LineHeight;
	CFont *PtrOldFont;
	int X, Y;
	
	CFont TempFont;
	
	//TempFont.CreateStockObject(SYSTEM_FONT);

	TempFont.CreateStockObject(SYSTEM_FIXED_FONT);
	TempFont.GetObject(sizeof(LOGFONT), &LF);
	
	LF.lfCharSet = RUSSIAN_CHARSET; //RUSSIAN_CHARSET
	//LF.lfWeight = FW_BOLD;

	//LF.lfItalic = 1;

	//LF.lfUnderline = 1;


	Font.CreateFontIndirect(&LF);
	PtrOldFont = pDC->SelectObject(&Font);

	GetClientRect(&ClientRect);
	
	pDC->SetTextAlign(TA_CENTER);
	X = (ClientRect.left + ClientRect.right) / 2;
	
	// установка цвета текста и режима фона:
	pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	pDC->SetBkMode(TRANSPARENT);

	// вывод строк текста:
	LineHeight = LF.lfHeight;
	Y = (ClientRect.top + ClientRect.bottom) / 2 - 50;

	CRect rect(X-200, Y-50, X+200, Y+80);
	pDC->Rectangle(rect);

	CBrush brush;
	brush.CreateSolidBrush(RGB(210, 210, 210));
	pDC->FillRect(&rect, &brush);
	

	pDC->TextOut(X, Y, L"Добро пожаловать в ИГРУ!!!!");
	Y += LineHeight;
	pDC->TextOut(X, Y,_T("Начните новую игру, "));
	Y += LineHeight;
	pDC->TextOut(X, Y,_T("Чтобы зарубиться в жестокой битве за кусок сыра"));

	


	// отмена выбора шрифта:
	pDC->SelectObject(PtrOldFont);
}


void CLabyrinthGameView::DrawGrid(CDC* pDC)
{
	CLabyrinthGameDoc* doc = GetDocument();
	
	int n = doc->LGrid.nRows;
	int m = doc->LGrid.nColumns;
	int height = cellHeight;
	int width = cellWidth;

	pDC->MoveTo(sPoint);
	pDC->LineTo(sPoint.x, sPoint.y + n * height);
	pDC->MoveTo(sPoint.x, sPoint.y + n * height);
	pDC->LineTo(sPoint.x + m * width, sPoint.y + n * height);//рисуем границы
	


	for (int i = 0; i < n; i++)
	{
		int curY = sPoint.y + i * height;
		int curX = sPoint.x;
		for (int j = 0; j < m; j++)
		{
			if (doc->LGrid.grid[i][j].top)
			{
				pDC->MoveTo(curX, curY);
				pDC->LineTo(curX + width, curY);

			}

			if (doc->LGrid.grid[i][j].right)
			{
				pDC->MoveTo(curX + width, curY);
				pDC->LineTo(curX + width, curY + height);

			}

			curX += width;
		}
	}
}



void CLabyrinthGameView::DrawNames(CDC * pDC)
{
	CLabyrinthGameDoc* doc = GetDocument();
	RECT rect;
	rect.left = 5;
	rect.top = 25;
	CString strName1;
	char* str1 = GetDocument()->player1Name;
	char* str2 = GetDocument()->player2Name;
	strName1.Format(_T("%S VS %S"), str1, str2);
	pDC->DrawText(strName1, &rect, DT_SINGLELINE | DT_NOCLIP);
}



// CLabyrinthGameView diagnostics

#ifdef _DEBUG
void CLabyrinthGameView::AssertValid() const
{
	CView::AssertValid();
}

void CLabyrinthGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLabyrinthGameDoc* CLabyrinthGameView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLabyrinthGameDoc)));
	return (CLabyrinthGameDoc*)m_pDocument;
}
#endif //_DEBUG




void CLabyrinthGameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CLabyrinthGameDoc* doc = GetDocument();
	if (!doc->GameStarted)
		return;
	switch (nChar)
	{
	case VK_LEFT:	// стрелка влево
		doc->LeftStep();
		break;
	case VK_RIGHT:	// стрелка вправо
		doc->RightStep();
		break;
	case VK_UP:		// стрелка вверх
		doc->UpStep();
		break;
	case VK_DOWN:		// стрелка вверх
		doc->DownStep();
		break;
	}

	//this->RedrawWindow();
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CLabyrinthGameView::ResizeWindowForGame()
{

	CLabyrinthGameDoc* doc = GetDocument();


	if (!doc)
		return;

	CRect rcClient, rcWindow;
	GetClientRect(&rcClient);
	
	if (!hwnd)
	{
		hwnd = GetParentFrame()->m_hWnd;
	}
	if (hwnd)
	{

		CFrameWnd  *wnd =
			(CFrameWnd *)(CFrameWnd::FromHandle(hwnd));


		wnd->GetWindowRect(&rcWindow);
		//GetParentFrame()->GetWindowRect(&rcWindow);

		int nWidthDiff = rcWindow.Width() - rcClient.Width();
		int nHeightDiff = rcWindow.Height() - rcClient.Height();

		rcWindow.right = rcWindow.left +
			cellWidth * doc->LGrid.nColumns + nWidthDiff + sPoint.x + 5;
		rcWindow.bottom = rcWindow.top +
			cellHeight * doc->LGrid.nRows + nHeightDiff + sPoint.y + 5;

		wnd->MoveWindow(&rcWindow);
	}
}

void CLabyrinthGameView::ResizeWindowForWaiting()
{
	if (!GetDocument()->GameStarted)
	{
		sPoint = CPoint(5, 5);
		cellHeight = 35;
		cellWidth = 35;
		ResizeWindowForGame();
	}
}



void CLabyrinthGameView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	ResizeWindowForWaiting();
}



void CLabyrinthGameView::FinishGame()
{
	KillTimer(mainTimer);
	ResizeWindowForWaiting();
	RedrawWindow();

}

void CLabyrinthGameView::StartGame()
{
	cellHeight = 35;
	cellWidth = 35;
	sPoint = CPoint(5, 50);



	hBitmapMouse = (HBITMAP)LoadImage(NULL, L"mouse.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBitmapCheese = (HBITMAP)LoadImage(NULL, L"cheese.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hBitmapEnemy = (HBITMAP)LoadImage(NULL, L"enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	ResizeWindowForGame();


	mainTimer = SetTimer(1, 1000, NULL);

}


void CLabyrinthGameView::OnNewGame()
{

	GetDocument()->StartGame();
	
}


void CLabyrinthGameView::OnFinishGame()
{
	
	GetDocument()->FinishGame(false, false);
}


void CLabyrinthGameView::OnUpdateFinishGame(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GameStarted || GetDocument()->WaitingForSecondPlayer);
}


void CLabyrinthGameView::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GameStarted);
}


void CLabyrinthGameView::OnUpdateNewGame(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->GameStarted && !GetDocument()->WaitingForSecondPlayer);
}




void CLabyrinthGameView::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->GameStarted);
}


void CLabyrinthGameView::OnOptions()
{
	OptionsDialog dlg;
	dlg.name = GetDocument()->playerName;
	dlg.port = GetDocument()->port;
	dlg.server = GetDocument()->server;
	dlg.DoModal();
	if (dlg.OK)
	{
		GetDocument()->playerName = dlg.name;
		GetDocument()->server = dlg.server;
		GetDocument()->port = dlg.port;
	}

}
