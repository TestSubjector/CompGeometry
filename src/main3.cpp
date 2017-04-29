#include <stdlib.h>
#include "pointtriag.h"

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

int extractVertexCount(string inp, char delim = ' ')
{
  size_t loc = inp.find(delim); //first space
  loc = inp.find(delim,loc+1); // second space

  int num;
  if(loc != string::npos)
  {
    num = atoi(inp.substr(loc+1).c_str());
    return num;
  }
  printf("%s\n", "Not reaching");
}

void printOutputToFile(Point input[], int len, list<Poly> list_out){
    ofstream outputFile;
    outputFile.open("triangulate_points.ply");
    int num_output = list_out.size();
    outputFile << "ply" << endl;
    outputFile << "format ascii 1.0" << endl;
    outputFile << "element vertex " << len << endl;
    outputFile << "property float x" << endl;
    outputFile << "property float y" << endl;
    outputFile << "property float z" << endl;
    outputFile << "element face " << num_output << endl;
    outputFile << "property list uchar int vertex_list" << endl;
    outputFile << "end_header" << endl;
    for (int i=0; i < len; i++) {
        outputFile << input[i].x << " " << input[i].y << " " << 0.0 << endl;
    }
    for (std::list<Poly>::iterator iter = list_out.begin(); iter != list_out.end(); iter++) {
        (*iter).printVert(outputFile);
    }
    cout << "Done" << endl;
    outputFile.close();
}


int main() {
    ifstream inputFile;
    string filePath;
    cout << "Enter input file path: ";
    getline(cin, filePath);
    inputFile.open(filePath.c_str(),ios::in);
    string lineinput;
    int len = 0;

    if(inputFile.is_open())
    {
        getline(inputFile,lineinput); // ply
        getline(inputFile,lineinput); // ascii
        getline(inputFile,lineinput); // vertex
        len = extractVertexCount(lineinput);
    }

    for(int i=0;i<6;i++) getline(inputFile,lineinput); //skipping till after end_header
    Point* input = new Point[len];
    for(int i = 0; i < len; i++)
    {
      getline(inputFile,lineinput);
      input[i] = parsePoint(lineinput);
      input[i].index = i;
    }
    inputFile.close();

    list<Poly> list_out;
    cout << TriangulatePoints(input,len,&(list_out)) << endl;

    printOutputToFile(input, len, list_out);
    return 0;
}
