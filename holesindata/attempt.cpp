#include "common.h"
#include "pointcontainer.h"
#include "hyperrectangle.h"


int myrandom (int i) { return std::rand()%i;}

bool pruneEmptyRec(HyperRectangle h2){

	return h2.destructed;
}

class monteCarloRectangles{

public:
	vector <HyperRectangle> hrects;
	void insert(HyperRectangle h){
		hrects.push_back(h);
	}
	void sort(){	
		std::sort(this->hrects.begin(), this->hrects.end()); 
	}
	HyperRectangle getMaxEmptyRectangle(){
	
		return hrects.at(0);
	}
	string printIfThenRules(int limit=0){	
		
		int nbr=0;
		string s="";
		for (std::vector<HyperRectangle>::iterator it = this->hrects.begin(); it != this->hrects.end(); ++it){
			if (limit !=0 && nbr > limit) break;
			else nbr++;
			for(int i=0;i<it->D;i++){
				
				string d=tostr(i)+":";
				string start =tostr(*it->points.at(i).points.begin());			
				string end= tostr(*(--it->points.at(i).points.end()));
				
				if(i==0) s=s+"If "+"(D"+d+" Is Between "+start +" and " +end+")"; 
				else s=s+ " AND  " +"(D"+d+" Is Between "+start +" and " +end+")"; 								
			}
			s=s+" With a volume of "+tostr(it->getSize());
			s=s+" Then the hyper rectangle bound by those points is empty:\n\n";
		}
		return s;
	}
};

