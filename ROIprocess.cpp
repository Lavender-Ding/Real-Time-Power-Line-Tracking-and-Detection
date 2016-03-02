#include "stdafx.h"
#include "ROIprocess.h"
#include "dataManager.h"

void roiProcess::getRotateRect()
{
	int conSize = m_contour.size();
	if(conSize == 0)
		return;
	else if(conSize == 4)
	{
		for(int i = 0; i < conSize; i ++)
			m_ptsReset.push_back(m_contour[i]);
		return;
	}
	else  //3 pts or not below 5
	{

		RotatedRect rotatedROI = minAreaRect(m_contour);
	
		//float slope = rotatedROI.angle;

		Point2f *pts = new Point2f[4];

		rotatedROI.points(pts);
	
		int max_x = m_inputImg.cols - 1, max_y = m_inputImg.rows - 1;

		for (int i = 0; i < 4; i++)
		{

		//check if inside the image, if not, reset it to the border
		//std::cout << "before" << pts[i] << std::endl;

			pruneRect(pts[i]);
			Point tmpPt = pts[i];
			m_ptsReset.push_back(tmpPt);
			
		//std::cout << "after" << pts[i] << std::endl;
		}
		delete []pts;
		return;
	}
	

}



void roiProcess::pruneRect(Point2f& pt) const
{
	int x = pt.x, y = pt.y;
	int maxX = m_inputImg.cols - 1, maxY = m_inputImg.rows - 1;

	//define the condition
	int typeX, typeY;
	if( (x <= maxX) && (x >= 0))
		typeX = INSIDE_IMG;
	else
		if( x > maxX )
			typeX = TOO_RIGHT;
		else if( x < 0)
			typeX = TOO_LEFT;

	if( (y <= maxY) && (y >= 0))
		typeY = INSIDE_IMG;
	else
		if( y > maxY )
			typeY = TOO_BELOW;
		else if( y < 0)
			typeY = TOO_ABOVE;

	int typeID = typeX + typeY;

	//reset the points outside into the zone
	switch (typeID)
	{
		case INSIDE_IMG + INSIDE_IMG:
			break;
		case TOO_ABOVE + INSIDE_IMG:
			{
				pt.x = x - y / m_lineSlope;
				pt.y = 0;
				break;
			}
		case TOO_BELOW + INSIDE_IMG:
			{
				pt.y = maxY;
				pt.x = x - (y - maxY) / m_lineSlope;
				break;
			}
		case TOO_LEFT + INSIDE_IMG:
			{
				pt.x = 0;
				pt.y = y - x * m_lineSlope;
				break;
			}
		case TOO_RIGHT + INSIDE_IMG:
			{
				pt.x = maxX;
				pt.y = y - (x - maxX) * m_lineSlope;
				break;
			}

	default:
		{
			CV_Error(250, "rotated rectangle wrong u motherfuckin lil stupid ass!");
			break;
		}
	}
	
}

void roiProcess::reset(const Mat& img,const vector<Point>& cont, float slope)
{
	m_inputImg.release();
	img.copyTo(m_inputImg);
	m_contour = cont;
	m_lineSlope = slope;
	m_ptsReset.clear();
	m_haveIeverCalcedTheTransMatrixInTheFirstTrialAndWhyAmITypingSuchALongName=false;
}

void roiProcess::getROImatPro(const Mat& img, const vector<Point>& cont, float slope)
{
	dataManager* data = dataManager::getInstance();

	reset(img, cont, slope);
	getRotateRect();
	
}

void roiProcess::perspectiveProg(Mat& outputArray)
{
	dataManager* data = dataManager::getInstance();

	CV_Assert(m_ptsReset.size() == 4);

	Mat affined = Mat::zeros(Size(300, 60), CV_8UC3);

	if(!m_haveIeverCalcedTheTransMatrixInTheFirstTrialAndWhyAmITypingSuchALongName)
	{
		reshapeROI();
		Mat m = getPerspectiveTransform(m_ptsReset, m_upRightCorners);
		int i = m.type();
		m_trans = m;
		m_haveIeverCalcedTheTransMatrixInTheFirstTrialAndWhyAmITypingSuchALongName = true;
	}
	else
	{
		m_haveIeverCalcedTheTransMatrixInTheFirstTrialAndWhyAmITypingSuchALongName = false;
	}

	warpPerspective(m_inputImg, affined, m_trans, Size(affined.size()));

	if(data->d_hasBgColorBeenCalced == false)
	{
		Scalar bgColor = mean(affined);
		data->d_bgColor = bgColor;
		//
		//data->d_hasBgColorBeenCalced = true;
	}
	else
	{
		data->d_hasBgColorBeenCalced = false;
	}

	affined.copyTo(outputArray);
}

