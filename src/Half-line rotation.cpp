// Half-line sweep algorithm
#include <iostream>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#define PI 3.1415926537
using namespace std;
double cam_x,cam_y;
struct vertex
{
	double x_coord,y_coord;
	double r,theta; // Instead of deducing every time, polar is stored with cartesian.
	vertex *other; // Pointer to other vertex of segment
	int index;
	vertex()
	{
		x_coord=0;
		y_coord=0;
		r=0;
		theta=0;
		other=NULL;
	}
	void convert()
	{
		x_coord=x_coord-cam_x;
		y_coord=y_coord-cam_y;
		r=sqrt(x_coord*x_coord+y_coord*y_coord);
		if(x_coord)
		{
			theta=atan2(y_coord,x_coord); // Value should be between 0 and 2*PI
			if(theta<0)
				theta=2*PI+theta;
		}
		else
			if(y_coord>0)
				theta=PI/2;
			else if(y_coord<0)
				theta=-PI/2;
			else
				theta=0;
	}
};
void assign(vertex &a,vertex &b) // does the operation a=b for data type vertex
{
	a.x_coord=b.x_coord;
	a.y_coord=b.y_coord;
	a.r=b.r;
	a.theta=b.theta;
	a.other=b.other;
}

void printOutputToFile(vertex input[], int final[], int len, int N){
    ofstream outputFile;
    outputFile.open("segments.ply");
    outputFile << "ply" << endl;
    outputFile << "format ascii 1.0" << endl;
    outputFile << "element vertex " << N*2 + 1 << endl;
    outputFile << "property float x" << endl;
    outputFile << "property float y" << endl;
    outputFile << "property float z" << endl;
    outputFile << "element face " << len+N*2 << endl;
    outputFile << "property list uchar int vertex_list" << endl;
    outputFile << "end_header" << endl;
	//list of vertices
	for (int i=0; i < 2*N; i++) {
        outputFile << input[i].x_coord + cam_x << " " << input[i].y_coord + cam_y << " " << 0.0 << endl;
    }
	outputFile << cam_x << " " << cam_y << " " << 0.0 << endl;
	//all input segments
	for (int i=0; i < 2*N; i++) {
        outputFile << "2 " << input[i].index << " " << input[i].other->index << endl;
    }
	for(int i=0; i < len; i++) {
		outputFile << "2 " << 2*N << " "; // connect to cam
		outputFile << final[i] << endl;
	}
}

