#include<stdlib.h>
#include<iomanip>

#include "polytriag.h"

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
  getline(cin, filePath);
  inputFile.open(filePath.c_str(),ios::in);
  string lineinput;
  int len = 0;

  if(inputFile.is_open())
  {
      getline(inputFile,lineinput);
      len = atoi(lineinput.c_str());
  }

  Poly inp = Poly();
  inp.Init(len);

  for(int i = 0; i < len; i++)
  {
    getline(inputFile,lineinput);
    inp[i] = parsePoint(lineinput);
    inp[i].index = i;
  }
  inputFile.close();

  list<Poly> list_inp;
  list_inp.push_back(inp);

  list<Poly> list_out;

  int success = Partition().Triangulate_MONO(&list_inp, &list_out);

  if (success != 1) {
      cout << "Error: Unable to process" << endl;
  }
  else {
      outputFile.open("output_triangulate.ply");
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
          outputFile << inp[i].x << " " << inp[i].y << " " << 0.0 << endl;
      }
      for (std::list<Poly>::iterator iter = list_out.begin(); iter != list_out.end(); iter++) {
          (*iter).printVert(outputFile);
      }
      cout << "Output at ./output_triangulate.ply";
      outputFile.close();
  }
}
