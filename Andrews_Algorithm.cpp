#include <iostream>
#include <fstream>
using namespace std;

/// Used triple slashes for doxygen incorporation 

///Co-ordinates Storage
struct pos
{
    double x;
    double y;
};

int Size; /// Global for convenience 

void Size_Input();
void Arrange(pos P[]);
int Convex(pos Q[]);
float Dir(pos &A, pos &B, pos &C);
void Input_XY(pos R[]);

int main()
{   
    Size_Input();

    pos coord[Size]; /// Array to store coord
    int F_Size; /// Stores size of Hull 
    
    /// Feeding points
    Input_XY(coord);

    Arrange(coord);
    
    F_Size = Convex(coord);

    /// Displaying Convex Hulls
    for(int I = 0; I < F_Size; I++)
    {   
        cout<<"( ";
        cout<<coord[I].x;
        cout<<", ";
        cout<<coord[I].y<<" )\n";
    } 
    
    return 0;
}

void Size_Input()
{
    ifstream Myfile;
	Myfile.open("input.txt",ios::in);
    if(Myfile.is_open())
    {	if(Myfile.eof())
		{
			cout<<"Invalid file."<<endl;
        }
        else
            Myfile>>Size;
    }
	else
		cout<<"File not found."<<endl;
Myfile.close();
}

void Input_XY(pos R[])
{
    ifstream Myfile;
	Myfile.open("input.txt",ios::in);
	if(Myfile.is_open())
		for(int B = 0;B <=Size; B++)
		{
			if(Myfile.eof())
			{
				cout<<"Invalid file."<<endl;
				break;
			}
            else if (B==0)
            {
                Myfile>>Size;
            }
            else
			{   
				Myfile>>R[Size].x;
				Myfile>>R[Size].y;
                cout<<R[Size].x<<"\n";
                cout<<R[Size].y<<"\n";
			}
		}
	else
		cout<<"File not found."<<endl;
Myfile.close();
}

/// Linear Sort
void Arrange(pos P[])   
{
    pos temp;
    for(int I = 0; I< Size; I++)
    {   
        temp = P[I];
        for(int J = 0; J < Size-1; J++)
        {    
            if( P[J].x > P[J+1].x)
              {
                 temp = P[J+1];
                 P[J+1] = P[J];
                 P[J] = temp;
              } 
            else if (P[J].x == P[J+1].x)
            {
               if(P[J].y > P[J+1].y)
               {
                 temp = P[J+1];
                 P[J+1] = P[J];
                 P[J] = temp;
               }
            }
        }
    }
}

/// Function to pick out convex hull points
int Convex(pos P[])
{   
    pos List[2*Size];
    int J = 2;

    /// For Lower C[H]
    List[0] = P[0];
    List[1] = P[1];

    for( int I=2; I< Size; ++I){
        while(J>=2 && Dir(List[J-2], List[J-1], P[I]) <= 0.0)
        J--;
        List[J++] = P[I];
    }
    
    int flag = J+1; 
    
    /// For Upper C[H]
    for( int K = Size-2; K>=0; K--){
        while(J>=flag && Dir(List[J-2], List[J-1], P[K]) <= 0)
        J--;
        List[J++] = P[K];
    }
       
    for(int L=0;L<J;L++)
    P[L] = List[L];
    return J-1; 
}

/** 
* Cross Product Rotation
* A problem that could occur here is that via multiplacation
* the range of the value may exceed what can be stored by double
*/
float Dir(pos &A, pos &B, pos &C)
{
    float (B.x - A.x)*(C.y - A.y) - (B.y - A.y)*(C.x - A.x);
}