#pragma once
#include "stdafx.h"
#include "opencv2\opencv.hpp"

using namespace cv;
using namespace std;

typedef struct pairFound
{
	pairFound():ptFound(),corRate(0), flag(false){};
	pairFound(Point _ptFound, double _corRate):ptFound(_ptFound), corRate(_corRate), flag(true){};
	Point ptFound;
	double corRate;
	bool flag;
}pairFound;

class gradPostPro
{
public:
	gradPostPro():m_maskSize(80), m_maskEdge(Mat(Size(m_maskSize, m_maskSize),CV_8U)){reset();};


	void gradPostProcessing(const Mat& mask, const Point& pt, double slope);

	inline void statisAnalyse() const;

	Mat gradMask(const Point&, const Mat&) const;


	Mat gradMask(const Point2f&, const Mat&) const;

	Mat gradCalc(const Mat&) const;

	void setGoodPoint() ;

	inline void setMaskSize(int);

private:
	pairFound m_pair;

	void reset();

	Point m_maxEdgePoint;
	int m_maxEdgeArea;

	int m_maskSize;
	double m_normalRate;
	Mat m_maskEdge;
	vector<double> m_gradStatis;
};

int nonzeroNumCalc(const Mat&);