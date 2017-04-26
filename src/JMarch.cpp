#include "jmarch.h"

/** Given 3 points p1, p2 and p3, function checks how p2-p3 is oriented wrt p1-p2
	Returns 0 in case of no turn
	Returns 1 in case of clockwise turn
	Returns 2 in case of counterclockwise turn
*/
int orient(Point p1,Point p2,Point p3)
{
	double det=0;
	det=(p2.y-p1.y)*(p3.x-p2.x)-(p2.x-p1.x)*(p3.y-p2.y);
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
int hull_compute(Point input[],int result[],int N)
{
	int j=0;
	double small1=input[0].x,small2=input[0].y;
	int small_add=0;
	for(int i=1;i<N;i++)
	{
		if(input[i].x<small1)
		{
			small1=input[i].x;
			small_add=i;
		}
		else if(input[i].x==small1)
			if(input[i].y<small2)
			{
				small2=input[i].y;
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
	return j-1;
}
