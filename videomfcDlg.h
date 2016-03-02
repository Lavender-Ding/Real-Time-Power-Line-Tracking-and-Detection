
// videomfcDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CvvImage.h"


// CvideomfcDlg dialog
class CvideomfcDlg : public CDialogEx
{
// Construction
public:
	CvideomfcDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VIDEOMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void showlist();
	afx_msg void OnBnClickedOpenButton();
	afx_msg void OnBnClickedPlayButton();
	afx_msg void OnBnClickedPauseButton();
	afx_msg void OnBnClickedStopButton();
	afx_msg void OnBnClickedCancel();
	void ShowAvi();
	void DestroyAvi();
	void ResizeImage(IplImage* img);
	void ShowImage(IplImage* img, UINT ID);
//	HBRUSH OnCtlColor(CDC *pDC,CWnd *pWnd,UINT nCtlColor);
	//void ShowPicture(int p[], int n);
	afx_msg void OnBnClickedCheck1Sangu();
	afx_msg void OnBnClickedCheck2Duangu();
	afx_msg void OnBnClickedCheck3Yiwu();
	afx_msg void OnBnClickedButtonStartdetection();
	afx_msg void OnBnClickedButtonstopdetection();
	afx_msg void OnHScroll(UINT pSBCode,UINT nPos,CScrollBar* pScrollBar);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
