#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include "graham_scan.h"
using namespace std;

// This assumes only one delimiter per string
Point parsePoint(string inp, char delim = ' ')
{
  size_t loc = inp.find(delim);
  float f1,f2;
  // Though there will always be one delimiter
  if(loc != string::npos)
  {
    f1 = atof(inp.substr(0,loc).c_str());
    f2 = atof(inp.substr(loc+1).c_str());
    return Point(f1,f2);
  }
  printf("%s\n", "Not reaching");
}

// ALGORITHM STEPS
// NOTE: Steps 1 and 5 are not a part of the algorithm as such, since they are i/o processing. Their complexities should not be considered.
// STEP 1: Take input in appropriate form. Preferably by reading a file, or allowing one to be piped in using a redirection operator.
// STEP 2: Convert the points (presumably passed in cartesian) into their polar forms about an origin of our choice. This can be clubbed with above.
// STEP 3: Sort the points on in increasing order of angle. If the point chosen as polar origin is the left-most point,
// then the angle will only vary between -90 and 90 degrees. This means direct theta comparision can be used to sort.
// STEP 4: Iterate over the sorted array of points, and apply the Graham's Scan condition and add to stack accordingly.
// STEP 5: The points in the stack are the vertices of the hull, in order. (Ensure the initial point is not added twice into the stack)
// Write these points to a file. This is the output of our program. Let this file be of the format specified in the sample CH file.
// This lets us directly load it into the Visualiser.

int main(int argc, char const *argv[]) {

  ifstream inputFile;
  string filePath = "input.txt";
  // cin >> filePath;
  inputFile.open(filePath.c_str(),ios::in);
  string lineinput;
  int len = 0;
  if(inputFile.is_open())
  {
      getline(inputFile,lineinput);
      len = atoi(lineinput.c_str());
  }
  Point* input = new Point[len];
  for(int i = 0; i < len; i++)
  {
    getline(inputFile,lineinput);
    input[i] = parsePoint(lineinput);
  }

  Node* root = NULL;
  getHull(input,len,&root);
  printStack(&root);
  return 0;
}
