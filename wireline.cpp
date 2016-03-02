#include "stdafx.h"
#include "wireline.h"

wireline::wireline()
{
	a = 0;
	b = 0;
	num = 0;
	pt1[0].x=0;
	pt1[0].y = 0;
	pt2[0].x = 0;
	pt2[0].y = 0;
}

wireline::~wireline()
{
	a = 0;
	b = 0;
	num = 0;
	pt1[0].x = 0;
	pt1[0].y = 0;
	pt2[0].x = 0;
	pt2[0].y = 0;
}


void wireline::merge()
{
	if (a > 1)
	{
		if (pt1[0].y > pt2[0].y)
		{
			CvPoint tmp;
			tmp = pt2[0];
			pt2[0] = pt1[0];
			pt1[0] = tmp;
		}

		for (int i = 1; i < num; i++)
		{
			pt1[0] = pt1[i].y < pt1[0].y ? pt1[i] : pt1[0];
			pt1[0] = pt2[i].y < pt1[0].y ? pt2[i] : pt1[0];
			pt2[0] = pt1[i].y > pt2[0].y ? pt1[i] : pt2[0];
			pt2[0] = pt2[i].y > pt2[0].y ? pt2[i] : pt2[0];
		}
		
		num = 1;
	}
	else
	{
		if (pt1[0].x > pt2[0].x)
		{
			CvPoint tmp;
			tmp = pt2[0];
			pt2[0] = pt1[0];
			pt1[0] = tmp;
		}

		for (int i = 1; i < num; i++)
		{
			pt1[0] = pt1[i].x < pt1[0].x ? pt1[i] : pt1[0];
			pt1[0] = pt2[i].x < pt1[0].x ? pt2[i] : pt1[0];
			pt2[0] = pt1[i].x > pt2[0].x ? pt1[i] : pt2[0];
			pt2[0] = pt2[i].x > pt2[0].x ? pt2[i] : pt2[0];
		}

		num = 1;
	}
}
	