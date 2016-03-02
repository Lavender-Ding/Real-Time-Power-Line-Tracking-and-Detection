
// videomfcDlg.cpp : implementation file
//

#include "opencv2/opencv.hpp"
#include "stdafx.h"
#include "videomfc.h"
#include "videomfcDlg.h"
#include "afxdialogex.h"
#include "Dialog1.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <fstream>
#include "wireline.h"
#include "alongTheLineDetector.h"
#include "dataManager.h"
#include "lineProcess.h"
#include "ROIprocess.h"
#include "Saliency_MC.h"
#include "SLIC.h"
#include <stdlib.h> 
#include "wrongnode.h"
#include "postProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IF_DETECT_INTRUDERS

using namespace cv;
using namespace std;

	
CTabCtrl m_tabctrl;
CListCtrl m_listbox1;
CDialog1 m_page1;
//CListCtrl 

//void showlist();
IplImage* checkcolor(IplImage* img, double t);
int breakline(IplImage* img, IplImage* org, wireline line);
int wronglocation(IplImage* img, IplImage* org, wireline *lines, int num);
CvPoint hllTest(bool& ifFault, IplImage*, lineProcess*, roiProcess*, vector<Vec4i>&, dataManager*, gradPostPro* postpro);

bool stopFlag = false;
bool playFlag = false;
bool pauseFlag = false;
bool aviFlag = false;
bool sangu = false;
bool duangu = false;
bool yiwu = false;
bool start_detect = false;
bool stop_detect = false;
int sg_count=0;
int dg_count=0;
int yw_count=0;
int isFrame=0;
CString s1,s2,s3;
int nSelect;
const int thre=5;
int fpnum=0;

//wrongnode *wronghead=NULL;
//wrongnode *q=new wrongnode();
int tmpwrong=0;
int sg_start=0;
int yw_start=0;
//wrongnode *p_tmp=NULL;

CScrollBar *pScrollBar;

int n=0;
int pic_all[]={1,30,60,90,120,140,160,200};
int pic_all_end[]={20,50,80,110,130,150,180,230};
int type[]={1,1,2,3,1,3,3,2};
int len;
int frames=100;
int framenum=5;
int frametotal;
string FrameNum;
int n_position = 1;  //position index

CString filePath=NULL;
CvCapture* capture = NULL;
wrongnode *head,*rear,*p;
IplImage* aviFrame = NULL;
CWnd *pwnd;
CRect rect;
CDC *pDC;
HDC hDC;

int sgf=0;

// CAboutDlg dialog used for App About


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, m_listbox1);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CvideomfcDlg dialog




CvideomfcDlg::CvideomfcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CvideomfcDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_ICO1);//////////////////////////////////////////////////////////
}

void CvideomfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CvideomfcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_BUTTON, &CvideomfcDlg::OnBnClickedOpenButton)
	ON_BN_CLICKED(IDC_PLAY_BUTTON, &CvideomfcDlg::OnBnClickedPlayButton)
	ON_BN_CLICKED(IDC_PAUSE_BUTTON, &CvideomfcDlg::OnBnClickedPauseButton)
	ON_BN_CLICKED(IDC_STOP_BUTTON, &CvideomfcDlg::OnBnClickedStopButton)
	ON_BN_CLICKED(IDCANCEL, &CvideomfcDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK1_SANGU, &CvideomfcDlg::OnBnClickedCheck1Sangu)
	ON_BN_CLICKED(IDC_CHECK2_DUANGU, &CvideomfcDlg::OnBnClickedCheck2Duangu)
	ON_BN_CLICKED(IDC_CHECK3_YIWU, &CvideomfcDlg::OnBnClickedCheck3Yiwu)
	ON_BN_CLICKED(IDC_BUTTON_STARTDETECTION, &CvideomfcDlg::OnBnClickedButtonStartdetection)
	ON_BN_CLICKED(IDC_BUTTONSTOPDETECTION, &CvideomfcDlg::OnBnClickedButtonstopdetection)
	ON_WM_HSCROLL()
	//ON_NOTIFY(NM_THEMECHANGED, IDC_SCROLLBAR1, &CvideomfcDlg::OnNMThemeChangedScrollbar1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CvideomfcDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CvideomfcDlg message handlers

