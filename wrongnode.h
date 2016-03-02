#include "stdafx.h"
#include <iostream>

using namespace std;

struct wrongnode{
	int start,end,type;
	wrongnode *next;

	wrongnode(const int &x, const int &y, const int &z, wrongnode *p=NULL)
	{
		start=x;end=y;type=z;
		next=p;
	}
	wrongnode():next(NULL){}
	~wrongnode(){}
};