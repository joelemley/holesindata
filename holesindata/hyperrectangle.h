#ifndef HYPERRECTANGLE_H
#define HYPERRECTANGLE_H

#include "common.h"
#include "pointcontainer.h"

class HyperRectangle{
    
public:
#ifdef _MSC_VER

	friend bool operator< (HyperRectangle &p1, HyperRectangle &p2);
	friend bool operator== (HyperRectangle &p1, HyperRectangle &p2);
#else
	friend bool operator< (const HyperRectangle &p1, const HyperRectangle &p2);
	friend bool operator== (const HyperRectangle &p1, const HyperRectangle &p2);
#endif


	bool destructed;
	int D;
	vector <PointContainer> points;// points is an array that contains sets of bounding points for each D for this rectangle. 
		
	HyperRectangle(int D);

	// Functions to get drawing data.
	bool isUnique(HyperRectangle &h2, double error=0.01);
	bool isIdenticalTo(HyperRectangle &h2);

	bool contains(double pt[]);
	bool contains(vector <double> pt);
	bool containmentSearch(vector <vector <double> > data);

	double getwidth();
	double getheight();
	double getdepth();
	double getx();
	double gety();
	double getz();

	double getSize();
	int containedIn( double pt[]);

	void insertBoundingPoint(double pt[]);


	string toString();
	string printIfThenRules();

	~HyperRectangle();
};



#endif