class mhrCollector2{


private:
	vector <HyperRectangle> points;
public:
	mhrCollector2(HyperRectangle *initial){
		
		points.push_back(*initial);

	}
	void insert( HyperRectangle T){
		points.push_back(T);		
	}
	void addPoint(vector <double> pt){
		addPoint(pt.data());
	}
	void addPoint(double pt[]){
		//Containment Search:
		int choice=0;
		vector <HyperRectangle> temp;

		for (std::vector<HyperRectangle>::iterator it = points.begin(); it != points.end(); ++it){

			 choice=(int)it->containedIn(pt);
			 //cout<<choice;
			 if(choice ==2 && !it->destructed) it->insertBoundingPoint(pt);
			 if(choice==1 && !it->destructed){ // We found a point within a MHR, now we need to split it.
				//1. Get sets from it. 
				//2. Remove it from vector. 
				//
				it->insertBoundingPoint(pt);
				int replaced=false;
				for(int i = 0; i<it->D;i++){
					
					// Split larger parts
					std::vector<double>::iterator it2 = it->points.at(i).points.end();
					std::vector<double>::iterator it3=it->points.at(i).find(pt[i]);
					 
					HyperRectangle temp1(it->D); //split 1
					HyperRectangle temp2(it->D); //split 2

					 //cout<<"test:"<<*it3<<endl;
					vector<double> t1;
					 double end=*(--it2);

					 temp1.points.at(i).insert(end);
					 //cout<<"\n inserted point "<<end<<endl;
						 
					 while(*it3 < end){
						 double tempk=*it3;
						 temp1.points.at(i).insert(tempk);
						// cout<<"\n inserted point "<<temp<<endl;
						 it3++; 						 
					 }
					 for(int j=0;j< it->D; j++){
						 if (i!=j){
							 temp1.points.at(j).insert(it->points[j].points.begin(),it->points[j].points.end());
							 temp1.points.at(j).erase(pt[j]);
							 temp1.points.at(j).sort();
						 }
					 }

					 //cout<<temp1.toString();
					 // Split smaller parts
					 it2 = it->points.at(i).points.begin();
					 it3=it->points.at(i).find(pt[i]);

					 //cout<<"test:"<<*it3<<endl;
					 vector<double> t2;
					 end=*it3;

					 temp2.points.at(i).insert(end);

					 //cout<<"\n inserted point "<<end<<endl;
						 
					 while(*it2 < end){
						 double tempk=*it2;
						 temp2.points.at(i).insert(tempk);
						 //cout<<"\n inserted point "<<temp<<endl;
						 it2++; 
					 }

					 for(int j=0;j< it->D; j++){
						 if (i!=j){						 
							 temp2.points.at(j).insert(it->points.at(j).points.begin(),it->points.at(j).points.end());
							 temp2.points.at(j).erase( pt[j]);
   						     temp2.points.at(j).sort();
						 }					 
					 }
					 
					 //cout<<temp2.toString();
					 //system("pause");

					 //BigEnough step:
					 double bigEnough=(700*10);

					 if (temp1.getSize()> bigEnough){
						 if  (temp.size()>1){
							int tempsize=temp.size();
							bool t1unique=true;
							for(int countcheck=0; countcheck<tempsize;countcheck++){

								if(temp.at(countcheck).isUnique(temp1)) t1unique=false;
							}

							if (t1unique) temp.push_back(temp1);

						 }
						 else{
							temp.push_back(temp1);
						 }
					 }


					 if (temp2.getSize()>bigEnough){
						 if  (temp.size()>1){
						 int tempsize=temp.size();
						
						 bool t2unique=true;
						 for(int countcheck=0; countcheck<tempsize;countcheck++){

							
							 if(temp.at(countcheck).isUnique(temp2)) t2unique=false;
						 }

						
						 if (t2unique) temp.push_back(temp2);

						 }
						 else{
							temp.push_back(temp2);
						 }
					 }

				}
				it->destructed=true;
				
			}	
		}
		this->points.insert(this->points.end(),temp.begin(),temp.end());
		this->points.erase(remove_if(this->points.begin(), this->points.end(), pruneEmptyRec), this->points.end());
	}
	void sort(){	
		std::sort(this->points.begin(), this->points.end()); 
	}
	void printSizes(int limit=0)
	{
		cout<<endl;
		int count=0;
		int d=0;
//		int k=0;
		for (std::vector<HyperRectangle>::iterator it = this->points.begin(); it != this->points.end(); ++it){
			if(!it->destructed){ 
				cout<<"Volume: "<<it->getSize()<<" Points: " <<it->toString()<<endl;
				count++;

				if (limit !=0 && count==limit) break;
			}else{
			d++;
			
			}		
		}
		//cout<<"w="<<d<<"s="<<this->points.size();
		cout<<"Number of found MHRs"<<(this->points.size()-d);
	}
	string printIfThenRules(int limit=0){
		
		
		int nbr=0;
		string s="";
		for (std::vector<HyperRectangle>::iterator it = this->points.begin(); it != this->points.end(); ++it){
			if (limit !=0 && nbr > limit) break;
			else nbr++;
			for(int i=0;i<it->D;i++){
				
				string d=tostr(i)+":";
				string start =tostr(*it->points.at(i).points.begin());			
				string end= tostr(*(--it->points.at(i).points.end()));
				
				if(i==0) s=s+"If "+"(D"+d+" Is Between "+start +" and " +end+")"; 
				else s=s+ " AND  " +"(D"+d+" Is Between "+start +" and " +end+")"; 								
			}
			s=s+" Then the hyper rectangle bound by those points is empty:\n\n";
		}
		return s;
	}

	HyperRectangle getMaxEmptyRectangle(){
	
		return points.at(0);
	}
};

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

#define EMPTY 0
#define ONEPOINT 1
#define BOUNDINGPOINT 2
#define MANYPOINTS 3


class HC{
public:
	int option;
	double value; 
};

HC countHelper(HyperRectangle * monteCarlo, vector<vector <double> > *data,int D){

	HC hc; 
	hc.option=EMPTY;
	hc.value=-1;
	int i=0;
	int n=0;
	for (std::vector< vector <double> >::iterator it = data->begin(); it != data->end(); ++it){
		int result=monteCarlo->containedIn((*it).data());

		if (result==1) {
			if (n<1){
				hc.option=ONEPOINT;
				hc.value=it->at(D);
			}
			else{
				hc.option=MANYPOINTS;
				return hc; // no point in going any further. 
			}
			n++;
		}

		//if (result==2&&n<1){
		//	hc.option=BOUNDINGPOINT;
		//	hc.value=it->at(D);
		//}

		i++;
	}
	return hc;
}


