
#include "polytriag.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <set>
#include <stack>
using namespace std;
#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692
#define VERTEXTYPE_REGULAR 0
#define VERTEXTYPE_START 1
#define VERTEXTYPE_END 2
#define VERTEXTYPE_SPLIT 3
#define VERTEXTYPE_MERGE 4

int equals(Point p1, Point p2) {
    if ((p1.x == p2.x) && (p1.y == p2.y)) return 1;
    return 0;
}

/*
Return the angle between two vectors on a plane
The angle is from vector 1 to vector 2, positive anticlockwise
The result is between -pi -> pi
*/
double Angle2D(double x1, double y1, double x2, double y2)
{
double dtheta,theta1,theta2;

theta1 = atan2(y1,x1);
theta2 = atan2(y2,x2);
dtheta = theta2 - theta1;
while (dtheta > PI)
  dtheta -= TWOPI;
while (dtheta < -PI)
  dtheta += TWOPI;

return(dtheta);
}

int InsidePolygon(Point *polygon,int n,Point p)
{
    int i;
    double angle=0;
    Point p1,p2;

    for (i=0;i<n;i++) {
      p1.x = polygon[i].x - p.y;
      p1.y = polygon[i].y - p.y;
      p2.x = polygon[(i+1)%n].x - p.x;
      p2.y = polygon[(i+1)%n].y - p.y;
      angle += Angle2D(p1.x,p1.y,p2.x,p2.y);
}

cout << fabs(angle) << endl;
if (fabs(angle) < TWOPI)
  return 0;
else
  return 1;
}

Poly::Poly() {
	numpoints = 0;
	points = NULL;
}

Poly::~Poly() {
	if(points) delete [] points;
}

void Poly::Clear() {
	if(points) delete [] points;
	numpoints = 0;
	points = NULL;
}

void Poly::Init(long numpoints) {
	Clear();
	this->numpoints = numpoints;
	points = new Point[numpoints];
}

Poly::Poly(const Poly &src) {
	numpoints = src.numpoints;
	points = new Point[numpoints];
	memcpy(points, src.points, numpoints*sizeof(Point));
}

Poly& Poly::operator=(const Poly &src) {
	Clear();
	numpoints = src.numpoints;
	points = new Point[numpoints];
	memcpy(points, src.points, numpoints*sizeof(Point));
	return *this;
}


void Poly::Triangle(Point &p1, Point &p2, Point &p3) {
	Init(3);
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
    // cout << "Triangle made with indices " << p1.index << " " << p2.index << " " << p3.index << endl;
}

/**
 * Checks whether the angle subtended at p1 is convex
 * @param  p1
 * @param  p2
 * @param  p3
 * @return    True if convex angle
 */
bool Partition::IsConvexAngle(Point& p1, Point& p2, Point& p3) {
	double  tmp;
	tmp = (p3.y-p1.y)*(p2.x-p1.x)-(p3.x-p1.x)*(p2.y-p1.y);
	if(tmp>0) return 1;
	else return 0;
}


bool Partition::Below(Point &p1, Point &p2) {
	if(p1.y < p2.y) return true;
	else if(p1.y == p2.y) {
		if(p1.x > p2.x) return true;
	}
	return false;
}

/**
 * Adds a diagonal among the specified vertices
 * @param vertices    Array of vertices
 * @param numvertices Pointer to number of vertices
 * @param index1      Index of the first vertex of the diagonal
 * @param index2      Index of the second vertex of the diagonal
 */
void Partition::AddDiagonal(MonotoneVertex *vertices, long *numvertices, long index1, long index2) {
	long newindex1,newindex2;

	newindex1 = *numvertices;
	(*numvertices)++;
	newindex2 = *numvertices;
	(*numvertices)++;

	vertices[newindex1].p = vertices[index1].p;
	vertices[newindex2].p = vertices[index2].p;

	vertices[newindex2].next = vertices[index2].next;
	vertices[newindex1].next = vertices[index1].next;

	vertices[vertices[index2].next].previous = newindex2;
	vertices[vertices[index1].next].previous = newindex1;

	vertices[index1].next = newindex2;
	vertices[newindex2].previous = index1;

	vertices[index2].next = newindex1;
	vertices[newindex1].previous = index2;
}

