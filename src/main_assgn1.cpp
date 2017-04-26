#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "Jmarch.h"
#include "andrews_algorithm.h"
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
}

int main(int argc, char const *argv[]) {

  ifstream inputFile;
  ofstream outputFile;
  string filePath;
  cout << "Enter input file path: ";
  getline(cin, filePath);
  inputFile.open(filePath.c_str(),ios::in);
  string lineinput;
  int total_points = 0;

  if(inputFile.is_open())
  {
      getline(inputFile,lineinput);
      total_points = atoi(lineinput.c_str());
  }

  Point inp[total_points];

  for(int i = 0; i < total_points; i++)
  {
    getline(inputFile,lineinput);
    inp[i] = parsePoint(lineinput);
    // index needed only for graham
    inp[i].index = i;
  }
  inputFile.close();

  // Using  JMarch
  // int result[total_points];
  // int points_on_hull = hull_compute(inp,result,total_points);
  // cout << points_on_hull << endl;
  // outputFile.open("output_jarvis.ch");
  // outputFile << "CH" << endl;
  // outputFile << total_points << " " << points_on_hull << endl;
  // for(int i=0;i < total_points; i++)
  // {
  //     outputFile << inp[i].x << " " << inp[i].y << " 0.0" << endl;
  // }
  // for(int i=0;i < points_on_hull;i++)
  // {
  //     outputFile<<result[i]<<" ";
  // }
  // outputFile << endl;
  // outputFile.close();
  // JMarch end

  // Using Andrews Algorithm
  // setSize(total_points);
  // Point indices[total_points];
  // Duplicate(indices,inp);
  // Arrange(inp);
  // int points_on_hull = Convex(inp);
  // Output_CH(inp,indices,points_on_hull);
  // Andrews end

  // Using Graham's Scan
  Node *root;
  int points_on_hull = getHull(inp,total_points,&root);
  if(points_on_hull==-1) return 0;

  outputFile.open("output_graham.ch");
  outputFile << "CH" << endl;
  outputFile << total_points << " " << points_on_hull << endl;

  for (int i = 0; i < total_points; i++) {
      outputFile << inp[i].x << " " << inp[i].y << " 0.0" << endl;
  }
  for (int i = 0; i < points_on_hull; i++) {
      Point temp = pop(&root);
      outputFile << temp.index << " ";
  }
  outputFile << endl;
  // Graham's end
}