BOOL CvideomfcDlg::OnInitDialog()
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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		// Set small icon
	pScrollBar=NULL;
	// TODO: Add extra initialization here

	IplImage *startbackground=cvLoadImage("SJTUbackground.jpeg",-1);
	ShowImage(startbackground, IDC_AVIPLAY_STATIC);

	m_listbox1.SubclassDlgItem(IDC_LIST1,this);
	//设置列表风格
	//设置列表视图的扩展风格
	
	//设置表头
	LONG styles;
       styles=GetWindowLong(m_listbox1.m_hWnd,GWL_STYLE);
       SetWindowLong(m_listbox1.m_hWnd,GWL_STYLE,styles | LVS_REPORT);
	m_listbox1.SetExtendedStyle(LVS_EX_FLATSB			//扁平风格显示滚动条
	|LVS_EX_FULLROWSELECT					//允许整行选中
	|LVS_EX_HEADERDRAGDROP				//允许整列拖动
	|LVS_EX_ONECLICKACTIVATE				//单击选中项
	|LVS_EX_GRIDLINES);						//画出网格线
	m_listbox1.InsertColumn(0,"起始帧",LVCFMT_LEFT,60,0); 	//设置起始帧
	m_listbox1.InsertColumn(1,"结束帧",LVCFMT_LEFT,60,1); 	//设置结束帧
	m_listbox1.InsertColumn(2,"错误类型",LVCFMT_LEFT,60,2); 	//设置错误类型
	m_listbox1.InsertColumn(3,"视频属性",LVCFMT_LEFT,112,2); 	//设置视频属性
	


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CvideomfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CvideomfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	/*else
	{
//		CDialogEx::OnPaint();
		CPaintDC dc(this);
		//dc.SetBkMode(TRANSPARENT);
		CBitmap m_bitmap;
		m_bitmap.LoadBitmap(IDB_BITMAP1);
		CDC memdc;
		memdc.CreateCompatibleDC(&dc);
		memdc.SelectObject(&m_bitmap);
		int width,height;
		BITMAP bmp;
		m_bitmap.GetBitmap(&bmp);
		width=bmp.bmWidth;
		height=bmp.bmHeight;
		CRect rect;
		this->GetClientRect(&rect);
		dc.StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&memdc,0,0,width,height,SRCCOPY);
	}*/
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CvideomfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CvideomfcDlg::OnBnClickedOpenButton()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilter[] = _T("视频文件(*.avi)|*.avi|所有文件(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("avi"), NULL, 0, szFilter, this);
	if (IDOK != fileDlg.DoModal()) return;

	if (aviFlag) { OnBnClickedStopButton(); DestroyAvi();}

	/*pwnd = GetDlgItem(IDC_AVIPLAY_STATIC);   //获取显示视频的控件ID
	pDC = pwnd->GetDC();
	hDC = pDC->GetSafeHdc();          //句柄
	pwnd->GetClientRect(rect);  //获取控件的RECT*/
	filePath = fileDlg.GetPathName();
    SetDlgItemText(IDC_OPEN_EDIT, filePath);
	capture = cvCreateFileCapture(filePath);
	IplImage* frame1;
	cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,5);
	frame1 = cvQueryFrame(capture);
	ShowImage(frame1, IDC_AVIPLAY_STATIC);
	frametotal=(int)cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
	pScrollBar=(CScrollBar*)GetDlgItem(IDC_SCROLLBAR1);
	//设置滚动条的滚动范围和当前位置
	pScrollBar->SetScrollRange(0,frametotal);
	pScrollBar->SetScrollPos(0);
	/*CvSize ImgSize;
	ImgSize.height = rect.Height();
	ImgSize.width = rect.Width();
	aviFrame = cvCreateImage(ImgSize,IPL_DEPTH_8U, 3);
	cvZero(aviFrame);*/
	//ShowImage(aviFrame, IDC_AVIPLAY_STATIC);
	//ShowAvi ();
}

void CvideomfcDlg::DestroyAvi()
{
	aviFlag = false;
	playFlag = false; stopFlag = false; pauseFlag = false;
}

int myWaitKey( int delay )
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

string num2str(int num)
{
	stringstream ss;
	ss<<num;
	return ss.str();
}