int findMax(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, HyperRectangle monteCarlo, int upperbound, int lowerbound, int mc){

	HC hc;

	bool foundit=false;
	int position = (lowerbound+upperbound)/2;

	for(int i= lowerbound; i<upperbound; i++){ // recursive solution is broken. Temporarily use this naive solution. (n vs lg n)
		monteCarlo.points.at(mc).points.at(1)= vpoints->at(mc).at(i);
		hc=countHelper(&monteCarlo,data,mc);
		if (hc.option!=EMPTY) return i-1;
	}

	//while(foundit==false){					
	//	monteCarlo.points.at(mc).points.at(1)= vpoints->at(mc).at(position);
	//	hc=countHelper(&monteCarlo,data,mc);

	//	if (hc.option==ONEPOINT) return position; // we found it;
	//	if(hc.option==MANYPOINTS) {
	//		upperbound = position - 1; 
	//	
	//	} // guess lower
	//	if(hc.option==EMPTY){
	//		
	//		lowerbound = position + 1;
	//	} // guess higher
	//	 position = (lowerbound + upperbound) / 2;
	//}
}



int findMin(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, HyperRectangle monteCarlo, int upperbound, int lowerbound, int mc){

	HC hc;

	bool foundit=false;

	for(int i= lowerbound; i<upperbound; i++){ // recursive solution is broken. Temporarily use this naive solution. (n vs lg n)
		monteCarlo.points.at(mc).points.at(0)= vpoints->at(mc).at(i);
		hc=countHelper(&monteCarlo,data,mc);
		if (hc.option!=EMPTY) return i-1;
	}

	//while(foundit==false){					
	//	monteCarlo.points.at(mc).points.at(1)= vpoints->at(mc).at(position);
	//	hc=countHelper(&monteCarlo,data,mc);

	//	if (hc.option==ONEPOINT) return position; // we found it;
	//	if(hc.option==MANYPOINTS) {
	//		upperbound = position - 1; 
	//	
	//	} // guess lower
	//	if(hc.option==EMPTY){
	//		
	//		lowerbound = position + 1;
	//	} // guess higher
	//	 position = (lowerbound + upperbound) / 2;
	//}
}


//void binarySearch(apvector <int> &array, int lowerbound, int upperbound, int key)
//{
//       int position;
//       int comparisonCount = 1;    //count the number of comparisons (optional)
//
//       // To start, find the subscript of the middle position.
//       position = ( lowerbound + upperbound) / 2;
//
//       while((array[position] != key) && (lowerbound <= upperbound))
//       {
//              comparisonCount++;
//              if (array[position] > key)               // If the number is > key, ..
//             {                                                       // decrease position by one.
//                    upperbound = position - 1;    
//             }                                                      
//             else                                                
//            {                                                        // Else, increase position by one.
//                   lowerbound = position + 1;     
//             }
//             position = (lowerbound + upperbound) / 2;
//       }
//} 


