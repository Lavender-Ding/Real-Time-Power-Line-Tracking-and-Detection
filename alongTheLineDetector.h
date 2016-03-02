#include "stdafx.h"
#include "lineProcess.h"



#define  BIG_O_ASS_LARGE_NUMBER 10000

class alongTheLineProcess : public lineProcess
{
public:
	

	void alongTheLinePro(const vector<Vec4i>& srcLines, Mat& src, bool);

	void detectPro(Mat& src, vector<Vec4i> &lines, int distThresh, int step);
	
	void drawContour(Mat&);

	void demoMaker();

private:

	
	vector<Point> m_dots;
	int m_step;
};