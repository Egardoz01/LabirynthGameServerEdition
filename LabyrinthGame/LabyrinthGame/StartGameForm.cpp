// StartGameForm.cpp : implementation file
//

#include "stdafx.h"
#include "LabyrinthGame.h"
#include "StartGameForm.h"
#include "afxdialogex.h"


// StartGameForm dialog

IMPLEMENT_DYNAMIC(StartGameForm, CDialogEx)

StartGameForm::StartGameForm(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_START_GAME, pParent),
	Level2(0)
{
	
	//CheckRadioButton(IDC_EASY, IDC_HARD, IDC_EASY);
}

StartGameForm::~StartGameForm()
{
}

void StartGameForm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_EASY, Level2);
	DDX_Control(pDX, IDC_IMAGE_LEVEL, picture);
}


BEGIN_MESSAGE_MAP(StartGameForm, CDialogEx)
	ON_BN_CLICKED(IDC_START, &StartGameForm::OnBnClickedStart)
	ON_BN_CLICKED(IDC_EASY, &StartGameForm::OnBnClickedEasy)
	ON_BN_CLICKED(IDC_MEDIUM, &StartGameForm::OnBnClickedMedium)
	ON_BN_CLICKED(IDC_HARD, &StartGameForm::OnBnClickedHard)
END_MESSAGE_MAP()


// StartGameForm message handlers


void StartGameForm::OnBnClickedStart()
{
	DialogResult = true;
	EndDialog(0);
}


void StartGameForm::OnBnClickedEasy()
{
	Level2 = EASY;
	picture.SetBitmap((HBITMAP)LoadImage(0, L"gerry.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
}


void StartGameForm::OnBnClickedMedium()
{
	Level2 = MEDIUM;
	picture.SetBitmap((HBITMAP)LoadImage(0, L"rat.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
}


void StartGameForm::OnBnClickedHard()
{
	Level2 = HARD;
	picture.SetBitmap((HBITMAP)LoadImage(0, L"splinter.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
}