HyperRectangle generateMonteCarloRectangle(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, int D){
	

	HyperRectangle monteCarlo(D);
	
	vector <double> mcpoint;
	for(int i=0; i<D;i++){
		
		int mc=order->at(i);


		//double ma=max->at(i);
		//double mi=min->at(i);
		double ma=MAX;
		double mi=MIN;
		if(mi==ma){
			monteCarlo.points.at(mc).insert(ma);
			monteCarlo.points.at(mc).insert(mi);
		
		}
		else{
			double temp=fRand(mi,ma);
			mcpoint.push_back(temp);	
			vector<double>:: iterator it=lower_bound(vpoints->at(mc).begin(), vpoints->at(mc).end(), temp);
			vector<double>:: iterator it2=upper_bound(vpoints->at(mc).begin(), vpoints->at(mc).end(), temp);
		
			double start=*it;
			
			if (it2 == vpoints->at(mc).end()){
				it2--;
			
			}
			double end=*it2; // it2 is out of bounds.  Dereference fails here. 

			while(start==end){
			
				if((it2+1)!=vpoints->at(mc).end()) it2++;
				else it--;
				start=*it;
				end=*it2;
			}
		
			monteCarlo.points.at(mc).insert(start);
			monteCarlo.points.at(mc).insert(end);
		}

	}
	HC hc;

		for(int i=0; i<D; i++){
			int mc=order->at(i);

			if(monteCarlo.points.at(mc).points.size()>1){
				double dmax=monteCarlo.points.at(mc).points.at(1);
				double dmin=monteCarlo.points.at(mc).points.at(0);
				std::vector<double>::iterator it;
			    it = find (vpoints->at(mc).begin(), vpoints->at(mc).end(), dmax);

				if(it != vpoints->at(mc).end()-1) // already at the end. No need to try expanding. 
				{
					int dist = std::distance(vpoints->at(mc).begin(),it);					
					monteCarlo.points.at(mc).points.at(1)=MAX;
					hc=countHelper(&monteCarlo,data,mc);

					if(hc.option!=EMPTY) // Not empty anymore. We need to reduce the size
					{
						monteCarlo.points.at(mc).points.at(1)=(vpoints->at(mc).at(dist+1));
						hc=countHelper(&monteCarlo,data,mc);

						if(hc.option!=EMPTY){
							monteCarlo.points.at(mc).points.at(1)=dmax; // We have proven this is maximal Keep this. 						
						}
						else{ // maximal is somewhere between vpoints->at(mc).at(dist+1) and vpoints->at(mc).end() do binary search to find it. 
							//cout<<"TODO: Implement new search for this case. ";

							int l=dist;
							int r=std::distance(vpoints->at(mc).begin(),vpoints->at(mc).end());
							
							int pos=findMax(data,vpoints,order,monteCarlo,r,l,mc);

							monteCarlo.points.at(mc).points.at(1)=vpoints->at(mc).at(pos); 
						}
					}
				}

				it = find (vpoints->at(mc).begin(), vpoints->at(mc).end(), dmin);
				if(it != vpoints->at(mc).begin()) // Already at start No need to try expanding. 
				{
					int dist = std::distance(vpoints->at(mc).begin(),it);		
					monteCarlo.points.at(mc).points.at(0)=MIN;
					hc=countHelper(&monteCarlo,data,mc);

					if(hc.option!=EMPTY) // Not empty anymore. We need to reduce the size
					{
						monteCarlo.points.at(mc).points.at(0)=(vpoints->at(mc).at(dist-1));
						hc=countHelper(&monteCarlo,data,mc);

						if(hc.option!=EMPTY){
							monteCarlo.points.at(mc).points.at(0)=dmin; // We have proven this is maximal Keep this. 
						}
						else{ // maximal is somewhere between vpoints->at(mc).at(dist+1) and vpoints->at(mc).min() do binary search to find it. 
							//cout<<"TODO: Implement new search for this case. ";
							monteCarlo.points.at(mc).points.at(0)=dmin; 

							int l=1;
							int r=dist;
							
							int pos=findMin(data,vpoints,order,monteCarlo,r,l,mc);

							monteCarlo.points.at(mc).points.at(0)=(vpoints->at(mc).at(pos));

						}
					}
				}



				/*#define EMPTY 0;
				#define ONEPOINT 1
				#define BOUNDINGPOINT 2
				#define MANYPOINTS 3
				
				*/
//				if(hc.option!=0)
//					monteCarlo.points.at(mc).points.at(1)=dmax;
					

//				if(hc.option!=0) {
//					if (hc.option==1 || hc.option==2) { monteCarlo.points.at(mc).points.at(1)=hc.value;
////						cout<<"case 1";
//					}
//					else {
////						cout<<"case 2";
//						double tempax=MAX;
//						double l=MAX;
//						double s=0;
//						while(hc.option!=1 && hc.option!=BOUNDINGPOINT){
//
//							if (hc.option!=0) // guess down
//							{
//								l=tempax;
//							} 
//							else // guess up 
//							{
//								s=tempax;		
//							}
//							tempax=(l+s)/2;
//							monteCarlo.points.at(mc).points.at(1)=tempax;
//							hc=countHelper(&monteCarlo,data,mc);
//						}
//					}
//				}
			
				monteCarlo.points.at(mc).points.at(0)=MIN;
				hc=countHelper(&monteCarlo,data,mc);
				if(hc.option!=0) monteCarlo.points.at(mc).points.at(0)=dmin;


//				if(hc.option!=0) {
//					if (hc.option==1 || hc.option==2) { monteCarlo.points.at(mc).points.at(0)=hc.value;
////						cout<<"case 1";
//					}
//					else {
////						cout<<"case 2";
//						double tempax=dmin;
//						double l=dmin;
//						double s=0;
//						while(hc.option!=1 && hc.option!=BOUNDINGPOINT){
//
//							if (hc.option!=0) // guess down
//							{
//								s=tempax;
//							} 
//							else // guess up 
//							{
//								l=tempax;		
//							}
//							tempax=(l+s)/2;
//							monteCarlo.points.at(mc).points.at(0)=tempax;
//							hc=countHelper(&monteCarlo,data,mc);
//						}
//					}
//				} // need to rethink this whole section

				//monteCarlo.points.at(mc).points.at(0)=MIN;
				//hc=countHelper(&monteCarlo,data,mc);
				//if(hc.option!=0) monteCarlo.points.at(mc).points.at(0)=dmin; // revert
			}
	}
	return monteCarlo;
}




