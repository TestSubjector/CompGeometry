#include<iostream>
#include<fstream>
using namespace std;
/// Data structure for representing a point in cartesian coordinates
struct point
{
	double x_coord,y_coord;
	point()
	{
		x_coord=0;
		y_coord=0;
	}
};
/// Function for taking input of set of points from file input.txt
void inp(int N,point input[])
{
	ifstream myfile;
	myfile.open("input.txt",ios::in);
	if(myfile.is_open())
		for(int i=0;i<N;i++)
		{
			if(myfile.eof())
			{
				cout<<"Invalid file."<<endl;
				break;
			}
			else
			{
				myfile>>input[i].x_coord;
				myfile>>input[i].y_coord;
			}
		}
	else
		cout<<"File not found."<<endl;
	myfile.close();
}
/** Given 3 points p1, p2 and p3, function checks how p2-p3 is oriented wrt p1-p2
	Returns 0 in case of no turn
	Returns 1 in case of clockwise turn
	Returns 2 in case of counterclockwise turn
*/
int orient(point p1,point p2,point p3)
{
	double det=0;
	det=(p2.y_coord-p1.y_coord)*(p3.x_coord-p2.x_coord)-(p2.x_coord-p1.x_coord)*(p3.y_coord-p2.y_coord);
	if(!det)
	{
		return 0;
	}
	else if (det>0)
	{
		return 1; // clockwise
	}
	else
	{
		return 2; // counterclockwise
	}
}
/// Computes the set of points which form the convex hull and returns the total number of such points.
int hull_compute(point input[],int result[],int N)
{
	int j=0;
	double small1=input[0].x_coord,small2=input[0].y_coord;
	int small_add=0;
	for(int i=1;i<N;i++)
	{
		if(input[i].x_coord<small1)
		{
			small1=input[i].x_coord;
			small_add=i;
		}
		else if(input[i].x_coord==small1)
			if(input[i].y_coord<small2)
			{
				small2=input[i].y_coord;
				small_add=i;
			}
	}
	result[j++]=small_add;
	int a=small_add,b=-1;
	while(b!=small_add)
	{
		b=(a+1)%N;
			for(int i=0;i<N;i++)
			{
				if(a==i||b==i)
					continue;
				if(orient(input[a],input[b],input[i])==2)
					b=i;
			}
			result[j++]=b;
			a=b;
	}
	return j;
}
/// Prints the set of points which form the convex hull of the inputted set of points.
void output(point input[],int result[],int j)
{
	cout<<"Convex Hull"<<endl;
	for(int i=0;i<j-1;i++)
	{
		cout<<result[i]<<endl;
		cout<<endl;
	}
}
int main()
{
	int N,j=0;
	cout<<"No. of points: ";
	cin>>N;
	if(N<3)
	{
		cout<<"Atleast 3 points."<<endl; /// Convex hull is not defined for 1 or 2 point(s).
		return 0;
	}
	point input[N]; /// The array which stores the input points. Does not change after input is taken.
	int result[N+1]; // Stores the addresses of points from input array
	for(int i=0;i<N;i++)
		result[i]=-1;
	inp(N,input);
	j=hull_compute(input,result,N); // No. of points on the hull
	output(input,result,j);
	return 0;
}
