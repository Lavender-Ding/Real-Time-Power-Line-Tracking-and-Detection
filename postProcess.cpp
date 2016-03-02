#include "stdafx.h"
#include "postProcess.h"
#include "dataManager.h"

Mat gradPostPro::gradMask(const Point2f& ptFound, const Mat& src) const
{
	Point ptTemp = Point((int)ptFound.x, (int)ptFound.y);
	return gradMask(ptTemp,src);
}

Mat gradPostPro::gradMask(const Point& ptFound, const Mat& src) const
{
	int p1x = ptFound.x - (m_maskSize >> 1), p1y = ptFound.y - (m_maskSize >> 1);
	
	if(p1x < 0) p1x = 0;
	if(p1x >= src.cols - 1 - m_maskSize) p1x = src.cols - 1;
	if(p1y < 0) p1y = 0;
	if(p1y >= src.rows - 1 - m_maskSize) p1y = src.rows - 1;

	Rect roiRect = Rect(Point(p1x, p1y), Size(m_maskSize, m_maskSize));

	//circle(copied, ptFound, 40, Scalar(255, 0, 0), 2);
	Mat maskRefed(src, roiRect);
	//imshow("video", maskRefed);
	//waitKey(0);

	Mat edge;
	Canny(maskRefed, edge, 80, 150);
	//return gradCalc(edge);
	return edge;
}

inline void gradPostPro::setMaskSize(int size)
{
	//can put strain here
	this->m_maskSize = size;
}


//to be finished...
Mat gradPostPro::gradCalc(const Mat& edge) const
{
	Mat grad_x, grad_y;
	Mat AngMat;
	
	/*int morph_elem = 1, morph_size = 1;
	Mat elem = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
	morphologyEx(edge, edge, MORPH_OPEN, elem);*/
	Sobel(edge,grad_x,CV_32F,1,0,3);
	Sobel(edge,grad_y,CV_32F,0,1,3);
	
	divide(grad_y,grad_x,AngMat);
	
	return AngMat;
}

//main entry
void gradPostPro::gradPostProcessing(const Mat& mask, const Point& pt, double slope)
{
	//reset();
	
	Mat edge = gradMask(pt, mask);
	//gradCalc(edge);
	//imshow("video", edge);
	//waitKey(0);
	



	int t_edgeArea = nonzeroNumCalc(edge);
	
	cout << t_edgeArea << endl;
	if(t_edgeArea > m_maxEdgeArea)
	{
		m_maxEdgeArea = t_edgeArea;
		m_maxEdgePoint = pt;
	}
	
}

void gradPostPro::setGoodPoint()
{
	dataManager* data = dataManager::getInstance();
	if(m_maxEdgeArea < (m_maskSize * m_maskSize / 50))
	{
		cout << "skip all " << endl;
		m_maxEdgePoint = hehe;
		return;
	}

	data->d_ptToBeDrawn = m_maxEdgePoint;
	reset();
}

inline void gradPostPro::reset()
{
	m_gradStatis.clear();
	m_maxEdgeArea = -100;
	
}

inline void gradPostPro::statisAnalyse() const
{
	Mat idx;
	//findNonZero(edge,idx);
}
////////////////////////////////////////////////////////////////////////
///////////////////                                   //////////////////
////////////////////////////////////////////////////////////////////////
int nonzeroNumCalc(const Mat& src)
{
	int num = 0;
	int sz = src.cols * src.rows;
	CV_Assert(src.type() == CV_8UC1 && src.isContinuous());

	const uchar* p = src.ptr<uchar>(0);

	for (int i = 0; i < sz; ++i)
	{
		if((int)p[i] != 0) num ++;
	}
	return num;
	
}