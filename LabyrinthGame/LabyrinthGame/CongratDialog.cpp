// CongratDialog.cpp : implementation file
//

#include "stdafx.h"
#include "LabyrinthGame.h"
#include "CongratDialog.h"
#include "afxdialogex.h"


// CongratDialog dialog

IMPLEMENT_DYNAMIC(CongratDialog, CDialogEx)

CongratDialog::CongratDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CCONGRAT_DIALOG, pParent)
	, strCongrText(_T(""))
{

}

CongratDialog::~CongratDialog()
{
}

void CongratDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CongratText, strCongrText);
}


BEGIN_MESSAGE_MAP(CongratDialog, CDialogEx)
//	ON_STN_CLICKED(IDC_CongratText, &CongratDialog::OnStnClickedCongrattext)
END_MESSAGE_MAP()


// CongratDialog message handlers


//void CongratDialog::OnStnClickedCongrattext()
//{
//	// TODO: Add your control notification handler code here
//}