void CvideomfcDlg::ShowAvi()
{
	capture = cvCreateFileCapture(filePath);
	aviFlag = true;
	playFlag = true; stopFlag = false; pauseFlag = false;
	CvPoint pd1,pd2;
	IplImage* frame;
	frame = cvQueryFrame(capture);
	IplImage* frameshow = cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U,3);
	double **theta;
	wireline wires[200];
	

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 2, CV_AA);
	lineProcess* linePro = new lineProcess();
	vector<Vec4i> lineVec;

	roiProcess* roiPro = new roiProcess();
	gradPostPro* postPro = new gradPostPro();

	alongTheLineProcess* alongDetector = new alongTheLineProcess();
	//namedWindow("ROI", 0);

	dataManager* data = dataManager::getInstance();
	data->d_frameSize = Size(frame->width, frame->height);
	data->d_frameCounter = 0;
	
	IplImage *dst = cvCreateImage(cvGetSize(frame), 8, 1);
	IplImage *color_dst = cvCreateImage(cvGetSize(frame), 8, 3);
	IplImage *check = cvCreateImage(cvGetSize(frame), 8, 3);

	head=rear=new wrongnode();
	while (true) {
		isFrame=0;
		if (playFlag) {
			cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,framenum);
			frame = cvQueryFrame(capture);
			if(!frame)
			{
				isFrame=1;
				if (sg_count>=thre)
				{
					p=new wrongnode();
					p->type=1;
					p->start=sg_start;
					p->end=framenum-1;
					rear->next=p;
					rear=p;
				}
				sg_count=0;
				if (yw_count>=thre)
				{
					p=new wrongnode();
					p->type=3;
					p->start=yw_start;
					p->end=framenum-1;
					rear->next=p;
					rear=p;
				}
				yw_count=0;
				m_listbox1.SubclassDlgItem(IDC_LIST1,this);
	m_listbox1.DeleteAllItems();
	LPCTSTR lp;
	CString str;
	wrongnode *tmp=new wrongnode();
	tmp=head->next;
	int row=0;
	while (tmp!=NULL)
	{
		str.Format("%d",tmp->start);
		lp=str;
		m_listbox1.InsertItem(row,_T(lp));
		str.Format("%d",tmp->end);
		lp=str;
		m_listbox1.SetItemText(row,1,_T(lp));
		if (tmp->type==1) m_listbox1.SetItemText(row,2,"散股");
		if (tmp->type==2) m_listbox1.SetItemText(row,2,"断股");
		if (tmp->type==3) m_listbox1.SetItemText(row,2,"异物");
		m_listbox1.SetItemText(row,3,"");
		tmp=tmp->next;
		row++;
	}
				stop_detect=true;
				break;
			}
			cvCopy(frame,frameshow,0);
			framenum=cvGetCaptureProperty(capture, 1);
			pScrollBar->SetScrollPos(framenum);
			FrameNum=num2str(framenum);
			
			tmpwrong=0;

			//cvCopy(checkcolor(img, 0.99), check);

			cvCanny(frame, dst, 80, 150, 3);

			cvCvtColor(dst, color_dst, CV_GRAY2BGR);

			//cvNamedWindow("Hough");
			//cvShowImage("Hough", color_dst);

			//cvWaitKey(0);

			int linenum = 0;
			CvMemStorage *storage = cvCreateMemStorage();
			CvSeq *lines = 0;

			lines = cvHoughLines2(dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 100, 50, 5);

			for (int i = 0; i < lines->total; i++)
			{
				CvPoint *line = (CvPoint *)cvGetSeqElem(lines, i);
				if (line[0].x - line[1].x == 0)
					continue;
				double tmpa = double(line[0].y - line[1].y) / (line[0].x - line[1].x);
				double tmpb = double(line[0].y - tmpa*line[0].x);
				bool flag = 1;
				if (linenum == 0)
				{
					wires[linenum].a = tmpa;
					wires[linenum].b = tmpb;
					wires[linenum].pt1[0] = line[0];
					wires[linenum].pt2[0] = line[1];
					wires[linenum].num++;
					linenum = 1;
				}
				else
				{
					for (int k = 0; k < linenum; k++)
					{
						if (abs(tmpa - wires[k].a) < 0.3 && abs(double(line[0].y) - double(line[0].x)*wires[k].a - wires[k].b) / sqrt(wires[k].a*wires[k].a + 1) < 5)
						{
							wires[k].pt1[wires[k].num] = line[0];
							wires[k].pt2[wires[k].num] = line[1];
							wires[k].num++;
							flag = 0;
							break;
						}
					}
				}
				if (flag)
				{
					wires[linenum].a = tmpa;
					wires[linenum].b = tmpb;
					wires[linenum].pt1[0] = line[0];
					wires[linenum].pt2[0] = line[1];
					wires[linenum].num++;
					linenum++;
				}
			}

			for (int i = 0; i < linenum; i++)
				wires[i].merge();
			if (duangu&&start_detect)
			{
				pd1.x=100; pd1.y=100;
				pd2.x=200; pd2.y=200;
				cvRectangle(frameshow,pd1,pd2,cvScalar(0,0,255),1,8,0);
			}
    		if (sangu&&start_detect)//////////////////////////////////////////
			{
				fpnum++;
				if (sgf==0)
				{
				wireline wrong[20];
				int numofline=0;
				for (int i = 0; i < linenum; i++)
				{
					int G = rand() % 255;
					int B = rand() % 255;
					int R = rand() % 255;

					double length;
					double a=wires[i].pt1[0].x - wires[i].pt2[0].x;
					double b=wires[i].pt1[0].y - wires[i].pt2[0].y;

					length = sqrt(double(a*a+b*b));

					if (length > frame->height / 1.6)
					{
						if ((framenum<134||framenum>505)&&(b/a<3||b/a>8)) continue;
						//if (wires[i].pt1[0].x<3&&wires[i].pt1[0].y<3) continue;

							cvLine(frameshow, wires[i].pt1[0], wires[i].pt2[0], CV_RGB(255, 0, 0), 1, CV_AA);
							if (framenum<400) 
						{
							//tmpwrong=breakline(color_dst, frameshow, wires[i]);
							if (breakline(color_dst, frameshow, wires[i])==1)  tmpwrong=1;
						}
						else 
						{
							wrong[numofline]=wires[i];
							numofline++;
						}
					}
					if (framenum>400&&framenum<500)
					{
						//int tmpwrong=wronglocation(color_dst, frameshow, wrong, numofline);
						if (wronglocation(color_dst, frameshow, wrong, numofline)==1) tmpwrong=1;
					}
					wires[i].num = 0;
				}
				if (tmpwrong==1) sg_count++;
				if (tmpwrong==0)
				{
					if (sg_count>=thre && framenum>sg_start)
					{
						p=new wrongnode();
						p->type=1;
						p->start=sg_start;
						p->end=framenum;
						rear->next=p;
						rear=p;
					}
					sg_count=0;
					m_listbox1.SubclassDlgItem(IDC_LIST1,this);
	m_listbox1.DeleteAllItems();
	LPCTSTR lp;
	CString str;
	wrongnode *tmp=new wrongnode();
	tmp=head->next;
	int row=0;
	while (tmp!=NULL)
	{
		str.Format("%d",tmp->start);
		lp=str;
		m_listbox1.InsertItem(row,_T(lp));
		str.Format("%d",tmp->end);
		lp=str;
		m_listbox1.SetItemText(row,1,_T(lp));
		if (tmp->type==1) m_listbox1.SetItemText(row,2,"散股");
		if (tmp->type==2) m_listbox1.SetItemText(row,2,"断股");
		if (tmp->type==3) m_listbox1.SetItemText(row,2,"异物");
		m_listbox1.SetItemText(row,3,"");
		tmp=tmp->next;
		row++;
	}
				}
				if (sg_count==1) sg_start=framenum;
			}
			if (sgf==1) 
				{
					sgf=0;
				}
			}
			if (yiwu&&start_detect)
			{
				int index = 0;
		for (int i = 0; i < linenum; i++)
		{
			int G = rand() % 255;
			int B = rand() % 255;
			int R = rand() % 255;

			double length;
			//here can be optimized
			length = (wires[i].pt1[0].x - wires[i].pt2[0].x)*(wires[i].pt1[0].x - wires[i].pt2[0].x) +
				(wires[i].pt1[0].y - wires[i].pt2[0].y)*(wires[i].pt1[0].y - wires[i].pt2[0].y);

			//modified
			if (length > pow(double(frame->height / 3), double(2)))
			{
				index ++;
				lineVec.push_back(Vec4i(wires[i].pt1[0].x,wires[i].pt1[0].y,wires[i].pt2[0].x,wires[i].pt2[0].y) );
				#ifndef IF_DETECT_INTRUDERS
						cvLine(frame, wires[i].pt1[0], wires[i].pt2[0], CV_RGB(255,0,0), 2, CV_AA);
				#endif // !IF_DETECT_INTRUDERS

			}
			wires[i].num = 0;
		}

		#ifndef IF_DETECT_INTRUDERS		
						//cvShowImage("video", frame);
		#endif // !IF_DETECT_INTRUDERS

		#ifdef IF_DETECT_INTRUDERS

		//主处理函数
		bool faultFrame;  //返回的布尔值
		CvPoint _i = hllTest( faultFrame,frame, linePro, roiPro, lineVec, data, postPro);

		if (faultFrame==true) yw_count++;
		if (faultFrame==false)
		{
			if (yw_count>=thre && framenum>yw_start)
			{
				p=new wrongnode();
				p->type=3;
				p->start=yw_start;
				p->end=framenum;
				rear->next=p;
				rear=p;
			}
			yw_count=0;
			m_listbox1.SubclassDlgItem(IDC_LIST1,this);
	m_listbox1.DeleteAllItems();
	LPCTSTR lp;
	CString str;
	wrongnode *tmp=new wrongnode();
	tmp=head->next;
	int row=0;
	while (tmp!=NULL)
	{
		str.Format("%d",tmp->start);
		lp=str;
		m_listbox1.InsertItem(row,_T(lp));
		str.Format("%d",tmp->end);
		lp=str;
		m_listbox1.SetItemText(row,1,_T(lp));
		if (tmp->type==1) m_listbox1.SetItemText(row,2,"散股");
		if (tmp->type==2) m_listbox1.SetItemText(row,2,"断股");
		if (tmp->type==3) m_listbox1.SetItemText(row,2,"异物");
		m_listbox1.SetItemText(row,3,"");
		tmp=tmp->next;
		row++;
	}
		}
		if (yw_count==1) yw_start=framenum;

		if(_i.x + _i.y != 0 && yw_count>=2)
			cvCircle(frameshow, _i, (frameshow->height)/20 , cvScalar(255,0,0),3);

		#endif // IF_DETECT_INTRUDERS
		//lineVec.clear();
		}
			//ResizeImage(frame);
			ShowImage(frameshow, IDC_AVIPLAY_STATIC);
			lineVec.clear();
			myWaitKey(1);
		}
		if (stopFlag||pauseFlag) break;
	}
	
	//ShowImage(aviFrame, IDC_AVIPLAY_STATIC);
	//playFlag = false; stopFlag = true; pauseFlag = false;
	cvReleaseCapture(&capture);
}

