#include "stdafx.h"
#include "alongTheLineDetector.h"


using namespace std;




//VideoWriter sd("F:/FFOutput/tested1.avi",CV_FOURCC('M','J','P','G'),15.0,Size(1920, 1080));


void alongTheLineProcess::alongTheLinePro(const vector<Vec4i>& srcLines, Mat& src, bool ifShowTheResult)
{


//#define SHOW_PROCESS



	Mat tem;
	tem = src.clone();

	if(srcLines.size()  ==  0)
		return;
	 
	m_dots.clear();
	Vec3b preColor,curColor;

	
	double k;

	for(size_t i  =  0;i  <  srcLines.size();i ++)
	{
		int x1  =  srcLines[i][0],y1  =  srcLines[i][1],x2  =  srcLines[i][2],y2  =  srcLines[i][3];

		double diff;

		if(x1 != x2)
			k = (double)(y1 - y2) / (x1 - x2);
		else k  =  BIG_O_ASS_LARGE_NUMBER;

		if(abs(k)  <  1 && k!= BIG_O_ASS_LARGE_NUMBER)
		{

			for(int x = x1,j = 0;x < x2;x += m_step,j++)
			{
				int y = cvRound(k*(x - x1)+y1);

				curColor = src.at < Vec3b > (y,x);
				
				/*circle(tem,Point(x,y),2,Scalar(255,0,0),2,8);*/
				//sd << tem;
#ifdef SHOW_PROCESS

				imshow("pic1",tem);
				waitKey(0);
#endif
				if(x == x1)
				{preColor = curColor;continue;}
				else
				{

					double mol1 = norm(preColor,NORM_L2),mol2 = norm(curColor,NORM_L2);
					int dotP = Mat(preColor).dot(Mat(curColor));

					diff = dotP / (mol1*mol2);
					
					if(diff < 0.875)
					{
						cout << diff << endl;
						m_dots.push_back(Point(x,y));
						 
						/*rectangle(tem,Point(x - 9,y - 9),Point(x+9,y+9),Scalar(0,255,0),2);
						circle(tem,Point(x,y),40,Scalar(255,255,255),2,8);*/
						//sd << tem;
#ifdef SHOW_PROCESS

						imshow("pic1",tem);
						waitKey(0);
#endif
						cout << diff << endl;
					}
				}
				preColor = curColor;
			}
		}

		else if(abs(k) > 1)
		{

			if(y1 > y2)
				swap(y1,y2);
			k = 1 / k;	
			for(int y = y1,j = 0;y < y2;y += m_step,j++)
			{
				int x = cvRound(k*(y - y1)+x1);

				curColor = src.at < Vec3b > (y,x);
				
				/*circle(tem,Point(x,y),2,Scalar(255,0,0),2,8);*/
				//sd << tem;

#ifdef SHOW_PROCESS

				imshow("pic1",tem);
				waitKey(0);
#endif

				if(y == y1)
				{preColor = curColor;continue;}
				else
				{

					double mol1 = norm(preColor,NORM_L2),mol2 = norm(curColor,NORM_L2);
					int dotP = Mat(preColor).dot(Mat(curColor));

					diff = dotP / (mol1*mol2);

					if(diff < 0.85)
					{
						cout << diff << endl;
						m_dots.push_back(Point(x,y));

						/*rectangle(tem,Point(x - 9,y - 9),Point(x+9,y+9),Scalar(0,255,0),2);
						circle(tem,Point(x,y),40,Scalar(255,255,255),2,8);*/
						//sd << tem;
#ifdef SHOW_PROCESS

						imshow("pic1",tem);
						waitKey(0);
#endif
						cout << diff << endl;
					}
				}
				preColor = curColor;
			}
		}
		

	}
	drawContour(src);
}

void alongTheLineProcess::detectPro(Mat &src, vector<Vec4i> &lines, int distThresh, int step)
{
	linesClusteringPro(lines, distThresh);
	m_step = step;

	int clusterSize = m_paralleledLines.size();
	for(int i = 0; i < clusterSize; i++)
	{
		alongTheLinePro(m_paralleledLines.at(i), src, true);
	}
}


void alongTheLineProcess::drawContour(Mat& src)
{
	int n = m_dots.size();
	if(n == 0) return;
	for(int i = 0;i < n;i ++)
	{
		circle(src,m_dots.at(i), 40, Scalar(255,255,255), 2, 8);

		imshow("ROI",src);
		waitKey(1);
		/*sd<<tem;*/
	}
}