#pragma once
#include "stdafx.h"
#include "opencv2\opencv.hpp"

using namespace cv;




class lineProcess
{
public:
	
	lineProcess() 
	{
		//display colored lines
		x_mosaics.push_back(Scalar(200,0,0));
		x_mosaics.push_back(Scalar(200,200,0));
		x_mosaics.push_back(Scalar(0,200,0));
		x_mosaics.push_back(Scalar(0,0,200));


	};

	void reset(vector<Vec4i>& inputLines);
	//find contours
	void findLineCluster();
	//find parallel lines
	void paraLines(int distThresh);

	//draw ROI 
	void calcConvexHull();

	void linesClusteringPro(vector<Vec4i> &lines, int distThresh);
	
	void lineProcess::compensateArea(Mat &img, const vector<Vec4i>& cont) const;
	//for each roi
	
	//debug
	void calcHull(vector<Point>& vertex, vector<Point>& hull)
	{
		convexHull(vertex,hull,false);
	}
public:
	vector<vector<Vec4i>> m_paralleledLines;
	vector<Vec4i> m_linesInput;
	vector<int> m_types;
	vector<vector<Point>> m_convexHulls;
	vector<float> m_lineSlope;
	int m_numLineTypes;

	
private:
	vector<Scalar> x_mosaics;

};




//general methods

bool Distance(const Vec4i& a,const Vec4i& b, int distThresh);

bool Angle(const Vec4i& a,const Vec4i& b);

Scalar getBgColor(Mat&, const vector<Point>&);