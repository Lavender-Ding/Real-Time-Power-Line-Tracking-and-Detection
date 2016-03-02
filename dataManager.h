#pragma once

#include "opencv2\opencv.hpp"
#include "postProcess.h"

using namespace std;
using namespace cv;


#define INVALID_STATUS 10086

const Point hehe = Point(INVALID_STATUS, INVALID_STATUS);


inline int pointDistance(Point2f& a, Point2f& b)
{
	return pow((a.x - b.x),2) + pow ((a.y - b.y),2);
}

class dataManager
{
private:
	static dataManager* m_dataMana;
	dataManager()
	{
		
	};

public:
	void init()
	{
		d_ptToBeDrawn = hehe;
		d_hasBgColorBeenCalced = false;
		d_lastFrameHasPt = false;
		d_postGradProIndex = new bool[20];
		d_maxSalEver = 0;
		/*if(d_increment == NULL)
		{
			d_increment = new uchar[2];
			d_increment[0] = 0;
			d_increment[1] = 0;
		}*/
	}

	static dataManager* getInstance()
	{
		if ( m_dataMana == NULL)
		{
			m_dataMana = new dataManager();
		}
		return m_dataMana;
	}

	//to be finished


public:
	//for whole project
	Size d_frameSize;
	int d_frameCounter;

	//for compensation of bg color
	Scalar d_bgColor;
	bool d_hasBgColorBeenCalced;

	//for roi invasion point locating
	vector<Point2f> d_invasionPt;
	Point2f d_invasionPtOffset;
	vector<int> d_maskAreaCount;
	Point d_ptToBeDrawn;
	vector<Point2f> d_preInvasionPt;

	vector<pairFound> d_pairs;
	pairFound d_goodPair;

	bool* d_postGradProIndex;
	//for logistic 
	bool d_lastFrameHasPt;

	uchar* d_increment;
	Point2f d_goodPt;

	//for saliency
	double d_maxSalEver;
};

IplImage* checkcolor1(IplImage* img);
const double Thresh = 0.9604; //0.98^2