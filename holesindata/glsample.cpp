#include <windows.h>
#include <gl/gl.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <wingdi.h>
#include "mcexp.h"
char st[5000];

struct camera
{
float orientation[3];
float position[3];

} CAMERA_X;

//#define cameraData CAMERA_X;
void updateCam(){
	
		glRotatef(CAMERA_X.orientation[0], 1,0,0);
		glRotatef(CAMERA_X.orientation[1], 0,1,0);
		glRotatef(CAMERA_X.orientation[2], 0,0,1);
		glTranslatef(-CAMERA_X.position[0],
		-CAMERA_X.position[1],
		CAMERA_X.position[2]);
}

int numtoshow=1;
float xoff=0;
float yoff=0;
float zoff=0;
bool rotation =false;
float uchange=0.01;

char * reportmer;

using namespace std;
class HyperRectangle2{
    
public:

	friend bool operator< (HyperRectangle2 &p1, HyperRectangle2 &p2);
	friend bool operator== (HyperRectangle2 &p1, HyperRectangle2 &p2);

	bool destructed;
	int D;
	set <int> *points; // points is an array that contains sets of bounding points for each D for this rectangle. 
		
	HyperRectangle2(int D){ 
		destructed=false;
		this->D=D;
		points= new set<int> [D]; 
	}


	// Functions to get drawing data.
	int getwidth(){

		int u=*this->points[0].begin();
		int l=*(--this->points[0].end());
		return l-u;
	}
	int getheight(){
		int u=*this->points[1].begin();
		int l=*(--this->points[1].end());
		return l-u;	
	}
	int getdepth(){
		int u=*this->points[2].begin();
		int l=*(--this->points[2].end());
		return l-u;	
	}

	int getx(){
		return *this->points[0].begin();
	
	}
	int gety(){
		return *this->points[1].begin();	
	}
	int getz(){
		return *this->points[2].begin();	
	}


	int getSize(){
		int end=*(--points[0].end());
		int size=end - *points[0].begin();
		for(int i=1;i<D;i++){
			end=*(--points[i].end());
			size=size*(end - *points[i].begin());
		}
		return size;
	}
	int containedIn( int pt[]){
		if (this->destructed==true) return 0;
		int start;
		int end;
		bool bounding=false;
		for( int i=0; i< this->D; i++)
		{
			int p=pt[i];
			start=*this->points[i].begin();
			end=*(--this->points[i].end()); // Dereference the pointer to the element before end() 

			// is p between points[i] and points[i]+hyperplanes[i] inclusive?
			//if (p< this->points[i] || p> this->points[i]+this->hyperPlanes[i]) return 0;
			if (p<start || p>end) return 0;
			else {
				if(pt[i]==start || pt[i]==end) bounding=true; 
			}
		}

		if(bounding==true) return 2;
		else return 1;
	}

	void insertBoundingPoint(int pt[]){
		for( int i=0; i< this->D; i++)
		{
			this->points[i].insert(pt[i]);
		}
	}

	string toString(){
		string s="";

		for(int i=0;i<this->D;i++){
			s=s+" D="+std::to_string((_ULonglong)i)+":";
			for (std::set<int>::iterator it = this->points[i].begin(); it != this->points[i].end(); ++it){
				//s=s+" "+ it+",";
				int k=*it;
				s=s+" "+std::to_string((_ULonglong)k);
			}
		}
		return s;
	}

	~HyperRectangle2(){
/*		
		cout<<"Hyper Rectangle Destructor Called"<<endl;

		for (int i=0;i<D;i++){
			this->points[i].clear();
			this->points[i].erase(this->points[i].begin(),this->points[i].end());	
		}

		delete[] this->points;
	*/	
	}
};

bool operator< (HyperRectangle2 &p1, HyperRectangle2 &p2){
	return (p1.getSize() > p2.getSize());
}
bool operator== (HyperRectangle2 &p1, HyperRectangle2 &p2){
	return (p1.getSize() == p2.getSize());
}


