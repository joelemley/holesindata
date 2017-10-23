#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <exception>

using namespace std;

#define MAX 1
#define MIN 0

template <typename T> string tostr(const T& t) { 
   ostringstream os; 
   os<<t; 
   return os.str(); 
} 


#endif

#ifndef FRAND

#define FRAND


#endif