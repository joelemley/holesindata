# holesindata

This is the github repository for the code associated with the paper Big Holes in Big Data: A Monte Carlo Algorithm for Detecting Large Hyper-rectangles in High Dimensional Data.

If this code helps your research please cite:

Lemley, Joseph, Filip Jagodzinski, and Razvan Andonie. "Big Holes in Big Data: A Monte Carlo Algorithm for Detecting Large Hyper-rectangles in High Dimensional Data." In Computer Software and Applications Conference (COMPSAC), 2016 IEEE 40th Annual, vol. 1, pp. 563-571. IEEE, 2016.

Please keep in mind that this is unpolished, unrefined research code which implements specific experiments and it should not be used in any product without significant revision. 

Both the OpenGL front end and the command line interfaces are compatible with Windows with Visual Studio 2013. 

The command line interface is compatible with linux and the gnu compiler set but the OpenGL front end is windows only. 

To compile on linux:

g++ -std=c++11 -fpermissive attemptverify.cpp hyperrectangle.cpp mcexp.cpp pointcontainer.cpp -o findholes.bin 

Then use as follows:

 ./findholes.bin cavRigPDBMetrics.txt 100 \  3

The first argument is a file name, the second is the termination condition (see paper), the third is the delimiter (cavRigPBDMetrics.txt is space delimited), and the final argument indicates your choice of expansion method (see paper for details). 

On Windows with Visual Studio 2013, include attemptverify.cpp if you want a console version. Exclude it from the project if you want the OpenGL visualization interface instead.  glsample.cpp contains all the visualization code. 

The OpenGL visualization code is controlled with the keyboard. After enough holes are found, one rectangle will show up on the screen. This will be the largest found rectangle. 


To see more rectangles press + 

To see less press -

To enable an animated rotation press a

To see a messagebox with volume data on the larger holes, press m


These and other ways of using this visualization tool are located in: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) in the file glsample.cpp
