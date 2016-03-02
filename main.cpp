#include "stdafx.h"
#include "wireline.h"
#include "lineProcess.h"
#include "ROIprocess.h"
#include "alongTheLineDetector.h"
#include "Saliency_MC.h"
#include "dataManager.h"
#include "postProcess.h"
#include "Functions.h"

#define IF_DETECT_INTRUDERS

//
int detectionCounter = 0;

void hllTest(IplImage*, lineProcess*, roiProcess*, gradPostPro*, vector<Vec4i>&, dataManager*);
void tupleTest();//if not used, comment it up


int main(int agrc, char**argv)
{

	//hllTest();
	//tupleTest();
	//return 3600;

	size_t startTime = clock();
	size_t endTime;

	srand((int)time(0));
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 2, CV_AA);

	CvCapture* pCapture = NULL;
	cvNamedWindow("video", 1);

	//kiteInvasion_02.mp4
	//branchInvasion_03.mp4
	//origi.mp4
	//F:/FFOutput/branch1.avi
	if (!(pCapture = cvCaptureFromFile("F:/FFOutput/branch1.mp4")))
	{
		fprintf(stderr, "Can not open video file %s\n", "test.wmv");
		return -250;
	}
	
	IplImage *imgs = NULL;
	imgs = cvQueryFrame(pCapture);


	wireline wires[200];

	int frameCount = 0;


	IplImage *dst = cvCreateImage(cvSize(1800, 1000), 8, 1);
	IplImage *color_dst = cvCreateImage(cvSize(1800, 1000), 8, 3);
	IplImage *check = cvCreateImage(cvSize(1800, 1000), 8, 3);
	IplImage* img = cvCreateImage(cvSize(1800, 1000), imgs->depth, imgs->nChannels);
	CvMemStorage *storage = cvCreateMemStorage();
	CvSeq *lines = 0;
	
	////////////////////////////////////////////////////////////////////	
	//init the detector
	lineProcess* linePro = new lineProcess();
	vector<Vec4i> lineVec;

	roiProcess* roiPro = new roiProcess();

	gradPostPro* gradPro = new gradPostPro();

	namedWindow("ROI", 0);

	dataManager* data = dataManager::getInstance();
	data->init();
	data->d_frameSize = Size(img->width, img->height);
	data->d_frameCounter = 0;
	///////////////////////////////////////////////////////////////////
	

	while (imgs = cvQueryFrame(pCapture))

	{
		
		cvResize(imgs, img);
		
		//cvReleaseImage(&imgs);

		frameCount ++;
		if(frameCount <= 3)
			continue;
		else if(frameCount > 800)
		{
			break;
		}

		IplImage* tmpCheck = checkcolor(img);

		//
		

		cvCopy(tmpCheck, check);

		


		cvReleaseImage(&tmpCheck);

		cvCanny(check, dst, 80, 150, 3);

		cvCvtColor(dst, color_dst, CV_GRAY2BGR);

		int linenum = 0;

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
					if (abs(tmpa - wires[k].a) < 0.3 && abs(double(line[0].y) - double(line[0].x)*wires[k].a - wires[k].b) / sqrt(wires[k].a*wires[k].a + 1) < 10)
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
			if (length > img->height * img->height/ 9)
			{
				index++;
				lineVec.push_back(Vec4i(wires[i].pt1[0].x,wires[i].pt1[0].y,wires[i].pt2[0].x,wires[i].pt2[0].y) );
#ifndef IF_DETECT_INTRUDERS
				cvLine(img, wires[i].pt1[0], wires[i].pt2[0], CV_RGB(255,0,0), 2, CV_AA);
#endif // !IF_DETECT_INTRUDERS

			}
			wires[i].num = 0;
	
		}

#ifndef IF_DETECT_INTRUDERS		
		cvShowImage("video", img);
		if (cvWaitKey(2) >= 0)
			break;
#endif // !IF_DETECT_INTRUDERS

#ifdef IF_DETECT_INTRUDERS

		//主处理函数
		
		hllTest(img, linePro, roiPro, gradPro, lineVec, data);
		endTime = clock() - startTime;
		cout<<"frame no. "<< data->d_frameCounter << "time is: "<< endTime <<endl;

#endif // IF_DETECT_INTRUDERS
		lineVec.clear();
	}

	//
	cout << detectionCounter << endl;
	//memory release
	cvReleaseImage(&dst);
	cvReleaseImage(&color_dst);
	cvReleaseImage(&check);
	


	cvDestroyWindow("video");
	cvReleaseCapture(&pCapture);
	return 0;
}

//test code here
void hllTest(IplImage* img, lineProcess* linePro, roiProcess* roiPro, gradPostPro* gradPro, vector<Vec4i>& lineVec, dataManager* data)
{
///////////////////////////////algorithm part here////////////////////////////////////////

	Mat imgMat(img, true);
	//debugShow(imgMat,0);

	//alongDetector->detectPro(imgMat, lineVec, img->width / 8, 1);

	linePro->linesClusteringPro(lineVec, imgMat.cols / 8);

	int hullSize = linePro->m_convexHulls.size();
	//myDrawMultiContours(imgMat, linePro->m_convexHulls,Scalar(255,0,0));
	//drawLines(imgMat,linePro->m_paralleledLines);
	//debugShow(imgMat, 0);
	Mat maskROI;
	Mat roiExed;
	for(int i = 0; i < hullSize; i ++){
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
		//debugShow(maskROI, 0);

		//test our module here
		//gradPro->gradPostProcessing(imgMat, tempPtForPostPro, linePro->m_lineSlope[i]);

		//


	}

	//debugShow(roiPro->m_inputImg, 0);
	//gradPro->setGoodPoint();
	//roiPro->getFinalPos();


	if(data->d_invasionPt.empty()) {data->d_lastFrameHasPt = false; cout << "man what up" << endl;}
	else {data->d_lastFrameHasPt = true;}

	for(int pti = 0; pti < data->d_invasionPt.size(); ++pti)
		circle(imgMat, data->d_invasionPt.at(pti), 50, Scalar(255,0,0),4);

	

	debugShow(imgMat,1);
	
	//std::cout << endTime << std::endl;
	data->d_frameCounter++;
	data->d_preInvasionPt = data->d_invasionPt;
	data->d_invasionPt.clear();
	data->d_maskAreaCount.clear();
	data->d_pairs.clear();
	data->d_maxSalEver = 0;
	//cout << "the increment is " << (int)(data->d_increment[0]) << endl;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//void tupleTest()
//{
//	int start = clock();
//	Mat testmat = imread("C:/Users/Administrator/Desktop/test.jpg");
//	Mat mask = SaliencyPro(testmat);
//	debugShow(mask,1);
//	int end = clock() - start;
//}