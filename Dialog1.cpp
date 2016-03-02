// Dialog1.cpp : 实现文件
//

#include "stdafx.h"
#include "videomfc.h"
#include "Dialog1.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include "wrongnode.h"
#include <opencv2\opencv.hpp>

bool start=false;
extern CString filePath;
CvCapture* recap=NULL;
extern int nSelect;
extern int pic_all[];
extern int type[];
extern int pic_all_end[];
extern CString s1;
extern CString s2;
extern CString s3;
extern wrongnode *wronghead;


int startframe,endframe,currentframe;
CWnd *pwnd1;
CRect rect1;
CDC *pDC1;
HDC hDC1;

// CDialog1 对话框

IMPLEMENT_DYNAMIC(CDialog1, CDialogEx)

CDialog1::CDialog1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialog1::IDD, pParent)
{

}

CDialog1::~CDialog1()
{
}

void CDialog1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialog1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDialog1::OnBnClickedButtonStart)
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDialog1::OnBnClickedButtonStop)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDialog1 消息处理程序

int myWaitKey_sub( int delay )
{
    int time0 = GetTickCount();

    for(;;)
    {
        /*CvWindow* window;*/
        MSG message;
        int is_processed = 0;

        if( (delay > 0 && abs((int)(GetTickCount() - time0)) >= delay) /*|| hg_windows == 0*/ )
            return -1;

        if( delay <= 0 )
            GetMessage(&message, 0, 0, 0);
        else if( PeekMessage(&message, 0, 0, 0, PM_REMOVE) == FALSE )
        {
            Sleep(1);
            continue;
        }

        /*for( window = hg_windows; window != 0 && is_processed == 0; window = window->next )
        {
            if( window->hwnd == message.hwnd || window->frame == message.hwnd )
            {*/
                is_processed = 1;
                switch(message.message)
                {
                case WM_DESTROY:
                case WM_CHAR:
                    DispatchMessage(&message);
                    return (int)message.wParam;

                case WM_SYSKEYDOWN:
                    if( message.wParam == VK_F10 )
                    {
                        is_processed = 1;
                        return (int)(message.wParam << 16);
                    }
                    break;

                case WM_KEYDOWN:
                    TranslateMessage(&message);
                    if( (message.wParam >= VK_F1 && message.wParam <= VK_F24) ||
                        message.wParam == VK_HOME || message.wParam == VK_END ||
                        message.wParam == VK_UP || message.wParam == VK_DOWN ||
                        message.wParam == VK_LEFT || message.wParam == VK_RIGHT ||
                        message.wParam == VK_INSERT || message.wParam == VK_DELETE ||
                        message.wParam == VK_PRIOR || message.wParam == VK_NEXT )
                    {
                        DispatchMessage(&message);
                        is_processed = 1;
                        return (int)(message.wParam << 16);
                    }
                default:
                    DispatchMessage(&message);
                    is_processed = 1;
                    break;
                }
            /*}
        }*/

        if( !is_processed )
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

BOOL CDialog1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//AfxGetApp()->LoadIcon(IDR_ICO1);

	// TODO: Add extra initialization here

	recap = cvCreateFileCapture(filePath);
	IplImage* frame11;
	cvSetCaptureProperty(recap,CV_CAP_PROP_POS_FRAMES,atoi(s1));
	frame11 = cvQueryFrame(recap);
	ReShowImage(frame11, IDC_RESHOW);

	CEdit* edit1=(CEdit*) GetDlgItem(IDC_EDIT1);
	CEdit* edit2=(CEdit*) GetDlgItem(IDC_EDIT2);
	CEdit* edit3=(CEdit*) GetDlgItem(IDC_EDIT3);
	edit1->SetWindowText(s1);
	edit2->SetWindowText(s2);
	edit3->SetWindowText(s3);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDialog1::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	cvReleaseCapture(&recap);
	cvDestroyAllWindows();
	CDialogEx::OnCancel();
	exit(1);
}

void CDialog1::ReShow()
{
	recap = cvCreateFileCapture(filePath);
	CvPoint pd1,pd2;
	IplImage* frame;


	while (true) {
		if (start&&(currentframe<endframe)) 
		{
			cvSetCaptureProperty(recap,CV_CAP_PROP_POS_FRAMES,currentframe);
			frame=cvQueryFrame(recap);
			currentframe=cvGetCaptureProperty(recap, 1);
//			pScrollBar->SetScrollPos(framenum);
//			FrameNum=num2str(framenum);/////////////////////////////////////////////
			if(!frame)break;
			//ResizeImage(frame);
			ReShowImage(frame, IDC_RESHOW);
			myWaitKey_sub(33);
		}
		if ((!start)||currentframe>=endframe) break;
	}
	cvReleaseCapture(&recap);
}

void CDialog1::ReShowImage(IplImage* img , UINT ID)
{
	CvvImage cimg1;
	cimg1.CopyOf( img );
	pwnd1=GetDlgItem(ID);   //获取显示视频的控件ID
	pDC1=pwnd1->GetDC();
	hDC1=pDC1->GetSafeHdc();          //句柄
	pwnd1->GetClientRect(rect1);  //获取控件的RECT
	cimg1.DrawToHDC(hDC1,&rect1);	
}

void CDialog1::OnBnClickedButtonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	start=true;
	startframe=atoi(s1);
	currentframe=startframe;
	endframe=atoi(s2);
	ReShow();
	return;
}

void CDialog1::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMaxSize.x=800;
	lpMMI->ptMaxSize.y=600;
	lpMMI->ptMaxPosition.x=70;
	lpMMI->ptMaxPosition.y=70;
	CDialogEx::OnGetMinMaxInfo(lpMMI);

}


void CDialog1::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	start = false;
	
}


void CDialog1::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	cvReleaseCapture(&recap);
	//CDialogEx::OnCancel();
	//exit(1);
	CDialogEx::OnClose();
}
