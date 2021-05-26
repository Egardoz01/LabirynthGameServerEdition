#pragma once

enum {EASY, MEDIUM, HARD};
// StartGameForm dialog

class StartGameForm : public CDialogEx
{
	DECLARE_DYNAMIC(StartGameForm)

public:
	StartGameForm(CWnd* pParent = nullptr);   // standard constructor
	virtual ~StartGameForm();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_START_GAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
//	int Level;
	afx_msg void OnBnClickedEasy();
	afx_msg void OnBnClickedMedium();
	afx_msg void OnBnClickedHard();
//	BOOL EasyChecked;
	int Level2;
	bool DialogResult;
	CStatic picture;
};
