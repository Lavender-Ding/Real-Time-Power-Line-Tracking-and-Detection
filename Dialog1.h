#pragma once


// CDialog1 �Ի���

class CDialog1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDialog1)

public:
	CDialog1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialog1();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