int main(){

   std::srand ( unsigned ( std::time(0) ) );

	/*
	Iris database is bound by the following points:

	4.3 to 7.9
	2 to 4.4
	1 to 6.9
	0.1 to 2.5
	*/


	vector <vector <double> > data;

	string s="";
	ifstream infile;
	//infile.open ("iris.data.csv");
	//infile.open ("wine.data.txt");
	//infile.open ("test2d.txt");
	//infile.open("pure_spectra_matrix.csv");
	infile.open("cavRigPDBMetrics.txt");

	vector <double> max;
	vector <double> min;


	bool firstpass=true;
	int dcounter=0;
	while (infile)
	{
		string s;
		if (!getline( infile, s )) break;
		//cout<<s;
		istringstream ss( s );
		vector <double> record;
		int lookingat=0;
		
		while (ss)
		{
			string s;
			if (!getline( ss, s, ' ' )) break;
			    
			   if(!s.empty() && s.find_first_not_of("-.0123456789") == std::string::npos){ // && lookingat <6
                               
				   double f=stof(s);
				   record.push_back(f);

				   if(firstpass){
					   max.push_back(f);
					   min.push_back(f);
				   }
				   else{
					   if(max.at(lookingat) < f) max.at(lookingat)=f;
					   if(min.at(lookingat) > f) min.at(lookingat)=f;
				   } 
				   lookingat++;
				   //cout<<lookingat;
			   }
			   dcounter=0;
		}
		firstpass=false;

		data.push_back( record );
	}
	if (!infile.eof())
	{
	cerr << "error reading file!\n";
	}
	cout << "Imported "<<data.size()<<" Records. Now creating empty hyper rectangles on unit hyper cube "<<endl;

    int acount=0;
	for(int i=0; i< min.size();i++){
		if(min.at(i)==max.at(i)){
			cout<<"IMPORTANT! "<<" dimension # "<<i<<" has a geometric size of 0 and its index is being replaced by that of the next attribute"<<endl;
			acount++;
		}
	}
		
	if (acount>0) cout<<" A total of  "<<acount<< " attributes have been removed from the database"<<endl;
	
	//system("pause");

	for(int i=0; i< min.size();i++){
		if(min.at(i)==max.at(i)){

			for(int j=0; j<data.size(); j++){

				data.at(j).erase(data.at(j).begin()+i);
			}

			min.erase(min.begin()+i);
			max.erase(max.begin()+i);
			i--;
		}
	}



	int D=data.at(0).size();

	static vector<int> myvector;

	for(int i=0; i<D;i++) myvector.push_back(i);
	random_shuffle ( myvector.begin(), myvector.end());

	// NORMALIZE SPACE
	cout<<"\nNormalizing space...";
	cout<<data.size();

	for(int i=0;i<data.size();i++){
		for(int j=0;j<data.at(i).size();j++){

			data.at(i).at(j)=(data.at(i).at(j)-min.at(j))/(max.at(j)-min.at(j));
			//cout<<data.at(i).at(j)<<" ";
		}
	}

	cout<<"done."<<endl;

	//system("pause");

	// End NORMALIZE SPACE

	vector< vector <double> > vpoints;
	vector<double> dpoints;
	for(int i=0;i<D;i++){
		vpoints.push_back(dpoints);
		for(int j=0;j<data.size();j++){
			vpoints.at(i).push_back(data.at(j).at(i));
		}
	}

	cout<<"Sorting along each axis ";
		for(int j=0;j<D;j++){
			std::sort(vpoints.at(j).begin(),vpoints.at(j).end());
			cout<<"sorting D="<<j<<endl;
		}
	cout<<"\nRemoving duplicates from orthogonal projections"<<endl;

	for(int i=0; i<vpoints.size(); i++){
		double pval=vpoints.at(i).at(0);
		for(int j=1; j<	vpoints.at(i).size(); j++){
		
			if (pval==vpoints.at(i).at(j))
			{
				vpoints.at(i).erase(vpoints.at(i).begin()+j);
				j--;
			}
			else{
				pval=vpoints.at(i).at(j);
			}
		}		
	}
	cout<<"Seeding random number generator"<<endl;
	srand (time(NULL));

//	int stime= clock();
// TODO: Important! The bug is caused when max=min for a given dimention. /Fixed
// In this dataset the first two dimentions are always zero. This presents unneeded complexity. Our program should remove anything like this automatically. 
// Above issue fixed. But a related bug exists for size(). We need to alter function that generates volume such that it checks to see if a dimention has size zero before multiplying it by other dimentions. 
// Fixed - we now remove any dimentions that have a size of zero because trying to map a zero width attribute onto a unit hyper cube is a little silly. 	

	cout<<"Creating monte Carlo rectangles"<<endl;
	monteCarloRectangles mcr;

	double minsizeofmax=1/data.size();
	double currentmax=0;
	int sinceLastMax=0;
	int numberExamined=0;
	int numLargeExamined=0;
	bool keepgoing=true;
	while(keepgoing){
		numberExamined++;
		random_shuffle ( myvector.begin(), myvector.end());
		HyperRectangle temp=generateMonteCarloRectangle(&data,&vpoints,&myvector,D);
		
		bool haspoints=false;//temp.containmentSearch(data); //
		if (haspoints) {			
			cout<<"We broke something... This empty rectangle is not actually empty";
		}
		mcr.insert(temp);
		double tempsize=temp.getSize();	

		if (tempsize>currentmax) {	
			cout<<"Found a larger hyper rectangle with size="<<tempsize<<endl;
			currentmax=tempsize;
			sinceLastMax=0;
		}
		else if(tempsize>minsizeofmax){
			numLargeExamined++;
			sinceLastMax++;
		}
		
		if(sinceLastMax>100000) keepgoing=false;
	}
	mcr.sort();
	cout<<"\n Sorting Maximal empty hyper rectangles and removing duplicates"<<endl;

	for(int i=0; i<mcr.hrects.size(); i++){
		for (int j=i+1; j<mcr.hrects.size(); j++){
			if(mcr.hrects.at(i).isIdenticalTo(mcr.hrects.at(j))){
				mcr.hrects.erase(mcr.hrects.begin()+j);
				j--;
			}
		}
	}

	cout<<mcr.printIfThenRules(10);

	cout<<"\nNumber of Hyper Rectangles Examined was:"<<numberExamined<<endl;
	cout<<"\nNumber of Hyper Rectangles with size > 1/n examined:"<<numLargeExamined<<endl;
	cout<<"\nNumber of times we landed in a tight cluster:"<<(numberExamined-numLargeExamined);

	cout<<"For comparison, the smallest hyper rectangle found was: ";

	
	cout<<(--mcr.hrects.end())->printIfThenRules();
	//cout<<"\nLargest found was " << mcr.getMaxEmptyRectangle().printIfThenRules();
	//cout<<"\n With a volume of "<<mcr.getMaxEmptyRectangle().getSize();
	
	int *stats = new int[D];

	for (int i = 0; i < D; i++) stats[i] = 0;

	for (int i = 0; i < 1000; i++){
		for (int j = 0; j < D; j++){
			if (mcr.hrects.at(i).points.at(j).points.at(0) != 0 || mcr.hrects.at(i).points.at(j).points.at(1) != 1){
				stats[j]++;
				
			}
			cout << mcr.hrects.at(i).points.at(j).points.at(0) << "\t" << mcr.hrects.at(i).points.at(j).points.at(1)<<"\t";
		}
		cout << endl;
	}
	cout << "Stats:"<<endl;
	for (int i = 0; i < D; i++) cout<<stats[i]<<"\t";
//	cout<<"For comparison, the smallest hyper rectangle found was: ";

	
	//cout<<(--mcr.hrects.end())->printIfThenRules();
	//cout<<"\nLargest found was " << mcr.getMaxEmptyRectangle().printIfThenRules();
	//cout<<"\n With a volume of "<<mcr.getMaxEmptyRectangle().getSize();
	

//	system("pause");
	delete[] stats;
return 0;
}