bool Partition::ScanLineEdge::IsConvex(const Point& p1, const Point& p2, const Point& p3) const {
	double tmp;
	tmp = (p3.y-p1.y)*(p2.x-p1.x)-(p3.x-p1.x)*(p2.y-p1.y);
	if(tmp>0) return 1;
	else return 0;
}

bool Partition::ScanLineEdge::operator < (const ScanLineEdge & other) const {
	if(other.p1.y == other.p2.y) {
		if(p1.y == p2.y) {
			if(p1.y < other.p1.y) return true;
			else return false;
		}
		if(IsConvex(p1,p2,other.p1)) return true;
		else return false;
	} else if(p1.y == p2.y) {
		if(IsConvex(other.p1,other.p2,p1)) return false;
		else return true;
	} else if(p1.y < other.p1.y) {
		if(IsConvex(other.p1,other.p2,p1)) return false;
		else return true;
	} else {
		if(IsConvex(p1,p2,other.p1)) return true;
		else return false;
	}
}

//sorts in the falling order of y values, if y is equal, x is used instead
bool Partition::VertexSorter::operator() (long index1, long index2) {
	if(vertices[index1].p.y > vertices[index2].p.y) return true;
	else if(vertices[index1].p.y == vertices[index2].p.y) {
		if(vertices[index1].p.x < vertices[index2].p.x) return true;
	}
	return false;
}

