
// LabyrinthGame.h : main header file for the LabyrinthGame application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CLabyrinthGameApp:
// See LabyrinthGame.cpp for the implementation of this class
//

class CLabyrinthGameApp : public CWinApp
{
public:
	CLabyrinthGameApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

};

extern CLabyrinthGameApp theApp;
