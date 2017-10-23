#include "common.h"

#ifndef POINTCONTAINER_H
#define POINTCONTAINER_H


class PointContainer{
	

public:
	vector <double> points;
	PointContainer();

	PointContainer Something(vector <double> p);
	void insert(vector <double>::iterator it, vector <double>::iterator end);
	void sort();
	void insert(double d);
	std::vector <double>::iterator find(double d);
	void erase(double d);
};

#endif