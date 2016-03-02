#pragma once


#include "opencv2\opencv.hpp"

using namespace cv;

#define TOO_LEFT 0
#define TOO_RIGHT 1
#define TOO_ABOVE 20
#define TOO_BELOW 30
#define INSIDE_IMG 100

class roiProcess
{
public:
	roiProcess()
	{
		m_upRightCorners.push_back(Point(0, 0));
		m_upRightCorners.push_back(Point(300, 0));
		m_upRightCorners.push_back(Point(300, 60));
		m_upRightCorners.push_back(Point(0, 60));
		m_haveIeverCalcedTheTransMatrixInTheFirstTrialAndWhyAmITypingSuchALongName = false;
	};
	roiProcess(vector<Point> _contours, float _lineSlope) : m_contour(_contours), 
		m_lineSlope(_lineSlope){roiProcess();};
	~roiProcess();

	//min-area rectangle
	void getROImatPro(const Mat& img, const vector<Point>&, float);

	void perspectiveProg(Mat&);

	void reset(const Mat& img, const vector<Point>&, float);

	void getRotateRect();
	//reset those points outside the sector
	void pruneRect(Point2f&) const;

	Point findEstInvaPts(Mat& mask);

	void getFinalPosByArea() const;
	//by post gradient process
	Point& getFinalPos() const;
	//IT IS NECESSARY! cuz we need a lean-down rect roi	
	void reshapeROI();

	bool abandonContour(const vector<Point>& ) const;
public:
	Point m_invasionPosition;
	Mat m_trans;
	Mat  m_inputImg;
	Mat m_transMask;
private:
	vector<Point> m_contour;
	vector<Point2f> m_ptsReset;
	
	float m_lineSlope;
	vector<Point2f> m_upRightCorners;
	
	bool m_haveIeverCalcedTheTransMatrixInTheFirstTrialAndWhyAmITypingSuchALongName;
};
