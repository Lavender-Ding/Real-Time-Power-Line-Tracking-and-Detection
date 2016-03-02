#include "stdafx.h"
#include "LineProcess.h"
#include "dataManager.h"


void lineProcess::findLineCluster()
{

	int typeSize = m_types.size();
	CV_Assert( m_linesInput.size() == typeSize);

	vector<vector<Vec4i>> t_clusters;
	vector<Vec4i> t_oneCluster;

	
	for(int i = 0 ; i <= m_numLineTypes; ++ i )
	{
		t_oneCluster.clear();

		for(int j = 0; j < typeSize; ++ j )
		{
			if(m_types[j] == i)
			{
				t_oneCluster.push_back(m_linesInput[j]);
			}
		}
		Vec4i lineSample = t_oneCluster[0];
		float slope = (float)(lineSample[1] - lineSample[3]) / (lineSample[0] - lineSample[2]);
		m_lineSlope.push_back(slope);
		t_clusters.push_back(t_oneCluster);
	}
	
	m_paralleledLines = t_clusters;

	return;
}

void lineProcess::paraLines(int distThresh)
{
	if(m_linesInput.size() == 0)
		return;

	
	int typeIndex = 0;
	bool ifNewLine = true;

	m_types.resize(m_linesInput.size(),0);

	for(int i = 0; i < m_linesInput.size(); ++ i)
	{
		if(i == 0)
		{
			m_types[i] = typeIndex;
			continue;
		}

		for(int j = 0; j < i; ++ j )
		{
			if(Distance(m_linesInput[j], m_linesInput[i], distThresh) && Angle(m_linesInput[j], m_linesInput[i]))
			{
				m_types[i] = m_types[j];
				ifNewLine = false;
				break;
			}
		}

		if(ifNewLine)
		{
			typeIndex ++;
			m_types[i] = typeIndex;
		}

		ifNewLine = true;
	}

	//the number of the types of the lines
	m_numLineTypes = typeIndex ;

	return;

}


void lineProcess::reset(vector<Vec4i>& inputLines)
{
	m_linesInput.clear();
	m_linesInput = inputLines;
	m_paralleledLines.clear();
	m_types.clear();
	m_numLineTypes = 0;
	m_convexHulls.clear();
	m_lineSlope.clear();
}

void lineProcess::linesClusteringPro(vector<Vec4i>& inputLines, int distThresh)
{

	


	reset(inputLines);

	if(m_linesInput.empty())
	{
		std::cout << "no lines input" << std::endl;
		return;
	}



	paraLines(distThresh);
	findLineCluster();

	calcConvexHull();
}

void lineProcess::calcConvexHull()
{
	vector<vector<Vec4i>>::const_iterator iterCluster;
	vector<Vec4i>::const_iterator iterLine;
	vector<Point> vertex, hull;
	
	int i = 0;

	Scalar lineColor; 

	vector<vector<Vec4i>>::const_iterator iterClusterEnd = m_paralleledLines.end();
	

	for(iterCluster = m_paralleledLines.begin() ; iterCluster != iterClusterEnd; ++ iterCluster)
	{
		i++;
		vector<Vec4i>::const_iterator iterLineEnd = (*iterCluster).end();
		for(iterLine = (*iterCluster).begin(); iterLine != iterLineEnd; ++ iterLine)
		{
			Point tmp1 = Point((*iterLine)[0], (*iterLine)[1]), tmp2 = Point((*iterLine)[2], (*iterLine)[3]);
			vertex.push_back(tmp1);
			vertex.push_back(tmp2);

			//line(img, tmp1, tmp2, x_mosaics[i%3], 2);
		}
		
		//it is critical !
		hull.resize(vertex.size());

		if(vertex.size() != 2)
			calcHull(vertex, hull);
		else
			hull = vertex;

		//store the convex hulls
		m_convexHulls.push_back(hull);

		//lineColor = getBgColor(img, hull);

		vertex.clear();
		hull.clear();
	}

	//contours
}



void lineProcess::compensateArea(Mat &img, const vector<Vec4i>& cont) const
{
	dataManager* data = dataManager::getInstance();
	Scalar color = data->d_bgColor;

	for(int i = 0; i < cont.size(); ++i)
	{
		const Vec4i &tline = cont[i];
		line(img, Point(tline[0], tline[1]), Point(tline[2], tline[3]), color, 3);
	}
	//myDrawContour(img, cont, color);


}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

Scalar getBgColor(Mat& src, const vector<Point>& cont)
{
	Mat mask = Mat::zeros(Size(src.cols, src.rows),CV_8U);
	Point test;
	uchar* p;
	for(int i = 0; i < src.rows; i++)
	{
		p = mask.ptr<uchar>(i);
		for(int j = 0; j < src.cols; j++)
		{
			if (pointPolygonTest(cont, test, false))
			{
				*p = 255;
				p ++;
			}
		}
	}
	Scalar bg = mean(src, mask);
	return bg;
}

bool Distance(const Vec4i& a,const Vec4i& b, int thresh)
{
	//hhe
	int a1 = a[0], a2 = a[1], a3 = a[2], a4 = a[3], b1 = b[0], b2 = b[1];
	float k = float((a2 - a4)) / (a1 - a3);
	float A = k, C = -k * a1 + a2, B = -1;
	float dist = pow(abs(A * b1 + B * b2 + C), 2) / (pow(A, 2)+pow(B, 2));
	//std::cout << "dist is " << dist << std::endl;
	return dist < (thresh * thresh) ? 1 : 0;

}
bool Angle(const Vec4i& a,const Vec4i& b)
{
	//vec(x,y)-formated
	Vec2i vec_a = Vec2i(a[0] - a[2], a[1] - a[3]);
	Vec2i vec_b = Vec2i(b[0] - b[2], b[1] - b[3]);


	float cosineAB = (float)(vec_a[0] * vec_b[0] + vec_a[1] * vec_b[1]) / ( sqrt(pow(double(vec_a[0]),double(2)) + pow(double(vec_a[1]),double(2))) * sqrt(pow(double(vec_b[0]),double(2)) + pow(double(vec_b[1]),double(2))) );
	
	//std::cout << "the angle is " << cosineAB <<std::endl;
	//5 degree
	return abs(cosineAB) > 0.9721 ? 1 : 0;
	
}