//===========================================================================
//	Saliency_MC.h: interface for the Saliency_MC class.
//  This code implements the Saliency_Absorb_MC with 
//	MATLAB code of Saliency detection via absorb Markov chain of ICCV 2013.
//===========================================================================
//	Copyright (c) 2014 Tan Shen [SJTU]. All rights reserved.
//	Email: tanshen@sjtu.edu.cn
//	Date: 2014/10/26
//  The code is run on VS2012 + OpenCV2.48
//===========================================================================
//////////////////////////////////////////////////////////////////////
#pragma once
#include "SLIC.h"
using namespace cv;



Mat SaliencyPro(Mat& src, double threshold);

class Saliency_MC{
private:
	Mat get_WconFirst(int &k,Mat &ConPix2, double *Supll,double *Supaa,double *Supbb);
	vector <Mat> find_connect_superpixel_DoubleIn_Opposite(int &k,int &height,int &width,Mat &Label);
	Mat Find_Edge_Superpixels(Mat &Label, int &k, int &height,int &width , Mat &WconFirst,Mat &ConPix);
	vector <Mat> absorb_MC(int &NumIn,int &NumEdg,int &k,Mat & EdgSup,Mat &EdgWcon, Mat &Discard, Mat &WconFirst);
	double *sup2pixel(int &PixNum, Mat &Label, Mat &sup);
	Mat reshape(double * &labelline,int &height,int &width);
	//int otsu2 (IplImage *image);
	//void MeanShiftSmooth(IplImage* src, IplImage* dst, int hs, int hr, int maxIter);

public:
	Saliency_MC(IplImage * imgI,IplImage *Mask,double *box, bool &ifBg, double thresh);
	//int Saliency_MC::MiniCross(IplImage *pGrayMat);
	//IplImage * GetMask(IplImage * imgI,double *box);
	//void MeanShiftSmooth2(IplImage* src, IplImage* dst, int hs, int hr, int maxIter);
private:
	double x_thresh;
	
};


////////////////////////////////////////////////////////////////////////////////////////////
////====================================================================================////
////////////////////////////////////////////////////////////////////////////////////////////

void traverseMatOneByOne(const Mat&);

bool drawHistS(const Mat&, double max, double& outputClimax, double& maxSalEver);

template<typename T>
void findMaxMinInHist(const Mat& src, int& mostBinIndex, double& mostBinV, int& maxVBinIndex)
{
	CV_Assert(src.isContinuous());
	const T* data;
	data = src.ptr<T>(0);
	int sz = src.cols* src.rows * src.channels();
	mostBinV = -10000000000.00000;
	mostBinIndex = -1;
	maxVBinIndex = -1;
	
	for (int i = 0; i < sz; ++i) //W T F?
	{

		double cur = (double)data[i];
		if(cur > mostBinV){
			mostBinIndex = i;
			mostBinV = cur;
		}
		if(cur > 0){
			maxVBinIndex = i;
		}
	}
	
};