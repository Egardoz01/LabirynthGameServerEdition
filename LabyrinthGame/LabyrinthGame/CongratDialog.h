#pragma once


// CongratDialog dialog

class CongratDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CongratDialog)

public:
	CongratDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CongratDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CCONGRAT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString strCongrText;
//	afx_msg void OnStnClickedCongrattext();
};
