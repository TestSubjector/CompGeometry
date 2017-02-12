#include <iostream>
using namespace std;

//Co-ordinates Storage
struct pos
{
    int x;
    int y;
};

int Size; // Global for convenience 
void Arrange(pos P[]);


int main()
{   
    cout<<" Enter total number of coordinates\n ";
    cin>> Size;

    pos coord[Size]; // Array to store coord
    
    // Feeding points
    for(int I = 0; I < Size; I++)
    {   
        cout<<"\nEnter x coordinate of #"<<I+1<<" point - ";
        cin>>coord[I].x;
        cout<<"Enter y coordinate of #"<<I+1<<" point - ";
        cin>>coord[I].y;
    } 

    return 0;
}

void Arrange(pos P[])   //
{
    pos temp;
    for(int I = 0; I< Size; I++)
    {   
        temp = P[I];
        for(int J = I; J < Size-1; J++)
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