CvPoint hllTest(bool& ifFault,IplImage* img, lineProcess* linePro, roiProcess* roiPro, vector<Vec4i>& lineVec, dataManager* data, gradPostPro* postpro)
{
///////////////////////////////algorithm part here////////////////////////////////////////
	ifFault = false; //init
	Mat imgMat(img, true);
	
	//alongDetector->detectPro(imgMat, lineVec, img->width / 8, 1);


	linePro->linesClusteringPro(lineVec, img->width / 9);

	int hullSize = linePro->m_convexHulls.size();

	
	Mat maskROI;
	Mat roiExed;
	for(int i = 0; i < hullSize; i ++)
	{
		//only a single line
		if(linePro->m_convexHulls[i].size() <= 2 || ( roiPro->abandonContour(linePro->m_convexHulls[i]) ) )
		{
			//cout << "skip the area" << endl;
			continue;
		}
		roiPro->getROImatPro(imgMat,linePro->m_convexHulls[i],linePro->m_lineSlope[i]);
		roiPro->perspectiveProg(roiExed);
		//linePro->compensateArea(roiPro->m_inputImg, linePro->m_paralleledLines[i]);
		//debugShow(imgMat,0);
		//roiPro->perspectiveProg(roiExed);
		maskROI = SaliencyPro(roiExed, 0.2);

		if (maskROI.empty())
			continue;

		Point tempPtForPostPro = roiPro->findEstInvaPts(maskROI);
		//debugShow(roiExed);
		//debugShow(maskROI, 1);
		//postpro->gradPostProcessing(imgMat, tmpFoundPoint, linePro->m_lineSlope[i]);
	}

	if(data->d_invasionPt.empty()) {data->d_lastFrameHasPt = false; cout << "man what up" << endl;}
	else {data->d_lastFrameHasPt = true;}

	//for(int pti = 0; pti < data->d_invasionPt.size(); ++pti)
		//circle(imgMat, data->d_invasionPt.at(pti), 50, Scalar(255,0,0),4);
	CvPoint pt;
	int size = data->d_invasionPt.size();
	pt = size > 0 ? data->d_invasionPt[0] : cvPoint(0, 0);

	//std::cout << endTime << std::endl;
	data->d_frameCounter ++;
	data->d_preInvasionPt = data->d_invasionPt;
	data->d_invasionPt.clear();
	data->d_maskAreaCount.clear();
	data->d_maxSalEver = 0;
	//data->d_maskAreaCount.clear();
	//data->d_pairs.clear();
	

	//imshow("hehe",imgMat);
	//waitKey(0);
	
	//std::cout << endTime << std::endl;
	//select a best one, remember!!
	for(int pti = 0; pti < data->d_preInvasionPt.size(); ++pti)
	{
		ifFault = true;
		//returnCvpt.push_back(cvPoint((int)data->d_preInvasionPt[pti].x, (int)data->d_preInvasionPt[pti].y) );
	}
	

	return pt;
}

