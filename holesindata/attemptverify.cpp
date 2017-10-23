#include "mcexp.h"



int main(int argc, char *argv[]){

	string filename = "cavRigPDBMetrics.txt";
	int stopcondition = 100;
	string delimiter = " ";
	int strategy = 1;

	if (argc >1 ){
		filename = argv[1];
		cout << "filename" << filename;
	}
	if (argc > 2){
		stopcondition = stoi(argv[2]); // note: stoi is a c++11 feature. May not be supported on older compilers.
	}
	if (argc > 3)
	{
		delimiter = argv[3];
	}
	if (argc > 4)
	{
		strategy = stoi(argv[4]);
	}

	MEHRExperimenter exp1;
	monteCarloRectangles mcr=exp1.run(filename,stopcondition,delimiter,strategy);

	cout<<"\nNumber of Hyper Rectangles Examined was:"<<mcr.numberExamined<<endl;
	cout<<"\nNumber of Hyper Rectangles with size > 1/n examined:"<<mcr.numberLargeExamined<<endl;
	cout<<"\nNumber of times we landed in a tight cluster:"<<(mcr.numberExamined-mcr.numberLargeExamined)<<endl;
	//cout<<"For comparison, the smallest hyper rectangle found was: ";
	mcr.distinctify();
	cout << mcr.printIfThenRules(20);

//	cout<<(--mcr.hrects.end())->printIfThenRules();
	//cout<<"\nLargest found was " << mcr.getMaxEmptyRectangle().printIfThenRules();
	//cout<<"\n With a volume of "<<mcr.getMaxEmptyRectangle().getSize();

	//int D= exp1.D;
	//int *stats = new int[exp1.D];

	//for (int i = 0; i < D; i++) stats[i] = 0;

	//for (int i = 0; i < 5; i++){
	//	for (int j = 0; j < D; j++){
	//		if (mcr.hrects.at(i).points.at(j).points.at(0) != 0 || mcr.hrects.at(i).points.at(j).points.at(1) != 1){
	//			stats[j]++;
	//		}
	//		cout << mcr.hrects.at(i).points.at(j).points.at(0) << "\t" << mcr.hrects.at(i).points.at(j).points.at(1)<<"\t";
	//	}
	//	cout << endl;
	//}
	//cout << "Stats:"<<endl;
	//for (int i = 0; i < D; i++) cout<<stats[i]<<"\t";
	//system("pause");
	//delete[] stats;
return 0;
}
