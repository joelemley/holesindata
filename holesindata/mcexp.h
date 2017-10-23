#ifndef MCEXP
#define MCEXP
#include "hyperrectangle.h"

#define EMPTY 0
#define ONEPOINT 1
#define BOUNDINGPOINT 2
#define MANYPOINTS 3

class HC{
public:
	int option;
	double value; 
};

//int myrandom (int i) { return std::rand()%i;}


// start of prototypes

double fRand(double fMin, double fMax);

class monteCarloRectangles{

public:
	int numberExamined;
	int numberLargeExamined;
	vector <HyperRectangle> hrects;
	void insert(HyperRectangle h);
	void sort();
	HyperRectangle getMaxEmptyRectangle();
	void distinctify();
	string printIfThenRules(int);
};

HC countHelper(HyperRectangle * monteCarlo, vector<vector <double> > *data,int D);
int findMax(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, HyperRectangle monteCarlo, int upperbound, int lowerbound, int mc);
int findMin(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, HyperRectangle monteCarlo, int upperbound, int lowerbound, int mc);
HyperRectangle generateMonteCarloRectangle(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, int D, int strategy);


class MEHRExperimenter{
	public:
	vector <vector <double> > data; // raw data from the input file
	vector <double> max; // max values for each attribute
	vector <double> min; // min values for each attribute
	string s;
	ifstream infile; // file handle 
	int D; // Number of attributes
	vector< vector <double> > vpoints; // for holding sorted, distinct attributes 
	vector<double> dpoints;// used to help create vpoints

	MEHRExperimenter();	
	void loadfile(string filename, string delimiter);
	void normalizeSpace();
	void prepareProjections();
	monteCarloRectangles runMonteCarlo(int stop, int strategy);

	monteCarloRectangles run(string filename, int stopcondition, string delimiter, int strategy);
};



#endif
