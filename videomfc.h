
// videomfc.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "highgui.h"
#include "cv.h"
#include "cxcore.h"

// CvideomfcApp:
// See videomfc.cpp for the implementation of this class
//

class CvideomfcApp : public CWinApp
{
public:
	CvideomfcApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CvideomfcApp theApp;