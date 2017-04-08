#include <iostream>
#include <fstream>
#include <iomanip>
#include "convex_hull_structs.h"
using namespace std;

/// Using triple slashes for doxygen incorporation 

struct pos ///< Structure for Co-ordinates Storage
{   
    double x; ///< Assigned double to increase robustness
    double y;
};

int Size; ///< Global for convenience 

void Size_Input();
void Arrange(pos P[]);
int Convex(pos Q[]);
float Dir(pos &A, pos &B, pos &C);
void Input_XY(pos R[]);
void Output_CH(pos S[], pos Si[], int Convex_Size);
void Duplicate(pos T[],pos U[]);

int main()
{   
    Size_Input();

    pos coord[Size]; ///< Array to store points
    pos indice[Size]; ///< Reference array for visualizer indices
    int F_Size; ///< Stores number of C(H) points 
       
    Input_XY(coord); ///< Feeding points
    
    Duplicate(indice, coord); ///< Give points to Indice Array

    Arrange(coord); ///< Sorting of points with reference to their x and then y coordinate
    
    F_Size = Convex(coord); 

    /// Displaying Convex Hulls
    Output_CH(coord, indice, F_Size);
   
    for(int I = 0; I < F_Size; I++)
    {   
        cout<<"( ";
        /// To Display A More Accurate Representation Of The Points
        std::cout << std::setprecision(15)<<coord[I].x;
        cout<<", ";
        std::cout << std::setprecision(15)<<coord[I].y<<" )\n";
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

void Duplicate(pos T[], pos U[])
{
    for (int I=0; I<Size; I++)
        {
            T[I].x = U[I].x;
            T[I].y = U[I].y;
        }
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
float Dir(pos &A, pos &B, pos &C)
{
    return (B.x - A.x)*(C.y - A.y) - (B.y - A.y)*(C.x - A.x);
}

/// Function for visualizer requirements 
void Output_CH(pos S[], pos Si[], int Convex_Size)
{
    ofstream Myfile;
    Myfile.open("Answer.ch", ios::out | ios::trunc);
    Myfile<<Size<<" "<<Convex_Size<<"\n";

    for(int H=0; H< Size; H++)     ///< Reading Points to file
    {
        Myfile<<Si[H].x<<" "<<Si[H].y<<"\n";
    }    

    for (int I=0; I< Convex_Size; I++)     
    {
        for(int J=0; J<Size; J++)
        {   
            /// Robustness issues may occur here
            if(S[I].x==Si[J].x && S[I].y == Si[J].y)  ///< Matching Point with Indice. 
            {                                           
                Myfile<<J+1<<" ";
                break;
            }
        }
    }
}