#pragma once


// CDialog1 对话框

class CDialog1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDialog1)

public:
	CDialog1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialog1();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedReshow();
	void CDialog1::ReShow();
	void CDialog1::ReShowImage(IplImage* img , UINT ID);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI);
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
};
