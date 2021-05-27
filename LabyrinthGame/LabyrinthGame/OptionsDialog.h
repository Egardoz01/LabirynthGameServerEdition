#pragma once


// OptionsDialog dialog

class OptionsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(OptionsDialog)

public:
	OptionsDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~OptionsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS_DIALOG};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit editName;
	CEdit editPort;
//	CEdit EditServer;
	CEdit editServer;
	CString name;
	int port;
	CString server;
	bool OK;
	afx_msg void OnBnClickedOk();
};
