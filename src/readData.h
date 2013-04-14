#include <iostream>
#include <fstream>
//#include <cstdio>
#include <cmath>
#include <algorithm>
#include <vector>
using std::vector;

#ifndef READDATA_H
#define READDATA_H

void findDimension();
struct psiArray;
struct psiArray
{
  vector< vector< vector< float > > > matrix3d;
  vector< vector< float > > matrix2d;
  vector< float > matrix1d;
  vector< vector< vector< vector< int > > > > coords3d;
  vector< vector< vector< int > > > coords2d;
  vector< int > coords1d;
  int extent, dimension;
  float max;
  float min;
};
psiArray readData ();

#endif // READDATA_H
