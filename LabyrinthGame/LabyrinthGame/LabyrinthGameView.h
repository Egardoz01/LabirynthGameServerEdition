
// LabyrinthGameView.h : interface of the CLabyrinthGameView class
//

#pragma once


class CLabyrinthGameView : public CView
{
protected: // create from serialization only
	CLabyrinthGameView() noexcept;
	DECLARE_DYNCREATE(CLabyrinthGameView)

// Attributes
public:
	CLabyrinthGameDoc* GetDocument() const;
private:
	int mouse_x;
	int mouse_y;
	int cheese_x;
	int cheese_y;
	int enemy_x;
	int enemy_y;

	int cellHeight;
	int cellWidth;
	HBITMAP hBitmapMouse;
	HBITMAP hBitmapCheese;
	HBITMAP hBitmapEnemy;
	CPoint sPoint;
	UINT_PTR mainTimer;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void FinishGame();
	void StartGame();
protected:

// Implementation
public:
	virtual ~CLabyrinthGameView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()


private:
	void DrawGrid(CDC* pDC);
	void DrawTime(CDC* pDC);
	void DrawMouse(HDC hdc);
	void DrawWaitingScreen(CDC * pDC);
	void DrawInitialScreen(CDC* pDC);
	void ResizeWindowForGame();
	void ResizeWindowForWaiting();

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNewGame();
	afx_msg void OnFinishGame();
	afx_msg void OnUpdateFinishGame(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNewGame(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in LabyrinthGameView.cpp
inline CLabyrinthGameDoc* CLabyrinthGameView::GetDocument() const
   { return reinterpret_cast<CLabyrinthGameDoc*>(m_pDocument); }
#endif

