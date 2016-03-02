#include "stdafx.h"
#include "Saliency_MC.h"

//#include "Functions.h"

extern double thresholdForSal;

void traverseMatOneByOne(const Mat& src)
{

}


int incre = 0;

bool drawHistS(const Mat& src, double maxEle, double& mount, double& maxSalEver)
{
	Mat srcCvt;
	src.convertTo(srcCvt, CV_32FC1);
	float range[] = {0, maxEle};
	const float *ranges = {range};
	
	int histbinsize= 220;
	//bool uniform = true, accumulate = false;

	Mat histMat;

	calcHist(&srcCvt, 1, 0, Mat(), histMat, 1, &histbinsize, &ranges);

	/*int histw = 660, histh = 400;
	int binw = (double)histw / histbinsize;
	Mat histImage = Mat::zeros(histh, histw, CV_8UC3);
	Mat forHistRealMax;
	normalize(histMat, forHistRealMax, 0, histImage.rows, NORM_MINMAX);
	for (int i = 0; i < histbinsize; ++i)
	{
	line(histImage, Point(binw * i, histh - forHistRealMax.at<float>(i)), Point(binw * i, histh - 1), Scalar(255, 0, 0), 2);
	}
	namedWindow("hist");
	imshow("hist", histImage);
	waitKey(0);*/

	int mostPlace, maxPlace;
	double mostBinV;
	findMaxMinInHist<float>(histMat, mostPlace, mostBinV, maxPlace);
	double diff = (maxPlace - mostPlace)*range[1]/histbinsize;
	
	cout << "the sal diff is "<< diff << endl;
	
	if((diff >= thresholdForSal) && (diff >= maxSalEver))
	{maxSalEver = diff; return true;}
	return false;

}


void drawLines( Mat& img, const vector<vector<Vec4i>>& lines)
{
	if(lines.size() == 0) return;

	for(int i = 0; i < lines.size(); ++i)
	{
		const vector<Vec4i>& ones = lines.at(i);
		for (int j = 0; j < ones.size(); ++j)
		{
			const Vec4i& oneline = ones.at(j);
			line(img, Point(oneline[0], oneline[1]), Point(oneline[2], oneline[3]), Scalar(255,0,0),2);
		}
	}
}
