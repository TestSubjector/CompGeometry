#include "dcel.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <list>
#include <algorithm>
#include <set>

using namespace std;

#define TPPL_VERTEXTYPE_REGULAR 0
#define TPPL_VERTEXTYPE_START 1
#define TPPL_VERTEXTYPE_END 2
#define TPPL_VERTEXTYPE_SPLIT 3
#define TPPL_VERTEXTYPE_MERGE 4

TPPLPoly::TPPLPoly() { 
	hole = false;
	numpoints = 0;
	points = NULL;
}

TPPLPoly::~TPPLPoly() {
	if(points) delete [] points;
}

void TPPLPoly::Clear() {
	if(points) delete [] points;
	hole = false;
	numpoints = 0;
	points = NULL;
}

void TPPLPoly::Init(long numpoints) {
	Clear();
	this->numpoints = numpoints;
	points = new TPPLPoint[numpoints];
}

void TPPLPoly::Triangle(TPPLPoint &p1, TPPLPoint &p2, TPPLPoint &p3) {
	Init(3);
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
}

TPPLPoly::TPPLPoly(const TPPLPoly &src) {
	hole = src.hole;
	numpoints = src.numpoints;
	points = new TPPLPoint[numpoints];
	memcpy(points, src.points, numpoints*sizeof(TPPLPoint));
}

TPPLPoly& TPPLPoly::operator=(const TPPLPoly &src) {
	Clear();
	hole = src.hole;
	numpoints = src.numpoints;
	points = new TPPLPoint[numpoints];
	memcpy(points, src.points, numpoints*sizeof(TPPLPoint));
	return *this;
}

int TPPLPoly::GetOrientation() {
	long i1,i2;
	tppl_float area = 0;
	for(i1=0; i1<numpoints; i1++) {
		i2 = i1+1;
		if(i2 == numpoints) i2 = 0;
		area += points[i1].x * points[i2].y - points[i1].y * points[i2].x;
	}
	if(area>0) return TPPL_CCW;
	if(area<0) return TPPL_CW;
	return 0;
}

void TPPLPoly::SetOrientation(int orientation) {
	int polyorientation = GetOrientation();
	if(polyorientation&&(polyorientation!=orientation)) {
		Invert();
	}
}

void TPPLPoly::Invert() {
	long i;
	TPPLPoint *invpoints;

	invpoints = new TPPLPoint[numpoints];
	for(i=0;i<numpoints;i++) {
		invpoints[i] = points[numpoints-i-1];
	}

	delete [] points;
	points = invpoints;
}

TPPLPoint TPPLPartition::Normalize(const TPPLPoint &p) {
	TPPLPoint r;
	tppl_float n = sqrt(p.x*p.x + p.y*p.y);
	if(n!=0) {
		r = p/n;
	} else {
		r.x = 0;
		r.y = 0;
	}
	return r;
}

tppl_float TPPLPartition::Distance(const TPPLPoint &p1, const TPPLPoint &p2) {
	tppl_float dx,dy;
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	return(sqrt(dx*dx + dy*dy));
}

int TPPLPartition::MonotonePartition(list<TPPLPoly> *inpolys, list<TPPLPoly> *monotonePolys) {
	list<TPPLPoly>::iterator iter;
	MonotoneVertex *vertices;
	long i,numvertices,vindex,vindex2,newnumvertices,maxnumvertices;
	long polystartindex, polyendindex;
	TPPLPoly *poly;
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
			if(IsConvex(vnext->p,vprev->p,v->p)) {
				vertextypes[i] = TPPL_VERTEXTYPE_START;
			} else {
				vertextypes[i] = TPPL_VERTEXTYPE_SPLIT;
			}
		} else if(Below(v->p,vprev->p)&&Below(v->p,vnext->p)) {
			if(IsConvex(vnext->p,vprev->p,v->p))
			{
				vertextypes[i] = TPPL_VERTEXTYPE_END;
			} else {
				vertextypes[i] = TPPL_VERTEXTYPE_MERGE;
			}
		} else {
			vertextypes[i] = TPPL_VERTEXTYPE_REGULAR;
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
			case TPPL_VERTEXTYPE_START:
				//Insert ei in T and set helper(ei) to vi.
				newedge.p1 = v->p;
				newedge.p2 = vertices[v->next].p;
				newedge.index = vindex;
				edgeTreeRet = edgeTree.insert(newedge);
				edgeTreeIterators[vindex] = edgeTreeRet.first;
				helpers[vindex] = vindex;
				break;

			case TPPL_VERTEXTYPE_END:
				//if helper(ei-1) is a merge vertex
				if(vertextypes[helpers[v->previous]]==TPPL_VERTEXTYPE_MERGE) {
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

			case TPPL_VERTEXTYPE_SPLIT:
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
				//helper(e j)�vi
				helpers[edgeIter->index] = vindex;
				//Insert ei in T and set helper(ei) to vi.
				newedge.p1 = v2->p;
				newedge.p2 = vertices[v2->next].p;
				newedge.index = vindex2;
				edgeTreeRet = edgeTree.insert(newedge);
				edgeTreeIterators[vindex2] = edgeTreeRet.first;
				helpers[vindex2] = vindex2;
				break;

			case TPPL_VERTEXTYPE_MERGE:
				//if helper(ei-1) is a merge vertex
				if(vertextypes[helpers[v->previous]]==TPPL_VERTEXTYPE_MERGE) {
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
				if(vertextypes[helpers[edgeIter->index]]==TPPL_VERTEXTYPE_MERGE) {
					//Insert the diagonal connecting vi to helper(e j) in D.
					AddDiagonal(vertices,&newnumvertices,vindex2,helpers[edgeIter->index]);
					vertextypes[newnumvertices-2] = vertextypes[vindex2];
					edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex2];
					helpers[newnumvertices-2] = helpers[vindex2];
					vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
					edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
					helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
				}
				//helper(e j)�vi
				helpers[edgeIter->index] = vindex2;
				break;

			case TPPL_VERTEXTYPE_REGULAR:
				//if the interior of P lies to the right of vi
				if(Below(v->p,vertices[v->previous].p)) {
					//if helper(ei-1) is a merge vertex
					if(vertextypes[helpers[v->previous]]==TPPL_VERTEXTYPE_MERGE) {
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
					if(vertextypes[helpers[edgeIter->index]]==TPPL_VERTEXTYPE_MERGE) {
						//Insert the diagonal connecting vi to helper(e j) in D.
						AddDiagonal(vertices,&newnumvertices,vindex,helpers[edgeIter->index]);
						vertextypes[newnumvertices-2] = vertextypes[vindex];
						edgeTreeIterators[newnumvertices-2] = edgeTreeIterators[vindex];
						helpers[newnumvertices-2] = helpers[vindex];
						vertextypes[newnumvertices-1] = vertextypes[helpers[edgeIter->index]];
						edgeTreeIterators[newnumvertices-1] = edgeTreeIterators[helpers[edgeIter->index]];
						helpers[newnumvertices-1] = helpers[helpers[edgeIter->index]];
					}
					//helper(e j)�vi
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
		TPPLPoly mpoly;
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