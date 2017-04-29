#include <iostream>
#include <fstream>
#include <iomanip>
#include "convex_hull_structs.h"
using namespace std;

/// Using triple slashes for doxygen incorporation

int Size; ///< Global for convenience

void Size_Input();
void Arrange(Point P[]);
int Convex(Point Q[]);
float Dir(Point &A, Point &B, Point &C);
void Input_XY(Point R[]);
void Output_CH(Point S[], Point Si[], int Convex_Size);
void Duplicate(Point T[],Point U[]);
void setSize(int size);

void setSize(int size) {
    Size = size;
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

void Input_XY(Point R[])
{
    ifstream Myfile;
    Myfile.open("input.txt",ios::in);
	if(Myfile.is_open())
	{   if (1)
            {
                Myfile>>Size;
            }
        for(int B = 0; B <=Size; B++)
		{
			if(Myfile.eof())
			{
				cout<<"Invalid file."<<endl;
				break;
			}
            else
			{
				Myfile>>R[B].x;
				Myfile>>R[B].y;
            }
    	}
	}
	else
		cout<<"File not found."<<endl;
    Myfile.close();
}

void Duplicate(Point T[], Point U[])
{
    for (int I=0; I<Size; I++)
        {
            T[I].x = U[I].x;
            T[I].y = U[I].y;
            T[I].index = U[I].index;
        }
}

/// Linear Sort
void Arrange(Point P[])
{
    Point temp;
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
int Convex(Point P[])
{
    Point List[2*Size];
    int J = 2;

    /// For Lower C[H]
    List[0] = P[0];
    List[1] = P[1];

    for( int I=2; I< Size; ++I){
        while(J>=2 && Dir(List[J-2], List[J-1], P[I]) <= 0)
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
* A problem that could occur here is that via multiplication
* the range of the value may exceed what can be stored by double
*/
float Dir(Point &A, Point &B, Point &C)
{
    return (B.x - A.x)*(C.y - A.y) - (B.y - A.y)*(C.x - A.x);
}

/// Function for visualizer requirements
void Output_CH(Point S[], Point Si[], int Convex_Size)
{
    ofstream Myfile;
    Myfile.open("output_andrew.ch", ios::out | ios::trunc);
    Myfile<<"CH"<<endl;
    Myfile<<Size<<" "<<Convex_Size<<"\n";

    for(int H=0; H< Size; H++)     ///< Reading Points to file
    {
        Myfile<<Si[H].x<<" "<<Si[H].y<<" "<<"0.0"<<endl;
    }

    for (int I=0; I< Convex_Size; I++)
    {
        for(int J=0; J<Size; J++)
        {
            /// Robustness issues may occur here
            if(S[I].x==Si[J].x && S[I].y == Si[J].y)  ///< Matching Point with Indice.
            {
                Myfile<<J<<" ";
                break;
            }
        }
    }
}
