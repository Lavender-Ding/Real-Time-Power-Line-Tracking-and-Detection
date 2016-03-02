#include "StdAfx.h"
#include "dataManager.h"
#include "postProcess.h"

//has to exist!
dataManager* dataManager::m_dataMana;


IplImage* checkcolor1(IplImage* img)
{
	IplImage *check = cvCreateImage(cvGetSize(img), 8, 3);

	cvCopy(img, check);
	uchar* data = (uchar *)check->imageData;

	int step = check->widthStep / sizeof(uchar);    
	int channels = check->nChannels;          
	int height = check->height;
	int width = check->width;

	uchar b, g, r;
	double modular2pow;
	double theta, newThresh;


#pragma omp parallel for
	for (int i = 0; i < height; i++)
		for (int j = 0; j<width; j++)
		{
			b = data[i*step + j*channels + 0];    
			g = data[i*step + j*channels + 1];
			r = data[i*step + j*channels + 2];

			modular2pow = (r *r + b *b + g *g)*3;

			int sumBgr = b + g + r;

			theta = (double)(sumBgr*sumBgr) / modular2pow;


			if (theta > Thresh && modular2pow  > 202500)
			{
				data[i*step + j*channels + 0] = 255;
				data[i*step + j*channels + 1] = 255;
				data[i*step + j*channels + 2] = 255;
			}
			else
			{
				data[i*step + j*channels + 0] = 0;
				data[i*step + j*channels + 1] = 0;
				data[i*step + j*channels + 2] = 0;
			}
		}

		/*cvShowImage("video",check);
		cvWaitKey(0);*/

		return check;
}