/**
 * Takes in a list of polygons, makes them monotone by joining certain vertices, and stores it in the second parameter
 * @param  inpolys       input in the form of a list of polygons
 * @param  monotonePolys the result
 * @return               1 for success, 0 for failure
 */
 int Partition::MonotonePartition(list<Poly> *inpolys, list<Poly> *monotonePolys) {
 	list<Poly>::iterator iter;
 	MonotoneVertex *vertices;
 	long i,numvertices,vindex,vindex2,newnumvertices,maxnumvertices;
 	long polystartindex, polyendindex;
 	Poly *poly;
 	MonotoneVertex *v,*v2,*vprev,*vnext;
 	ScanLineEdge newedge;
 	bool error = false;

 	numvertices = 0;
 	for(iter = inpolys->begin(); iter != inpolys->end(); iter++) {
 		numvertices += iter->GetNumPoints();
 	}

 	maxnumvertices = numvertices*3;
 	vertices = new MonotoneVertex[maxnumvertices];
 	newnumvertices = numvertices;

 	polystartindex = 0;
 	for(iter = inpolys->begin(); iter != inpolys->end(); iter++) {
 		poly = &(*iter);
 		polyendindex = polystartindex + poly->GetNumPoints()-1;
 		for(i=0;i<poly->GetNumPoints();i++) {
 			vertices[i+polystartindex].p = poly->GetPoint(i);
 			if(i==0) vertices[i+polystartindex].previous = polyendindex;
 			else vertices[i+polystartindex].previous = i+polystartindex-1;
 			if(i==(poly->GetNumPoints()-1)) vertices[i+polystartindex].next = polystartindex;
 			else vertices[i+polystartindex].next = i+polystartindex+1;
 		}
 		polystartindex = polyendindex+1;
 	}

 	//construct the priority queue
 	long *priority = new long [numvertices];
 	for(i=0;i<numvertices;i++) priority[i] = i;
 	std::sort(priority,&(priority[numvertices]),VertexSorter(vertices));

 	//determine vertex types
 	char *vertextypes = new char[maxnumvertices];
 	for(i=0;i<numvertices;i++) {
 		v = &(vertices[i]);
 		vprev = &(vertices[v->previous]);
 		vnext = &(vertices[v->next]);

 		if(Below(vprev->p,v->p)&&Below(vnext->p,v->p)) {
 			if(IsConvexAngle(vnext->p,vprev->p,v->p)) {
 				vertextypes[i] = VERTEXTYPE_START;
 			} else {
 				vertextypes[i] = VERTEXTYPE_SPLIT;
 			}
 		} else if(Below(v->p,vprev->p)&&Below(v->p,vnext->p)) {
 			if(IsConvexAngle(vnext->p,vprev->p,v->p))
 			{
 				vertextypes[i] = VERTEXTYPE_END;
 			} else {
 				vertextypes[i] = VERTEXTYPE_MERGE;
 			}
 		} else {
 			vertextypes[i] = VERTEXTYPE_REGULAR;
 		}
 	}

 	//helpers
 	long *helpers = new long[maxnumvertices];

 	//binary search tree that holds edges intersecting the scanline
 	//note that while set doesn't actually have to be implemented as a tree
 	//complexity requirements for operations are the same as for the balanced binary search tree
 	set<ScanLineEdge> edgeTree;
 	//store iterators to the edge tree elements
 	//this makes deleting existing edges much faster
 	set<ScanLineEdge>::iterator *edgeTreeIterators,edgeIter;
 	edgeTreeIterators = new set<ScanLineEdge>::iterator[maxnumvertices];
 	pair<set<ScanLineEdge>::iterator,bool> edgeTreeRet;

 	//for each vertex
 	for(i=0;i<numvertices;i++) {
 		vindex = priority[i];
 		v = &(vertices[vindex]);
 		vindex2 = vindex;
 		v2 = v;

 		//depending on the vertex type, do the appropriate action
 		//comments in the following sections are copied from "Computational Geometry: Algorithms and Applications"
 		switch(vertextypes[vindex]) {
 			case VERTEXTYPE_START:
 				//Insert ei in T and set helper(ei) to vi.
 				newedge.p1 = v->p;
 				newedge.p2 = vertices[v->next].p;
 				newedge.index = vindex;
 				edgeTreeRet = edgeTree.insert(newedge);
 				edgeTreeIterators[vindex] = edgeTreeRet.first;
 				helpers[vindex] = vindex;
 				break;

 			case VERTEXTYPE_END:
 				//if helper(ei-1) is a merge vertex
 				if(vertextypes[helpers[v->previous]]==VERTEXTYPE_MERGE) {
 					//Insert the diagonal connecting vi to helper(ei-1) in D.
 					AddDiagonal(vertices,&newnumvertices,vindex,helpers[v->previous]);
 					vertextypes[newnumvertices-2] = vertextypes[vindex];
 					edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
 					helpers[newnumvertices-2] = helpers[vindex];
 					vertextypes[newnumvertices-1] = vertextypes[helpers[v->previous]];
 					edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[v->previous]];
 					helpers[newnumvertices-1] = helpers[helpers[v->previous]];
 				}
 				//Delete ei-1 from T
 				edgeTree.erase(edgeTreeIterators[v->previous]);
 				break;

 			case VERTEXTYPE_SPLIT:
 				//Search in T to find the edge e j directly left of vi.
 				newedge.p1 = v->p;
 				newedge.p2 = v->p;
 				edgeIter = edgeTree.lower_bound(newedge);
 				if(edgeIter == edgeTree.begin()) {
 					error = true;
 					break;
 				}
 				edgeIter--;
 				//Insert the diagonal connecting vi to helper(ej) in D.
 				AddDiagonal(vertices,&newnumvertices,vindex,helpers[edgeIter->index]);
 				vertextypes[newnumvertices-2] = vertextypes[vindex];
 				edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
 				helpers[newnumvertices-2] = helpers[vindex];
 				vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
 				edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
 				helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
 				vindex2 = newnumvertices-2;
 				v2 = &(vertices[vindex2]);
 				//helper(e j) = vi
 				helpers[edgeIter->index] = vindex;
 				//Insert ei in T and set helper(ei) to vi.
 				newedge.p1 = v2->p;
 				newedge.p2 = vertices[v2->next].p;
 				newedge.index = vindex2;
 				edgeTreeRet = edgeTree.insert(newedge);
 				edgeTreeIterators[vindex2] = edgeTreeRet.first;
 				helpers[vindex2] = vindex2;
 				break;

 			case VERTEXTYPE_MERGE:
 				//if helper(ei-1) is a merge vertex
 				if(vertextypes[helpers[v->previous]]==VERTEXTYPE_MERGE) {
 					//Insert the diagonal connecting vi to helper(ei-1) in D.
 					AddDiagonal(vertices,&newnumvertices,vindex,helpers[v->previous]);
 					vertextypes[newnumvertices-2] = vertextypes[vindex];
 					edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
 					helpers[newnumvertices-2] = helpers[vindex];
 					vertextypes[newnumvertices-1] = vertextypes[helpers[v->previous]];
 					edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[v->previous]];
 					helpers[newnumvertices-1] = helpers[helpers[v->previous]];
 					vindex2 = newnumvertices-2;
 					v2 = &(vertices[vindex2]);
 				}
 				//Delete ei-1 from T.
 				edgeTree.erase(edgeTreeIterators[v->previous]);
 				//Search in T to find the edge e j directly left of vi.
 				newedge.p1 = v->p;
 				newedge.p2 = v->p;
 				edgeIter = edgeTree.lower_bound(newedge);
 				if(edgeIter == edgeTree.begin()) {
 					error = true;
 					break;
 				}
 				edgeIter--;
 				//if helper(ej) is a merge vertex
 				if(vertextypes[helpers[edgeIter->index]]==VERTEXTYPE_MERGE) {
 					//Insert the diagonal connecting vi to helper(e j) in D.
 					AddDiagonal(vertices,&newnumvertices,vindex2,helpers[edgeIter->index]);
 					vertextypes[newnumvertices-2] = vertextypes[vindex2];
 					edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex2];
 					helpers[newnumvertices-2] = helpers[vindex2];
 					vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
 					edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
 					helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
 				}
 				//helper(e j) = vi
 				helpers[edgeIter->index] = vindex2;
 				break;

 			case VERTEXTYPE_REGULAR:
 				//if the interior of P lies to the right of vi
 				if(Below(v->p,vertices[v->previous].p)) {
 					//if helper(ei-1) is a merge vertex
 					if(vertextypes[helpers[v->previous]]==VERTEXTYPE_MERGE) {
 						//Insert the diagonal connecting vi to helper(ei-1) in D.
 						AddDiagonal(vertices,&newnumvertices,vindex,helpers[v->previous]);
 						vertextypes[newnumvertices-2] = vertextypes[vindex];
 						edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
 						helpers[newnumvertices-2] = helpers[vindex];
 						vertextypes[newnumvertices-1] = vertextypes[helpers[v->previous]];
 						edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[v->previous]];
 						helpers[newnumvertices-1] = helpers[helpers[v->previous]];
 						vindex2 = newnumvertices-2;
 						v2 = &(vertices[vindex2]);
 					}
 					//Delete ei-1 from T.
 					edgeTree.erase(edgeTreeIterators[v->previous]);
 					//Insert ei in T and set helper(ei) to vi.
 					newedge.p1 = v2->p;
 					newedge.p2 = vertices[v2->next].p;
 					newedge.index = vindex2;
 					edgeTreeRet = edgeTree.insert(newedge);
 					edgeTreeIterators[vindex2] = edgeTreeRet.first;
 					helpers[vindex2] = vindex;
 				} else {
 					//Search in T to find the edge ej directly left of vi.
 					newedge.p1 = v->p;
 					newedge.p2 = v->p;
 					edgeIter = edgeTree.lower_bound(newedge);
 					if(edgeIter == edgeTree.begin()) {
 						error = true;
 						break;
 					}
 					edgeIter--;
 					//if helper(ej) is a merge vertex
 					if(vertextypes[helpers[edgeIter->index]]==VERTEXTYPE_MERGE) {
 						//Insert the diagonal connecting vi to helper(e j) in D.
 						AddDiagonal(vertices,&newnumvertices,vindex,helpers[edgeIter->index]);
 						vertextypes[newnumvertices-2] = vertextypes[vindex];
 						edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
 						helpers[newnumvertices-2] = helpers[vindex];
 						vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
 						edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
 						helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
 					}
 					//helper(e j) = vi
 					helpers[edgeIter->index] = vindex;
 				}
 				break;
 		}

 		if(error) break;
 	}

 	char *used = new char[newnumvertices];
 	memset(used,0,newnumvertices*sizeof(char));

 	if(!error) {
 		//return result
 		long size;
 		Poly mpoly;
 		for(i=0;i<newnumvertices;i++) {
 			if(used[i]) continue;
 			v = &(vertices[i]);
 			vnext = &(vertices[v->next]);
 			size = 1;
 			while(vnext!=v) {
 				vnext = &(vertices[vnext->next]);
 				size++;
 			}
 			mpoly.Init(size);
 			v = &(vertices[i]);
 			mpoly[0] = v->p;
 			vnext = &(vertices[v->next]);
 			size = 1;
 			used[i] = 1;
 			used[v->next] = 1;
 			while(vnext!=v) {
 				mpoly[size] = vnext->p;
 				used[vnext->next] = 1;
 				vnext = &(vertices[vnext->next]);
 				size++;
 			}
 			monotonePolys->push_back(mpoly);
 		}
 	}

 	//cleanup
 	delete [] vertices;
 	delete [] priority;
 	delete [] vertextypes;
 	delete [] edgeTreeIterators;
 	delete [] helpers;
 	delete [] used;

 	if(error) {
 		return 0;
 	} else {
 		return 1;
 	}
 }