IplImage* checkcolor(IplImage* img, double t)
{
	IplImage *check = cvCreateImage(cvGetSize(img), 8, 3);

	cvCopy(img, check);
	uchar* data = (uchar *)check->imageData;

	int step = check->widthStep / sizeof(uchar);     //step即为上图的widthstep
	int channels = check->nChannels;             //这个图片为3通道的
	int height = check->height;
	int width = check->width;

	double b, g, r, modular;
	double theta;

	for (int i = 0; i < height; i++)
	for (int j = 0; j<width; j++)
	{
		b = data[i*step + j*channels + 0];    //此时可以通过更改bgr的值达到访问效果。
		g = data[i*step + j*channels + 1];
		r = data[i*step + j*channels + 2];

		modular = sqrt(r *r + b *b + g *g);
		modular = modular *sqrt(double(3));

		theta = (b + g + r) / modular;
		if (theta>t && modular / 3 > 150)
		{
			data[i*step + j*channels + 0] = 255;
			data[i*step + j*channels + 1] = 255;
			data[i*step + j*channels + 2] = 255;
		}
		else
		{
			data[i*step + j*channels + 0] = 0;
			data[i*step + j*channels + 1] = 0;
			data[i*step + j*channels + 2] = 0;
		}
	}
	return check;
}

