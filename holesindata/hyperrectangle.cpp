#include "hyperrectangle.h"
	// Functions to get drawing data.

#ifdef _MSC_VER

bool operator< ( HyperRectangle &p1,  HyperRectangle &p2){
	return (p1.getSize() > p2.getSize());
}
bool operator== ( HyperRectangle &p1,  HyperRectangle &p2){
	return (p1.getSize() == p2.getSize());
}
#else
bool operator< (const HyperRectangle &p1, const HyperRectangle &p2){
	return (p1.getSize() > p2.getSize());
}
bool operator== (const HyperRectangle &p1, const HyperRectangle &p2){
	return (p1.getSize() == p2.getSize());
}
#endif

	HyperRectangle::HyperRectangle(int D){ 

		vector <PointContainer> lpoints(D);
		this->points=lpoints;
		destructed=false;
		this->D=D; 
	}

	bool HyperRectangle::isUnique(HyperRectangle &h2,double error){

		for(int i=0; i<D; i++){
			
			double f1=*this->points.at(i).points.begin();
			double f2=*h2.points.at(i).points.begin();

			if(abs(f2-f1)>error){ return false;
			}
			f1=*(--this->points.at(i).points.end());
			f2=*(--h2.points.at(i).points.end());

			if(abs(f2-f1)>error){ return false;}		
		}
		return true;
	}
	bool HyperRectangle::isIdenticalTo(HyperRectangle &h2){

	if (this->getSize() != h2.getSize()) return false; 
		for(int i=0; i<D; i++){
			

			if (this->points.at(i).points.size() != h2.points.at(i).points.size()) return false; 
			double f1=*this->points.at(i).points.begin();
			double f2=*h2.points.at(i).points.begin();

			if(f1!=f2) return false; 

			f1=*(--this->points.at(i).points.end());
			f2=*(--h2.points.at(i).points.end());

			if(f1!=f2) return false; 
		}
	}

	bool HyperRectangle::contains(double *pt){
		
		if(this->containedIn(pt)==1) return true; 
		else return false; 
	}

	bool HyperRectangle::contains(vector <double> pt){
		return this->contains(pt.data());
	}

	bool HyperRectangle::containmentSearch(vector< vector<double> > data){
		for(int i=0; i < data.size();i++){
			if (this->contains(data.at(i)))
			{
				//cout<<"Contained point "<<i<<endl;
				return true;
			}
		}


		//bool b= this->contains(data.at(0));
		//if (b) 
		//	{
		//		return true;
		//	}
		return false;
	}

	double HyperRectangle::getwidth(){

		double u=*this->points.at(0).points.begin();
		double l=*(--this->points.at(0).points.end());
		return l-u;
	}
	double HyperRectangle::getheight(){
		double u=*this->points.at(1).points.begin();
		double l=*(--this->points.at(1).points.end());
		return l-u;	
	}
	double HyperRectangle::getdepth(){
		double u=*this->points.at(2).points.begin();
		double l=*(--this->points.at(2).points.end());
		return l-u;	
	}

	double HyperRectangle::getx(){
		return *this->points.at(0).points.begin();
	
	}
	double HyperRectangle::gety(){
		return *this->points.at(1).points.begin();	
	}
	double HyperRectangle::getz(){
		return *this->points.at(2).points.begin();	
	}

	double HyperRectangle::getSize() const {
		double end=*(--points.at(0).points.end());
		double size=end - *points.at(0).points.begin();
		for(int i=1;i<D;i++){
			end=*(--points.at(i).points.end());
			size=size*(end - *points.at(i).points.begin());
		}
		return size;
	}
	int HyperRectangle::containedIn( double pt[]){
		if (!pt) return 0;
		if (this->destructed==true) return 0;
		double start;
		double end;
		bool bounding=false;
		for( int i=0; i< this->D; i++)
		{
			double p=pt[i];
			start=*this->points.at(i).points.begin();
			end=*(--this->points.at(i).points.end()); // Dereference the pointer to the element before end() 

			if (p <=start || p>=end) return 0; // TODO: Prove this is correct. 
			else {
				if(pt[i]==start || pt[i]==end) bounding=true; 
			}
		}

		//if(bounding==true) return 2;
		//else return 1;
		return 1;
	}


	void HyperRectangle::insertBoundingPoint(double pt[]){
		for( int i=0; i< this->D; i++)
		{
			this->points.at(i).insert(pt[i]);
		}
	}



	string HyperRectangle::toString(){
		string s="";

		for(int i=0;i<this->D;i++){
			s=s+" D="+tostr(i)+":";
			for (std::vector<double>::iterator it = this->points.at(i).points.begin(); it != this->points.at(i).points.end(); ++it){
				//s=s+" "+ it+",";
				double k=*it;
				
				s=s+" "+tostr(k);
			}
		}
		return s;
	}

	string HyperRectangle::printIfThenRules(){
		string s;
		for(int i=0;i<this->D;i++){
		
			string d=tostr(i)+":";
			string start =tostr(*this->points.at(i).points.begin());			
			string end= tostr(*(--this->points.at(i).points.end()));
				
			if(i==0) s=s+"If "+"(D"+d+" Is Between "+start +" and " +end+")"; 
			else s=s+ " AND  " +"(D"+d+" Is Between "+start +" and " +end+")"; 
								
			}
			s=s+" Then the hyper rectangle bound by those points is empty:\n\n";

		return s;
	}

	HyperRectangle::~HyperRectangle(){
	}