/**
 * Triangulates the single monotone polygon passed and stores the triangles in the second parameter
 * @param  inPoly    input, a monotone polygon
 * @param  triangles result, a list of triangles
 * @return           1 for success, 0 for failure
 */
 int Partition::TriangulateMonotone(Poly *inPoly, list<Poly> *triangles) {
 	long i,i2,j,topindex,bottomindex,leftindex,rightindex,vindex;
 	Point *points;
 	long numpoints;
 	Poly triangle;

 	numpoints = inPoly->GetNumPoints();
 	points = inPoly->GetPoints();
    // inPoly->printVert(cout);

 	//trivial cases
 	if(numpoints < 3) return 0;
 	if(numpoints == 3) {
 		triangles->push_back(*inPoly);
 		return 1;
 	}

 	topindex = 0; bottomindex=0;
 	for(i=1;i<numpoints;i++) {
 		if (points[i].y < points[bottomindex].y) {
 			bottomindex = i;
 		}
 		else if (points[i].y == points[bottomindex].y) {
 			 if (points[i].x > points[bottomindex].x) bottomindex = i;
 		}

 		if (points[topindex].y < points[i].y) {
 			topindex = i;
 		}
 		else if (points[topindex].y == points[i].y) {
 			if (points[topindex].x > points[i].x) topindex = i;
 		}
 	}

 	/// check if the poly is really monotone
 	i = topindex;
 	while(i!=bottomindex) {
 		i2 = i+1;
 		if(i2>=numpoints) i2 = 0;
 		if(points[i2].y > points[i].y) return 0;
 		i = i2;
 	}
 	i = bottomindex;
 	while(i!=topindex) {
 		i2 = i+1; if(i2>=numpoints) i2 = 0;
 		if(points[i].y > points[i2].y) return 0;
 		i = i2;
 	}

 	int *vertextypes = new int[numpoints];
 	long *priority = new long[numpoints];

 	/// merge left and right vertex chains
 	priority[0] = topindex;
 	vertextypes[topindex] = 0; // vertextypes 0 for top and bottom, 1 for left and -1 for right
 	leftindex = topindex + 1; // because CCW ordering of input points
 	if(leftindex >= numpoints) leftindex = 0;
 	rightindex = topindex - 1; // because CCW ordering of input points
 	if(rightindex < 0) rightindex = numpoints-1;

 	for(i=1;i<(numpoints-1);i++) {
 		if(leftindex==bottomindex) {
 			priority[i] = rightindex;
 			rightindex--;
 			if(rightindex < 0) rightindex = numpoints-1;
 			vertextypes[priority[i]] = -1;
 		} else if(rightindex == bottomindex) {
 			priority[i] = leftindex;
 			leftindex++;
 			if(leftindex >= numpoints) leftindex = 0;
 			vertextypes[priority[i]] = 1;
 		} else {
 			if(points[leftindex].y < points[rightindex].y) {
 				priority[i] = rightindex;
 				rightindex--;
 				if(rightindex<0) rightindex = numpoints-1;
 				vertextypes[priority[i]] = -1;
 			} else {
 				priority[i] = leftindex;
 				leftindex++;
 				if(leftindex>=numpoints) leftindex = 0;
 				vertextypes[priority[i]] = 1;
 			}
 		}
 	}
 	priority[i] = bottomindex;
 	vertextypes[bottomindex] = 0;

 	std::stack<long> mystack;

 	mystack.push(priority[0]);
 	mystack.push(priority[1]);

 	for(i = 2; i < (numpoints-1); i++) {
 		vindex = priority[i];
 		/// on different chains
 		if(vertextypes[vindex]!=vertextypes[mystack.top()]) {
 			long popped_index;
 			while(!mystack.empty()) {
 				popped_index = mystack.top();
                mystack.pop();
 				if(mystack.empty()) break; // Skipping the last element
 				if(vertextypes[vindex] == -1) {
 					triangle.Triangle(points[vindex],points[mystack.top()],points[popped_index]);
 				} else {
 					triangle.Triangle(points[vindex],points[popped_index],points[mystack.top()]);
 				}
 				triangles->push_back(triangle);
 			}
 			mystack.push(priority[i-1]);
 			mystack.push(vindex);
 		}
 		/// on the same chain
 		else {
 			long popped_index = mystack.top();
            mystack.pop();
 			while(!mystack.empty()) {
 				if(vertextypes[vindex] == -1) {
 					if(IsConvexAngle(points[vindex],points[mystack.top()],points[popped_index])) {
 						triangle.Triangle(points[vindex],points[mystack.top()],points[popped_index]);
 						triangles->push_back(triangle);
                        popped_index = mystack.top();
                        mystack.pop();
 					} else {
 						break;
 					}
 				} else {
 					if(IsConvexAngle(points[vindex],points[popped_index],points[mystack.top()])) {
 						triangle.Triangle(points[vindex],points[popped_index],points[mystack.top()]);
 						triangles->push_back(triangle);
                        popped_index = mystack.top();
                        mystack.pop();
 					} else {
 						break;
 					}
 				}
 			}
 			mystack.push(popped_index);
 			mystack.push(vindex);
 		}
 	}

 	vindex = priority[i]; // this is the bottommost vertex Un
 	while(!mystack.empty()) {
 		long popped_index = mystack.top();
        mystack.pop();
 		if(mystack.empty()) break; // discard last
 		if(vertextypes[popped_index] == -1) {
 			triangle.Triangle(points[popped_index],points[mystack.top()],points[vindex]);
 		} else {
 			triangle.Triangle(points[mystack.top()],points[popped_index],points[vindex]);
 		}
 		triangles->push_back(triangle);
 	}

 	delete [] priority;
 	delete [] vertextypes;

 	return 1;
 }

/**
 * First partitions the polygons passed into monotone ones, then applies TriangulateMonotone on each of them
 * @param  inpolys   input, a list of Simple polygons
 * @param  triangles result, a list of triangles
 * @return           1 for success, 0 for failure
 */
int Partition::Triangulate_MONO(list<Poly> *inpolys, list<Poly> *triangles) {
	list<Poly> monotone;
	list<Poly>::iterator iter;

	if(!MonotonePartition(inpolys,&monotone)) {
        cout << "MonotonePartition error" << endl;
        return 0;
    }
	for(iter = monotone.begin(); iter!=monotone.end();iter++) {
		if(!TriangulateMonotone(&(*iter),triangles)) {
            cout << "TriangulateMonotone error" << endl;
            return 0;
        }
	}
	return 1;
}

/**
 * Adds the polygon to a list and calls the other overloaded Triangulate_MONO method
 * @param  poly
 * @param  triangles
 * @return
 */
int Partition::Triangulate_MONO(Poly *poly, list<Poly> *triangles) {
	list<Poly> polys;
	polys.push_back(*poly);

	return Triangulate_MONO(&polys, triangles);
}