int breakline(IplImage* img, IplImage* org, wireline line)
{
	uchar* data = (uchar *)img->imageData;
	uchar* dataorg = (uchar *)org->imageData;
	int pixelnum=0;

	int step = img->widthStep / sizeof(uchar);     //step即为上图的widthstep
	int channels = img->nChannels;             //这个图片为3通道的
	int height = img->height;
	int width = img->width;
	bool flag = 0;
	double avgcount = 0;
	bool wrong = 0;
	int wrongflag=0;

	if (line.a > 1)
	{
		for (int i = line.pt1[0].y; i < line.pt2[0].y - 6; i = i + 6)
		{
			int j;
			j = double(i - line.b) / line.a;
			if (i + 11>height) continue;
			if (j - 5 < 1) continue;
			if (j + 6 > width) continue;
			int count = 0;
			for (int w = j - 5; w < j + 6; w++)
			{
				//pixelnum++;
				for (int h = i; h < i + 11; h++)
				if (data[h*step + w*channels + 0])
				{
					double r =dataorg[h*step + w*channels + 0];
					double g=dataorg[h*step + w*channels + 1];
					double b=dataorg[h*step + w*channels + 2];

					double modular = sqrt(r *r + b *b + g *g);
					modular = modular *sqrt(double(3));

					double theta = (b + g + r) / modular;

					if (theta>0.996&&modular/3>150)
						count++;
					else
						continue;
				}
			}
			if (avgcount == 0) avgcount = count;
			else if (abs(avgcount - count)>15) wrong = 1;
			else wrong = 0;
			avgcount = count;
			if (wrong)
			{
				wrongflag=1;
				if (flag == 0)
				{
					cvLine(org, cvPoint(j - 5, i), cvPoint(j + 5, i), CV_RGB(0, 255, 0), 1, CV_AA);
					cvLine(org, cvPoint(j - 5, i), cvPoint(j - 5, i + 10), CV_RGB(0, 255, 0), 1, CV_AA);
					cvLine(org, cvPoint(j + 5, i), cvPoint(j + 5, i + 10), CV_RGB(0, 255, 0), 1, CV_AA);
					flag = 1;
				}
				else
				{
					cvLine(org, cvPoint(j - 5, i), cvPoint(j - 5, i + 10), CV_RGB(0, 255, 0), 1, CV_AA);
					cvLine(org, cvPoint(j + 5, i), cvPoint(j + 5, i + 10), CV_RGB(0, 255, 0), 1, CV_AA);
				}
			}
			else
			{
				if (flag)
				{
					cvLine(org, cvPoint(j - 5, i), cvPoint(j + 5, i), CV_RGB(0, 255, 0), 1, CV_AA);
					flag = 0;
				}
			}
		}
	}
	else
	{
		for (int i = line.pt1[0].x; i < line.pt2[0].x - 5; i = i + 4)
		{
			int j;
			
			int count = 0;
			for (int w = i; w < i + 5; w++)
			{
				j = w*line.a + line.b;
				if (j + 6 > height) continue;
				if (j - 5< 1) continue;
				for (int h = j - 5; h < j + 6; h++)
				{
					//pixelnum++;
					if (data[h*step + w*channels + 0])
				{
					double r =dataorg[h*step + w*channels + 0];
					double g=dataorg[h*step + w*channels + 1];
					double b=dataorg[h*step + w*channels + 2];

					double modular = sqrt(r *r + b *b + g *g);
					modular = modular *sqrt(double(3));

					double theta = (b + g + r) / modular;

					//if (theta<0.996&&modular/3>100)
						//continue;
					//else
						count++;
				}
				}
			}
			int k=0;
			for (k=0;k<10;k++) avgcount=avgcount+count;
			avgcount=avgcount/10;
			if (abs(avgcount - count)>2&&k==10) wrong = 1;
			//else if (count>15) wrong = 1;
			else wrong = 0;
			if (wrong)
			{
				wrongflag=1;
				if (flag == 0)
				{
					//std::cout<<"wrong point:"<<i<<' '<<j<<std::endl;
					cvLine(org, cvPoint(i, j - 5), cvPoint(i, j + 5), CV_RGB(0, 255, 0), 1, CV_AA);
					cvLine(org, cvPoint(i, j - 5), cvPoint(i + 4, j - 5), CV_RGB(0, 255, 0), 1, CV_AA);
					cvLine(org, cvPoint(i, j + 5), cvPoint(i + 4, j + 5), CV_RGB(0, 255, 0), 1, CV_AA);
				}
				else
				{
					cvLine(org, cvPoint(i, j - 5), cvPoint(i + 4, j - 5), CV_RGB(0, 255, 0), 1, CV_AA);
					cvLine(org, cvPoint(i, j + 5), cvPoint(i + 4, j + 5), CV_RGB(0, 255, 0), 1, CV_AA);
				}
				flag = 1;
			}
			else
			{
				if (flag)
				{
					cvLine(org, cvPoint(i, j - 5), cvPoint(i, j + 5), CV_RGB(0, 255, 0), 1, CV_AA);
					flag = 0;
				}
			}
		}
	}
	//std::cout<<pixelnum<<' ';
	return wrongflag;
}

