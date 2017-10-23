#include "mcexp.h"

void monteCarloRectangles::insert(HyperRectangle h){
		hrects.push_back(h);
	}
void monteCarloRectangles::sort(){	
		std::sort(this->hrects.begin(), this->hrects.end()); 
	}

HyperRectangle monteCarloRectangles::getMaxEmptyRectangle(){
	
		return hrects.at(0);
	}
void monteCarloRectangles::distinctify(){
		hrects.erase( unique( hrects.begin(), hrects.end() ), hrects.end() );
		return;
	}
string monteCarloRectangles::printIfThenRules(int limit=0){	
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
		i++;
	}
	return hc;
}

bool isGrowable(HyperRectangle monteCarlo, int dimension, int direction, vector <vector <double> > *data,vector <vector <double> > *vpoints, int vindex){

	if (vpoints->at(dimension).size() <= vindex+direction) return false; // Hit bounding box. Return false. 
	if (vindex+direction<0) return false; // Hit bounding box. Return false. 

	int m=0;
	if (direction >0) m=1;
	else m=0;
	if (direction==0) direction=-1;
	
	monteCarlo.points.at(dimension).points.at(m)= vpoints->at(dimension).at(vindex+direction);	
	HC hc=countHelper(&monteCarlo,data,dimension);
	if (hc.option!=EMPTY){ 
		return false;
	}
	else{
		return true;
	}
}

bool allfull(int* fullup, int* fulldown,int D){
	for(int i=0;i<D;i++){
		if(fullup[i]!=1) return false;
		if(fulldown[i]!=1) return false;
	}
	return true;
}

// Intended for use with random growth strategy. Called when random index results in non empty rectangle. For efficiency (to avoid a second sweep of all points) we assume any rectangle passed is non empty.
int shrinkTillEmpty(vector <vector <double> > *data, vector <vector <double> > *vpoints, HyperRectangle &monteCarlo, int D, int direction, int index){
	 
	bool growable = false;	
	int pindex = index;
	while (growable == false){
		if(direction > 0) index--;
		else index++;
		if (index >= vpoints->at(D).size()) return vpoints->at(D).size()-1;

		monteCarlo.points.at(D).points.at(direction) = vpoints->at(D).at(index);
		growable = isGrowable(monteCarlo, D, direction, data, vpoints, index);
		if (growable != true) pindex = index;
	}
	monteCarlo.points.at(D).points.at(direction) = vpoints->at(D).at(pindex);
	return pindex;
}

