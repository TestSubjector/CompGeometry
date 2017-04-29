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

  Point* input_unsorted = new Point[len];
  for(int i = 0; i < len; i++)
  {
    getline(inputFile,lineinput);
    input_unsorted[i] = parsePoint(lineinput);
    input_unsorted[i].index = i;
  }
  inputFile.close();

  Poly inp = Poly();
  inp.Init(len);
  for(int i = 0; i < len; i++)
  {
    inp[i] = input_unsorted[i];
  }

  list<Poly> list_inp;
  list_inp.push_back(inp);

  list<Poly> list_out;

  // cout << "success " << Partition().MonotonePartition(&list_inp,&list_out) << endl;
  // for(std::list<Poly>::iterator iter = list_inp.begin(); iter!=list_inp.end();iter++) {
  //     if(!Partition().TriangulateMonotone(&(*iter),&list_out)) {
  //         cout << "TriangulateMonotone error" << endl;
  //     }
  // }
  cout << Partition().Triangulate_MONO(&list_inp, &list_out) << endl;
  // cout << Partition().TriangulateMonotone(&inp,&list_out) << endl;

  if (0) {
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