int wronglocation(IplImage* img, IplImage* org, wireline *lines, int num)
{
	uchar* data = (uchar *)img->imageData;

	int step = img->widthStep / sizeof(uchar);     //step即为上图的widthstep
	int channels = img->nChannels;             //这个图片为3通道的
	int height = img->height;
	int width = img->width;
	int wrongflag = 0;

	while (1)
	{	
		bool flag = 0;
		for (int i = 0; i < num-1; i++)
		{
			if (lines[i].b>lines[i + 1].b)
			{
				wireline tmp;
				tmp = lines[i];
				lines[i] = lines[i + 1];
				lines[i + 1] = tmp;
				flag = 1;
			}
		}
		if (flag == 0) break;
	}

	for (int i = 0; i < num - 1; i++)
	{
		double distance;
		distance = (lines[i + 1].b - lines[i].b) / sqrt(lines[i].a*lines[i].a+1);
		if (distance < 50 && abs(lines[i + 1].a - lines[i].a) < 0.3)
		{
			bool flag = 0;
			for (int x = 0; x < width - 10; x = x + 10)
			{
				int count=0;
				int up = x*lines[i].a + lines[i].b;
				int low = x*lines[i + 1].a + lines[i + 1].b;
				if (up < 11) continue;
				if (low>height-11) continue;
				for (int w = x; w < x + 10; w++)
				{
					for (int h = up - 10; h < low + 11; h++)
					{
						if (data[h*step + w*channels + 0])
							count++;
					}
				}
				if (count>60)
				{
					if (flag == 0)
					{
						cvLine(org, cvPoint(x, up - 10), cvPoint(x, low + 10), CV_RGB(0, 255, 0), 2, CV_AA);
						cvLine(org, cvPoint(x, up - 10), cvPoint(x + 10, up - 10), CV_RGB(0, 255, 0), 2, CV_AA);
						cvLine(org, cvPoint(x, low + 10), cvPoint(x + 10, low + 10), CV_RGB(0, 255, 0), 2, CV_AA);
					}
					else
					{
						cvLine(org, cvPoint(x, up - 10), cvPoint(x + 10, up - 10), CV_RGB(0, 255, 0), 2, CV_AA);
						cvLine(org, cvPoint(x, low + 10), cvPoint(x + 10, low + 10), CV_RGB(0, 255, 0), 2, CV_AA);
					}
					flag = 1;
					wrongflag=1;
				}
				else
				{
					if (flag)
					{
						cvLine(org, cvPoint(x, up - 10), cvPoint(x, low + 10), CV_RGB(0, 255, 0), 2, CV_AA);
						flag = 0;
					}
				}
			}
		}
	}
	return wrongflag;
}


void CvideomfcDlg::OnBnClickedPlayButton()
{
	// TODO: Add your control notification handler code here
	ShowAvi();
}


void CvideomfcDlg::OnBnClickedPauseButton()
{
	// TODO: Add your control notification handler code here
	if (playFlag) {
		playFlag = false; pauseFlag = true; stopFlag = false;
	}
}


void CvideomfcDlg::OnBnClickedStopButton()
{
	// TODO: Add your control notification handler code here
	if(playFlag || pauseFlag) {
		stopFlag = true; playFlag = false; pauseFlag = false;
		framenum=1;
	}
}

void CvideomfcDlg::ShowImage(IplImage* img , UINT ID)
{
	CvvImage cimg;
	cimg.CopyOf( img );
	pwnd = GetDlgItem(ID);   //获取显示视频的控件ID
	pDC = pwnd->GetDC();
	hDC = pDC->GetSafeHdc();          //句柄
	pwnd->GetClientRect(rect);  //获取控件的RECT
	cimg.DrawToHDC( hDC, &rect );	
}

void CvideomfcDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if (aviFlag) { OnBnClickedStopButton(); DestroyAvi();}
	cvDestroyAllWindows();
	CDialogEx::OnCancel();
	exit(1);
}

void CvideomfcDlg::OnBnClickedCheck1Sangu()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!sangu) {sangu = true;sgf=1;}
	else sangu = false;
}


void CvideomfcDlg::OnBnClickedCheck2Duangu()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!duangu) duangu = true;
	else duangu = false;
}


void CvideomfcDlg::OnBnClickedCheck3Yiwu()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!yiwu) yiwu = true;
	else yiwu = false;
}

void CvideomfcDlg::OnBnClickedButtonStartdetection()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!start_detect)
	{
		start_detect=true;
		stop_detect=false;
		sgf=1;
		fpnum=0;
	}
}