void growRectangleRandomly(vector <vector <double> > *data, vector <vector <double> > *vpoints, vector <int> *order, HyperRectangle &monteCarlo, int * high, int *low){
	
	/*
	For each k in D. 
		if up 
			monteCarlo[k].upper= rand( between k.i and vindex.at(k).size()-1)
			if(isnotempty) shrinktillempty() return; 
		if down
			monteCarlo[k].upper= rand( between 0 and k.i)
			if (isnotempty) shrinktillempty() return; 
*/
	int D = monteCarlo.D;

	int *fullup = new int[D];
	int *fulldown = new int[D];
	for (int i = 0; i< D; i++) { fullup[i] = 0; fulldown[i] = 0; }

	while (!allfull(fullup, fulldown, D)){
		for (int i = 0; i < (2 * vpoints->size()); i++){
			int vindex = 0;
			if (i % 2 == 1){
				if (fullup[order->at(i / 2)] != 1){
					vindex = high[order->at(i / 2)];
					if (vindex == vpoints->at(order->at(i / 2)).size() - 1) fullup[order->at(i / 2)] = 1;
					else{
						int w = vpoints->at(order->at(i / 2)).size() - vindex;
						int r = rand() % w;
						int newindex = r + vindex;

						monteCarlo.points.at(order->at(i / 2)).points.at(1) = vpoints->at(order->at(i / 2)).at(newindex);

						high[order->at(i / 2)] = newindex;

						if (monteCarlo.containmentSearch(*data)){
							int nindex = shrinkTillEmpty(data, vpoints, monteCarlo, order->at(i / 2), (i + 2) % 2, newindex);
							fullup[order->at(i / 2)] = 1;
							high[order->at(i / 2)] = nindex;
						}
					}
				}
			}
			else{
				if (fulldown[order->at(i / 2)] != 1){
					vindex = low[order->at(i / 2)];
					if (vindex < 1) fulldown[order->at(i / 2)] = 1;
					else{
						int w = vindex;
						int r = rand() % w;
						int newindex = r;

						monteCarlo.points.at(order->at(i / 2)).points.at(0) = vpoints->at(order->at(i / 2)).at(newindex);
						low[order->at(i / 2)] = newindex;

						if (monteCarlo.containmentSearch(*data)){
							int nindex = shrinkTillEmpty(data, vpoints, monteCarlo, order->at(i / 2), (i + 2) % 2, newindex);
							fulldown[order->at(i / 2)] = 1;
							low[order->at(i / 2)] = nindex;
						}
					}
				}
			}
			//	shrinkTillEmpty(data, vpoints, monteCarlo, i / 2, (i+2) % 2, vindex);
			//monteCarlo.points.at(i / 2).points.at(i % 2) = i % 2;
		}
	}
	string sss = monteCarlo.toString();

	return;
//	int D = monteCarlo.D;

//	int *fullup = new int[D];
//	int *fulldown = new int[D];
//	for (int i = 0; i< D; i++) { fullup[i] = 0; fulldown[i] = 0; }

	while (!allfull(fullup, fulldown, D)){
		for (int i = 0; i<D; i++){
			unsigned int flipcoin = rand() % 2;

			if (flipcoin == 1 && fullup[order->at(i)] == 0){ //try growing up on D.

				int dist = distance((vpoints->at(order->at(i)).begin() + high[order->at(i)]), vpoints->at(order->at(i)).end());
				if (dist == 0) {

					fullup[order->at(i)] = 1;
				}
				else{
					cout << "excuse for breakpoint";
					int randincrease = (rand() % dist) + high[order->at(i)];
					high[order->at(i)] = randincrease;

					monteCarlo.points.at(order->at(i)).points.at(1) = vpoints->at(order->at(i)).at(high[order->at(i)]);

					if (monteCarlo.containmentSearch(*data))
					{

						shrinkTillEmpty(data, vpoints, monteCarlo, order->at(i), 1, high[order->at(i)]);
						fullup[order->at(i)] = 1;
					}

				}
			}
			else if (fulldown[order->at(i)] == 0)
			{ // try growing down on D


				int dist = distance(vpoints->at(order->at(i)).begin(), (vpoints->at(order->at(i)).begin() + low[order->at(i)]));
				if (dist == 0) {
					fulldown[order->at(i)] = 1;
				}
				else{

					int randDecrease = (rand() % dist)+1;
					low[order->at(i)] = low[order->at(i)] - randDecrease;
					monteCarlo.points.at(order->at(i)).points.at(0) = vpoints->at(order->at(i)).at(low[order->at(i)]);

					if (monteCarlo.containmentSearch(*data))
					{

						shrinkTillEmpty(data, vpoints, monteCarlo, order->at(i), 0, low[order->at(i)]);
						fulldown[order->at(i)] = 1;
					}
				}
			}
			else{ // nothing we can do here. 
			}
		}
	}
	return;
}

void growRectangle(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, HyperRectangle &monteCarlo, int * high, int *low){
	
	int D = monteCarlo.D;
	
	int *fullup =new int[D];
	int *fulldown =new int[D];
	for(int i=0; i< D;i++) {fullup[i]=0; fulldown[i]=0;}

	while(!allfull(fullup,fulldown,D)){
		for(int i=0; i<D;i++){
			unsigned int flipcoin=rand()%2;

			if(flipcoin==1&&fullup[	order->at(i)]==0){ //try growing up on D.
				if(isGrowable(monteCarlo,order->at(i),1,data,vpoints,high[	order->at(i)])) {
					monteCarlo.points.at(	order->at(i)).points.at(1)= vpoints->at(	order->at(i)).at(high[	order->at(i)]+1);
					high[	order->at(i)]++;
				}
				else fullup[order->at(i)]=1;
			}
			else if(fulldown[order->at(i)]==0)
			{ // try growing down on D
			
				if(isGrowable(monteCarlo,order->at(i),-1,data,vpoints,low[order->at(i)])) {
					monteCarlo.points.at(order->at(i)).points.at(0)= vpoints->at(order->at(i)).at(low[	order->at(i)]-1);
					low[order->at(i)]--;
				}
				else fulldown[	order->at(i)]=1;
			}
			else{ // nothing we can do here. 
			}
		}
	}
	return;
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
}