Point roiProcess::findEstInvaPts(Mat& mask)
{
	dataManager* data = dataManager::getInstance();
	//debugShow(mask);
	Mat idx;
	findNonZero(mask, idx);
	int pix = idx.rows;
	


	if(pix < 70)
	{
		//cout << "skip the mask" << pix << endl;
		return Point();
	}
	Scalar sumPos = sum(idx) / pix;
	data->d_maskAreaCount.push_back(pix);

	Mat invM;
	invert(m_trans, invM);

	Scalar result;
	Mat resultMat(Size(1,1),CV_32FC2,result);
	Mat meanMat(Size(1,1),CV_32FC2,sumPos);

	perspectiveTransform(meanMat,resultMat,invM);
	Vec2f hehe = *(resultMat.ptr<Vec2f>(0));

	Point found = Point(hehe[0], hehe[1]);
	data->d_invasionPt.push_back(found);
	return found;

}


bool roiProcess::abandonContour(const vector<Point>& roi) const
{
	dataManager* data = dataManager::getInstance();

	if(!data->d_lastFrameHasPt)
		return false;

	
	for(int i = 0; i < data->d_preInvasionPt.size(); ++i){

		double distIndi = pointPolygonTest(roi, data->d_preInvasionPt.at(i), true);
		//cout << distIndi << endl;
		//cout << data->d_invasionPtOffset << "frame no." <<data->d_frameCounter << endl;
		//if detecting position is at least 1/10 width of frame away from this contour, we skip this.
		if(distIndi > (data->d_frameSize.width) / (-10))
		{
		//cout << "polytest false at frame no." << data->d_frameCounter << endl;
			return false;
		}
		}
		return true;
}

void roiProcess::reshapeROI()
{
	CV_Assert(m_ptsReset.size() == 4);
	int dist_1 = pow((m_ptsReset[0].x - m_ptsReset[1].x), 2) + pow((m_ptsReset[0].y - m_ptsReset[1].y), 2);
	int dist_2 = pow((m_ptsReset[1].x - m_ptsReset[2].x), 2) + pow((m_ptsReset[1].y - m_ptsReset[2].y), 2);
	if(dist_1 < dist_2)
	{
		Point2f first = m_ptsReset[0];
		m_ptsReset[0] = m_ptsReset[1];
		m_ptsReset[1] = m_ptsReset[2];
		m_ptsReset[2] = m_ptsReset[3];
		m_ptsReset[3] = first;
	}
}


void roiProcess::getFinalPosByArea() const
{
	dataManager* data = dataManager::getInstance();

	if(data->d_invasionPt.size() == 0)
	{
		data->d_invasionPtOffset = Point2f(INVALID_STATUS, INVALID_STATUS);
		 
		return;
	}
	int minArea = *min_element(data->d_maskAreaCount.begin(), data->d_maskAreaCount.begin());

	if(minArea > 700)
	{
		data->d_invasionPtOffset = Point2f(INVALID_STATUS, INVALID_STATUS);
	}

	for each (int area in data->d_maskAreaCount)
	{
		//cout << "one area is" << area << endl;
	}

	//cout << "min area " << minArea << endl;
	for (int i = 0; i < data->d_invasionPt.size(); ++ i)
	{
		if(minArea == data->d_maskAreaCount[i])
		{
			data->d_invasionPtOffset = data->d_invasionPt[i];
			data->d_ptToBeDrawn = data->d_invasionPtOffset;
			break;
		}
	}
	cout << data->d_invasionPtOffset << " frame no." << data->d_frameCounter << endl;
}

//by reflecting back and edge detection 
Point& roiProcess::getFinalPos() const
{
	dataManager* data = dataManager::getInstance();


	return data->d_ptToBeDrawn;
}