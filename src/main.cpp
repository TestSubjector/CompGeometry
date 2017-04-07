#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<iomanip>
#include "graham_scan.h"
using namespace std;

// This assumes only one delimiter per string
Point parsePoint(string inp, char delim = ' ')
{
  size_t loc = inp.find(delim);
  double f1,f2;
  // Though there will always be one delimiter
  if(loc != string::npos)
  {
    f1 = atof(inp.substr(0,loc).c_str());
    f2 = atof(inp.substr(loc+1).c_str());
    return Point(f1,f2);
  }
  printf("%s\n", "Not reaching");
}

int main(int argc, char const *argv[]) {

  ifstream inputFile;
  ofstream outputFile;
  string filePath;
  cout << "Enter input file path: ";
  cin >> filePath;
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
  inputFile.close();

  Node* root = NULL;
  getHull(input,len,&root);

  outputFile.open("output_graham.txt");
  outputFile << setprecision(5);
  while(!isEmpty(root))
  {
    Point temp = pop(&root);
    if(temp.x < 0.000001) temp.x = 0;
    if(temp.y < 0.000001) temp.y = 0;
    outputFile << temp.x << " " << temp.y << endl;
  }
  outputFile.close();
  cout << "Output at ./output_graham.txt";
  return 0;
}