void CvideomfcDlg::OnBnClickedButtonstopdetection()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!stop_detect)
	{
		stop_detect=true;
		start_detect=false;
	}
	if (stop_detect)
	{
		if (sg_count>=thre)
		{
			p=new wrongnode();
			p->type=1;
			p->start=sg_start;
			p->end=framenum;
			rear->next=p;
			rear=p;
		}
		sg_count=0;
		if (yw_count>=thre)
		{
			p=new wrongnode();
			p->type=3;
			p->start=yw_start;
			p->end=framenum;
			rear->next=p;
			rear=p;
		}
		yw_count=0;
		
		m_listbox1.SubclassDlgItem(IDC_LIST1,this);
		m_listbox1.DeleteAllItems();
		LPCTSTR lp;
		CString str;
		wrongnode *tmp=new wrongnode();
		tmp=head->next;
		int row=0;
		while (tmp!=NULL)
		{
			str.Format("%d",tmp->start);
			lp=str;
			m_listbox1.InsertItem(row,_T(lp));
			str.Format("%d",tmp->end);
			lp=str;
			m_listbox1.SetItemText(row,1,_T(lp));
			if (tmp->type==1) m_listbox1.SetItemText(row,2,"散股");
			if (tmp->type==2) m_listbox1.SetItemText(row,2,"断股");
			if (tmp->type==3) m_listbox1.SetItemText(row,2,"异物");
			m_listbox1.SetItemText(row,3,"");
			tmp=tmp->next;
			row++;
		}

		/*len=sizeof(pic_all) / sizeof(pic_all[0]);
		for (int i=0;i<len-1;i++)
		{
			str.Format("%d",pic_all[i]);
			lp = str;
			m_listbox1.InsertItem(i,_T(lp));
			str.Format("%d",pic_all_end[i]);
			lp = str;
			m_listbox1.SetItemText(i,1,_T(lp));
			if (type[i]==1)
			{
				m_listbox1.SetItemText(i,2,"散股");
			}
			else if (type[i]==2)
				m_listbox1.SetItemText(i,2,"断股");
			else
				m_listbox1.SetItemText(i,2,"异物");
			m_listbox1.SetItemText(i,3,"12/8 上海交通大学");
		}*/
	}
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	/*switch(pWnd->GetDlgCtrlID())
    {
    default:
        pDC->SetBkMode(TRANSPARENT);
        return (HBRUSH)GetStockObject(NULL_BRUSH);
        break;
    }

	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor==CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
	}*/
	CFont m_font;   //声明变量
	m_font.CreatePointFont(600,"华文行楷"); //设置字体大小和类型
	if(pWnd->GetDlgCtrlID()==CTLCOLOR_STATIC)//可以用CTLCOLOR_STATIC表示静态控件
	{
	   pDC->SelectObject(&m_font);       //设置字体 
	   pDC->SetTextColor(RGB(0,0,255)); //设置字体颜色
	   pDC->SetBkMode(TRANSPARENT);      //属性设置为透明
	   return (HBRUSH)::GetStockObject(NULL_BRUSH); //不返回画刷
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CvideomfcDlg::OnHScroll(UINT pSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	int nCurPos=pScrollBar->GetScrollPos();
	switch(pSBCode)
	{
		//点击右侧小箭头
	case SB_LINEDOWN:
		nCurPos+=1;
		break;
		//点击左侧小箭头
	case SB_LINEUP:
		nCurPos-=1;
		break;
		//单击滚动条
	case SB_PAGEDOWN:
		nCurPos+=10;
		break;
		//单击滚动条
	case SB_PAGEUP:
		nCurPos-=10;
		break;
		//拖动滚动条
	case SB_THUMBTRACK:
		nCurPos=nPos;
		break;
	default:
		break;   
	}
	pScrollBar->SetScrollPos(nCurPos);//	pScrollBar->SetScrollPos(nCurPos);//

	//SetDlgItemInt(IDC_EDIT1,nCurPos,TRUE);
	framenum=nCurPos;
	//cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES,framenum);

	CDialog::OnHScroll(pSBCode, nPos, pScrollBar);
}

void CvideomfcDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	// TODO: Add your control notification handler code here

    POSITION Pos = m_listbox1.GetFirstSelectedItemPosition();
	nSelect = -1;
	if (Pos == NULL) 
           TRACE0("No items were selected!\n"); 

	while (Pos)
	{
		nSelect=m_listbox1.GetNextSelectedItem(Pos);    //nSelect能获得第几行
		TRACE1("Item %d was selected!\n", nSelect); 
		if (nSelect>=0)
		{
			
			s1=_T(m_listbox1.GetItemText(nSelect,0));
			s2=_T(m_listbox1.GetItemText(nSelect,1));
			s3=_T(m_listbox1.GetItemText(nSelect,2));

			m_page1.SubclassDlgItem(IDD_DIALOG1,this);
			m_page1.DoModal();
			m_page1.ShowWindow(SW_SHOW);
		}
	}
	
	//char szFirstTrunk[9]={0};
	//m_listbox1.GetItemText(nSelect, 5, szFirstTrunk, 9);  //获得想要的列值

	*pResult = 0;
}
