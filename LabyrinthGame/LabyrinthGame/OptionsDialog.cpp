// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "LabyrinthGame.h"
#include "OptionsDialog.h"
#include "afxdialogex.h"


// OptionsDialog dialog

IMPLEMENT_DYNAMIC(OptionsDialog, CDialogEx)

OptionsDialog::OptionsDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

OptionsDialog::~OptionsDialog()
{
}

void OptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAME, editName);
	DDX_Control(pDX, IDC_PORT, editPort);
	DDX_Control(pDX, IDC_SERVER, editServer);
	editName.SetWindowTextW((LPCTSTR)name);
	CString str;
	str.Format(_T("%d"), port);
	editPort.SetWindowTextW((LPCTSTR)str);
	editServer.SetWindowTextW((LPCTSTR)server);
}


BEGIN_MESSAGE_MAP(OptionsDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &OptionsDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// OptionsDialog message handlers


void OptionsDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	editName.GetWindowTextW(name);
	CString portmp;
	editPort.GetWindowTextW(portmp);
	editServer.GetWindowTextW(server);
	
	if (name=="")
	{
		CString str;
		str.Format(_T("Имя не должно быть пустым"));
		MessageBox((LPCTSTR)str);
		return;
	}

	if (server == "")
	{
		CString str;
		str.Format(_T("Адрес сервера не должен быть пустым"));
		MessageBox((LPCTSTR)str);
		return;
	}

	int j = _stscanf(portmp, _T("%d"), &port);

	if (!j) {
		CString str;
		str.Format(_T("Порт должен быть целым числом"));
		MessageBox((LPCTSTR)str);
		return;
	}

	OK = true;
	CDialogEx::OnOK();
}
