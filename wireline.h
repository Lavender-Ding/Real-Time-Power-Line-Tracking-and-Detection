#pragma once

#include"highgui.h"  
#include"cv.h"  
#include"cxcore.h"
#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include<stdlib.h>  
#include <math.h>
#include<time.h>
#include<stdio.h>


class wireline
{
public:
	wireline();
	~wireline();
public:
	double a;
	double b;
	
	CvPoint pt1[50];
	CvPoint pt2[50];

	int num;

public:
	void merge();
};

