#include "pointcontainer.h"

	PointContainer::PointContainer(){

	}

	PointContainer PointContainer::Something(vector <double> p){
		PointContainer pc;
		pc.points=p;
		return pc;	
	}
	void PointContainer::insert(vector <double>::iterator it, vector <double>::iterator end){
		
		while(it!=end){
			this->points.push_back(*it);
			it++;
		}
	
	}

	void PointContainer::sort(){
		std::sort(this->points.begin(),this->points.end());
		this->points.erase( unique( this->points.begin(), this->points.end() ), this->points.end() );
	}
	void PointContainer::insert(double d){
	//	cout<<" Inserting "<<d<<endl;
		this->points.push_back(d);
		this->sort();
	}
	std::vector <double>::iterator PointContainer::find(double d){
		return lower_bound(this->points.begin(), this->points.end(), d);	
	}
	void PointContainer::erase(double d){
		vector<double>:: iterator it=lower_bound(this->points.begin(), this->points.end(), d);
		this->points.erase(it);
	
	}