class mhrCollector2{


private:
	vector <HyperRectangle2> points;
public:
	mhrCollector2(HyperRectangle2 *initial){
		
		points.push_back(*initial);

	}
	void insert( HyperRectangle2 T){
		points.push_back(T);		
	}
	void addPoint(vector <int> pt){
		addPoint(pt.data());
	
	}
	void addPoint(int pt[]){
		//Containment Search:
		int choice=0;
		vector <HyperRectangle2> temp;
		for (std::vector<HyperRectangle2>::iterator it = points.begin(); it != points.end(); ++it){

			 choice=(int)it->containedIn(pt);
			 //cout<<choice;
			 if(choice ==2 && !it->destructed) it->insertBoundingPoint(pt);
			 if(choice==1 && !it->destructed){ // We found a point within a MHR, now we need to split it.
				//1. Get sets from it. 
				//2. Remove it from vector. 
				//
				it->insertBoundingPoint(pt);

				for(int i = 0; i<it->D;i++){
					
					// Split larger parts
					 std::set<int>::iterator it2 = it->points[i].end();
					 std::set<int>::iterator it3=it->points[i].find(pt[i]);
					 
					 HyperRectangle2 temp1(it->D); //split 1
					 HyperRectangle2 temp2(it->D); //split 2

					 //cout<<"test:"<<*it3<<endl;
					 set<int> t1;
					 int end=*(--it2);

					 temp1.points[i].insert(end);
					 //cout<<"\n inserted point "<<end<<endl;
						 
					 while(*it3 < end){
						 int temp=*it3;
						 temp1.points[i].insert(temp);
						// cout<<"\n inserted point "<<temp<<endl;
						 it3++; 						 
					 }
					 for(int j=0;j< it->D; j++){
						 if (i!=j){						 
							 temp1.points[j].insert(it->points[j].begin(),it->points[j].end());
							 temp1.points[j].erase( temp1.points[j].find(pt[j]));
						 }
					 }

					 //cout<<temp1.toString();
					 // Split smaller parts
					 it2 = it->points[i].begin();
					 it3=it->points[i].find(pt[i]);
					 
					 //cout<<"test:"<<*it3<<endl;
					 set<int> t2;
					 end=*it3;

					 temp2.points[i].insert(end);

					 //cout<<"\n inserted point "<<end<<endl;
						 
					 while(*it2 < end){
						 int temp=*it2;
						 temp2.points[i].insert(temp);
						 //cout<<"\n inserted point "<<temp<<endl;
						 it2++; 
					 }

					 for(int j=0;j< it->D; j++){
						 if (i!=j){						 
							 temp2.points[j].insert(it->points[j].begin(),it->points[j].end());
							 temp2.points[j].erase( temp2.points[j].find(pt[j]));
						 }					 
					 }
					 //cout<<temp2.toString();
					 //system("pause");

					 //BigEnough step:
					 if (temp1.getSize()> 2) temp.push_back(temp1);
					 if (temp2.getSize()> 2) temp.push_back(temp2);
					 //temp.insert(temp1);
					 //temp.insert(temp2);
				 }

				//for (std::vector<HyperRectangle2>::iterator it3 = temp.begin(); it3 != temp.end(); ++it3){
					//cout<<it3->toString();
				//}
				it->destructed=true;
			}	
		}
		this->points.insert(this->points.end(),temp.begin(),temp.end());

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
		for (std::vector<HyperRectangle2>::iterator it = this->points.begin(); it != this->points.end(); ++it){
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
	void drawMHR2Ds(int limit=0){
		float zoffset=0;
		int count=0;
		int d=0;
//		int k=0;
		string tmp="";
		stringstream ss;
		for (std::vector<HyperRectangle2>::iterator it = this->points.begin(); it != this->points.end(); ++it){
			if(!it->destructed){ 

				float x=((float)it->getx()/20)-.5 ;
				float y=((float)it->gety()/20)-.5 ;
				float w=(float)it->getwidth()/20;
				float h=(float)it->getheight()/20;
				if (count%9==0) {glColor4f(1,0, 0, .3);}
				if (count%9==1) {glColor4f(0,1, 0, .3);}
				if (count%9==2) {glColor4f(0,0, 1, .3);}

				if (count%9==3) {glColor4f(1,1, 0, .3);}
				if (count%9==4) {glColor4f(.4,.1, 1, .3);}
				if (count%9==5) {glColor4f(0,1, 1, .3);}
				if (count%9==6) {glColor4f(0,0, 0, .3);}

				if (count%9==7) {glColor4f(1,.6, 0, .3);}
				if (count%9==8) {glColor4f(.6,0, .6, .3);}

				y=y+yoff;
				glVertex3f(x+xoff,y,0+zoffset);
				glVertex3f(x+w+xoff,y,+zoffset);

				glVertex3f(x+w+xoff,y+h,+zoffset);
				glVertex3f(x+xoff,y+h,+zoffset);
				
				glVertex3f(x+xoff,y,+zoffset);
				glVertex3f(x+xoff,y+h,+zoffset);

				glVertex3f(x+xoff+w,y+h,+zoffset);
				glVertex3f(x+xoff+w,y,+zoffset);
				//cout<<"Volume: "<<it->getSize()<<" Points: " <<it->toString()<<endl;
				count++;
				if (limit !=0 && count==limit) break;
			}else{
			d++;
			
			}		
		}
	
	}
	void drawMHR3Ds(int limit=0){
		float zoffset=0;
		int count=0;
		int d=0;
		stringstream ss;
//		int k=0;
		for (std::vector<HyperRectangle2>::iterator it = this->points.begin(); it != this->points.end(); ++it){
			if(!it->destructed){ 

				float x=((float)it->getx()/20)-.5 ;
				float y=((float)it->gety()/20)-.5 ;
				float z=((float)it->getz()/20)-.5 ;

				float w=(float)it->getwidth()/20;
				float h=(float)it->getheight()/20;
				float depth=(float)it->getdepth()/20;

				if (count%4==0) {glColor4f(1,0, 0, .3);}
				if (count%4==1) glColor4f(0,1, 0, .3);
				if (count%4==2) glColor4f(0,0, 1, .3);
				if (count%4==3) glColor4f(1,0, 1, .3);

				y=y+yoff;
				x=x+xoff;
				z=z+zoff;

				//w=w+1*zoff;
				//h=h+1*zoff;
				//depth=depth+1*zoff;


				glVertex3f(x,y,z);
				glVertex3f(x+w,y,z);
				glVertex3f(x+w,y+h,z);
				glVertex3f(x,y+h,z);
				glVertex3f(x,y,z);
				glVertex3f(x,y+h,z);
				glVertex3f(x+w,y+h,z);
				glVertex3f(x+w,y,z);

				glVertex3f(x,y,z);
				glVertex3f(x,y,z+depth);
				glVertex3f(x+w,y,z);
				glVertex3f(x+w,y,z+depth);
				glVertex3f(x+w,y+h,z);
				glVertex3f(x+w,y+h,z+depth);
				glVertex3f(x,y+h,z);
				glVertex3f(x,y+h,z+depth);

				glVertex3f(x,y,z+depth);
				glVertex3f(x+w,y,z+depth);
				glVertex3f(x+w,y+h,z+depth);
				glVertex3f(x,y+h,z+depth);
				glVertex3f(x,y,z+depth);
				glVertex3f(x,y+h,z+depth);
				glVertex3f(x+w,y+h,z+depth);
				glVertex3f(x+w,y,z+depth);
				
				
				if (count <10) ss<<"Volume: " <<it->getSize()<<" Points: " <<it->toString()<<"\n";
		//		<<"Volume: " <<it->getSize()<<" Points: " <<it->toString()<<endl;

				count++;
				if (limit !=0 && count==limit) break;
			}else{
			d++;
			
			}		
		}
		string s= ss.str();
		strcpy_s(st,s.c_str());				
	}
};

// Function Declarations

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

// WinMain




// testing
void drawMHR3Ds(vector <HyperRectangle> &ht, int limit=0){
		float zoffset=0;
		int count=0;
		int d=0;
		stringstream ss;
//		int k=0;
		for (std::vector<HyperRectangle>::iterator it = ht.begin(); it != ht.end(); ++it){
			if(!it->destructed){ 

				float x=((float)it->getx())-.5 ;
				float y=((float)it->gety())-.5 ;
				float z=((float)it->getz())-.5 ;

				float w=(float)it->getwidth();
				float h=(float)it->getheight();
				float depth=(float)it->getdepth();

				if (count%4==0) {glColor4f(1,0, 0, .3);}
				if (count%4==1) glColor4f(0,1, 0, .3);
				if (count%4==2) glColor4f(0,0, 1, .3);
				if (count%4==3) glColor4f(1,0, 1, .3);

				y=y+yoff;
				x=x+xoff;
				z=z+zoff;

				//w=w+1*zoff;
				//h=h+1*zoff;
				//depth=depth+1*zoff;


				glVertex3f(x,y,z);
				glVertex3f(x+w,y,z);
				glVertex3f(x+w,y+h,z);
				glVertex3f(x,y+h,z);
				glVertex3f(x,y,z);
				glVertex3f(x,y+h,z);
				glVertex3f(x+w,y+h,z);
				glVertex3f(x+w,y,z);

				glVertex3f(x,y,z);
				glVertex3f(x,y,z+depth);
				glVertex3f(x+w,y,z);
				glVertex3f(x+w,y,z+depth);
				glVertex3f(x+w,y+h,z);
				glVertex3f(x+w,y+h,z+depth);
				glVertex3f(x,y+h,z);
				glVertex3f(x,y+h,z+depth);

				glVertex3f(x,y,z+depth);
				glVertex3f(x+w,y,z+depth);
				glVertex3f(x+w,y+h,z+depth);
				glVertex3f(x,y+h,z+depth);
				glVertex3f(x,y,z+depth);
				glVertex3f(x,y+h,z+depth);
				glVertex3f(x+w,y+h,z+depth);
				glVertex3f(x+w,y,z+depth);
				
				
				if (count <10) ss<<"Volume: " <<it->getSize()<<" Points: " <<it->toString()<<"\n";
		//		<<"Volume: " <<it->getSize()<<" Points: " <<it->toString()<<endl;

				count++;
				if (limit !=0 && count==limit) break;
			}else{
			d++;
			
			}		
		}
		string s= ss.str();
		strcpy_s(st,s.c_str());				
	}


void drawMHR2Ds(vector <HyperRectangle> &ht, int limit=0 ){
		float zoffset=0;
		int count=0;
		int d=0;
//		int k=0;
		string tmp="";
		stringstream ss;

		double scale=1;
		for (std::vector<HyperRectangle>::iterator it = ht.begin(); it != ht.end(); ++it){
			if(!it->destructed){ 

				float x=((float)it->getx()/scale)-.5 ;
				float y=((float)it->gety()/scale)-.5 ;
				float w=(float)it->getwidth()/scale;
				float h=(float)it->getheight()/scale;
				if (count%9==0) {glColor4f(1,0, 0, .3);}
				if (count%9==1) {glColor4f(0,1, 0, .3);}
				if (count%9==2) {glColor4f(0,0, 1, .3);}

				if (count%9==3) {glColor4f(1,1, 0, .3);}
				if (count%9==4) {glColor4f(.4,.1, 1, .3);}
				if (count%9==5) {glColor4f(0,1, 1, .3);}
				if (count%9==6) {glColor4f(0,0, 0, .3);}

				if (count%9==7) {glColor4f(1,.6, 0, .3);}
				if (count%9==8) {glColor4f(.6,0, .6, .3);}

				y=y+yoff;
				glVertex3f(x+xoff,y,0+zoffset);
				glVertex3f(x+w+xoff,y,+zoffset);

				glVertex3f(x+w+xoff,y+h,+zoffset);
				glVertex3f(x+xoff,y+h,+zoffset);
				
				glVertex3f(x+xoff,y,+zoffset);
				glVertex3f(x+xoff,y+h,+zoffset);

				glVertex3f(x+xoff+w,y+h,+zoffset);
				glVertex3f(x+xoff+w,y,+zoffset);
				//cout<<"Volume: "<<it->getSize()<<" Points: " <<it->toString()<<endl;
				count++;
				if (limit !=0 && count==limit) break;
			}else{
			d++;
			
			}		
		}
	
	}


/// end testing











void drawcords(){
	float y=yoff-.5;
	float x=xoff-.5;
	float z=zoff-.5;

	float w=0.025;
	float h=1;
	float depth=1;


	// x Cordinate map
	for(int i=0; i <40; i++){

		if(i%2){
				glVertex3f(x,y,z);
				glVertex3f(x+w,y,z);

				glVertex3f(x,y+h,z);
				glVertex3f(x+w,y+h,z);

				glVertex3f(x,y,z+depth);
				glVertex3f(x+w,y,z+depth);

				glVertex3f(x,y+h,z+depth);
				glVertex3f(x+w,y+h,z+depth);
		}

						x=x+0.025;
//		y=y+0.05;
//		z=z+0.05;

	}
	// y cord map

	h=0.025;
	x=xoff-.5;
	w=1;
	for(int i=0; i<40;i++){
		if(i%2){
			glVertex3f(x,y,z);
			glVertex3f(x,y+h,z);
			
			glVertex3f(x+w,y+h,z);
			glVertex3f(x+w,y,z);

			glVertex3f(x+w,y,z+depth);
			glVertex3f(x+w,y+h,z+depth);
			
			glVertex3f(x,y,z+depth);
			glVertex3f(x,y+h,z+depth);


		}

				y=y+0.025;
	}

	
	h=1;
	x=xoff-.5;
	y=yoff-0.5;
	w=1;
	depth=0.025;
	for(int i=0; i<40;i++){
		if(i%2){
				glVertex3f(x,y,z);
				glVertex3f(x,y,z+depth);

				glVertex3f(x+w,y,z);
				glVertex3f(x+w,y,z+depth);

				glVertex3f(x+w,y+h,z);
				glVertex3f(x+w,y+h,z+depth);

				glVertex3f(x,y+h,z);
				glVertex3f(x,y+h,z+depth);


		}
				z=z+0.025;
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int iCmdShow)
{
	WNDCLASS wc;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL quit = FALSE;
	float theta = 0.0f;
	
	// register window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MaxRectangles";
	RegisterClass( &wc );
	
	// create main window
	hWnd = CreateWindow( 
		"MaxRectangles", "Max Empty HyperRectangles", 
		WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
		0, 0, 1200, 800,
		NULL, NULL, hInstance, NULL );
	
	// enable OpenGL for the window
	EnableOpenGL( hWnd, &hDC, &hRC );
	
//
//    vector <vector <int> > data;
//
//	string s="";
//	ifstream infile;
//	infile.open ("2LZM.pdb.txt");
//	while (infile)
//	{
//	string s;
//	if (!getline( infile, s )) break;
//
//	istringstream ss( s );
//	vector <int> record;
//
//	while (ss)
//	{
//		string s;
//		if (!getline( ss, s, ',' )) break;
//		record.push_back( atoi(s.c_str()) );
//	}
//
//	data.push_back( record );
//	}
//	if (!infile.eof())
//	{
//		//cerr << "error reading file!\n";
//		MessageBox(NULL,"Error","Error reading file",0);
//	}else MessageBox(NULL,"Sucessfully imported test.csv","Import Success",0);
//
//	HyperRectangle2 h2(3);
//	
//	// form an initial hyper rectangle bound by 0,0 and 20,20,
//	h2.points[0].insert(0);
//	h2.points[1].insert(0);
//	h2.points[2].insert(0);
//	h2.points[0].insert(20);
//	h2.points[1].insert(20);
//	h2.points[2].insert(20);
//
//
//	mhrCollector2 mhrs(&h2);
////	mhrs.addPoint(p1);
////	mhrs.addPoint(p2);
//
//	for (std::vector< vector <int>  >::iterator it = data.begin(); it != data.end(); ++it){
//		mhrs.addPoint(*it);
//	//	cout<<endl;
//	//	for (std::vector<int>::iterator it2 = it->begin(); it2 != it->end(); ++it2){
//			//cout<<*it2<<" ";
//	//	}
//	}	
//
//
//	mhrs.sort();
//

	MEHRExperimenter exp1;
	monteCarloRectangles mcr=exp1.run("dataimproved.txt",100,"\t",3);
//	mcr.distinctify();
	


	// program main loop
	while ( !quit )
	{
		
		// check for messages
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
		{
			
			// handle or dispatch messages
			if ( msg.message == WM_QUIT ) 
			{
				quit = TRUE;
			} 
			else 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			
		} 
		else 
		{
			// OpenGL animation code goes here
			    glEnable( GL_POINT_SMOOTH );
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glPointSize( 6.0 );

			glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
			
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//	glClear( GL_POLYGON );
			
			glPushMatrix();
			
			if (rotation) glRotatef( theta, 0.1f, 0.2f, 0.4f );

			updateCam();

			//glBegin( GL_TRIANGLE_FAN );
			//glBegin( GL_LINE_STRIP );

			glPointSize(5); 
			glBegin(GL_POINTS);
			
			glColor4f(0,0,0,1);
//			for (std::vector< vector <int>  >::iterator it = data.begin(); it != data.end(); ++it){
								
//					glVertex3f((float)it->at(0)/20-.5,(float)it->at(1)/20-.5,(float)it->at(2)/20-.5);
			//}
		for(int i=0; i<exp1.data.size();i++)
		{
			glVertex3f(exp1.data.at(i).at(0)-.5,exp1.data.at(i).at(1)-.5,exp1.data.at(i).at(2)-.5);

		}

			glEnd();

			glBegin( GL_LINES);

//			glBegin( GL_QUADS);
//			glBegin( GL_POLYGON);


			glColor4f(0,0,0,1);
				
			drawcords();
			glEnd();
			glBegin( GL_LINES);

			drawMHR3Ds(mcr.hrects,numtoshow);
			//mhrs.drawMHR3Ds(numtoshow);

			glEnd();
LPGLYPHMETRICSFLOAT agmf[256]; 
 
// Make hglrc the calling thread's current rendering context  
//wglMakeCurrent(hdc, hglrc); 
 
// create display lists for glyphs 0 through 255 with 0.1 extrusion  
// and default deviation. The display list numbering starts at 1000  
// (it could be any number)  
wglUseFontOutlines(hDC, 0, 255, 1000, 0.0f, 0.1f,  WGL_FONT_POLYGONS, *agmf); 
 
// Set up transformation to draw the string  
glLoadIdentity(); 
 
// Display a string  
glListBase(1000); // Indicates the start of display lists for the glyphs  
// Draw the characters in a string  
glCallLists(24, GL_UNSIGNED_BYTE, "Hello Windows OpenGL World.");
glLoadIdentity(); 
glTranslatef(0.0f, 0.0f, -5.0f); 
glScalef(2.0f, 2.0f, 2.0f); 



			glEnd();
			glPopMatrix();
			
			SwapBuffers( hDC );
			
			theta += 0.01f;
			
		}
		
	}
	
	// shutdown OpenGL
	DisableOpenGL( hWnd, hDC, hRC );
	
	// destroy the window explicitly
	DestroyWindow( hWnd );
	
	return msg.wParam;
	
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	
	case WM_CHAR:
   
     switch (wParam)
     {

	 case 't':
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		

		 break;
	 case 'r':
		CAMERA_X.orientation[1]=0;
		CAMERA_X.orientation[2]=0;
		CAMERA_X.orientation[0] +=.06;
		updateCam();

		 break;
	 case 'R':
		 CAMERA_X.orientation[0] -= .01;
		 CAMERA_X.orientation[1]=0;
		 CAMERA_X.orientation[2]=0;
		 updateCam();
		 break;

	 case 'e':
		CAMERA_X.orientation[1] +=.01;
		CAMERA_X.orientation[0] =0;
		CAMERA_X.orientation[2] =0;
		updateCam();

		 break;
	 case 'E':
		 CAMERA_X.orientation[1] -= .01;
		 CAMERA_X.orientation[0] =0;
		 CAMERA_X.orientation[2] =0;

		 updateCam();
		 break;

	 case 'w':
		CAMERA_X.orientation[1] =0;
		CAMERA_X.orientation[0] =0;
		CAMERA_X.orientation[2] +=0.1;
		updateCam();

		 break;
	 case 'W':
		 CAMERA_X.orientation[1] =0;
		 CAMERA_X.orientation[0] =0;
		 CAMERA_X.orientation[2] -= .01;

		 updateCam();
		 break;

	 case 'u':
		 updateCam();
		 break;

	 case '+':
		 numtoshow++;
          break ;

	 case '-':
		 numtoshow--;
         break ;
	 case 'z':
		 zoff=zoff+uchange;;
          break ;
	 case 'Z':
		 zoff=zoff-uchange;
         break ;
	 case 'x':
		 xoff=xoff+uchange;
          break ;
	 case 'X':
		 xoff=xoff-uchange;
         break ;
	 case 'y':
		yoff=yoff+uchange;
          break ;
	 case 'Y':
		yoff=yoff-uchange;
         break ;
	 case 'a':
		 rotation=false;
		 break;
	 case 'A':
		 rotation=true;
		 break;
	 case 'm':
		 MessageBox(NULL,st,"hi",0);
     return 0 ;
	 }
//	case WM_MOUSEWHEEL:
//		
//		return 0;
	case WM_CREATE:
		return 0;
		
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
		
	case WM_DESTROY:
		return 0;
		
	case WM_KEYDOWN:
		switch ( wParam )
		{
			
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
	
		
		}
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
			
	}
	
}

// Enable OpenGL

void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	
	// get the device context (DC)
	*hDC = GetDC( hWnd );
	
	// set the pixel format for the DC
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat( *hDC, &pfd );
	SetPixelFormat( *hDC, format, &pfd );
	
	// create and enable the render context (RC)
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );
	
}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}