int findMin(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, HyperRectangle monteCarlo, int upperbound, int lowerbound, int mc){

	HC hc;

	bool foundit=false;

	for(int i= lowerbound; i<upperbound; i++){ // recursive solution is broken. Temporarily use this naive solution. (n vs lg n)
		monteCarlo.points.at(mc).points.at(0)= vpoints->at(mc).at(i);
		hc=countHelper(&monteCarlo,data,mc);
		if (hc.option!=EMPTY) return i-1;
	}
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


HyperRectangle generateMonteCarloRectangle(vector <vector <double> > *data,vector <vector <double> > *vpoints, vector <int> *order, int D, int strategy){
	
	HyperRectangle monteCarlo(D);
	vector <double> mcpoint;

	int *high= new int[monteCarlo.D];
	int *low =new int[monteCarlo.D];


	for(int i=0; i<D;i++){
		
		int mc=order->at(i);
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
			
			high[mc]=distance(vpoints->at(mc).begin(),it);
			low[mc]=distance(vpoints->at(mc).begin(),it2);


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

	if (strategy == 2){

		growRectangle(data, vpoints, order, monteCarlo, high, low);

		return monteCarlo;
	}
	if (strategy == 3){
		
		growRectangleRandomly(data, vpoints, order, monteCarlo, high, low);

		return monteCarlo;
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


				monteCarlo.points.at(mc).points.at(0)=MIN;
				hc=countHelper(&monteCarlo,data,mc);
				if(hc.option!=0) monteCarlo.points.at(mc).points.at(0)=dmin;
			}
	}

	delete[] low;
	delete[] high;
	return monteCarlo;
}


MEHRExperimenter::MEHRExperimenter(){
		   std::srand ( unsigned ( std::time(0) ) );
		   s="";
	}
	
	void MEHRExperimenter::loadfile(string filename, string delimiter){
	
			infile.open(filename);
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
					if (!getline(ss, s, delimiter.c_str()[0])) break;
			    
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
			if (acount>0) cout<<" A total of  "<<acount<< " attributes have been removed from the database"<<endl;\
			

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


	}

	void MEHRExperimenter::normalizeSpace(){
		cout<<"\nNormalizing space...";
		cout<<data.size();

		for(int i=0;i<data.size();i++){
			for(int j=0;j<data.at(i).size();j++){

			data.at(i).at(j)=(data.at(i).at(j)-min.at(j))/(max.at(j)-min.at(j));
			//cout<<data.at(i).at(j)<<" ";
			}
		}

		cout<<"done."<<endl;
	
	}

	void MEHRExperimenter::prepareProjections(){

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
	}

	monteCarloRectangles MEHRExperimenter::runMonteCarlo(int stop,int strategy){
		srand (time(NULL));
		monteCarloRectangles mcr;

		double minsizeofmax=1/data.size();
		double currentmax=0;
		int sinceLastMax=0;
		int numberExamined=0;
		int numLargeExamined=0;
		bool keepgoing=true;

		static vector<int> myvector;

		for(int i=0; i<D;i++) myvector.push_back(i);
		random_shuffle ( myvector.begin(), myvector.end());

		while(keepgoing){
			numberExamined++;
			random_shuffle ( myvector.begin(), myvector.end());
			HyperRectangle temp=generateMonteCarloRectangle(&data,&vpoints,&myvector,D,strategy);
		
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
			if(sinceLastMax>stop) keepgoing=false;
		}

		mcr.numberExamined=numberExamined;
		mcr.numberLargeExamined=numLargeExamined;

		return mcr;
	}

	monteCarloRectangles MEHRExperimenter::run(string filename, int stopcondition, string delimiter, int strategy=1){
		
		loadfile(filename, delimiter);
		D=data.at(0).size();	
		normalizeSpace();
		prepareProjections();
		monteCarloRectangles mcr= runMonteCarlo(stopcondition,strategy);
		mcr.sort();
		return mcr;
	}