// Below function is for filling the vertex list
void input(string filename,vertex list[],int N)
{
	ifstream inputFile;
	inputFile.open(filename.c_str(),ios::in);
	string temp;
	getline(inputFile,temp); // discard n
	double x,y;
	inputFile>>cam_x;
	inputFile>>cam_y;
	for(int i=1;i<=N;i++)
	{
		// cout<<"Segment "<<i<<endl;
		// cout<<"First point x_coord: ";
		inputFile>>x;
		// cout<<"First point y_coord: ";
		inputFile>>y;
		list[2*i-2].x_coord = x;
		list[2*i-2].y_coord=y;
		list[2*i-2].convert();
		// cout<<"Second point x_coord: ";
		inputFile>>x;
		// cout<<"Second point y_coord: ";
		inputFile>>y;
		list[2*i-1].x_coord=x;
		list[2*i-1].y_coord=y;
		list[2*i-1].convert();
		list[2*i-2].other=&list[2*i-1];
		//cout<<list[2*i-2].other->x_coord+cam_x<<endl;
		list[2*i-1].other=&list[2*i-2];
		//cout<<list[2*i-1].other->x_coord+cam_x<<endl;
		// cout<<endl;
	}
	inputFile.close();
}
void merge(vertex list[], int low, int high, int mid)
{
    int i, j, k;
	vertex c[50];
    i = low;
    k = low;
    j = mid + 1;
    while (i <= mid && j <= high)
    {
        if (list[i].theta < list[j].theta)
        {
            assign(c[k],list[i]);
            list[i].other->other=&c[k];
            k++;
            i++;
        }
        else
        {
            assign(c[k],list[j]);
            list[j].other->other=&c[k];
            k++;
            j++;
        }
    }
    while (i <= mid)
    {
        assign(c[k],list[i]);
        list[i].other->other=&c[k];
        k++;
        i++;
    }
    while (j <= high)
    {
        assign(c[k],list[j]);
        list[j].other->other=&c[k];
        k++;
        j++;
    }
    for (i = low; i < k; i++)
    {
        assign(list[i],c[i]);
        c[i].other->other=&list[i];
    }
}
void mergesort(vertex list[], int low, int high)
{
    int mid;
    if (low < high)
    {
        mid=(low+high)/2;
        mergesort(list,low,mid);
        mergesort(list,mid+1,high);
        merge(list,low,high,mid);
    }
    return;
}
int check_equal(vertex *a,vertex b)
{
	if(a->x_coord!=b.x_coord)
		return 0;
	if(a->y_coord!=b.y_coord)
		return 0;
	return 1;
}
int check_beg(int i,vertex list[],vertex active[],int N,int k)
{
	for(int j=0;j<k;j++)
		if(check_equal(list[i].other,active[j]))
			return 0;
	return 1;
}
void insert(int i,vertex list[],vertex active[],int &k)
{
	assign(active[k++],list[i]);
}
// Delete a segment from active list
void del(int i,vertex list[],vertex active[],int &k)
{
	vertex zero;
	for(int j=0;j<k;j++)
		if(check_equal(list[i].other,active[j]))
		{
			while(j<k-1)
			{
				assign(active[j],active[j+1]);
				j++;
			}
			k--;
		}
}
// Checks intersection of two segments: origin to vertex a, segment with vertex b as one endpoint
int check_inter(vertex a,vertex b)
{
	vertex c;
	assign(c,*(b.other));
	double x1=0,y1=0;
	double x2=a.x_coord,y2=a.y_coord;
	double x3=b.x_coord,y3=b.y_coord;
	double x4=c.x_coord,y4=c.y_coord;
//	cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<" "<<x3<<" "<<y3<<" "<<x4<<" "<<y4<<" "<<endl;
	// condition for lines to intersect: (x1-x2)(y3-y4)!=(x3-x4)(y1-y2)
	if((x1-x2)*(y3-y4)==(y1-y2)*(x3-x4)) // lines are parallel or coincident
		return 0;
	else
	{
		double x=0;
		int check1=0,check2=0;
		x=((x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
		// cout<<x<<endl;
		if(x1<x2)
		{
			//  cout<<x1<<" "<<x2<<endl;
			 if(x>x1&&x<x2)
				check1=1;
	    }
		else
		{
			if(x>x2&&x<x1)
				check1=1;
	    }
		if(x3<x4)
		{
			// cout<<x3<<" "<<x4<<endl;
			if(x>x3&&x<x4)
				check2=1;
	    }
		else
		{
			if(x>x4&&x<x3)
				check2=1;
	    }
		if(check1&&check2)
			return 1;
	 else
			return 0;
	 /* if x_coord(intersection point) is in between x_coords of
		both segments' end points, then segments intersect. */
	}
}
/*bool onSegment(vertex p, vertex q, vertex r)
{
    if (q.x_coord <= max(p.x_coord, r.x_coord) && q.x_coord >= min(p.x_coord, r.x_coord) &&
        q.y_coord <= max(p.y_coord, r.y_coord) && q.y_coord >= min(p.y_coord, r.y_coord))
       return true;

    return false;
}
int orientation(vertex p, vertex q, vertex r)
{
    int val = (q.y_coord - p.y_coord) * (r.x_coord - q.x_coord) -
              (q.x_coord - p.x_coord) * (r.y_coord - q.y_coord);

    if (val == 0) return 0;  // colinear

    return (val > 0)? 1: 2; // clock or counterclock wise
}
bool doIntersect(vertex p1, vertex q1, vertex p2, vertex q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    return false; // Doesn't fall in any of the above cases
}*/
int visible(int i,vertex list[],vertex active[],int N,int k)
{
	vertex zero;
	for(int j=0;j<k;j++)
	{
		if(check_inter(list[i],active[j]))
		{
			return 0;
		}
	}
	return 1;
}
void output(vertex print[],int N) // Outputs any storage list
{
	for(int i=1;i<=N;i++)
	{
		cout<<print[2*i-2].theta<<endl;
		cout<<print[2*i-1].theta<<endl;
		cout<<endl;
	}
}
void print(vertex list[],int final[],int j)
{
	for(int i=0;i<j;i++)
	{
		cout<<list[final[i]].x_coord+cam_x<<endl;
		cout<<list[final[i]].y_coord+cam_y<<endl;
		cout<<endl;
	}
}
void rearr(vertex list[],int i)
{
}
void pr_active(vertex active[],int k)
{
	for(int i=0;i<k;i++)
	{
			cout<<active[i].x_coord+cam_x<<endl;
			cout<<active[i].y_coord+cam_y<<endl;
			cout<<endl;
	}
}
int main()
{
	int N;
	ifstream inputFile;
    string filePath;
    cout << "Enter input file path: ";
    getline(cin, filePath);
	inputFile.open(filePath.c_str(),ios::in);
	inputFile>>N;
	inputFile.close();
	 // No. of segments
	vertex list[2*N];
	input(filePath,list,N);
//	output(list,N);
//	cout<<endl;
	mergesort(list,0,2*N-1);
//	output(list,N);
	vertex active[N]; // stores one vertex of the segment, other can be accessed from it.
	int final[N]; // stores the final result
	int i,j,k;
/*	for(i=0,j=0,k=0;i<2*N;i++) // k+1 is number of elements in active
	{
		if(check_beg(i,list,active,N,k)) // Checks whether list[i] is a beginning vertex
		{
			insert(i,list,active,k); // Inserts list[i] into active list
			if(list[i].theta+PI<list[i].other->theta)
				rearr(list,i);
		}
		else
		{
			del(i,list,active,k); // Deletes list[i] from active list
		}
	}*/
	for(i=0,j=0,k=0;i<2*N;i++)
	{
		if(list[i].theta<list[i].other->theta)
		{
			/*cout<<list[i].x_coord+cam_x<<endl;
			cout<<list[i].y_coord+cam_y<<endl;
			cout<<endl;*/
			insert(i,list,active,k);
			//pr_active(active,k);
		}
		else if(list[i].theta==list[i].other->theta)
			if(list[i].r<list[i].other->r)
			{
				insert(i,list,active,k);
				//pr_active(active,k);
			}
		else
			del(i,list,active,k);
		if(visible(i,list,active,N,k)) // Checks for visibility of list[i] using active list
		{
			/*cout<<list[i].x_coord+cam_x<<endl;
			cout<<list[i].y_coord+cam_y<<endl;
			cout<<endl;*/
			final[j++]=i;
			//print(list,final,j);
		}
	}
	// print(list,final,j);
	for (int i = 0; i < 2*N; i++) {
		list[i].index = i;
	}
	printOutputToFile(list,final,j,N);
	return 0;
}
/* Problems to fix
	1. Segment cuts zero theta half-line.
	2. case where Endpoints of segment not visible, but segment still is.
	3. Vertical segments do not give correct output.
*/
