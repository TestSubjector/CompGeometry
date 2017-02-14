#include <iostream>
using namespace std;

/// Used triple slashes for doxygen incorporation 

///Co-ordinates Storage
struct pos
{
    int x;
    int y;
};

int Size; /// Global for convenience 

void Arrange(pos P[]);
int Convex(pos Q[]);
double Dir(pos &A, pos &B, pos &C);

int main()
{   
    cout<<" Enter total number of coordinates\n ";
    cin>> Size;

    pos coord[Size]; /// Array to store coord
    int F_Size; /// Stores size of Hull 
    
    /// Feeding points
    for(int I = 0; I < Size; I++)
    {   
        cout<<"\nEnter x coordinate of #"<<I+1<<" point - ";
        cin>>coord[I].x;
        cout<<"Enter y coordinate of #"<<I+1<<" point - ";
        cin>>coord[I].y;
    } 

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

int Convex(pos P[])
{   
    pos List[2*Size];
    int J=0;

    /// Lower Convex Hull
    List[0] = P[0];
    List[1] = P[1];

    for( int I=2; I< Size; ++I){
        while(J>=2 && Dir(List[J-2], List[J-1], P[I]) <= 0)
        J--;
        List[J++] = P[I];
    }
    
    int flag = J+1; 
    
    /// Upper Convex Hull
    for( int K = Size-2; K>=0; K--){
        while(J>=flag && Dir(List[J-2], List[J-1], P[K]) <= 0)
        J--;
        List[J++] = P[K];
    }
       
    for(int L=0;L<J;L++)
    P[L] = List[L];
    return J; 
}

/// Cross Product Rotation
double Dir(pos &A, pos &B, pos &C)
{
    return (B.x - A.x)*(C.y - A.y) - (B.y - A.y)*(C.x - A.x);
}