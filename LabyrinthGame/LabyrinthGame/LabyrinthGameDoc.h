
// LabyrinthGameDoc.h : interface of the CLabyrinthGameDoc class
//
#include "Grid.h"
#include "NetHelper.h"
#pragma once


class CLabyrinthGameDoc : public CDocument
{
public:
	CLabyrinthGameDoc * GetDoc();
	CLabyrinthGameDoc() noexcept;
	char * GetMessageGameStart();
	char * GetMessageGameFinish();
	char * GetMessageMouseMove(int x);
	void handleMessage(char * str);
	DECLARE_DYNCREATE(CLabyrinthGameDoc)

// Attributes
public:
	int sessionNumber;
	int player;


	Grid LGrid;
	NetHelper netHelper;
	int MouceCell_x;
	int MouceCell_y;
	int Enemy_x;
	int Enemy_y;
	int CheeseCell_x;
	int CheeseCell_y;
	int CurSeconds;

	bool GameStarted;
	bool WaitingForSecondPlayer;
// Operations
public:

// Overrides
public:
//	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	void StartGame();
	void FinishGame(bool congrat, bool win);
	void DoCongratulations(CString text);
	void CheckForGameFinish();
	void RightStep();
	void LeftStep();
	void UpStep();
	void DownStep();
	virtual ~CLabyrinthGameDoc();
	//CLabyrinthGameView* GetView()  const; //почему-то студия не хочет делать такой метод
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

#endif
private:

	
protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
//	afx_msg void OnNewGame();
//	afx_msg void OnNewGame();
};
